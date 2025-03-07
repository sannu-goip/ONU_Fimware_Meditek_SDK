
/*
 * Sysctl 1.01 - A utility to read and manipulate the sysctl parameters
 *
 *
 * "Copyright 1999 George Staikos
 * This file may be used subject to the terms and conditions of the
 * GNU General Public License Version 2, or any later version
 * at your option, as published by the Free Software Foundation.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details."
 *
 * Changelog:
 *	v1.01:
 *		- added -p <preload> to preload values from a file
 *	v1.01.1
 *		- busybox applet aware by <solar@gentoo.org>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "busybox.h"

/*
 *    Function Prototypes
 */
static int sysctl_read_setting(const char *setting, int output);
static int sysctl_write_setting(const char *setting, int output);
static int sysctl_preload_file(const char *filename, int output);
static int sysctl_display_all(const char *path, int output, int show_table);

/*
 *    Globals...
 */
static const char PROC_PATH[] = "/proc/sys/";
static const char DEFAULT_PRELOAD[] = "/etc/sysctl.conf";

/* error messages */
static const char ERR_UNKNOWN_PARAMETER[] = "error: Unknown parameter '%s'\n";
static const char ERR_MALFORMED_SETTING[] = "error: Malformed setting '%s'\n";
static const char ERR_NO_EQUALS[] =
	"error: '%s' must be of the form name=value\n";
static const char ERR_INVALID_KEY[] = "error: '%s' is an unknown key\n";
static const char ERR_UNKNOWN_WRITING[] =
	"error: unknown error %d setting key '%s'\n";
static const char ERR_UNKNOWN_READING[] =
	"error: unknown error %d reading key '%s'\n";
static const char ERR_PERMISSION_DENIED[] =
	"error: permission denied on key '%s'\n";
static const char ERR_OPENING_DIR[] = "error: unable to open directory '%s'\n";
static const char ERR_PRELOAD_FILE[] =
	"error: unable to open preload file '%s'\n";
static const char WARN_BAD_LINE[] =
	"warning: %s(%d): invalid syntax, continuing...\n";


static void dwrite_str(int fd, const char *buf)
{
	write(fd, buf, strlen(buf));
}

/*
 *    sysctl_main()...
 */
int sysctl_main(int argc, char **argv)
{
	int retval = 0;
	int output = 1;
	int write_mode = 0;
	int switches_allowed = 1;

	if (argc < 2)
		bb_show_usage();

	argv++;

	for (; argv && *argv && **argv; argv++) {
		if (switches_allowed && **argv == '-') {	/* we have a switch */
			switch ((*argv)[1]) {
			case 'n':
				output = 0;
				break;
			case 'w':
				write_mode = 1;
				switches_allowed = 0;
				break;
			case 'p':
				argv++;
				return
					sysctl_preload_file(((argv && *argv
										  && **argv) ? *argv :
										 DEFAULT_PRELOAD), output);
			case 'a':
			case 'A':
				switches_allowed = 0;
				return sysctl_display_all(PROC_PATH, output,
										  ((*argv)[1] == 'a') ? 0 : 1);
			case 'h':
			case '?':
				bb_show_usage();
			default:
				bb_error_msg(ERR_UNKNOWN_PARAMETER, *argv);
				bb_show_usage();
			}
		} else {
			switches_allowed = 0;
			if (write_mode)
				retval = sysctl_write_setting(*argv, output);
			else
				sysctl_read_setting(*argv, output);
		}
	}
	return retval;
}						/* end sysctl_main() */



/*
 *     sysctl_preload_file
 *	preload the sysctl's from a conf file
 *           - we parse the file and then reform it (strip out whitespace)
 */
#define PRELOAD_BUF 256

int sysctl_preload_file(const char *filename, int output)
{
	int lineno = 0;
	char oneline[PRELOAD_BUF];
	char buffer[PRELOAD_BUF]={0};
	char *name, *value, *ptr;
	FILE *fp = NULL;

	if (!filename || ((fp = fopen(filename, "r")) == NULL)) {
		bb_error_msg_and_die(ERR_PRELOAD_FILE, filename);
	}

	while (fgets(oneline, sizeof(oneline) - 1, fp)) {
		oneline[sizeof(oneline) - 1] = 0;
		lineno++;
		trim(oneline);
		ptr = (char *) oneline;

		if (*ptr == '#' || *ptr == ';')
			continue;

		if (bb_strlen(ptr) < 2)
			continue;

		name = strtok(ptr, "=");
		if (!name || !*name) {
			bb_error_msg(WARN_BAD_LINE, filename, lineno);
			continue;
		}

		trim(name);

		value = strtok(NULL, "\n\r");
		if (!value || !*value) {
			bb_error_msg(WARN_BAD_LINE, filename, lineno);
			continue;
		}

		while ((*value == ' ' || *value == '\t') && *value != 0)
			value++;
		strncpy(buffer, name,sizeof(buffer)-1);
		strncat(buffer, "=",sizeof(buffer)-strlen(buffer)-1);
		strncat(buffer, value,sizeof(buffer)-strlen(buffer)-1);
		sysctl_write_setting(buffer, output);
	}
	fclose(fp);
	return 0;
}						/* end sysctl_preload_file() */


