/* vi: set sw=4 ts=4: */
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <termios.h>
#include <unistd.h>
#include <utmp.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>

#include "busybox.h"
#ifdef CONFIG_SELINUX
#include <flask_util.h>
#include <get_sid_list.h>
#include <proc_secure.h>
#include <fs_secure.h>
#endif

#ifdef CONFIG_FEATURE_U_W_TMP
// import from utmp.c
static void checkutmp(int picky);
static void setutmp(const char *name, const char *line);
/* Stuff global to this file */
struct utmp utent;
#endif

// login defines
#define TIMEOUT       60
#define EMPTY_USERNAME_COUNT    10
#define USERNAME_SIZE 32

#define USERDEF_PASSWD_CONF "/etc/usertty"
#if defined(TCSUPPORT_CT)
#define USERDEF_PASSWD_CONF_C "/etc/userttyconsole"
#endif
#ifdef TCSUPPORT_TFTP_UPGRADE_PROTECT
#define TELNET_INFO_PATH	"tmp/telnet_info"
#endif

static inline struct passwd *getPasswd(const char *user);

static int check_nologin ( int amroot );

#if defined CONFIG_FEATURE_SECURETTY
static int check_tty ( const char *tty );

#else
static inline int check_tty ( const char *tty )  { return 1; }

#endif

static int is_my_tty ( const char *tty );
static int login_prompt ( char *buf_name, int is_telnet );
static void motd ( void );


static void alarm_handler ( int sig )
{
	fprintf (stderr, "\nLogin timed out after %d seconds.\n", TIMEOUT );
	exit ( EXIT_SUCCESS );
}

#if defined(TCSUPPORT_CT_JOYME2)
static char *fail_path = "/tmp/tlnt_fail_cnt";
static char *lockfile_path = "/tmp/tlnt_lock_t_chk";

static int check_ok(void)
{
	unlink(fail_path);
	unlink(lockfile_path);
	return 0;
}

static int create_lockfile(void)
{
	struct timespec cur_t = {0};
	FILE *fp = NULL;

	/* check exist */
	fp = fopen(lockfile_path, "r");
	if ( fp )
	{
		fclose(fp);
		return 0;
	}

	/* create current time. */
	bzero(&cur_t, sizeof(cur_t));
	clock_gettime(CLOCK_MONOTONIC, &cur_t);
	fp = fopen(lockfile_path, "w");
	if ( fp )
	{
		fprintf(fp, "%u", cur_t.tv_sec);
		fclose(fp);
	}

	unlink(fail_path);

	return 0;
}

static int login_fail(int count)
{
	FILE *fp = NULL;

	fp = fopen(fail_path, "w");
	if ( fp )
	{
		fprintf(fp, "%d", count);
		fclose(fp);

		if ( count >= 3 )
			create_lockfile();
	}

	return 0;
}

static int get_login_count(void)
{
	FILE *fp = NULL;
	char s_count[64] = {0};
	int i_cnt = 0;

	fp = fopen(fail_path, "r");
	if ( fp )
	{
		bzero(s_count, sizeof(s_count));
		i_cnt = fread(s_count, sizeof(char), sizeof(s_count) - 1, fp);
		fclose(fp);

		return atoi(s_count);
	}

	return 0;
}

static int is_login_locked(void)
{
	FILE *fp = NULL;
	struct timespec cur_t = {0};
	unsigned int pre_tv = 0;
	char s_pretime[64] = {0};
	int i_cnt = 0;

	fp = fopen(lockfile_path, "r");
	if ( fp )
	{
		bzero(s_pretime, sizeof(s_pretime));
		i_cnt = fread(s_pretime, sizeof(char), sizeof(s_pretime) - 1, fp);
		fclose(fp);
		sscanf(s_pretime, "%u", &pre_tv);

		bzero(&cur_t, sizeof(cur_t));
		clock_gettime(CLOCK_MONOTONIC, &cur_t);
		if ( cur_t.tv_sec - pre_tv <= 60 )
			return 1;

		unlink(lockfile_path);
	}

	return 0;
}
#endif


#if defined(TCSUPPORT_CT)
int isconsole = 0;
#endif

