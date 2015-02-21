/*
 * mtrCmd.c
 *
 *  Created on: Feb 19, 2015
 *      Author: metz
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../log/log.h"
#include "mtr_status.h"
#include "mtr_if.h"

//Private union
typedef union{
	uint8_t 	byte[4];
	int32_t	  	i32;
	uint32_t 	ui32;
}uData_t;

uData_t uData;


//This is a private function
mtrStatus_t MtrSendCmd(mtr_t* self, cmdID_t cmd, uint32_t data) {

	self->txBuffer[0] = self->address;
	self->txBuffer[1] = 5;
	self->txBuffer[2] = 4;
	self->txBuffer[3] = (uint8_t)cmd;
	memcpy(&self->txBuffer[4], &data, sizeof(uint8_t)*4);
	Log(self->log, DIAG, "Tx:0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
			self->txBuffer[0],self->txBuffer[1],self->txBuffer[2],self->txBuffer[3],
			self->txBuffer[4],self->txBuffer[5],self->txBuffer[6],self->txBuffer[7]);
	//SEND DATA
	memcpy(&uData, &self->rxBuffer[2], sizeof(uint8_t)*4);
	Log(self->log, DIAG, "Rx:0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
			self->rxBuffer[0],self->rxBuffer[1],self->rxBuffer[2],self->rxBuffer[3],
			self->rxBuffer[4],self->rxBuffer[5]);
	return MTR_OK;
}


mtr_t* MtrAlloc(void) {
	return (mtr_t*) malloc(sizeof(mtr_t*));
}
void MtrFree(mtr_t* self){
	Log(self->log, INFO, "Freeing motor (%p)", self);

	if(self != NULL) {
		if(self->log != NULL) {
			LogFree(self->log);
		}
		free(self);
	}
}

mtrStatus_t MtrInit(mtr_t* self, mtrID_t id, uint8_t addr) {
	int i;

	self->address = addr;

	for(i=0;i<MTR_BUF_SIZE;i++){
		self->rxBuffer[i] = 0;
		self->txBuffer[i] = 0;
	}

	self->log = LogAlloc();
	InitLog(self->log, STDOUT, "MTR");
	self->log->levels = ERROR | WARNING | INFO | DIAG;


	Log(self->log, INFO, "Initialized motor (%p), addr:%d", self, self->address);

	return MTR_OK;
}
//Get for the MTR module
mtrStatus_t MtrGetPos(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_MTR_POS, 0);
	*data = uData.i32;

	return ret;
}

mtrStatus_t MtrGetVel(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_MTR_VEL, 0);
	*data = uData.i32;

	return ret;
}
mtrStatus_t MtrGetVoltage(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_MTR_VOLTAGE, 0);
	*data = uData.i32;

	return ret;
}
mtrStatus_t MtrGetBusVoltage(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_MTR_BUS_VOLTAGE, 0);
	*data = uData.ui32;

	return ret;
}
mtrStatus_t MtrGetVoltLimit(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_MTR_VOLT_LIM, 0);
	*data = uData.ui32;

	return ret;
}
mtrStatus_t MtrGetVelLimit(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_MTR_VEL_LIM, 0);
	*data = uData.ui32;

	return ret;
}

mtrStatus_t MtrSetPos(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_MTR_POS, data);

	return ret;
}
mtrStatus_t MtrSetVoltage(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_MTR_VOLTAGE, data);

	return ret;
}
mtrStatus_t MtrSetBusVoltage(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_MTR_BUS_VOLTAGE, data);

	return ret;
}
mtrStatus_t MtrSetVoltLimit(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_MTR_VOLT_LIM, data);

	return ret;
}
mtrStatus_t MtrSetVelLimit(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_MTR_VEL_LIM, data);

	return ret;
}

mtrStatus_t MtrEnableMotor(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	if(data == 1){
		ret = MtrSendCmd(self, SET_MTR_ENABLE, data);
	}
	else {
		ret = MtrSendcmd(self, SET_MTR_DISABLE, data);
	}

	return ret;
}

mtrStatus_t MtrGetKp(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PID_KP, 0);
	*data = uData.ui32;

	return ret;
}
mtrStatus_t MtrGetKd(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PID_KD, 0);
	*data = uData.ui32;

	return ret;
}
mtrStatus_t MtrGetKi(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PID_KI, 0);
	*data = uData.ui32;

	return ret;
}
mtrStatus_t MtrGetIntLimit(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PID_INT_LIM, 0);
	*data = uData.ui32;

	return ret;
}
mtrStatus_t MtrGetPidOutputLimit(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PID_OUT_LIM, 0);
	*data = uData.ui32;

	return ret;
}
mtrStatus_t MtrGetFollowingError(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PID_FOLLOWING_ERROR, 0);
	*data = uData.i32;

	return ret;
}
mtrStatus_t MtrGetPidOutput(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PID_OUTPUT, 0);
	*data = uData.i32;

	return ret;
}
mtrStatus_t MtrGetIntError(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PID_INT_ERROR, 0);
	*data = uData.i32;

	return ret;
}
mtrStatus_t MtrGetGainShift(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PID_GAIN_SHIFT, 0);
	*data = uData.ui32;

	return ret;
}

mtrStatus_t MtrSetKp(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PID_KP, data);

	return ret;
}
mtrStatus_t MtrSetKd(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PID_KD, data);

	return ret;
}
mtrStatus_t MtrSetKi(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PID_KI, data);

	return ret;
}
mtrStatus_t MtrSetIntLimit(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PID_INT_LIM, data);

	return ret;
}
mtrStatus_t MtrSetPidOutputLimit(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PID_OUT_LIM, data);

	return ret;
}
mtrStatus_t MtrSetGainShift(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PID_GAIN_SHIFT, data);

	return ret;
}

mtrStatus_t MtrPidEnable(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	if(data == 1) {
		ret = MtrSendCmd(self, SET_PID_ENABLE, 0);
	}
	else {
		ret = MtrSendCmd(self, SET_PID_DISABLE, 0);
	}

	return ret;
}

mtrStatus_t MtrGetAccel(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PRO_ACCEL, 0);
	*data = uData.ui32;

	return ret;
}
mtrStatus_t MtrGetMaxVel(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PRO_MAX_VEL, 0);
	*data = uData.ui32;

	return ret;
}
mtrStatus_t MtrGetPosFinal(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PRO_POS_FINAL, 0);
	*data = uData.i32;

	return ret;
}
mtrStatus_t MtrGetVelFinal(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PRO_VEL_FINAL, 0);
	*data = uData.i32;

	return ret;
}
//////////ADD POS/NEG LIMITS/////////////////

mtrStatus_t MtrGetCmdPos(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PRO_CMD_POS, 0);
	*data = uData.i32;

	return ret;
}
mtrStatus_t MtrGetCmdVel(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PRO_CMD_VEL, 0);
	*data = uData.i32;

	return ret;
}
mtrStatus_t MtrGetCmdAccel(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PRO_CMD_ACCEL, 0);
	*data = uData.i32;

	return ret;
}

mtrStatus_t MtrSetAccel(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_ACCEL, data);

	return ret;
}
mtrStatus_t MtrSetMaxVel(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_MAX_VEL, data);

	return ret;
}
mtrStatus_t MtrSetPosFinal(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_POS_FINAL, data);

	return ret;
}
mtrStatus_t MtrSetVelFinal(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_VEL_FINAL, data);

	return ret;
}

mtrStatus_t MtrLoadMove(mtr_t* self){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_LOAD, 0);

	return ret;
}
mtrStatus_t MtrStartMove(mtr_t* self){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_START, 0);

	return ret;
}
mtrStatus_t MtrMoveNow(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_LOAD_AND_GO, data);

	return ret;
}

mtrStatus_t MtrStopMove(mtr_t* self){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_STOP, 0);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}


