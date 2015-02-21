// tcslog.h purpose of this file is to provide the functions 
// needed to log all activities taking place in the TCS.
// Goal - ability to select what levels of logging desired (ERROR, WARNING, COMMAND, DIAGNOSTIC)
// and also be able to specify where the log is going (to stdout or a server)

//This non-sense is so I can keep all globals declared in one include file while also defining them when needed
#ifdef DEFINE_VARIABLES
	#define EXTERN /* nothing */
#else
	#define EXTERN extern
#endif /* DEFINE_VARIABLES */


#ifndef _LOG_H_
#define _LOG_H_

//Different levels of logging
typedef enum {ERROR=8, WARNING=4, INFO=2, DIAG=1} logLevel_t;

//Log location
typedef enum {STDOUT=1, STDERR=2} logLoc_t;

typedef struct {
	uint8_t		logName[5];
	logLoc_t	location;
	logLevel_t	levels;		//What levels to record
}log_t;

//Setup what levels to log and where to log

// The input char is for specifying what to LOG
//	Bit 0: Diagnostic
// 	Bit 1: Command
// 	Bit 2: Warning
// 	Bit 3: Error

log_t* LogAlloc(void);
void   LogFree(log_t*);

// The logloc_t specifies where to log
int InitLog(log_t *self, logLoc_t location, char *);

//Specify the module level, the type of log level and the message string
//	Messages should be formated using the printf standard
int Log(log_t*, logLevel_t, char*, ...);

//Grab local unix epoch time
static double GetClockTime(void);

#endif
