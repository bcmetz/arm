#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <zmq.h>
#include "../log/log.h"
#include "../comm/comm.h"
#include "../mtr_if/mtr_if.h"
#include "../bin/mtr_srv/mtr_srv.h"

//Client interface to server
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
