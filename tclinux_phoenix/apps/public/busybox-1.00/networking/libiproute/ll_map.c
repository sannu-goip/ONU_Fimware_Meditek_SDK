/*
 * ll_map.c
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "libnetlink.h"

struct idxmap
{
	struct idxmap * next;
	int		index;
	int		type;
	int		alen;
	unsigned	flags;
	unsigned char	addr[8];
	char		name[16];
};

static struct idxmap *idxmap[16];

int ll_remember_index(struct sockaddr_nl *who, struct nlmsghdr *n, void *arg)
{
	int h;
	struct ifinfomsg *ifi = NLMSG_DATA(n);
	struct idxmap *im, **imp;
	struct rtattr *tb[IFLA_MAX+1];

	if (n->nlmsg_type != RTM_NEWLINK)
		return 0;

	if (n->nlmsg_len < NLMSG_LENGTH(sizeof(ifi)))
		return -1;


	memset(tb, 0, sizeof(tb));
	parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifi), IFLA_PAYLOAD(n));
	if (tb[IFLA_IFNAME] == NULL)
		return 0;

	h = ifi->ifi_index&0xF;

	for (imp=&idxmap[h]; (im=*imp)!=NULL; imp = &im->next)
		if (im->index == ifi->ifi_index)
			break;

	if (im == NULL) {
		im = malloc(sizeof(*im));
		if (im == NULL)
			return 0;
		im->next = *imp;
		im->index = ifi->ifi_index;
		*imp = im;
	}

	im->type = ifi->ifi_type;
	im->flags = ifi->ifi_flags;
	if (tb[IFLA_ADDRESS]) {
		int alen;
		im->alen = alen = RTA_PAYLOAD(tb[IFLA_ADDRESS]);
		if (alen > sizeof(im->addr))
			alen = sizeof(im->addr);
		memcpy(im->addr, RTA_DATA(tb[IFLA_ADDRESS]), alen);
	} else {
		im->alen = 0;
		memset(im->addr, 0, sizeof(im->addr));
	}
	strncpy(im->name, RTA_DATA(tb[IFLA_IFNAME]),sizeof(im->name)-1);
	return 0;
}

const char *ll_idx_n2a(int idx, char *buf)
{
	struct idxmap *im;

	if (idx == 0)
		return "*";
	for (im = idxmap[idx&0xF]; im; im = im->next)
		if (im->index == idx)
			return im->name;
	snprintf(buf, 16, "if%d", idx);
	return buf;
}


const char *ll_index_to_name(int idx)
{
	static char nbuf[16];

	return ll_idx_n2a(idx, nbuf);
}

int ll_index_to_type(int idx)
{
	struct idxmap *im;

	if (idx == 0)
		return -1;
	for (im = idxmap[idx&0xF]; im; im = im->next)
		if (im->index == idx)
			return im->type;
	return -1;
}

unsigned ll_index_to_flags(int idx)
{
	struct idxmap *im;

	if (idx == 0)
		return 0;

	for (im = idxmap[idx&0xF]; im; im = im->next)
		if (im->index == idx)
			return im->flags;
	return 0;
}

int ll_name_to_index(char *name)
{
	static char ncache[16];
	static int icache;
	struct idxmap *im;
	int i;

	if (name == NULL)
		return 0;
	if (icache && strcmp(name, ncache) == 0)
		return icache;
	for (i=0; i<16; i++) {
		for (im = idxmap[i]; im; im = im->next) {
			if (strcmp(im->name, name) == 0) {
				icache = im->index;
				strncpy(ncache, name,sizeof(ncache)-1);
				return im->index;
			}
		}
	}
	return 0;
}

int ll_init_map(struct rtnl_handle *rth)
{
	if (rtnl_wilddump_request(rth, AF_UNSPEC, RTM_GETLINK) < 0) {
		perror("Cannot send dump request");
		exit(1);
	}

	if (rtnl_dump_filter(rth, ll_remember_index, &idxmap, NULL, NULL) < 0) {
		fprintf(stderr, "Dump terminated\n");
		exit(1);
	}
	return 0;
}
