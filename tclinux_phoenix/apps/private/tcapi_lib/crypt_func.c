/******************************************************************************/
/*
 * Copyright (C) 1994-2008 TrendChip Technologies, Corp.
 * All Rights Reserved.
 *
 * TrendChip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of TrendChip Technologies, Corp. and
 * shall not be reproduced, copied, disclosed, or used in whole or
 * in part for any reason without the prior express written permission of
 * TrendChip Technologies, Corp.
 */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdarg.h>

/*add by brian for message mechanism*/
#include <sys/msg.h>
#include "tcapi.h"
#include "libtcapi.h"

int isPasswdAttr(char* attr)
{
	if(
	//	(strcmp(attr, "DownloadPassword")==0) || (strcmp(attr, "acsPassword")==0) || (strcmp(attr, "conReqPassword")==0)
	//	|| (strcmp(attr, "PASSWORD")==0) || (strcmp(attr, "SIPPassword")==0) || (strcmp(attr, "Passwd")==0) 
	//	|| (strcmp(attr, "adminpasswd")==0) || (strcmp(attr, "Password")==0) || (strcmp(attr, "Password0")==0)|| 
   		(strcmp(attr, "web_passwd")==0) || (strcmp(attr, "console_passwd")==0))
	{
		return 1;
	}

	return 0;
}

#define kAscii_0            0x30
#define kAscii_9            0x39
#define kAscii_A            0x41
#define kAscii_Z            0x5A
#define kAscii_a            0x61
#define kAscii_z            0x7A
#define kAscii_Plus         0x2B
#define kAscii_Slash        0x2F
#define kAscii_Equal        0x3D

static const char   gBase64Table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


static unsigned char ConvertBase64Character(unsigned char theInputCharacter) {
    unsigned char   theResult;

    theResult = 0;
    if (theInputCharacter >= kAscii_A &&
        theInputCharacter <= kAscii_Z ) {
        theResult = theInputCharacter - kAscii_A;
    }
    else if (theInputCharacter >= kAscii_a &&
                theInputCharacter <= kAscii_z) {
        theResult = theInputCharacter - kAscii_a + 26;
    }
    else if (theInputCharacter >= kAscii_0 &&
                theInputCharacter <= kAscii_9) {
        theResult = theInputCharacter - kAscii_0 + 52;
    }
    else if (theInputCharacter == kAscii_Plus) {
        theResult = 62;
    }
    else if (theInputCharacter == kAscii_Slash) {
        theResult = 63;
    }
    return theResult;
}


unsigned short decodeBase64Data(char *theInputPtr, unsigned short theInputCount,
        char *theOutputPtr) {
    unsigned int  thePackedGroup;
    char *      theOriginalOutputPtr;

    theOriginalOutputPtr = theOutputPtr;

    /*
        Make sure theInputCount is a multiple of 4.
    */
    theInputCount -= (unsigned short) (theInputCount % 4);

    while (theInputCount > 0) {
        thePackedGroup =
                (unsigned int) ConvertBase64Character(*theInputPtr++) << 18;
        thePackedGroup |=
                (unsigned int) ConvertBase64Character(*theInputPtr++) << 12;
        *theOutputPtr++ = (unsigned char) (thePackedGroup >> 16);
        if (*theInputPtr != 0x3D) {
            thePackedGroup |=
                    (unsigned int) ConvertBase64Character(*theInputPtr++) << 6;
            *theOutputPtr++ = (unsigned char) (thePackedGroup >> 8);
            if (*theInputPtr != 0x3D) {
                thePackedGroup |=
                        (unsigned int) ConvertBase64Character(*theInputPtr++);
                *theOutputPtr++ = (unsigned char) (thePackedGroup);
            }
        }
        theInputCount -= 4;
    }
    *theOutputPtr = '\0';

    return theOutputPtr - theOriginalOutputPtr;
}

static void BuildBase64Group(char *theInputPtr, char *theOutputPtr) {
    unsigned char theFirstByte, theSecondByte, theThirdByte, theFourthByte;

    theFirstByte = (*theInputPtr >> 2) & 077;
    theSecondByte =
            ((theInputPtr[0] << 4) & 060) | ((theInputPtr[1] >> 4) & 017);
    theThirdByte =
            ((theInputPtr[1] << 2) & 074) | ((theInputPtr[2] >> 6) & 03);
    theFourthByte = theInputPtr[2] & 077;

    *theOutputPtr++ = gBase64Table[theFirstByte];
    *theOutputPtr++ = gBase64Table[theSecondByte];
    *theOutputPtr++ = gBase64Table[theThirdByte];
    *theOutputPtr = gBase64Table[theFourthByte];
    return;
}

void encodeBase64String(char *theInputPtr, unsigned short theInputCount,
        char *theOutputPtr) {
    unsigned char theFirstInputByte, theSecondInputByte;
    unsigned char theFirstOutputByte, theSecondOutputByte, theThirdOutputByte;

    while (theInputCount > 2) {
        BuildBase64Group(theInputPtr, theOutputPtr);
        theInputCount -= 3;
        theInputPtr += 3;
        theOutputPtr += 4;
    }
    if (theInputCount == 2) {
        theFirstInputByte = *theInputPtr++;
        theSecondInputByte = *theInputPtr;
    }
    else if (theInputCount == 1) {
        theFirstInputByte = *theInputPtr;
        theSecondInputByte = 0x00;
    }
    else {
        /*
            Must be 0, so terminate the string and return.
        */
        *theOutputPtr = '\0';
        return;
    }
    theFirstOutputByte = theFirstInputByte >> 2;
    theSecondOutputByte = ((theFirstInputByte << 4) & 060) |
            ((theSecondInputByte >> 4) & 017);

    *theOutputPtr++ = gBase64Table[theFirstOutputByte];
    *theOutputPtr++ = gBase64Table[theSecondOutputByte];

    if (theInputCount == 1) {
        *theOutputPtr++ = 0x3D;
        *theOutputPtr++ = 0x3D;
    }
    else {
        theThirdOutputByte = (theSecondInputByte << 2) & 074;
        *theOutputPtr++ = gBase64Table[theThirdOutputByte];
        *theOutputPtr++ = 0x3D;
    }
    *theOutputPtr = '\0';
    return;
}

int enCryptPasswd(char* orgPass, char* cryptPass)
{
	int len = 0;

	if(orgPass == NULL)
	{
		return 1;//encrypt fail
	}

	len = strlen(orgPass);
	
	encodeBase64String(orgPass, len, cryptPass);

	return 0;//encrypt success
}

int deCryptPasswd(char* cryptPass, char* orgPass)
{
	int len = 0;

	if(cryptPass == NULL)
	{
		return 1;//encrypt fail
	}

	len = strlen(cryptPass);
	
	decodeBase64Data(cryptPass, len, orgPass);

	return 0;//encrypt success
}

