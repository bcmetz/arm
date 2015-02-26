#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdint.h>


#include "../log/log.h"
#include "../comm/comm.h"
#include "../mtr_if/mtr_if.h"
#include "../mtr_srv/mtr_srv.h"


int main(int argc, char **argv) {
	uint32_t i=0;
   int shmid;
	int pos, vel;	
   key_t key;
   mtrCmdIf_t *mtrCmdIf;

   key = 0x4848;

   if ((shmid = shmget(key, sizeof(mtrCmdIf_t), 0666)) < 0) {
       perror("shmget");
       exit(1);
   }

   if ((mtrCmdIf = shmat(shmid, NULL, 0)) == (mtrCmdIf_t *) -1) {
       perror("shmat");
       exit(1);
   }

	printf("Yay we connected!\n");

	while(1){
		i++;
		if(mtrCmdIf->state == IDLE) {
			mtrCmdIf->state = CLIENT_REQ;
			mtrCmdIf->mtrID = MTR_AZ;
			mtrCmdIf->cmdID = GET_MTR_POS;
			mtrCmdIf->state = RUN_CMD;
			while(mtrCmdIf->state < DATA_READY) {
				usleep(1000);
			}
			pos = (int32_t) mtrCmdIf->data;	
			
			//Grab velocity
			mtrCmdIf->state = CLIENT_REQ;
			mtrCmdIf->mtrID = MTR_AZ;
			mtrCmdIf->cmdID = GET_MTR_VEL;
			mtrCmdIf->state = RUN_CMD;
			while(mtrCmdIf->state < DATA_READY) {
				usleep(1000);
			}
			vel = (int32_t) mtrCmdIf->data;	
			mtrCmdIf->state = IDLE;
		}
		fprintf(stdout, "%d %d %d\n",i,pos,vel);
		fflush(stdout);
		usleep(100000);
	};


	return 0;
}
