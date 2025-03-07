/* vi: set sw=4 ts=4: */
/*
 * ftpget
 *
 * Mini implementation of FTP to retrieve a remote file.
 *
 * Copyright (C) 2002 Jeff Angielski, The PTR Group <jeff@theptrgroup.com>
 * Copyright (C) 2002 Glenn McGrath <bug1@iinet.net.au>
 *
 * Based on wget.c by Chip Rosenthal Covad Communications
 * <chip@laserlink.net>
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
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "busybox.h"

typedef struct ftp_host_info_s {
	char *user;
	char *password;
	struct sockaddr_in *s_in;
} ftp_host_info_t;

static char verbose_flag = 0;
static char do_continue = 0;

static int ftpcmd(const char *s1, const char *s2, FILE *stream, char *buf)
{
	if (verbose_flag) {
		bb_error_msg("cmd %s%s", s1, s2);
	}

	if (s1) {
		if (s2) {
#if defined(TCSUPPORT_CT_FTP_DOWNLOADCLIENT)
			fprintf(stream, "%s%s\r\n", s1, s2);
#else
			fprintf(stream, "%s%s\n", s1, s2);
#endif
		} else {
#if defined(TCSUPPORT_CT_FTP_DOWNLOADCLIENT)
			fprintf(stream, "%s\r\n", s1);
#else
			fprintf(stream, "%s\n", s1);
#endif
		}
	}
	do {
		char *buf_ptr=NULL;
		if (fgets(buf, 510, stream) == NULL) {
			bb_perror_msg_and_die("fgets()");
		}
		buf_ptr = strstr(buf, "\r\n");
		if (buf_ptr) {
			*buf_ptr = '\0';
		}
	} while (! isdigit(buf[0]) || buf[3] != ' ');

	return atoi(buf);
}

static int xconnect_ftpdata(ftp_host_info_t *server, const char *buf)
{
	char *buf_ptr;
	unsigned short port_num;

	buf_ptr = strrchr(buf, ',');
	*buf_ptr = '\0';
	port_num = atoi(buf_ptr + 1);

	buf_ptr = strrchr(buf, ',');
	*buf_ptr = '\0';
	port_num += atoi(buf_ptr + 1) * 256;

	server->s_in->sin_port=htons(port_num);
	return(xconnect(server->s_in));
}

#if defined(TCSUPPORT_CPU_PERFORMANCE_TEST)
static int get_portnum_from_ftpdata(const char *buf)
{
	char *buf_ptr;
	unsigned short port_num;

	buf_ptr = strrchr(buf, ',');
	*buf_ptr = '\0';
	port_num = atoi(buf_ptr + 1);

	buf_ptr = strrchr(buf, ',');
	*buf_ptr = '\0';
	port_num += atoi(buf_ptr + 1) * 256;

	return htons(port_num);
}
#endif

static FILE *ftp_login(ftp_host_info_t *server)
{
	FILE *control_stream;
	char buf[512];
	int fd = -1;

	/* Connect to the command socket */
	if((fd = xconnect(server->s_in)) < 0)
		bb_perror_msg_and_die("Couldnt connect server->s_in");
	
	control_stream = fdopen(fd, "r+");
	if (control_stream == NULL) {
		bb_perror_msg_and_die("Couldnt open control stream");
	}

	if (ftpcmd(NULL, NULL, control_stream, buf) != 220) {
		bb_error_msg_and_die("%s", buf + 4);
	}

	/*  Login to the server */
	switch (ftpcmd("USER ", server->user, control_stream, buf)) {
	case 230:
		break;
	case 331:
		if (ftpcmd("PASS ", server->password, control_stream, buf) != 230) {
			bb_error_msg_and_die("PASS error: %s", buf + 4);
		}
		break;
	default:
		bb_error_msg_and_die("USER error: %s", buf + 4);
	}

	ftpcmd("TYPE I", NULL, control_stream, buf);

	return(control_stream);
}

