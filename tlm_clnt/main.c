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
	int posAz=0, velAz=0;	
	int posEl=0, velEl=0;	
	int posElb=0, velElb=0;	
	int posWrst=0, velWrst=0;	
   key_t key;
   mtrCmdIf_t *mtrCmdIf;

   key = 0x4848;

	MtrClntShmat(&mtrCmdIf, key);
	printf("mtrCmdIf:%p\n", mtrCmdIf);

	mtrCmdIf->state = IDLE;
	while(1){
		i++;
		MtrClntSendCmd(mtrCmdIf, MTR_AZ, GET_MTR_POS, &posAz, 100);
		MtrClntSendCmd(mtrCmdIf, MTR_AZ, GET_MTR_VEL, &velAz, 100);
		MtrClntSendCmd(mtrCmdIf, MTR_EL, GET_MTR_POS, &posEl, 100);
		MtrClntSendCmd(mtrCmdIf, MTR_EL, GET_MTR_VEL, &velEl, 100);
		MtrClntSendCmd(mtrCmdIf, MTR_ELB, GET_MTR_POS, &posElb, 100);
		MtrClntSendCmd(mtrCmdIf, MTR_ELB, GET_MTR_VEL, &velElb, 100);
		MtrClntSendCmd(mtrCmdIf, MTR_WRST, GET_MTR_POS, &posWrst, 100);
		MtrClntSendCmd(mtrCmdIf, MTR_WRST, GET_MTR_VEL, &velWrst, 100);
		fprintf(stdout, "%d %d %d %d %d %d %d %d %d\n",i,posAz,velAz,posEl,velEl,posElb,velElb,posWrst,velWrst);
		fflush(stdout);
		usleep(100000);
	};


	return 0;
}
