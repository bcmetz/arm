#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#include "../log/log.h"
#include "../comm/comm.h"
#include "../mtr_if/mtr_if.h"
#include "../mtr_srv/mtr_srv.h"
#include "mtr_clnt.h"

#define TIMEOUT 100

int main(int argc, char **argv) {
	int i=0;
	int pos1[NUM_MOTORS], pos2[NUM_MOTORS];	
	int pos=0;
	uint32_t stat;
	uint32_t mtrStatus=0;
   key_t key;
   mtrCmdIf_t *mtrCmdIf;
	log_t *logMain;

   key = 0x4848;
	
	//Start logging 
	logMain = LogAlloc();
	LogInit(logMain,STDOUT,"MAIN"); 
	Log(logMain, INFO, "Starting simple i/f test");

	MtrClntShmat(&mtrCmdIf, key);
	printf("mtrCmdIf:%p\n", mtrCmdIf);

	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   SET_PID_DISABLE, &stat, TIMEOUT);
	}

	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   SET_MTR_DISABLE, &stat, TIMEOUT);
	}

	for(i=0;i<NUM_MOTORS;i++){	
		stat = 11000;
		MtrClntSendCmd(mtrCmdIf, i,   SET_MTR_BUS_VOLTAGE, &stat, TIMEOUT);
		stat = 11000;
		MtrClntSendCmd(mtrCmdIf, i,   SET_PID_OUT_LIM, &stat, TIMEOUT);
		stat = 4;
		MtrClntSendCmd(mtrCmdIf, i,   SET_PID_GAIN_SHIFT, &stat, TIMEOUT);
		stat = 10000;
		MtrClntSendCmd(mtrCmdIf, i,   SET_PID_INT_LIM, &stat, TIMEOUT);
	}
		
	stat = 500;
	MtrClntSendCmd(mtrCmdIf, MTR_AZ,   SET_PID_KP, &stat, TIMEOUT);
	stat = 1800;
	MtrClntSendCmd(mtrCmdIf, MTR_EL,   SET_PID_KP, &stat, TIMEOUT);
	stat = 600;
	MtrClntSendCmd(mtrCmdIf, MTR_ELB,  SET_PID_KP, &stat, TIMEOUT);
	stat = 500;
	MtrClntSendCmd(mtrCmdIf, MTR_WRST, SET_PID_KP, &stat, TIMEOUT);

	stat = 1;
	MtrClntSendCmd(mtrCmdIf, MTR_AZ,   SET_PID_KI, &stat, TIMEOUT);
	stat = 1;
	MtrClntSendCmd(mtrCmdIf, MTR_EL,   SET_PID_KI, &stat, TIMEOUT);
	stat = 1;
	MtrClntSendCmd(mtrCmdIf, MTR_ELB,  SET_PID_KI, &stat, TIMEOUT);
	stat = 1;
	MtrClntSendCmd(mtrCmdIf, MTR_WRST, SET_PID_KI, &stat, TIMEOUT);

	for(i=0;i<NUM_MOTORS;i++){	
		stat = 250;
		MtrClntSendCmd(mtrCmdIf, i,   SET_PRO_ACCEL, &stat, TIMEOUT);
		stat = 1000;
		MtrClntSendCmd(mtrCmdIf, i,   SET_PRO_MAX_VEL, &stat, TIMEOUT);
		stat = 0;
		MtrClntSendCmd(mtrCmdIf, i,   SET_PRO_VEL_FINAL, &stat, TIMEOUT);
	}
	
	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   GET_MTR_BUS_VOLTAGE, &stat, TIMEOUT);
		Log(logMain,INFO,"mtr[%d].busV = %d",i,stat);
		MtrClntSendCmd(mtrCmdIf, i,   GET_PID_KP, &stat, TIMEOUT);
		Log(logMain,INFO,"pid[%d].kp = %d",i,stat);
		MtrClntSendCmd(mtrCmdIf, i,   GET_PRO_ACCEL, &stat, TIMEOUT);
		Log(logMain,INFO,"pro[%d].accel = %d",i,stat);
		MtrClntSendCmd(mtrCmdIf, i,   GET_PRO_MAX_VEL, &stat, TIMEOUT);
		Log(logMain,INFO,"pro[%d].maxV = %d",i,stat);
		MtrClntSendCmd(mtrCmdIf, i,   GET_PRO_VEL_FINAL, &stat, TIMEOUT);
		Log(logMain,INFO,"pro[%d].vf = %d",i,(int)stat);
	}
	
	Log(logMain, INFO, "Place arm in its #1 position, press SPACE to continue");
	while(fgetc(stdin) != ' ');
	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   GET_MTR_POS, &pos1[i], TIMEOUT);
	}
	for(i=0;i<NUM_MOTORS;i++){
		Log(logMain,INFO,"mtr[%d].pos = %d",i,pos1[i]);
	}
	Log(logMain, INFO, "Place arm in its #2 position, press SPACE to continue");
	while(fgetc(stdin) != ' ');
	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   GET_MTR_POS, &pos2[i], TIMEOUT);
	}


	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   SET_MTR_ENABLE, &stat, TIMEOUT);
	}

	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   SET_PID_ENABLE, &stat, TIMEOUT);
	}
	
	for(i=0;i<NUM_MOTORS;i++){	
		pos = pos1[i];
		MtrClntSendCmd(mtrCmdIf, i,   SET_PRO_LOAD_AND_GO, &pos, TIMEOUT);
	}
	
	mtrStatus = 0;	
	for(i=0;i<NUM_MOTORS;i++){	
		MtrClntSendCmd(mtrCmdIf, i,   GET_STATUS, &stat, TIMEOUT);
		mtrStatus |= (stat & 0x0060);
		usleep(10000);
	}
	
	while(1){
		while(mtrStatus) {
			mtrStatus = 0;	
			for(i=0;i<NUM_MOTORS;i++){	
				MtrClntSendCmd(mtrCmdIf, i,   GET_STATUS, &stat, TIMEOUT);
				mtrStatus |= stat & 0x0060;
				usleep(10000);
			}
//			Log(logMain, INFO, "status %d", mtrStatus);
		}
		Log(logMain, INFO, "In position 1");
		for(i=0;i<NUM_MOTORS;i++){	
			pos = pos2[i];
			MtrClntSendCmd(mtrCmdIf, i,   SET_PRO_LOAD_AND_GO, &pos, TIMEOUT);
			usleep(10000);
		}
		mtrStatus = 0;	
		for(i=0;i<NUM_MOTORS;i++){	
			MtrClntSendCmd(mtrCmdIf, i,   GET_STATUS, &stat, TIMEOUT);
			mtrStatus |= stat & 0x0060;
			usleep(10000);
		}		
		while(mtrStatus) {
			mtrStatus = 0;	
			for(i=0;i<NUM_MOTORS;i++){	
				MtrClntSendCmd(mtrCmdIf, i,   GET_STATUS, &stat, TIMEOUT);
				mtrStatus |= stat & 0x0060;
				usleep(10000);
			}
		}
		Log(logMain, INFO, "In position 2");
		for(i=0;i<NUM_MOTORS;i++){	
			pos = pos1[i];
			MtrClntSendCmd(mtrCmdIf, i,   SET_PRO_LOAD_AND_GO, &pos, TIMEOUT);
			usleep(10000);
		}
		mtrStatus = 0;	
		for(i=0;i<NUM_MOTORS;i++){	
			MtrClntSendCmd(mtrCmdIf, i,   GET_STATUS, &stat, TIMEOUT);
			mtrStatus |= stat & 0x0060;
			usleep(10000);
		}
	};

	return 0;
}
