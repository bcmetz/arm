
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../log/log.h"
#include "comm.h"
#include "rs232.h"


comm_t* CommAlloc(){
	comm_t *ptr;
	ptr = (comm_t*)malloc(sizeof(comm_t));
	return ptr; 
}
void CommFree(comm_t* self){
	Log(self->log, INFO, "Closing comport %d", self->cport);
	RS232_CloseComport(self->cport);
	Log(self->log, DIAG, "Freeing comm object %p", self); 
	if(self != NULL){
		if(self->log != NULL){
			LogFree(self->log);
		}
		free(self);
	}
}
commStatus_t CommInit(comm_t *self, int cport, int brate, char *mode){
	self->cport = cport;
	self->brate = brate;
	strcpy(self->mode,mode);
	self->state = CLOSED;
	self->log = LogAlloc();

	LogInit(self->log, STDOUT, "COMM");

	Log(self->log, DIAG, "Comm object %p created", self);

	if(RS232_OpenComport(self->cport, self->brate, self->mode)){
		Log(self->log, ERROR, "Cannot open comm port %d", self->cport);
		return COMM_ERR_NO_PORT;
	}

	Log(self->log, INFO, "Opened comm port %d succesfully", self->cport);
	self->state = OPEN;
	return COMM_OK;
}

commStatus_t CommSendData(comm_t *self, uint8_t *buf, uint32_t size) {
	if(self->state == OPEN){
		Log(self->log, DIAG, "Sending %d bytes from %p to comm %p",size,buf,self);	
		RS232_SendBuf(self->cport, buf, size);
	}
	else {
		Log(self->log, WARNING, "Cannot send data, comm not open");
		return COMM_ERR_NO_PORT;	
	}	
	
	return COMM_OK;
}

commStatus_t CommRecvData(comm_t *self, uint8_t *buf, uint32_t size, uint32_t timeout) {
	uint32_t bufSize=0;
	uint32_t time=0;
	if(self->state == OPEN){
		Log(self->log, DIAG, "Reading %d bytes from %p to buf %p",size,self,buf);
		while((bufSize < size) && (time < timeout)) {
				bufSize = RS232_PollComport(self->cport, buf, 4095);
				//1ms sleep
				usleep(1000); 
				time++;
		}
		Log(self->log, DIAG,"bufSize: %d, time: %d",bufSize, time);
		if(time >= timeout){
			Log(self->log, ERROR, "Comm timeout, only recv'd %d/%d bytes in %d ms", bufSize, size, timeout);
			return COMM_ERR_TIMEOUT;
		}		
	}
	else {
		Log(self->log, WARNING, "Cannot recv data, comm not open");
		return COMM_ERR_NO_PORT;	
	}	
	
	return COMM_OK;
}
