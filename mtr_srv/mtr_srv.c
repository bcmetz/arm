#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../log/log.h"
#include "../comm/comm.h"
#include "../mtr_if/mtr_if.h"
#include "mtr_srv.h"



int main(int argc, char **argv) {
	key_t keyCmd, keyTlm;	

	int timeout;	
	int shmCmdID, shmTlmID;

	log_t *logMain;

	comm_t *comm;
	commStatus_t comStat;
	
	mtr_t *mtr[4];
	mtrStatus_t mtrStat;

	keyCmd = 0x4848;
	keyTlm = 0x8888;

	mtrCmdIf_t *mtrCmdIf;
	mtrTlmIf_t *mtrTlmIf;
	
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

	if ((shmCmdID = shmget(keyCmd, sizeof(mtrCmdIf_t), IPC_CREAT | 0666)) < 0){
		Log(logMain, ERROR, "Could not get shared memory ID");
		exit(1);
	}

	if ((mtrCmdIf = shmat(shmCmdID, NULL, 0)) == (mtrCmdIf_t *) -1){
		Log(logMain, ERROR, "Could not allocate shared memory");
		exit(1);
	}

	mtrCmdIf->state = IDLE;

	Log(logMain, INFO, "Initializing motors");	
	//Init the motors
	mtr[0] = (mtr_t*)MtrAlloc();
	mtr[1] = (mtr_t*)MtrAlloc();
	mtr[2] = (mtr_t*)MtrAlloc();
	mtr[3] = (mtr_t*)MtrAlloc();

	MtrInit(mtr[0], comm, MTR_AZ, 0x08);
	MtrInit(mtr[1], comm, MTR_EL, 0x18);
	MtrInit(mtr[2], comm, MTR_ELB, 0x48);
	MtrInit(mtr[3], comm, MTR_WRST, 0x38);

	Log(logMain, INFO, "Motor server ready");

	timeout = TIMEOUT;
	while(1) {
		if(mtrCmdIf->state == CLIENT_REQ){
			mtrCmdIf->grantID = mtrCmdIf->reqID;
			mtrCmdIf->state = SRV_GRANT;
			Log(logMain, DIAG, "Grant issued to %d", mtrCmdIf->grantID);
			if(mtrCmdIf->state == RUN_CMD) {
				mtrCmdIf->state = BUSY;
				Log(logMain, DIAG, "Transitioned to BUSY");
				if(mtrCmdIf->mtrID <= MTR_WRST){
					Log(logMain, INFO, "mtrID:%d cmdID:%d data:%d",mtrCmdIf->mtrID, mtrCmdIf->cmdID, mtrCmdIf->data);
					mtrStat = MtrSimpleIf(mtr[mtrCmdIf->mtrID],\
							mtrCmdIf->cmdID,&(mtrCmdIf->data));
					if(mtrStat == MTR_OK) {
						mtrCmdIf->state = DATA_READY;
					}
					else{
						mtrCmdIf->cmdID = -1;
						mtrCmdIf->data = mtrStat;
						mtrCmdIf->state = DATA_READY;
					}
				}
				else{
					mtrCmdIf->cmdID = -1;
					mtrCmdIf->data = 0;
					mtrCmdIf->state = DATA_READY;
				}
				Log(logMain, DIAG, "Transitioned to DATA_READY");
			}
		}
		//The client process has 1ms to move the server on to the 
		//	next state before timeout	
		if(mtrCmdIf->state & (DATA_READY | CLIENT_REQ | SRV_GRANT)){
			timeout--;
			if(timeout == 0){
				Log(logMain,WARNING,"Timeout reached, a process failed to meet timing, state = %d", mtrCmdIf->state);
				mtrCmdIf->state = IDLE;
				timeout = TIMEOUT;
			}
		}
		else{
			timeout = TIMEOUT;
		} 
		usleep(10);
	}	

	shmdt(mtrCmdIf);
	return 0;
}
