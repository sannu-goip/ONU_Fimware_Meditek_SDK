/* vi: set sw=4 ts=4: */
/*
 * Mini dd implementation for busybox
 *
 *
 * Copyright (C) 2000,2001  Matt Kraai
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "busybox.h"


static const struct suffix_mult dd_suffixes[] = {
	{ "c", 1 },
	{ "w", 2 },
	{ "b", 512 },
	{ "kD", 1000 },
	{ "k", 1024 },
	{ "MD", 1000000 },
	{ "M", 1048576 },
	{ "GD", 1000000000 },
	{ "G", 1073741824 },
	{ NULL, 0 }
};

int dd_main(int argc, char **argv)
{
	size_t out_full = 0;
	size_t out_part = 0;
	size_t in_full = 0;
	size_t in_part = 0;
	size_t count = -1;
	size_t bs = 512;
	ssize_t n;
	off_t seek = 0;
	off_t skip = 0;
	int sync_flag = FALSE;
	int noerror = FALSE;
	int trunc_flag = TRUE;
	int oflag;
	int ifd;
	int ofd;
	int i;
	const char *infile = NULL;
	const char *outfile = NULL;
	char *buf;

	for (i = 1; i < argc; i++) {
		if (strncmp("bs=", argv[i], 3) == 0)
			bs = bb_xparse_number(argv[i]+3, dd_suffixes);
		else if (strncmp("count=", argv[i], 6) == 0)
			count = bb_xparse_number(argv[i]+6, dd_suffixes);
		else if (strncmp("seek=", argv[i], 5) == 0)
			seek = bb_xparse_number(argv[i]+5, dd_suffixes);
		else if (strncmp("skip=", argv[i], 5) == 0)
			skip = bb_xparse_number(argv[i]+5, dd_suffixes);
		else if (strncmp("if=", argv[i], 3) == 0)
			infile = argv[i]+3;
		else if (strncmp("of=", argv[i], 3) == 0)
			outfile = argv[i]+3;
		else if (strncmp("conv=", argv[i], 5) == 0) {
			buf = argv[i]+5;
			while (1) {
				if (strncmp("notrunc", buf, 7) == 0) {
					trunc_flag = FALSE;
					buf += 7;
				} else if (strncmp("sync", buf, 4) == 0) {
					sync_flag = TRUE;
					buf += 4;
				} else if (strncmp("noerror", buf, 7) == 0) {
					noerror = TRUE;
					buf += 7;
				} else {
					bb_error_msg_and_die("invalid conversion `%s'", argv[i]+5);
				}
				if (buf[0] == '\0')
					break;
				if (buf[0] == ',')
					buf++;
			}
		} else
			bb_show_usage();
	}

	buf = xmalloc(bs);

	if (infile != NULL) {
		ifd = bb_xopen(infile, O_RDONLY);
	} else {
		ifd = STDIN_FILENO;
		infile = bb_msg_standard_input;
	}

	if (outfile != NULL) {
		oflag = O_WRONLY | O_CREAT;

		if (!seek && trunc_flag) {
			oflag |= O_TRUNC;
		}

		if ((ofd = open(outfile, oflag, 0666)) < 0) {
			bb_perror_msg_and_die("%s", outfile);
		}

		if (seek && trunc_flag) {
			if (ftruncate(ofd, seek * bs) < 0) {
				struct stat st;

				if (fstat (ofd, &st) < 0 || S_ISREG (st.st_mode) ||
						S_ISDIR (st.st_mode)) {
					bb_perror_msg_and_die("%s", outfile);
				}
			}
		}
	} else {
		ofd = STDOUT_FILENO;
		outfile = bb_msg_standard_output;
	}

	if (skip) {
		if (lseek(ifd, skip * bs, SEEK_CUR) < 0) {
			bb_perror_msg_and_die("%s", infile);
		}
	}

	if (seek) {
		if (lseek(ofd, seek * bs, SEEK_CUR) < 0) {
			bb_perror_msg_and_die("%s", outfile);
		}
	}

	while (in_full + in_part != count) {
		if (noerror) {
			/* Pre-zero the buffer when doing the noerror thing */
			memset(buf, '\0', bs);
		}
		n = safe_read(ifd, buf, bs);
		if (n < 0) {
			if (noerror) {
				n = bs;
				bb_perror_msg("%s", infile);
			} else {
				bb_perror_msg_and_die("%s", infile);
			}
		}
		if (n == 0) {
			break;
		}
		if (n == bs) {
			in_full++;
		} else {
			in_part++;
		}
		if (sync_flag) {
			if(n < bs)
				memset(buf + n, '\0', bs - n);
			n = bs;
		}
		n = bb_full_write(ofd, buf, n);
		if (n < 0) {
			bb_perror_msg_and_die("%s", outfile);
		}
		if (n == bs) {
			out_full++;
		} else {
			out_part++;
		}
	}

	if (close (ifd) < 0) {
		bb_perror_msg_and_die("%s", infile);
	}

	if (close (ofd) < 0) {
		bb_perror_msg_and_die("%s", outfile);
	}

	fprintf(stderr, "%ld+%ld records in\n%ld+%ld records out\n",
			(long)in_full, (long)in_part,
			(long)out_full, (long)out_part);
	free(buf);
	return EXIT_SUCCESS;
}
