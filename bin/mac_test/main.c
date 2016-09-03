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



int main(int argc, char **argv) {
	int i=0;
	int pos1[NUM_MOTORS], pos2[NUM_MOTORS];
	int data;	
	uint32_t stat;
	uint32_t mtrStatus=0;
	mtrCmdReq_t mtrCmdReq; //struct to send to server
	mtrCmdRep_t mtrCmdRep; //data to get back from server
	void *context = zmq_ctx_new ();
	void *req = zmq_socket (context, ZMQ_REQ);

	int32_t pos;
	uint32_t vel, accel;

	mtrParms.gearRatio[MTR_WRST] = 1.0;

	mtrParms.ctsPerRad[MTR_WRST] = 2000.0/2.0/M_PI;


	//Start logging 
	printf( "Starting simple i/f test\n");

	zmq_connect (req, "tcp://localhost:5555");

	for(i=0;i<NUM_MTRS;i++){
		SendCommand(req, i,SET_PID_DISABLE,&stat);
		SendCommand(req, i,SET_MTR_DISABLE,&stat);
		vel = (uint32_t)(10000 * DEG2RAD * mtrParms.gearRatio[i] * mtrParms.ctsPerRad[i]);
		SendCommand(req, i,SET_PRO_MAX_VEL,&vel);
		accel = (uint32_t)(10000 * DEG2RAD * mtrParms.gearRatio[i] * mtrParms.ctsPerRad[i]);
		SendCommand(req, i,SET_PRO_ACCEL,&accel);
		stat=0;
		SendCommand(req, i,SET_PRO_VEL_FINAL,&stat);
	}

  data=0;
	SendCommand(req, MTR_WRST, SET_MTR_VOLTAGE, &data);
	
	printf( "About to set gains, press SPACE to continue");
	while(fgetc(stdin) != ' ');

	data=0;
	SendCommand(req, MTR_WRST, SET_MTR_POS, &data);
	data=2;
	SendCommand(req, MTR_WRST, SET_PID_GAIN_SHIFT, &data);
	data=100;
	SendCommand(req, MTR_WRST, SET_PID_KP, &data);
	data=1;
	SendCommand(req, MTR_WRST, SET_PID_KD, &data);

	printf( "About to enable motor, press SPACE to continue");
	while(fgetc(stdin) != ' ');

	SendCommand(req, MTR_WRST, SET_MTR_ENABLE, &data);
	SendCommand(req, MTR_WRST, SET_PID_ENABLE, &data);
	
	printf( "About to go to 100000, press SPACE to continue");
	while(fgetc(stdin) != ' ');
	
	pos = 100000;
	SendCommand(req, MTR_WRST,   SET_PRO_LOAD_AND_GO, &pos);
	
	printf( "About to go to -100000, press SPACE to continue");
	while(fgetc(stdin) != ' ');
	
	pos = -100000;
	SendCommand(req, MTR_WRST,   SET_PRO_LOAD_AND_GO, &pos);

  printf( "About to disable motor, press SPACE to continue");
	while(fgetc(stdin) != ' ');

	SendCommand(req, MTR_WRST, SET_MTR_DISABLE, &data);
	SendCommand(req, MTR_WRST, SET_PID_DISABLE, &data);
/*
	printf( "Place arm in its #1 position, press SPACE to continue");
	while(fgetc(stdin) != ' ');

	for(i=0;i<NUM_MOTORS;i++){	
		SendCommand(req, i,   GET_MTR_POS, &pos1[i]);
		printf("mtr[%d].pos = %d",i,pos1[i]);
	}
	printf( "Place arm in its #2 position, press SPACE to continue");
	while(fgetc(stdin) != ' ');
	for(i=0;i<NUM_MOTORS;i++){	
		SendCommand(req, i,   GET_MTR_POS, &pos2[i]);
		printf("mtr[%d].pos = %d",i,pos2[i]);
	}


	for(i=0;i<NUM_MOTORS;i++){	
		SendCommand(req, i,   SET_MTR_ENABLE, &stat);
	}


	for(i=0;i<NUM_MOTORS;i++){	
		SendCommand(req, i,   SET_PID_ENABLE, &stat);
	}
	
	printf( "PID enabled, press SPACE to start motion");
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
		printf( "In position 1");
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
		printf( "In position 2");
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

  */
	zmq_close (req);
	zmq_ctx_destroy (context);
	return 0;
}
