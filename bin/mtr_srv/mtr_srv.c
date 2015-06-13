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



int main(int argc, char **argv) {
	int i;
	log_t *logMain;

	comm_t *comm;
	commStatus_t comStat;
	
	mtr_t *mtr[4];
	mtrStatus_t mtrStat;

	mtrCmdRcv_t mtrCmdRcv;
	mtrCmdRep_t mtrCmdRep;
	mtrParms_t mtrParms;
	
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

	mtrParms.gearRatio[MTR_AZ] = 141;
	mtrParms.gearRatio[MTR_EL] = 26;
	mtrParms.gearRatio[MTR_ELB] = 141;
	mtrParms.gearRatio[MTR_WRST] = 141;

	mtrParms.ctsPerRad[MTR_AZ] = 14/2/M_PI;
	mtrParms.ctsPerRad[MTR_EL] = 2000/2/M_PI;
	mtrParms.ctsPerRad[MTR_ELB] = 14/2/M_PI;
	mtrParms.ctsPerRad[MTR_WRST] = 14/2/M_PI;

	Log(logMain, INFO, "Motor server ready");

	while(1) {
		
		zmq_recv (responder, &mtrCmdRcv, sizeof(mtrCmdRcv_t), 0);
		Log(logMain, INFO, "Received new command");
		switch(mtrCmdRcv.cmdType){
			case MOTION: {
				for(i=0; i<NUM_MTRS; i++){
					if(mtrCmdRcv.enabled[i]){
						mtrCmdRcv.cmdData[i] = (int32_t)(mtrCmdRcv.move[i].pos * \
							mtrParms.gearRatio[i] * mtrParms.ctsPerRad[i]);
						Log(logMain, DIAG, "Pos: %f [rad]  %f [cts]",
							mtrCmdRcv.move[i].pos, mtrCmdRcv.cmdData[i]);
						mtrCmdRep.reply[i] = MtrSimpleIf(mtr[i], SET_PRO_POS_FINAL,
							&mtrCmdRcv.cmdData[i]);
						mtrCmdRcv.cmdData[i] = (uint32_t)(mtrCmdRcv.move[i].velMax * \
							mtrParms.gearRatio[i] * mtrParms.ctsPerRad[i]);
						Log(logMain, DIAG, "Vel: %f [rad/s]  %f [cts/s]",
							mtrCmdRcv.move[i].velMax, mtrCmdRcv.cmdData[i]);
						mtrCmdRep.reply[i] |= MtrSimpleIf(mtr[i], SET_PRO_MAX_VEL,
							&mtrCmdRcv.cmdData[i]);

						mtrCmdRcv.cmdData[i] = (int32_t)(mtrCmdRcv.move[i].velFinal * \
							mtrParms.gearRatio[i] * mtrParms.ctsPerRad[i]);
						mtrCmdRep.reply[i] |= MtrSimpleIf(mtr[i], SET_PRO_VEL_FINAL,
							&mtrCmdRcv.cmdData[i]);

						mtrCmdRcv.cmdData[i] = (uint32_t)(mtrCmdRcv.move[i].accel * \
							mtrParms.gearRatio[i] * mtrParms.ctsPerRad[i]);
						mtrCmdRep.reply[i] |= MtrSimpleIf(mtr[i], SET_PRO_ACCEL,
							&mtrCmdRcv.cmdData[i]);
					}
				} 
				for(i=0; i<NUM_MTRS; i++){
					if(mtrCmdRcv.enabled[i] && mtrCmdRep.reply[i] == MTR_OK){
						mtrCmdRep.reply[i] = MtrSimpleIf(mtr[i], SET_PRO_START,\
							&mtrCmdRcv.cmdData[i]);
					}
				} break;	
			}
			case PARAMETER: {
				for(i=0; i<NUM_MTRS; i++){
					if(mtrCmdRcv.enabled[i]) {
						mtrCmdRep.reply[i] = MtrSimpleIf(mtr[i], mtrCmdRcv.cmdID[i],\
							&mtrCmdRcv.cmdData[i]);
					}
				} break;
			}
			default: {
				Log(logMain, ERROR, "Unknown command type %d", mtrCmdRcv.cmdType);
			}
		}
		zmq_send (responder, &mtrCmdRep, sizeof(mtrCmdRep_t), 0);
	}	

	return 0;
}
