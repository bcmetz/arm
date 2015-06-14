#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <zmq.h>

#include "../../log/log.h"
#include "../../comm/comm.h"
#include "../../mtr_if/mtr_if.h"
#include "mtr_srv.h"

int ConfigMotors(mtr_t** mtr){

	int i=0;
	uint32_t data;

	for(i=0;i<NUM_MTRS;i++){
  		MtrSimpleIf(mtr[i], SET_PID_DISABLE, &data);
  		MtrSimpleIf(mtr[i], SET_MTR_DISABLE, &data);
	}
	
	for(i=0;i<NUM_MTRS;i++){
		data = 11000;
  		MtrSimpleIf(mtr[i], SET_MTR_BUS_VOLTAGE, &data);
		data = 11000;
  		MtrSimpleIf(mtr[i], SET_PID_OUT_LIM, &data);
		data = 4;
  		MtrSimpleIf(mtr[i], SET_PID_GAIN_SHIFT, &data);
		data = 10000;
  		MtrSimpleIf(mtr[i], SET_PID_INT_LIM, &data);
		data = -1;
  		MtrSimpleIf(mtr[i], SET_MTR_ENC_SIGN, &data);
	}

	data = 500;
	MtrSimpleIf(mtr[MTR_AZ], SET_PID_KP, &data);
	data = 1000;
	MtrSimpleIf(mtr[MTR_EL], SET_PID_KP, &data);
	data = 600;
	MtrSimpleIf(mtr[MTR_ELB], SET_PID_KP, &data);
	data = 500;
	MtrSimpleIf(mtr[MTR_WRST], SET_PID_KP, &data);
	
	data = 1;
	MtrSimpleIf(mtr[MTR_AZ], SET_PID_KI, &data);
	data = 0;
	MtrSimpleIf(mtr[MTR_EL], SET_PID_KI, &data);
	data = 1;
	MtrSimpleIf(mtr[MTR_ELB], SET_PID_KI, &data);
	data = 1;
	MtrSimpleIf(mtr[MTR_WRST], SET_PID_KI, &data);
}

int main(int argc, char **argv) {
	int i;
	log_t *logMain;

	comm_t *comm;
	commStatus_t comStat;
	
	mtr_t *mtr[4];
	mtrStatus_t mtrStat;

	mtrCmdReq_t mtrCmdReq;
	mtrCmdRep_t mtrCmdRep;
	
	void *context = zmq_ctx_new ();
	void *responder = zmq_socket (context, ZMQ_REP);
	int rc = zmq_bind (responder, "tcp://*:5555");
	assert (rc == 0);
	
	logMain = LogAlloc();
	LogInit(logMain,STDOUT,"SERV"); 
	Log(logMain, INFO, "Starting motor interface server");

	//Open the comm port
	comm = CommAlloc();
	comStat = CommInit(comm, 16, 115200, "8N1");
	if(comStat != COMM_OK) {
		Log(logMain, ERROR, "Could not open comm port, shutting down");
		CommFree(comm);
		LogFree(logMain);
		exit(1);
	}

	Log(logMain, INFO, "Initializing motors");	
	//Init the motors
	mtr[0] = (mtr_t*)MtrAlloc();
	mtr[1] = (mtr_t*)MtrAlloc();
	mtr[2] = (mtr_t*)MtrAlloc();
	mtr[3] = (mtr_t*)MtrAlloc();

	MtrInit(mtr[0], comm, MTR_AZ, 0x08);
	MtrInit(mtr[1], comm, MTR_EL, 0x48);
	MtrInit(mtr[2], comm, MTR_ELB, 0x18);
	MtrInit(mtr[3], comm, MTR_WRST, 0x38);

	ConfigMotors(mtr);

	Log(logMain, INFO, "Motor server ready");

	while(1) {
		zmq_recv (responder, &mtrCmdReq, sizeof(mtrCmdReq_t), 0);
		Log(logMain, INFO, "Received new command");
		mtrCmdRep.ret = MtrSimpleIf(mtr[mtrCmdReq.mtrID], mtrCmdReq.cmdID, &mtrCmdReq.data);
		mtrCmdRep.data = mtrCmdReq.data;
		zmq_send (responder, &mtrCmdRep, sizeof(mtrCmdRep_t), 0);
	}	

	return 0;
}
