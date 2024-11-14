/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or
its licensors. Without the prior written permission of EcoNet and/or its licensors,
any reproduction, modification, use or disclosure of EcoNet Software, and
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE.
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH
ECONET SOFTWARE.
***************************************************************/
/************************************************************************
*                  I N C L U D E S
*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "queue_api.h"

/*
	function:init queue
	result:return queue point when init succeed, otherwise return NULL 
*/
Queue *QueueInit()
{
	Queue *queue = NULL;

	queue = (Queue *)malloc(sizeof(Queue));
	if ( NULL == queue )
		return NULL;

	queue->length = 0;
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
}

/*
	function:add queue data to tail
	params:
		queue: queue point
		data: data filed
	result:return 0 when add succeed, otherwise return -1
*/
int QueuePushTail(Queue *queue, void *data)
{
	QueueNode *node = NULL;

	if ( NULL == queue )
		return -1;

	/* create new QueueNode */
	node = (QueueNode *)malloc(sizeof(QueueNode));
	if ( NULL == node )
		return -1;

	memset(node, 0, sizeof(QueueNode));
	node->data = data;

	if ( NULL == queue->tail && NULL == queue->head )
	{
		/* if current queue is empty, add first node */
		queue->tail = node;
		queue->head = node;
		node->next = NULL;
		node->prev = NULL;
	}
	else
	{
		/* current queue is not empty, add to tail */
		queue->tail->next = node;
		node->prev = queue->tail;
		node->next = NULL;
		queue->tail = node;
	}

	queue->length++;
	return 0;
}

/*
	function:get queue data from head
	params:
		queue: queue point
	result:return data point when get succeed, otherwise return NULL
*/
void *QueuePopHead(Queue *queue)
{
	void *data = NULL;
	QueueNode *node = NULL;

	/* queue is NULL or queue is empty */
	if ( NULL == queue || NULL == queue->head )
		return NULL;

	/* pop head node */
	node = queue->head;
	queue->head = node->next;
	queue->length--;
	
	/* if new head of queue is not NULL, reset prev node of queue head to NULL */
	if ( NULL != queue->head )
	{
		queue->head->prev = NULL;
	}
	/* reset queue head and tail, when all element is popped out */
	else
	{
		queue->head = NULL;
		queue->tail = NULL;
	}
	
	/* return data */
	data = node->data;
	free(node);
	return data;
}

/*
	function:free queue and data filed
	params:
		queue: queue point
		isFreeData:auto free data filed or not
*/
void QueueFree(Queue *queue, int isFreeData)
{
	void *data = NULL;

	if ( NULL == queue )
		return;

	/*  */
	while ( NULL != ( data = QueuePopHead(queue) ) )
	{
		if ( isFreeData )
			free(data);
	}

	free(queue);
	return;
}