#ifdef CONFIG_FTPGET
static int ftp_recieve(ftp_host_info_t *server, FILE *control_stream,
		const char *local_path, char *server_path)
{
	char buf[512];
	off_t filesize = 0;
	int fd_data;
	int fd_local = -1;
	off_t beg_range = 0;
#if defined(TCSUPPORT_CPU_PERFORMANCE_TEST)
	char cmdbuf[512] = {0};
	char server_ip[64] = {0};
	unsigned short port_num = 0;
#endif	

	/* Connect to the data socket */
	if (ftpcmd("PASV", NULL, control_stream, buf) != 227) {
		bb_error_msg_and_die("PASV error: %s", buf + 4);
	}

#if !defined(TCSUPPORT_CPU_PERFORMANCE_TEST)
	fd_data = xconnect_ftpdata(server, buf);
#else
	port_num = get_portnum_from_ftpdata(buf);
	strcpy(server_ip, inet_ntoa(server->s_in->sin_addr));

	sprintf(cmdbuf, "echo %s %s %d %d > /proc/tc3162/kftp", "getconn", server_ip, port_num, filesize);
	printf("ftpget CMDBuf is [%s] \r\n", cmdbuf);
	system(cmdbuf);
#endif
	if (ftpcmd("SIZE ", server_path, control_stream, buf) == 213) {
		unsigned long value=filesize;
		if (safe_strtoul(buf + 4, &value))
			bb_error_msg_and_die("SIZE error: %s", buf + 4);
		filesize = value;
	}

	if ((local_path[0] == '-') && (local_path[1] == '\0')) {
		fd_local = STDOUT_FILENO;
		do_continue = 0;
	}

	if (do_continue) {
		struct stat sbuf;
		if (lstat(local_path, &sbuf) < 0) {
			bb_perror_msg_and_die("fstat()");
		}
		if (sbuf.st_size > 0) {
			beg_range = sbuf.st_size;
		} else {
			do_continue = 0;
		}
	}

	if (do_continue) {
		sprintf(buf, "REST %ld", (long)beg_range);
		if (ftpcmd(buf, NULL, control_stream, buf) != 350) {
			do_continue = 0;
		} else {
			filesize -= beg_range;
		}
	}

	if (ftpcmd("RETR ", server_path, control_stream, buf) > 150) {
		bb_error_msg_and_die("RETR error: %s", buf + 4);
	}

#if !defined(TCSUPPORT_CPU_PERFORMANCE_TEST)
	/* only make a local file if we know that one exists on the remote server */
	if (fd_local == -1) {
		if (do_continue) {
			fd_local = bb_xopen(local_path, O_APPEND | O_WRONLY);
		} else {
			fd_local = bb_xopen(local_path, O_CREAT | O_TRUNC | O_WRONLY);
		}
	}
	/* Copy the file */
	if (bb_copyfd_size(fd_data, fd_local, filesize) == -1) {
		exit(EXIT_FAILURE);
	}

	/* close it all down */
	close(fd_data);

#else
	sprintf(cmdbuf, "echo %s %s %d %d > /proc/tc3162/kftp", "getdata", server_ip, port_num, filesize);
	printf("ftpget CMDBuf is [%s] \r\n", cmdbuf);
	system(cmdbuf);
#endif
	
	if (ftpcmd(NULL, NULL, control_stream, buf) != 226) {
		bb_error_msg_and_die("ftp error: %s", buf + 4);	
	}
	ftpcmd("QUIT", NULL, control_stream, buf);

	return(EXIT_SUCCESS);
}
#endif

