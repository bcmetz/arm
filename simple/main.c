
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../log/log.h"
#include "../comm/comm.h"
#include "../mtr_if/mtr_if.h"


int main(int argc, char** argv){
	log_t *logMain;
	
	comm_t *comm;
	commStatus_t comStat;

	int32_t pos;
	mtr_t *mtr[3];
	mtrStatus_t mtrStat;

	//Start logging 
	logMain = LogAlloc();
	LogInit(logMain,STDOUT,"MAIN"); 
	Log(logMain, INFO, "Starting simple i/f test");

	//Open the comm port
	comm = CommAlloc();
	comStat = CommInit(comm, 16, 115200, "8N1");
	if(comStat != COMM_OK) {
		Log(logMain, ERROR, "Could not open comm port, shutting down");
		CommFree(comm);
		LogFree(logMain);
		exit(1);
	}
	//Init the motors
	mtr[0] = (mtr_t*)MtrAlloc();
	mtr[1] = (mtr_t*)MtrAlloc();
	mtr[2] = (mtr_t*)MtrAlloc();

	MtrInit(mtr[0], comm, MTR_AZ, 0x08);
	MtrInit(mtr[1], comm, MTR_EL, 0x68);
	MtrInit(mtr[2], comm, MTR_ELB, 0x58);

	MtrGetPos(mtr[0], &pos);
	
	Log(logMain, INFO, "Exiting program");

	return 0;
}