extern int login_main(int argc, char **argv)
{
#ifdef TCSUPPORT_TFTP_UPGRADE_PROTECT
	FILE *fp = NULL;
	char str[32];
#endif
	char tty[BUFSIZ];
	char full_tty[200];
	char fromhost[512];
	char username[USERNAME_SIZE];
	const char *tmp;
	int amroot;
	int flag;
	int failed;
	int count=0;
	int res = 0;
	struct passwd *pw, pw_copy;
#ifdef CONFIG_WHEEL_GROUP
	struct group *grp;
#endif
	int opt_preserve = 0;
	int opt_fflag = 0;
	char *opt_host = 0;
#if defined(TCSUPPORT_CT)
	char cmdbuf[128] = {0};
	pid_t loginPID = getpid();
#endif
	int is_telnet = 0;


	int alarmstarted = 0;
#ifdef CONFIG_SELINUX
	int flask_enabled = is_flask_enabled();
	security_id_t sid = 0, old_tty_sid, new_tty_sid;
#endif

#ifdef TCSUPPORT_TFTP_UPGRADE_PROTECT
	pid_t pid = getpid();
#endif
	username[0]=0;
	amroot = ( getuid ( ) == 0 );
	signal ( SIGALRM, alarm_handler );
	alarm ( TIMEOUT );
	alarmstarted = 1;

#if defined(TCSUPPORT_CT)
	isconsole = 0;
#endif
#if !defined(TCSUPPORT_ACCOUNT_ACL) 
#if defined(TCSUPPORT_CT)
	while (( flag = getopt(argc, argv, "f:h:p:cdt")) != EOF )
#else
	while (( flag = getopt(argc, argv, "f:h:p")) != EOF )
#endif
#endif
	{
		switch ( flag ) {
		case 'p':
			opt_preserve = 1;
			break;
		case 'f':
			/*
			 * username must be a separate token
			 * (-f root, *NOT* -froot). --marekm
			 */
			if ( optarg != argv[optind-1] )
				bb_show_usage( );

			if ( !amroot ) 		/* Auth bypass only if real UID is zero */
				bb_error_msg_and_die ( "-f permission denied" );

			safe_strncpy(username, optarg, USERNAME_SIZE);
			opt_fflag = 1;
			break;
		case 'h':
			opt_host = optarg;
			break;
#if defined(TCSUPPORT_CT)
		case 'c':
			isconsole = 1;
			break;
		case 't':
			is_telnet = 1;
			break;
		case 'd':
			puts("User already login.");
			return EXIT_FAILURE;
#endif
		default:
			bb_show_usage( );
		}
	}

#if defined(TCSUPPORT_CT_JOYME2)
	if ( 1 == is_telnet )
	{
		count = get_login_count();
		if ( 1 == is_login_locked() )
		{
			puts("Please login again after 60 seconds.");
			return EXIT_FAILURE;
		}
	}
#endif

	if (optind < argc)             // user from command line (getty)
		safe_strncpy(username, argv[optind], USERNAME_SIZE);

	if ( !isatty ( 0 ) || !isatty ( 1 ) || !isatty ( 2 ))
		return EXIT_FAILURE;		/* Must be a terminal */

#ifdef CONFIG_FEATURE_U_W_TMP
	checkutmp ( !amroot );
#endif

	tmp = ttyname ( 0 );
	if ( tmp && ( strncmp ( tmp, "/dev/", 5 ) == 0 ))
		safe_strncpy ( tty, tmp + 5, sizeof( tty ));
	else if ( tmp && *tmp == '/' )
		safe_strncpy ( tty, tmp, sizeof( tty ));
	else
		safe_strncpy ( tty, "UNKNOWN", sizeof( tty ));

#ifdef CONFIG_FEATURE_U_W_TMP
	if ( amroot )
		memset ( utent.ut_host, 0, sizeof utent.ut_host );
#endif

	if ( opt_host ) {
#ifdef CONFIG_FEATURE_U_W_TMP
		safe_strncpy ( utent.ut_host, opt_host, sizeof( utent. ut_host ));
#endif
		snprintf ( fromhost, sizeof( fromhost ) - 1, " on `%.100s' from `%.200s'", tty, opt_host );
	}
	else
		snprintf ( fromhost, sizeof( fromhost ) - 1, " on `%.100s'", tty );

	res = setpgrp();

	openlog ( "login", LOG_PID | LOG_CONS | LOG_NOWAIT, LOG_AUTH );

	while ( 1 ) {
		failed = 0;

		if ( !username[0] )
			if(!login_prompt ( username, is_telnet ))
				return EXIT_FAILURE;

		/*if ( !alarmstarted && ( TIMEOUT > 0 )) {
			alarm ( TIMEOUT );
			alarmstarted = 1;
		}*/
		/*
		if (!( pw = getpwnam ( username ))) {
			pw_copy.pw_name   = "UNKNOWN";
			pw_copy.pw_passwd = "!";
			opt_fflag = 0;
			failed = 1;
		} else
			pw_copy = *pw;
		*/
		/*here_20080220 Load account information from usertty file*/
		if (!( pw = getPasswd ( username ))) {
			pw_copy.pw_name   = "UNKNOWN";
			pw_copy.pw_passwd = "!";
			opt_fflag = 0;
			failed = 1;
		} else
			pw_copy = *pw;
		
		pw = &pw_copy;

		if (( pw-> pw_passwd [0] == '!' ) || ( pw-> pw_passwd[0] == '*' ))
			failed = 1;

		if ( opt_fflag ) {
			opt_fflag = 0;
			goto auth_ok;
		}

		if (!failed && ( pw-> pw_uid == 0 ) && ( !check_tty ( tty )))
			failed = 1;

		/* Don't check the password if password entry is empty (!) */
		if ( !pw-> pw_passwd[0] )
			goto auth_ok;

		/* authorization takes place here */
		if ( correct_password ( pw ))
			goto auth_ok;

		failed = 1;

auth_ok:
		if ( !failed)
			break;

		{ // delay next try
			time_t start, now;

			time ( &start );
			now = start;
			while ( difftime ( now, start ) < FAIL_DELAY) {
				sleep ( FAIL_DELAY );
				time ( &now );
			}
		}

#if !defined(TCSUPPORT_ACCOUNT_ACL) 
#if defined(TCSUPPORT_CT_JOYME4)
		puts("Error!");
#else
		puts("Login incorrect");
#endif
#endif
		username[0] = 0;

#if defined(TCSUPPORT_CT_JOYME2)
		if ( 1 == is_telnet )
		{
			login_fail(count + 1);
			if ( 1 == is_login_locked() )
			{
				syslog ( LOG_WARNING, "invalid password for `%s'%s\n", pw->pw_name, fromhost);
				puts("Please login again after 60 seconds.");
				return EXIT_FAILURE;
			}
		}
#endif


		if (
#if defined(TCSUPPORT_CT_JOYME4)
			++count /* always exit when auth failed. */
#else
			++count == 3
#endif
			)
		{
			syslog ( LOG_WARNING, "invalid password for `%s'%s\n", pw->pw_name, fromhost);
			return EXIT_FAILURE;
		}

	}

	alarm ( 0 );
	if ( check_nologin ( pw-> pw_uid == 0 ))
		return EXIT_FAILURE;

#if defined(TCSUPPORT_CT_JOYME2)
	if ( 1 == is_telnet )
		check_ok();
#endif

#ifdef CONFIG_FEATURE_U_W_TMP
	setutmp ( username, tty );
#endif
#ifdef CONFIG_SELINUX
	if (flask_enabled)
	{
		struct stat st;

		if (get_default_sid(username, 0, &sid))
		{
			fprintf(stderr, "Unable to get SID for %s\n", username);
			exit(1);
		}
		if (stat_secure(tty, &st, &old_tty_sid))
		{
			fprintf(stderr, "stat_secure(%.100s) failed: %.100s\n", tty, strerror(errno));
			return EXIT_FAILURE;
		}
		if (security_change_sid (sid, old_tty_sid, SECCLASS_CHR_FILE, &new_tty_sid) != 0)
		{
			fprintf(stderr, "security_change_sid(%.100s) failed: %.100s\n", tty, strerror(errno));
			return EXIT_FAILURE;
		}
		if(chsid(tty, new_tty_sid) != 0)
		{
			fprintf(stderr, "chsid(%.100s, %d) failed: %.100s\n", tty, new_tty_sid, strerror(errno));
			return EXIT_FAILURE;
		}
	}
	else
		sid = 0;
#endif

	if ( *tty != '/' )
		snprintf ( full_tty, sizeof( full_tty ) - 1, "/dev/%s", tty);
	else
		safe_strncpy ( full_tty, tty, sizeof( full_tty ) - 1 );

	if ( !is_my_tty ( full_tty ))
		syslog ( LOG_ERR, "unable to determine TTY name, got %s\n", full_tty );

	/* Try these, but don't complain if they fail
	 * (for example when the root fs is read only) */
	chown ( full_tty, pw-> pw_uid, pw-> pw_gid );
	res = chmod ( full_tty, 0600 );

	change_identity ( pw );

#if defined(TCSUPPORT_CT_JOYME4)
	if ( 1 == is_telnet ){
		if(pw-> pw_uid == 0)
		{
			syslog ( LOG_WARNING, "root login fail %s\n", fromhost );
			puts("Error!");
			return EXIT_FAILURE;
		}
	}
#endif

	tmp = pw-> pw_shell;
	if(!tmp || !*tmp)
		tmp = DEFAULT_SHELL;
	setup_environment ( tmp, 1, !opt_preserve, pw );

	motd ( );
	signal ( SIGALRM, SIG_DFL );	/* default alarm signal */

	if ( pw-> pw_uid == 0 )
		syslog ( LOG_INFO, "root login %s\n", fromhost );
#ifdef TCSUPPORT_TFTP_UPGRADE_PROTECT
	snprintf(str,sizeof(str), "%s/session%d", TELNET_INFO_PATH, pid);
	if (access(str, F_OK)==0) {
		fp = fopen(str, "w");
		if (fp != NULL) { 
			fputs("1", fp);
			fclose(fp);
		}	
	}	
#endif	
#if defined(TCSUPPORT_CT)
	if ( isconsole )
	{
		// auth ok, record the sh (equal with login) pid
		snprintf(cmdbuf, sizeof(cmdbuf), "echo -n %d > /etc/console.pid", loginPID);
		system(cmdbuf);
	}
#endif

	run_shell ( tmp, 1, 0, 0
#ifdef CONFIG_SELINUX
	, sid
#endif
	 );	/* exec the shell finally. */

	return EXIT_FAILURE;
}


