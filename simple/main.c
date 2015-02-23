
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

	int32_t posAz1, posEl1, posElb1;
	int32_t posAz2, posEl2, posElb2;
	int32_t velAz;
	uint32_t statAz, statEl, statElb, status;
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

	//Turn off the PID
	MtrPidEnable(mtr[0], 0);
//	MtrPidEnable(mtr[1], 0);
//	MtrPidEnable(mtr[2], 0);
	//Disable the motors	
	MtrEnableMotor(mtr[0], 0);
//	MtrEnableMotor(mtr[1], 0);
//	MtrEnableMotor(mtr[2], 0);
	//Set the correct bus voltage
	MtrSetBusVoltage(mtr[0], 11000);	
//	MtrSetBusVoltage(mtr[1], 11000);	
//	MtrSetBusVoltage(mtr[2], 11000);	
	//Set the gains
	MtrSetKp(mtr[0], 60);
//	MtrSetKp(mtr[1], 300);	
//	MtrSetKp(mtr[2], 60);	

	Log(logMain, INFO, "Place arm in its #1 position, press SPACE to continue");
	while(fgetc(stdin) != ' ');
	MtrGetPos(mtr[0], &posAz1);
//	MtrGetPos(mtr[1], &posEl1);
//	MtrGetPos(mtr[2], &posElb1);
	Log(logMain, INFO, "Place arm in its #2 position, press SPACE to continue");
	while(fgetc(stdin) != ' ');
	MtrGetPos(mtr[0], &posAz2);
//	MtrGetPos(mtr[1], &posEl2);
//	MtrGetPos(mtr[2], &posElb2);

	MtrSetAccel(mtr[0], 250);	
//	MtrSetAccel(mtr[1], 250);	
//	MtrSetAccel(mtr[2], 250);	
	MtrSetMaxVel(mtr[0], 1000);	
//	MtrSetMaxVel(mtr[1], 1000);	
//	MtrSetMaxVel(mtr[2], 1000);	
	MtrSetVelFinal(mtr[0], 0);	
//	MtrSetVelFinal(mtr[1], 0);	
//	MtrSetVelFinal(mtr[2], 0);	

	MtrEnableMotor(mtr[0], 1);
//	MtrEnableMotor(mtr[1], 1);
//	MtrEnableMotor(mtr[2], 1);
	MtrPidEnable(mtr[0], 1);
//	MtrPidEnable(mtr[1], 1);
//	MtrPidEnable(mtr[2], 1);

	MtrMoveNow(mtr[0], posAz1);
//	MtrMoveNow(mtr[1], posEl1);
//	MtrMoveNow(mtr[2], posElb1);

	MtrGetStatus(mtr[0], &statAz);	
//	MtrGetStatus(mtr[1], &statEl);	
//	MtrGetStatus(mtr[2], &statElb);	
	status = (statAz/* | statEl | statElb*/) & 0x60;
/*	while(1){
		MtrGetStatus(mtr[0], &statAz);	
//		usleep(100000);
	}	*/
	while(1) {
		//Waiting to get to position 1 
		while(status) {
			MtrGetStatus(mtr[0], &statAz);	
//			MtrGetStatus(mtr[1], &statEl);	
//			MtrGetStatus(mtr[2], &statElb);	
			status = (statAz /*| statEl | statElb*/) & 0x60;
		}	
		Log(logMain, INFO, "In position 1");
		//We got to pos 1, now go to 2	
		MtrMoveNow(mtr[0], posAz2);
//		MtrMoveNow(mtr[1], posEl2);
//		MtrMoveNow(mtr[2], posElb2);
		MtrGetStatus(mtr[0], &statAz);	
//		MtrGetStatus(mtr[1], &statEl);	
//		MtrGetStatus(mtr[2], &statElb);	
		status = (statAz /*| statEl | statElb*/) & 0x60;
		while(status) {
			MtrGetStatus(mtr[0], &statAz);	
//			MtrGetStatus(mtr[1], &statEl);	
//			MtrGetStatus(mtr[2], &statElb);	
			status = (statAz /*| statEl | statElb*/) & 0x60;
		}	
		Log(logMain, INFO, "In position 2");
		MtrMoveNow(mtr[0], posAz1);
//		MtrMoveNow(mtr[1], posEl1);
//		MtrMoveNow(mtr[2], posElb1);
		MtrGetStatus(mtr[0], &statAz);	
//		MtrGetStatus(mtr[1], &statEl);	
//		MtrGetStatus(mtr[2], &statElb);	
		status = (statAz /*| statEl | statElb*/) & 0x60;
	}
	
	Log(logMain, INFO, "Exiting program");

	CommFree(comm);
	MtrFree(mtr[0]);
	MtrFree(mtr[1]);
	MtrFree(mtr[2]);
	return 0;
}

