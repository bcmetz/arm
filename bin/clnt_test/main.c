#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <zmq.h>

#include "../../log/log.h"
#include "../../comm/comm.h"
#include "../../mtr_if/mtr_if.h"
#include "../mtr_srv/mtr_srv.h"

#define DEG2RAD M_PI/180.0

typedef struct {
	double gearRatio[NUM_MTRS];
	double ctsPerRad[NUM_MTRS];
} mtrParms_t;

mtrParms_t mtrParms;

mtrStatus_t SendCommand(void *req, mtrID_t mtrID, cmdID_t cmd, uint32_t *data){
	int i;
	mtrCmdReq_t mtrCmdReq;
  mtrCmdRep_t mtrCmdRep;
	mtrStatus_t ret; 

	mtrCmdReq.mtrID=mtrID;
	mtrCmdReq.cmdID = cmd;
	mtrCmdReq.data = *data;		
	zmq_send (req, &mtrCmdReq, sizeof(mtrCmdReq_t), 0);	
	zmq_recv (req, &mtrCmdRep, sizeof(mtrCmdRep_t), 0);
	*data = mtrCmdRep.data;
	return mtrCmdRep.ret;
}

int main(int argc, char **argv) {
	int i=0;
	int pos1[NUM_MOTORS], pos2[NUM_MOTORS];	
	uint32_t stat;
	uint32_t mtrStatus=0;
	log_t *logMain;
	mtrCmdReq_t mtrCmdReq; //struct to send to server
	mtrCmdRep_t mtrCmdRep; //data to get back from server
	void *context = zmq_ctx_new ();
	void *req = zmq_socket (context, ZMQ_REQ);

	int32_t pos;
	uint32_t vel, accel;

	mtrParms.gearRatio[MTR_AZ] = 141.0;
	mtrParms.gearRatio[MTR_EL] = 24.0;
	mtrParms.gearRatio[MTR_ELB] = 141.0;
	mtrParms.gearRatio[MTR_WRST] = 141.0;

	mtrParms.ctsPerRad[MTR_AZ] = 64.0/2.0/M_PI;
	mtrParms.ctsPerRad[MTR_EL] = 1960.0/2.0/M_PI;
	mtrParms.ctsPerRad[MTR_ELB] = 64.0/2.0/M_PI;
	mtrParms.ctsPerRad[MTR_WRST] = 64.0/2.0/M_PI;


	//Start logging 
	logMain = LogAlloc();
	LogInit(logMain,STDOUT,"MAIN"); 
	Log(logMain, INFO, "Starting simple i/f test");

	zmq_connect (req, "tcp://localhost:5555");

	for(i=0;i<NUM_MTRS;i++){
		SendCommand(req, i,SET_PID_DISABLE,&stat);
		SendCommand(req, i,SET_MTR_DISABLE,&stat);
		vel = (uint32_t)(20 * DEG2RAD * mtrParms.gearRatio[i] * mtrParms.ctsPerRad[i]);
		SendCommand(req, i,SET_PRO_MAX_VEL,&vel);
		accel = (uint32_t)(80 * DEG2RAD * mtrParms.gearRatio[i] * mtrParms.ctsPerRad[i]);
		SendCommand(req, i,SET_PRO_ACCEL,&accel);
		stat=0;
		SendCommand(req, i,SET_PRO_VEL_FINAL,&stat);
	}


	Log(logMain, INFO, "Place arm in its #1 position, press SPACE to continue");
	while(fgetc(stdin) != ' ');

	for(i=0;i<NUM_MOTORS;i++){	
		SendCommand(req, i,   GET_MTR_POS, &pos1[i]);
		Log(logMain,INFO,"mtr[%d].pos = %d",i,pos1[i]);
	}
	Log(logMain, INFO, "Place arm in its #2 position, press SPACE to continue");
	while(fgetc(stdin) != ' ');
	for(i=0;i<NUM_MOTORS;i++){	
		SendCommand(req, i,   GET_MTR_POS, &pos2[i]);
		Log(logMain,INFO,"mtr[%d].pos = %d",i,pos2[i]);
	}


	for(i=0;i<NUM_MOTORS;i++){	
		SendCommand(req, i,   SET_MTR_ENABLE, &stat);
	}


	for(i=0;i<NUM_MOTORS;i++){	
		SendCommand(req, i,   SET_PID_ENABLE, &stat);
	}
	
	Log(logMain, INFO, "PID enabled, press SPACE to start motion");
	while(fgetc(stdin) != ' ');

	for(i=0;i<NUM_MOTORS;i++){	
		pos = pos1[i];
		SendCommand(req, i,   SET_PRO_LOAD_AND_GO, &pos);
	}

	mtrStatus = 0;	
	for(i=0;i<NUM_MOTORS;i++){	
		SendCommand(req, i,   GET_STATUS, &stat);
		mtrStatus |= (stat & 0x0060);
		usleep(10000);
	}
	
	while(1){
		while(mtrStatus) {
			mtrStatus = 0;	
			for(i=0;i<NUM_MOTORS;i++){	
				SendCommand(req, i,   GET_STATUS, &stat);
				mtrStatus |= stat & 0x0060;
				usleep(10000);
			}
		}
		Log(logMain, INFO, "In position 1");
		for(i=0;i<NUM_MOTORS;i++){	
			pos = pos2[i];
			SendCommand(req, i,   SET_PRO_LOAD_AND_GO, &pos);
			usleep(10000);
		}
		mtrStatus = 0;	
		for(i=0;i<NUM_MOTORS;i++){	
			SendCommand(req, i,   GET_STATUS, &stat);
			mtrStatus |= stat & 0x0060;
			usleep(10000);
		}		
		while(mtrStatus) {
			mtrStatus = 0;	
			for(i=0;i<NUM_MOTORS;i++){	
				SendCommand(req, i,   GET_STATUS, &stat);
				mtrStatus |= stat & 0x0060;
				usleep(10000);
			}
		}
		Log(logMain, INFO, "In position 2");
		for(i=0;i<NUM_MOTORS;i++){	
			pos = pos1[i];
			SendCommand(req, i,   SET_PRO_LOAD_AND_GO, &pos);
			usleep(10000);
		}
		mtrStatus = 0;	
		for(i=0;i<NUM_MOTORS;i++){	
			SendCommand(req, i,   GET_STATUS, &stat);
			mtrStatus |= stat & 0x0060;
			usleep(10000);
		}
	};
	zmq_close (req);
	zmq_ctx_destroy (context);
	return 0;
}