static void show_login_prompt(void)
{
	fputs("Login: ", stdout);
	fflush(stdout);
}


static int login_prompt ( char *buf_name, int is_telnet )
{
	char buf [1024];
	char *sp, *ep;
	int i;

	for(i=0; i<EMPTY_USERNAME_COUNT; i++) {
#if defined(TCSUPPORT_CT_JOYME4)
		if ( is_telnet )
			show_login_prompt();
		else
#endif
		print_login_prompt();

		if ( !fgets ( buf, sizeof( buf ) - 1, stdin ))
			return 0;

		if ( !strchr ( buf, '\n' ))
			return 0;

		for ( sp = buf; isspace ( *sp ); sp++ ) { }
		for ( ep = sp; isgraph ( *ep ); ep++ ) { }

		*ep = 0;
		safe_strncpy(buf_name, sp, USERNAME_SIZE);
		if(buf_name[0])
			return 1;
	}
	return 0;
}


static int check_nologin ( int amroot )
{
	if ( access ( bb_path_nologin_file, F_OK ) == 0 ) {
		FILE *fp;
		int c;

		if (( fp = fopen ( bb_path_nologin_file, "r" ))) {
			while (( c = getc ( fp )) != EOF )
				putchar (( c == '\n' ) ? '\r' : c );

			fflush ( stdout );
			fclose ( fp );
		} else {
			puts ( "\r\nSystem closed for routine maintenance.\r" );
		}
		if ( !amroot )
			return 1;

		puts ( "\r\n[Disconnect bypassed -- root login allowed.]\r" );
	}
	return 0;
}