/*
 *     Write a single sysctl setting
 */
int sysctl_write_setting(const char *setting, int output)
{
	int retval = 0;
	const char *name = setting;
	const char *value;
	const char *equals;
	char *tmpname, *outname, *cptr;
	int fd = -1;

	if (!name)			/* probably dont' want to display this  err */
		return 0;

	if (!(equals = strchr(setting, '='))) {
		bb_error_msg(ERR_NO_EQUALS, setting);
		return -1;
	}

	value = equals + sizeof(char);	/* point to the value in name=value */

	if (!*name || !*value || name == equals) {
		bb_error_msg(ERR_MALFORMED_SETTING, setting);
		return -2;
	}

	bb_xasprintf(&tmpname, "%s%.*s", PROC_PATH, (equals - name), name);
	outname = bb_xstrdup(tmpname + strlen(PROC_PATH));

	while ((cptr = strchr(tmpname, '.')) != NULL)
		*cptr = '/';

	while ((cptr = strchr(outname, '/')) != NULL)
		*cptr = '.';

	if ((fd = open(tmpname, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
		switch (errno) {
		case ENOENT:
			bb_error_msg(ERR_INVALID_KEY, outname);
			break;
		case EACCES:
			bb_perror_msg(ERR_PERMISSION_DENIED, outname);
			break;
		default:
			bb_error_msg(ERR_UNKNOWN_WRITING, errno, outname);
			break;
		}
		retval = -1;
	} else {
		dwrite_str(fd, value);
		close(fd);
		if (output) {
			dwrite_str(STDOUT_FILENO, outname);
			dwrite_str(STDOUT_FILENO, " = ");
		}
		dwrite_str(STDOUT_FILENO, value);
		dwrite_str(STDOUT_FILENO, "\n");
	}

	/* cleanup */
	free(tmpname);
	free(outname);
	return retval;
}						/* end sysctl_write_setting() */


/*
 *     Read a sysctl setting
 *
 */
int sysctl_read_setting(const char *setting, int output)
{
	int retval = 0;
	char *tmpname, *outname, *cptr;
	char inbuf[1025];
	const char *name = setting;
	FILE *fp;

	if (!setting || !*setting)
		bb_error_msg(ERR_INVALID_KEY, setting);

	tmpname = concat_path_file(PROC_PATH, name);
	outname = bb_xstrdup(tmpname + strlen(PROC_PATH));

	while ((cptr = strchr(tmpname, '.')) != NULL)
		*cptr = '/';
	while ((cptr = strchr(outname, '/')) != NULL)
		*cptr = '.';

	if ((fp = fopen(tmpname, "r")) == NULL) {
		switch (errno) {
		case ENOENT:
			bb_error_msg(ERR_INVALID_KEY, outname);
			break;
		case EACCES:
			bb_error_msg(ERR_PERMISSION_DENIED, outname);
			break;
		default:
			bb_error_msg(ERR_UNKNOWN_READING, errno, outname);
			break;
		}
		retval = -1;
	} else {
		while (fgets(inbuf, sizeof(inbuf) - 1, fp)) {
			if (output) {
				dwrite_str(STDOUT_FILENO, outname);
				dwrite_str(STDOUT_FILENO, " = ");
			}
			dwrite_str(STDOUT_FILENO, inbuf);
		}
		fclose(fp);
	}

	free(tmpname);
	free(outname);
	return retval;
}						/* end sysctl_read_setting() */



/*
 *     Display all the sysctl settings
 *
 */
int sysctl_display_all(const char *path, int output, int show_table)
{
	int retval = 0;
	int retval2;
	DIR *dp;
	struct dirent *de;
	char *tmpdir;
	struct stat ts;

	if (!(dp = opendir(path))) {
		bb_perror_msg(ERR_OPENING_DIR, path);
		retval = -1;
	} else {
		while ((de = readdir(dp)) != NULL) {
			tmpdir = concat_subpath_file(path, de->d_name);
			if(tmpdir == NULL)
				continue;
			if ((retval2 = stat(tmpdir, &ts)) != 0)
				bb_perror_msg(tmpdir);
			else {
				if (S_ISDIR(ts.st_mode)) {
					sysctl_display_all(tmpdir, output, show_table);
				} else
					retval |=
						sysctl_read_setting(tmpdir + bb_strlen(PROC_PATH),
											output);

			}
			free(tmpdir);
		}				/* end while */
		closedir(dp);
	}

	return retval;
}						/* end sysctl_display_all() */

#ifdef STANDALONE_SYSCTL
int main(int argc, char **argv)
{
	return sysctl_main(argc, argv);
}
const char *bb_applet_name = "sysctl";
#endif