#ifdef CONFIG_FTPPUT
static int ftp_send(ftp_host_info_t *server, FILE *control_stream,
		const char *server_path, char *local_path)
{
	struct stat sbuf;
	char buf[512];
	int fd_data;
	int fd_local;
	int response;
#if defined(TCSUPPORT_CPU_PERFORMANCE_TEST)
	char cmdbuf[512] = {0};
	char server_ip[64] = {0};
	unsigned short port_num=0;
	off_t filesize = 0;	
	char cmdStr[128]={0};
#endif

	/*  Connect to the data socket */
	if (ftpcmd("PASV", NULL, control_stream, buf) != 227) {
		bb_error_msg_and_die("PASV error: %s", buf + 4);
	}
#if !defined(TCSUPPORT_CPU_PERFORMANCE_TEST)
	fd_data = xconnect_ftpdata(server, buf);
#else
	port_num = get_portnum_from_ftpdata(buf);

	strcpy(server_ip, inet_ntoa(server->s_in->sin_addr));

	sprintf(cmdbuf, "echo %s %s %d %d > /proc/tc3162/kftp", "getconn", server_ip, port_num, filesize);
	printf("ftpput CMDBuf is [%s] \r\n", cmdbuf);
	system(cmdbuf);

#endif

	if (ftpcmd("CWD ", server_path, control_stream, buf) != 250) {
		bb_error_msg_and_die("CWD error: %s", buf + 4);
	}

#if !defined(TCSUPPORT_CPU_PERFORMANCE_TEST)
	/* get the local file */
	if ((local_path[0] == '-') && (local_path[1] == '\0')) {
		fd_local = STDIN_FILENO;
	} else {
		fd_local = bb_xopen(local_path, O_RDONLY);
		fstat(fd_local, &sbuf);

		sprintf(buf, "ALLO %lu", (unsigned long)sbuf.st_size);
		response = ftpcmd(buf, NULL, control_stream, buf);
		switch (response) {
		case 200:
		case 202:
			break;
		default:
			close(fd_local);
			bb_error_msg_and_die("ALLO error: %s", buf + 4);
			break;
		}
	}
	response = ftpcmd("STOR ", local_path, control_stream, buf);
	switch (response) {
	case 125:
	case 150:
		break;
	default:
		close(fd_local);
		bb_error_msg_and_die("STOR error: %s", buf + 4);
	}

	/* transfer the file  */
	if (bb_copyfd_eof(fd_local, fd_data) == -1) {
		exit(EXIT_FAILURE);
	}

	/* close it all down */
	close(fd_data);
#else
	/*modify code for ignore whether local file is exist*/
	filesize = 100*1024*1024;
	sprintf(buf, "ALLO %lu", filesize);
	response = ftpcmd(buf, NULL, control_stream, buf);
	switch (response) {
	case 200:
	case 202:
		break;
	default:
		bb_error_msg_and_die("ALLO error: %s", buf + 4);
		break;
	}
	
	response = ftpcmd("STOR ", local_path, control_stream, buf);
	switch (response) {
	case 125:
	case 150:
		break;
	default:
		bb_error_msg_and_die("STOR error: %s", buf + 4);
	}

	sprintf(cmdbuf, "echo %s %s %d %d %s > /proc/tc3162/kftp", "putdata", server_ip, port_num, filesize, local_path);
	printf("ftpput CMDBuf is [%s] \r\n", cmdbuf);
	system(cmdbuf);	
#endif


	if (ftpcmd(NULL, NULL, control_stream, buf) != 226) {
		bb_error_msg_and_die("error: %s", buf + 4);
	}
	ftpcmd("QUIT", NULL, control_stream, buf);

	return(EXIT_SUCCESS);
}
#endif

#define FTPGETPUT_OPT_CONTINUE	1
#define FTPGETPUT_OPT_VERBOSE	2
#define FTPGETPUT_OPT_USER	4
#define FTPGETPUT_OPT_PASSWORD	8
#define FTPGETPUT_OPT_PORT	16

static const struct option ftpgetput_long_options[] = {
	{"continue", 1, NULL, 'c'},
	{"verbose", 0, NULL, 'v'},
	{"username", 1, NULL, 'u'},
	{"password", 1, NULL, 'p'},
	{"port", 1, NULL, 'P'},
	{0, 0, 0, 0}
};


#if defined(TCSUPPORT_CPU_PERFORMANCE_TEST)
static int ftp_quit_flag = 0;
static void quit_ftp_signal(int sig)
{

	if (ftp_quit_flag)
		return;
	
	ftp_quit_flag = 1;

//	printf("quit_ftp_signal:---quit ftp pid unnormal!\r\n");

	/* signal to kill echo */
	system("echo clear >/proc/tc3162/port_sock_map");
	system("echo 0 >/proc/tc3162/add_ftp_port");
	system("killall -9 echo");
	
	ftp_quit_flag = 0;

}
#endif