#ifdef CONFIG_FEATURE_SECURETTY

static int check_tty ( const char *tty )
{
	FILE *fp;
	int i;
	char buf[BUFSIZ];

	if (( fp = fopen ( bb_path_securetty_file, "r" ))) {
		while ( fgets ( buf, sizeof( buf ) - 1, fp )) {
			for ( i = bb_strlen( buf ) - 1; i >= 0; --i ) {
				if ( !isspace ( buf[i] ))
					break;
			}
			buf[++i] = '\0';
			if (( buf [0] == '\0' ) || ( buf [0] == '#' ))
				continue;

			if ( strcmp ( buf, tty ) == 0 ) {
				fclose ( fp );
				return 1;
			}
		}
		fclose(fp);
		return 0;
	}
	/* A missing securetty file is not an error. */
	return 1;
}

#endif

/* returns 1 if true */
static int is_my_tty ( const char *tty )
{
	struct stat by_name, by_fd;

	if ( stat ( tty, &by_name ) || fstat ( 0, &by_fd ))
		return 0;

	if ( by_name. st_rdev != by_fd. st_rdev )
		return 0;
	else
		return 1;
}


static void motd ( )
{
	FILE *fp;
	register int c;

	if (( fp = fopen ( bb_path_motd_file, "r" ))) {
		while (( c = getc ( fp )) != EOF )
			putchar ( c );
		fclose ( fp );
	}
}


