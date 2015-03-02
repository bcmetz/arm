#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#include "../log/log.h"
#include "../comm/comm.h"
#include "../mtr_if/mtr_if.h"
#include "../mtr_srv/mtr_srv.h"

log_t logClnt;

int MtrClntShmat(mtrCmdIf_t** self, key_t key) {
   int shmid;

	LogInit(&logClnt, STDOUT, "CLNT");

	Log(&logClnt, INFO, "Attaching to shm key 0x%04x", (uint32_t)key);

   if ((shmid = shmget(key, sizeof(mtrCmdIf_t), 0666)) < 0) {
		Log(&logClnt, ERROR, "Could not get shmid, exiting!"); 
      exit(1);
   }

   if ((*self = shmat(shmid, NULL, 0)) == (mtrCmdIf_t *) -1) {
		Log(&logClnt, ERROR, "Could not attach to shmid: %d, exiting!",shmid); 
      exit(1);
   }

	Log(&logClnt, INFO, "Attached to shared memory");
	
	return 0;
}

int MtrClntSendCmd(mtrCmdIf_t* self, mtrID_t mtr, cmdID_t cmd, uint32_t *data, const uint32_t timeout){ 

	uint32_t i=0;

	//Sleeping for 10us * 100 = 1ms
	while(i < 100*timeout){
		if(self->state == IDLE) {
			self->state = CLIENT_REQ;
			self->mtrID = mtr;
			self->cmdID = cmd;
			self->state = RUN_CMD;
			i=0;
			//Wait for server to update the flag
			while(self->state < DATA_READY) {
				usleep(100);
				i++;
				if(i > 100*timeout) {
					Log(&logClnt, WARNING, "Timed out waiting for DATA_READY");
					return -1;
				}
			}
			if(self->cmdID == cmd && \
					self->mtrID == mtr){
				*data = self->data;	
				self->state = IDLE;
				return 0;
			}
			else {
				Log(&logClnt, WARNING, "CmdID or mtrID mismatch, cmd client:%d / cmd serv %d, mtr client %d / mtr serv %d", cmd, self->cmdID, mtr, self->mtrID);
				return -2;
			}
		}
		
		i++;
		usleep(100);
	};

	Log(&logClnt, WARNING, "Timed out waiting for IDLE state");
	return -1;
}