int ftpgetput_main(int argc, char **argv)
{
	/* content-length of the file */
	unsigned long opt;
	char *port = "ftp";
	char *LocalAddress;
	char *FtpMode = "pasv"; /* actv/pasv */
	char *SpeedTest = "disable"; /* enable/disable */

	/* socket to ftp server */
	FILE *control_stream;
	struct sockaddr_in s_in;

	/* continue a prev transfer (-c) */
	ftp_host_info_t *server;
	char speed_test_cmd[128];

	int (*ftp_action)(ftp_host_info_t *, FILE *, const char *, char *) = NULL;

#if defined(TCSUPPORT_CPU_PERFORMANCE_TEST)
	unsigned i = 0;
	unsigned long aff = 0xf;
	cpu_set_t new_mask;
	CPU_ZERO(&new_mask);
	while (i < CPU_SETSIZE && aff >= (1<<i)) {
		if ((1<<i) & aff)
			CPU_SET(i, &new_mask);
		++i;
	}
	sched_setaffinity(getpid(), sizeof (new_mask), &new_mask);
	
	signal(SIGTERM, quit_ftp_signal);
	signal(SIGSTOP, quit_ftp_signal);
	signal(SIGQUIT, quit_ftp_signal);
	signal(SIGILL, quit_ftp_signal);
	signal(SIGABRT, quit_ftp_signal);
#endif
	/* Check to see if the command is ftpget or ftput */
#ifdef CONFIG_FTPPUT
# ifdef CONFIG_FTPGET
	if (bb_applet_name[3] == 'p') {
		ftp_action = ftp_send;
	}
# else
	ftp_action = ftp_send;
# endif
#endif
#ifdef CONFIG_FTPGET
# ifdef CONFIG_FTPPUT
	if (bb_applet_name[3] == 'g') {
		ftp_action = ftp_recieve;
	}
# else
	ftp_action = ftp_recieve;
# endif
#endif

	/* Set default values */
	server = xmalloc(sizeof(ftp_host_info_t));
	server->user = "anonymous";
	server->password = "busybox@";
	verbose_flag = 0;

	/*
	 * Decipher the command line
	 */
	bb_applet_long_options = ftpgetput_long_options;
	opt = bb_getopt_ulflags(argc, argv, "cvu:p:P:A:m:s:", &server->user, &server->password, &port, &LocalAddress, &FtpMode, &SpeedTest);

	/* Process the non-option command line arguments */
	if (argc - optind != 3) {
		bb_show_usage();
	}

	if (opt & FTPGETPUT_OPT_CONTINUE) {
		do_continue = 1;
	}
	if (opt & FTPGETPUT_OPT_VERBOSE) {
		verbose_flag = 1;
	}

	/* We want to do exactly _one_ DNS lookup, since some
	 * sites (i.e. ftp.us.debian.org) use round-robin DNS
	 * and we want to connect to only one IP... */
	server->s_in = &s_in;
	bb_lookup_host(&s_in, argv[optind]);
	s_in.sin_port = bb_lookup_port(port, "tcp", 21);
	if (verbose_flag) {
		printf("Connecting to %s[%s]:%d\n",
				argv[optind], inet_ntoa(s_in.sin_addr), ntohs(s_in.sin_port));
	}

	if (!strncmp(SpeedTest, "enable", 6))
	{
		memset(speed_test_cmd, 0, sizeof(speed_test_cmd) );
		if(ftp_action == ftp_recieve){
			if (!strncmp(FtpMode, "pasv", 4)){
				snprintf(speed_test_cmd, sizeof(speed_test_cmd), "curl -o /dev/null -O ftp://%s/%s -u %s:%s -P %s --ftp-pasv --silent -speed-test enable ",
							argv[optind],argv[optind + 2],server->user,server->password,LocalAddress );
			}else{
				snprintf(speed_test_cmd, sizeof(speed_test_cmd), "curl -o /dev/null -O ftp://%s/%s -u %s:%s -P %s --silent -speed-test enable ",
							argv[optind],argv[optind + 2],server->user,server->password,LocalAddress );
			}
		}else{
			if (!strncmp(FtpMode, "pasv", 4)){
				snprintf(speed_test_cmd, sizeof(speed_test_cmd), "curl -T %s -u %s:%s ftp://%s -P %s --ftp-pasv --silent -speed-test enable ",
								argv[optind + 2],server->user,server->password,argv[optind],LocalAddress);
			}else{
				snprintf(speed_test_cmd, sizeof(speed_test_cmd), "curl -T %s -u %s:%s ftp://%s -P %s --silent -speed-test enable ",
								argv[optind + 2],server->user,server->password,argv[optind],LocalAddress);
			}
		}
		printf("%s\n", speed_test_cmd);
		system(speed_test_cmd);

		free(server);
		return(EXIT_SUCCESS);
	}

	/*  Connect/Setup/Configure the FTP session */
	control_stream = ftp_login(server);
	if(ftp_action)
		return(ftp_action(server, control_stream, argv[optind + 1], argv[optind + 2]));
	else
		return 0;
}

/*
Local Variables:
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/