#ifdef CONFIG_FEATURE_U_W_TMP
// vv  Taken from tinylogin utmp.c  vv

#define	NO_UTENT \
	"No utmp entry.  You must exec \"login\" from the lowest level \"sh\""
#define	NO_TTY \
	"Unable to determine your tty name."

/*
 * checkutmp - see if utmp file is correct for this process
 *
 *	System V is very picky about the contents of the utmp file
 *	and requires that a slot for the current process exist.
 *	The utmp file is scanned for an entry with the same process
 *	ID.  If no entry exists the process exits with a message.
 *
 *	The "picky" flag is for network and other logins that may
 *	use special flags.  It allows the pid checks to be overridden.
 *	This means that getty should never invoke login with any
 *	command line flags.
 */

static void checkutmp(int picky)
{
	char *line;
	struct utmp *ut;
	pid_t pid = getpid();

	setutent();

	/* First, try to find a valid utmp entry for this process.  */
	while ((ut = getutent()))
		if (ut->ut_pid == pid && ut->ut_line[0] && ut->ut_id[0] &&
			(ut->ut_type == LOGIN_PROCESS || ut->ut_type == USER_PROCESS))
			break;

	/* If there is one, just use it, otherwise create a new one.  */
	if (ut) {
		utent = *ut;
	} else {
		if (picky) {
			puts(NO_UTENT);
			exit(1);
		}
		line = ttyname(0);
		if (!line) {
			puts(NO_TTY);
			exit(1);
		}
		if (strncmp(line, "/dev/", 5) == 0)
			line += 5;
		memset((void *) &utent, 0, sizeof utent);
		utent.ut_type = LOGIN_PROCESS;
		utent.ut_pid = pid;
		strncpy(utent.ut_line, line, sizeof(utent.ut_line) - 1);
		/* XXX - assumes /dev/tty?? */
		strncpy(utent.ut_id, utent.ut_line + 3, sizeof(utent.ut_id) - 1);
		strncpy(utent.ut_user, "LOGIN", sizeof(utent.ut_user) - 1);
		time(&utent.ut_time);
	}
}

/*
 * setutmp - put a USER_PROCESS entry in the utmp file
 *
 *	setutmp changes the type of the current utmp entry to
 *	USER_PROCESS.  the wtmp file will be updated as well.
 */

static void setutmp(const char *name, const char *line)
{
	int fd = 0;
	utent.ut_type = USER_PROCESS;
	strncpy(utent.ut_user, name, sizeof(utent.ut_user) - 1);
	time(&utent.ut_time);
	/* other fields already filled in by checkutmp above */
	setutent();
	pututline(&utent);
	endutent();
	if (access(_PATH_WTMP, R_OK|W_OK) == -1) {
		if((fd = creat(_PATH_WTMP, 0664)) >= 0)
			close(fd);
	}
	updwtmp(_PATH_WTMP, &utent);
}
#endif /* CONFIG_FEATURE_U_W_TMP */

static inline struct passwd *getPasswd(const char *user){
	struct passwd *pwd = NULL;
	FILE *file = NULL;
	char *password = NULL;

#if !defined(TCSUPPORT_ACCOUNT_ACL) 
#if defined(TCSUPPORT_CT)
	if ( isconsole )
		file = fopen(USERDEF_PASSWD_CONF_C, "r");
	else
		file = fopen(USERDEF_PASSWD_CONF, "r");
#else
	file = fopen(USERDEF_PASSWD_CONF, "r");
#endif
#endif
	
	if(file == NULL){
		return NULL;
	}
        /*20081004krammer add
          get a line from /etc/usertty
          and compare with the user name which user
          type in.
        */
	while((pwd = fgetpwent(file))!=0){
		if (!strcmp(user,pwd->pw_name)){
			break;
		}
	}
	fclose(file);
	return pwd;
}
