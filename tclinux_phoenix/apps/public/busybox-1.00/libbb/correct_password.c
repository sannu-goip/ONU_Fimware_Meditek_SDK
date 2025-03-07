/* vi: set sw=4 ts=4: */
/*
 * Copyright 1989 - 1991, Julianne Frances Haugh <jockgrrl@austin.rr.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Julianne F. Haugh nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JULIE HAUGH AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL JULIE HAUGH OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <ctype.h>
#include <crypt.h>

#include "libbb.h"



/* Ask the user for a password.
   Return 1 if the user gives the correct password for entry PW,
   0 if not.  Return 1 without asking for a password if run by UID 0
   or if PW has an empty password.  */

int correct_password ( const struct passwd *pw )
{
	char *unencrypted, *encrypted, *correct;

#ifdef CONFIG_FEATURE_SHADOWPASSWDS
	if (( strcmp ( pw-> pw_passwd, "x" ) == 0 ) || ( strcmp ( pw-> pw_passwd, "*" ) == 0 )) {
		struct spwd *sp = getspnam ( pw-> pw_name );

		if ( !sp )
			bb_error_msg_and_die ( "no valid shadow password" );

		correct = sp-> sp_pwdp;
	}
	else
#endif
    	correct = pw-> pw_passwd;

	if ( correct == 0 || correct[0] == '\0' )
		return 1;

	unencrypted = bb_askpass ( 0, "Password: " );
	if ( !unencrypted )
	{
		return 0;
	}
	encrypted = crypt ( unencrypted, correct );
	memset ( unencrypted, 0, bb_strlen ( unencrypted ));

	/* encrypted maybe NULL, crash when do strcmp */
	if ( !encrypted )
		return 0;

	return ( strcmp ( encrypted, correct ) == 0 ) ? 1 : 0;
}
