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
	
	int shmCmdID, shmTlmID;

	keyCmd = 4848;
	keyTlm = 8888;

	mtrCmdIf_t *mtrCmdIf;
	mtrTlmIf_t *mtrTlmIf;

	if ((shmCmdID = shmget(keyCmd, sizeof(mtrCmdIf_t), IPC_CREAT | 0666)) < 0){
		perror("shmget: mtrCmdIf_t");
		exit(1);
	}

	if ((mtrCmdIf = shmat(shmCmdID, NULL, 0)) == (mtrCmdIf_t *) -1){
		perror("shmat: mtrCmdIf_t");
		exit(1);
	}

	while(1);	

	shmdt(mtrCmdIf);
	return 0;
}
