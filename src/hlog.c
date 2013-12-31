/*
 *	(c) Heikki Hannikainen, OH7LZB <hessu@hes.iki.fi>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	
 */

/*
 *	hlog.c
 *
 *	logging facility with configurable log levels and
 *	logging destinations
 */

#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "hlog.h"
#include "hmalloc.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

int log_dest = L_DEFDEST;	/* Logging destination */
int log_level = LOG_INFO;	/* Logging level */
int log_facility = LOG_LOCAL1;	/* Logging facility */
char *log_name = NULL;		/* Logging name */

char log_basename[] = "gnuais.log";
char *log_dir = NULL;		/* Access log directory */
char *log_fname = NULL;		/* Access log file name */
int log_file = -1;		/* If logging to a file, the file name */

char accesslog_basename[] = "gnuais.access.log";
char *accesslog_dir = NULL;	/* Access log directory */
char *accesslog_fname = NULL;	/* Access log file name */
int accesslog_file = -1;	/* Access log fd */

char *log_levelnames[] = {
	"EMERG",
	"ALERT",
	"CRIT",
	"ERR",
	"WARNING",
	"NOTICE",
	"INFO",
	"DEBUG",
	NULL
};

char *log_destnames[] = {
	"none",
	"stderr",
	"syslog",
	"file",
	NULL
};


/*
 *	Append a formatted string to a dynamically allocated string
 */

char *str_append(char *s, const char *fmt, ...)
{
	va_list args;
	char buf[LOG_LEN];
	int len;
	char *ret;
	
	va_start(args, fmt);
	vsnprintf(buf, LOG_LEN, fmt, args);
	va_end(args);
	buf[LOG_LEN-1] = 0; /* maybe not necessary... paranoid? */
	
	if (s)
		len = strlen(s);
	else
		len = 0;
		
	ret = hrealloc(s, len + strlen(buf) + 1);
	strcpy(ret + len, buf);
	
	return ret;
}

/*
 *	Pick a log level
 */

int pick_loglevel(char *s, char **names)
{
	int i;
	
	for (i = 0; (names[i]); i++)
		if (!strcasecmp(s, names[i]))
			return i;
			
	return -1;
}

/*
 *	Open log
 */
 
int open_log(char *name, int reopen)
{

	return 0;
}

/*
 *	Close log
 */
 
int close_log(int reopen)
{

	return 0;
}

/*
 *	Log a message
 */

int hlog(int priority, const char *fmt, ...)
{
	va_list args;
	printf(fmt, args);

	return 1;
}


/*
 *	Open access log
 */

int accesslog_open(char *logd, int reopen)
{
	return 0;
}

/*
 *	Close access log
 */
 
int accesslog_close(char *reopenpath)
{
	return 0;
}

/*
 *	Log an access log message
 */

int accesslog(const char *fmt, ...)
{

	return 1;
}

/*
 *	Write my PID to file
 *	FIXME: add flock(TRY) to prevent multiple copies from running
 */

int writepid(char *name)
{

	
	return 1;
}

