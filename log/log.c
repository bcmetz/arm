#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>

#include "log.h"						//Logging methods

#ifdef _OS_X_
#include <mach/mach_time.h>
#define CLOCK_REALTIME 0
#define ORWL_NANO (+1.0E-9)
#define ORWL_GIGA UINT64_C(1000000000)

static double orwl_timebase = 0.0;
static uint64_t orwl_timestart = 0;

int clock_gettime(uint32_t i, struct timespec* t) {
  // be more careful in a multithreaded environement
  if (!orwl_timestart) {
    mach_timebase_info_data_t tb = { 0 };
    mach_timebase_info(&tb);
    orwl_timebase = tb.numer;
    orwl_timebase /= tb.denom;
    orwl_timestart = mach_absolute_time();
  }
  double diff = (mach_absolute_time() - orwl_timestart) * orwl_timebase;
  t->tv_sec = diff * ORWL_NANO;
  t->tv_nsec = diff - (t->tv_sec * ORWL_GIGA);

  return 1;
}
#endif


//Private self log
log_t selfLog ={
	.logName = "LOG",
	.location = STDOUT,
	.levels = ERROR | WARNING | INFO | DIAG};

log_t *LogAlloc(void){
	log_t *pLog;
	pLog = (log_t*)malloc(sizeof(log_t));
	return pLog;
}

void LogFree(log_t* self){
	Log(&selfLog, DIAG, (char*)"Freeing %p", self);
	if(self != NULL) {
		free(self);
	}
}

int LogInit(log_t *self, logLoc_t location, char *name) {

	int flags;

	strcpy(self->logName,name);

	//If there is a config file then the hardcoded value is over written	
	if(location == STDOUT) {
		self->location = STDOUT;
		setvbuf(stdout,NULL,_IOLBF,0);
		if ((flags = fcntl(fileno(stdout), F_GETFL)) == -1 ||
			fcntl(fileno(stdout), F_SETFL, flags|O_NONBLOCK) == -1) {
			fprintf(stderr, "unable to set stdout to be non-blocking");
		}
	}
	else if(location == STDERR) {
		self->location = STDERR;
		setvbuf(stderr,NULL,_IOLBF,0);
		if ((flags = fcntl(fileno(stderr), F_GETFL)) == -1 ||
			fcntl(fileno(stderr), F_SETFL, flags|O_NONBLOCK) == -1) {
			fprintf(stderr, "unable to set stderr to be non-blocking");
		}
	}

	self->levels = ERROR | WARNING | INFO | DIAG;

	Log(&selfLog, DIAG, (char*)"Logging Initialized (%p)", self);

	return 0;
}

int Log(log_t *self, logLevel_t level, char* message, ...) {
	char *logModuleString;
	char *logTypeString;
	char logFormattedString[2048];
	char logMessage[2072];
	double seconds;
	va_list argList;

	if(!(self->levels & level)){
		//User doesn't want to report this message level
		return 1;
	}

	//Take the message and format it correctly
	va_start(argList, message);
	vsnprintf(logFormattedString, 2048, message, argList);
	va_end(argList);

	switch(level) {
		case ERROR: {
			logTypeString = (char*)"ERROR";
			break;
		}
		case WARNING: {
			logTypeString = (char*)"WARNING";
			break;
		}
		case INFO: {
			logTypeString = (char*)"INFO";
			break;
		}
		case DIAG: {
			logTypeString = (char*)"DIAG";
			break;
		}
		default: {
			logTypeString = (char*)"UNKNOWN";
			break;
		}
	}

	if(self->location & STDERR) {
		seconds = GetClockTime(); 
		snprintf(logMessage,2072, "%.5f\t%s\t%s\t%s\n",
			seconds, self->logName, logTypeString, logFormattedString);	
		fprintf(stderr, "%s", logMessage);
		fflush(stderr);
	}
	if(self->location & STDOUT) {
		seconds = GetClockTime(); 
		snprintf(logMessage,2072, "%.5f\t%s\t%s\t%s\n",
			seconds, self->logName, logTypeString, logFormattedString);	
		fprintf(stdout, "%s", logMessage);
		fflush(stdout);
	}

	return 0;
}

static double GetClockTime(void) {
	struct timespec ts;

	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		Log(&selfLog, ERROR,
		(char*)"[%s:%d] Unable to get clock timestamp", 
		__FILE__, __LINE__);
		return 0;
	}
	return (double)(ts.tv_sec + ts.tv_nsec / 1000000000.0);
}
