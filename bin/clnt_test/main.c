#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <zmq.h>

#include "../../log/log.h"
#include "../../comm/comm.h"
#include "../../mtr_if/mtr_if.h"
#include "../mtr_srv/mtr_srv.h"

#define AZ 1
#define EL 2
#define ELB 4
#define WRST 8


typedef struct {
	double gearRatio[NUM_MTRS];
	double ctsPerRad[NUM_MTRS];
} mtrParms_t;

mtrParms_t mtrParms;

void SetupCommand(uint32_t motors, cmdID_t cmd, uint32_t data, mtrCmdRcv_t* mtrCmdRcv){
	int i;
	
	mtrCmdRcv->cmdType = PARAMETER;
	for(i=0;i<NUM_MTRS;i++) {
		if((1<<i)&motors){
			mtrCmdRcv->enabled[i]=TRUE;
			mtrCmdRcv->cmdID[i] = cmd;
			mtrCmdRcv->cmdData[i] = data;		
		}
		else{
			mtrCmdRcv->enabled[i]=FALSE;
		}
	}
}

int main(int argc, char **argv) {
	int i=0;
	int pos1[NUM_MOTORS], pos2[NUM_MOTORS];	
	int pos=0;
	uint32_t stat;
	uint32_t mtrStatus=0;
	log_t *logMain;
	mtrCmdRcv_t mtrCmdRcv; //struct to send to server
	mtrCmdRep_t mtrCmdRep; //data to get back from server
	void *context = zmq_ctx_new ();
	void *requester = zmq_socket (context, ZMQ_REQ);

	//Clear out the stucts;
	memset(&mtrCmdRcv, 0, sizeof(mtrCmdRcv_t));
	memset(&mtrCmdRep, 0, sizeof(mtrCmdRep_t));

	mtrParms.gearRatio[MTR_AZ] = 141.0;
	mtrParms.gearRatio[MTR_EL] = 24.0;
	mtrParms.gearRatio[MTR_ELB] = 141.0;
	mtrParms.gearRatio[MTR_WRST] = 141.0;

	mtrParms.ctsPerRad[MTR_AZ] = 16.0/2.0/M_PI;
	mtrParms.ctsPerRad[MTR_EL] = 1960.0/2.0/M_PI;
	mtrParms.ctsPerRad[MTR_ELB] = 16.0/2.0/M_PI;
	mtrParms.ctsPerRad[MTR_WRST] = 16.0/2.0/M_PI;

	//Start logging 
	logMain = LogAlloc();
	LogInit(logMain,STDOUT,"MAIN"); 
	Log(logMain, INFO, "Starting simple i/f test");

	zmq_connect (requester, "tcp://localhost:5555");


	SetupCommand(AZ|EL|ELB|WRST,SET_PID_DISABLE,0,&mtrCmdRcv);
	zmq_send (requester, &mtrCmdRcv, sizeof(mtrCmdRcv_t), 0);	
	zmq_recv (requester, &mtrCmdRep, sizeof(mtrCmdRep_t), 0);	

	SetupCommand(AZ|EL|ELB|WRST,SET_MTR_DISABLE,0,&mtrCmdRcv);
	zmq_send (requester, &mtrCmdRcv, sizeof(mtrCmdRcv_t), 0);	
	zmq_recv (requester, &mtrCmdRep, sizeof(mtrCmdRep_t), 0);	

	for(i=0;i<NUM_MTRS;i++){
		printf("Reply[%d]:%d\n",i,mtrCmdRep.reply[i]);
	}	

	
	Log(logMain, INFO, "Place arm in its #1 position, press SPACE to continue");
	while(fgetc(stdin) != ' ');


	SetupCommand(AZ|EL|ELB|WRST,SET_MTR_ENABLE,0,&mtrCmdRcv);
	zmq_send (requester, &mtrCmdRcv, sizeof(mtrCmdRcv_t), 0);	
	zmq_recv (requester, &mtrCmdRep, sizeof(mtrCmdRep_t), 0);	
	
	SetupCommand(AZ|EL|ELB|WRST,SET_PID_ENABLE,0,&mtrCmdRcv);
	zmq_send (requester, &mtrCmdRcv, sizeof(mtrCmdRcv_t), 0);	
	zmq_recv (requester, &mtrCmdRep, sizeof(mtrCmdRep_t), 0);	
/*	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   GET_MTR_POS, &pos1[i], TIMEOUT,CLNTID);
		Log(logMain,INFO,"mtr[%d].pos = %d",i,pos1[i]);
	}
	Log(logMain, INFO, "Place arm in its #2 position, press SPACE to continue");
	while(fgetc(stdin) != ' ');
	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   GET_MTR_POS, &pos2[i], TIMEOUT,CLNTID);
		Log(logMain,INFO,"mtr[%d].pos = %d",i,pos2[i]);
	}


	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   SET_MTR_ENABLE, &stat, TIMEOUT,CLNTID);
	}


	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   SET_PID_ENABLE, &stat, TIMEOUT,CLNTID);
	}
	
	Log(logMain, INFO, "PID enabled, press SPACE to start motion");
	while(fgetc(stdin) != ' ');

	for(i=0;i<NUM_MOTORS;i++){	
		pos = pos1[i];
		MtrClntSendCmd(mtrCmdIf, i,   SET_PRO_LOAD_AND_GO, &pos, TIMEOUT,CLNTID);
	}

	mtrStatus = 0;	
	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   GET_STATUS, &stat, TIMEOUT,CLNTID);
		mtrStatus |= (stat & 0x0060);
		usleep(10000);
	}
	
	while(1){
		while(mtrStatus) {
			mtrStatus = 0;	
			for(i=0;i<NUM_MOTORS;i++){	
				MtrClntSendCmd(mtrCmdIf, i,   GET_STATUS, &stat, TIMEOUT,CLNTID);
				mtrStatus |= stat & 0x0060;
				usleep(10000);
			}
		}
		Log(logMain, INFO, "In position 1");
		for(i=0;i<NUM_MOTORS;i++){	
			pos = pos2[i];
			MtrClntSendCmd(mtrCmdIf, i,   SET_PRO_LOAD_AND_GO, &pos, TIMEOUT,CLNTID);
			usleep(10000);
		}
		mtrStatus = 0;	
		for(i=0;i<NUM_MOTORS;i++){	
			MtrClntSendCmd(mtrCmdIf, i,   GET_STATUS, &stat, TIMEOUT,CLNTID);
			mtrStatus |= stat & 0x0060;
			usleep(10000);
		}		
		while(mtrStatus) {
			mtrStatus = 0;	
			for(i=0;i<NUM_MOTORS;i++){	
				MtrClntSendCmd(mtrCmdIf, i,   GET_STATUS, &stat, TIMEOUT,CLNTID);
				mtrStatus |= stat & 0x0060;
				usleep(10000);
			}
		}
		Log(logMain, INFO, "In position 2");
		for(i=0;i<NUM_MOTORS;i++){	
			pos = pos1[i];
			MtrClntSendCmd(mtrCmdIf, i,   SET_PRO_LOAD_AND_GO, &pos, TIMEOUT,CLNTID);
			usleep(10000);
		}
		mtrStatus = 0;	
		for(i=0;i<NUM_MOTORS;i++){	
			MtrClntSendCmd(mtrCmdIf, i,   GET_STATUS, &stat, TIMEOUT,CLNTID);
			mtrStatus |= stat & 0x0060;
			usleep(10000);
		}
	};
*/
	zmq_close (requester);
	zmq_ctx_destroy (context);
	return 0;
}
