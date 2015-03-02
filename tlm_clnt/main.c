#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#include "../log/log.h"
#include "../comm/comm.h"
#include "../mtr_if/mtr_if.h"
#include "../mtr_srv/mtr_srv.h"
#include "tlm_clnt.h"

int main(int argc, char **argv) {
	int i=0;
	int pos=0, vel=0;	
   key_t key;
   mtrCmdIf_t *mtrCmdIf;

   key = 0x4848;

	MtrClntShmat(&mtrCmdIf, key);
	printf("mtrCmdIf:%p\n", mtrCmdIf);

	mtrCmdIf->state = IDLE;
	while(1){
		i++;
		MtrClntSendCmd(mtrCmdIf, MTR_AZ, GET_MTR_POS, &pos, 100);
		MtrClntSendCmd(mtrCmdIf, MTR_AZ, GET_MTR_VEL, &vel, 100);
		fprintf(stdout, "%d %d %d\n",i,pos,vel);
		fflush(stdout);
		usleep(100000);
	};


	return 0;
}
