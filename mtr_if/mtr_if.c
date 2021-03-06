/*
 * mtrCmd.c
 *
 *  Created on: Feb 19, 2015
 *      Author: metz
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../log/log.h"
#include "../comm/comm.h"
#include "mtr_if.h"

//Private union
typedef union{
	uint8_t 	byte[4];
	int32_t	  	i32;
	uint32_t 	ui32;
}uData_t;

uData_t uData;

const char *mtrDict[1]={
"WRST"
};

const char *cmdDict[NUM_COMMANDS]={
	"CMD_SET_MTR_POS",
	"CMD_SET_MTR_BUS_VOLTAGE",
	"CMD_SET_MTR_VOLTAGE",
	"CMD_SET_MTR_VOLT_LIM",
	"CMD_SET_MTR_VEL_LIM",
	"CMD_SET_MTR_ENC_SIGN",
	"CMD_SET_MTR_HALL_WIRING",
	"CMD_SET_MTR_TYPE",
	"CMD_SET_MTR_ENABLE",
	"CMD_SET_MTR_DISABLE",
	"CMD_GET_MTR_POS",
	"CMD_GET_MTR_VEL",
	"CMD_GET_MTR_BUS_VOLTAGE",
	"CMD_GET_MTR_VOLTAGE",
	"CMD_GET_MTR_VOLT_LIM",
	"CMD_GET_MTR_VEL_LIM",
	"CMD_GET_MTR_ENC_SIGN",
	"CMD_GET_MTR_HALL_WIRING",
	"CMD_GET_MTR_TYPE",
	"CMD_SET_PID_KP",
	"CMD_SET_PID_KI",
	"CMD_SET_PID_KD",
	"CMD_SET_PID_INT_LIM",
	"CMD_SET_PID_OUT_LIM",
	"CMD_SET_PID_GAIN_SHIFT",
	"CMD_SET_PID_ENABLE",
	"CMD_SET_PID_DISABLE",
	"CMD_GET_PID_KP",
	"CMD_GET_PID_KI",
	"CMD_GET_PID_KD",
	"CMD_GET_PID_INT_LIM",
	"CMD_GET_PID_OUT_LIM",
	"CMD_GET_PID_FOLLOWING_ERROR", //Error before gains
	"CMD_GET_PID_OUTPUT",			 //Error after gains
	"CMD_GET_PID_INT_ERROR",
	"CMD_GET_PID_GAIN_SHIFT",
	"CMD_SET_PRO_ACCEL",
	"CMD_SET_PRO_STOP_ACCEL",
	"CMD_SET_PRO_MAX_VEL",
	"CMD_SET_PRO_VEL_FINAL",
	"CMD_SET_PRO_POS_FINAL",
	"CMD_SET_PRO_POS_LIM",
	"CMD_SET_PRO_NEG_LIM",
	"CMD_SET_PRO_LOAD",
	"CMD_SET_PRO_LOAD_AND_GO",
	"CMD_SET_PRO_STOP",		//Come to a stop
	"CMD_SET_PRO_START",		//Start the move
	"CMD_GET_PRO_CMD_POS",
	"CMD_GET_PRO_CMD_VEL",
	"CMD_GET_PRO_CMD_ACCEL",
	"CMD_GET_PRO_ACCEL",
	"CMD_GET_PRO_STOP_ACCEL",
	"CMD_GET_PRO_MAX_VEL",
	"CMD_GET_PRO_VEL_FINAL",
	"CMD_GET_PRO_POS_FINAL",
	"CMD_GET_PRO_POS_LIM",
	"CMD_GET_PRO_NEG_LIM",
	"CMD_GET_STATUS"};

struct stat{
	uint8_t get;
	uint8_t sign;
} stat[]= {
	{0,1}, //	SET_MTR_POS=0,
	{0,0}, //	SET_MTR_BUS_VOLTAGE,
	{0,0}, //	SET_MTR_VOLTAGE,
	{0,0}, //	SET_MTR_VOLT_LIM,
	{0,0}, //	SET_MTR_VEL_LIM,
	{0,1}, //	SET_MTR_ENC_SIGN,
	{0,0}, //	SET_MTR_HALL_WIRING,
	{0,0}, //	SET_MTR_MTR_TYPE,
	{0,0}, //	SET_MTR_ENABLE,
	{0,0}, //	SET_MTR_DISABLE,
	{1,1}, //	GET_MTR_POS,
	{1,1}, //	GET_MTR_VEL,
	{1,0}, //	GET_MTR_BUS_VOLTAGE,
	{1,1}, //	GET_MTR_VOLTAGE,
	{1,0}, //	GET_MTR_VOLT_LIM,
	{1,0}, //	GET_MTR_VEL_LIM,
	{1,1}, //	GET_MTR_ENC_SIGN,
	{1,0}, //	SET_MTR_HALL_WIRING,
	{1,0}, //	SET_MTR_MTR_TYPE,
	{0,0}, //	SET_PID_KP,
	{0,0}, //	SET_PID_KI,
	{0,0}, //	SET_PID_KD,
	{0,0}, //	SET_PID_INT_LIM,
	{0,0}, //	SET_PID_OUT_LIM,
	{0,0}, //	SET_PID_GAIN_SHIFT,
	{1,0}, //	SET_PID_ENABLE,
	{1,0}, //	SET_PID_DISABLE,
	{1,0}, //	GET_PID_KP,
	{1,0}, //	GET_PID_KI,
	{1,0}, //	GET_PID_KD,
	{1,0}, //	GET_PID_INT_LIM,
	{1,0}, //	GET_PID_OUT_LIM,
	{1,1}, //	GET_PID_FOLLOWING_ERROR,
	{1,1}, //	GET_PID_OUTPUT,
	{1,1}, //	GET_PID_INT_ERROR,
	{1,0}, //	GET_PID_GAIN_SHIFT,
	{0,0}, //	SET_PRO_ACCEL,
	{0,0}, //	SET_PRO_STOP_ACCEL,
	{0,0}, //	SET_PRO_MAX_VEL,
	{0,1}, //	SET_PRO_VEL_FINAL,
	{0,1}, //	SET_PRO_POS_FINAL,
	{0,1}, //	SET_PRO_POS_LIM,
	{0,1}, //	SET_PRO_NEG_LIM,
	{0,0}, //	SET_PRO_LOAD,
	{0,1}, //	SET_PRO_LOAD_AND_GO,
	{0,0}, //	SET_PRO_STOP,
	{0,0}, //	SET_PRO_START,
	{1,1}, //	GET_PRO_CMD_POS,
	{1,1}, //	GET_PRO_CMD_VEL,
	{1,1}, //	GET_PRO_CMD_ACCEL,
	{1,0}, //	GET_PRO_ACCEL,
	{1,0}, //	GET_PRO_STOP_ACCEL,
	{1,0}, //	GET_PRO_MAX_VEL,
	{1,1}, //	GET_PRO_VEL_FINAL,
	{1,1}, //	GET_PRO_POS_FINAL,
	{1,1}, //	GET_PRO_POS_LIM,
	{1,1}, //	GET_PRO_NEG_LIM,
	{1,0}, //	GET_STATUS,
};

//This is a private function
mtrStatus_t MtrSendCmd(mtr_t* self, cmdID_t cmd, uint32_t data) {
	commStatus_t commRet;

	Log(self->log, INFO, "Sending command \"%s %s %d\"", mtrDict[self->mtrID], cmdDict[cmd], (int32_t)data);
	self->txBuffer[0] = self->address;
	self->txBuffer[1] = 5;
	self->txBuffer[2] = 4;
	self->txBuffer[3] = (uint8_t)cmd;
	memcpy(&self->txBuffer[4], &data, sizeof(uint8_t)*4);
	Log(self->log, DIAG, "Tx:0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
			self->txBuffer[0],self->txBuffer[1],self->txBuffer[2],self->txBuffer[3],
			self->txBuffer[4],self->txBuffer[5],self->txBuffer[6],self->txBuffer[7]);
	//Send the command out
	commRet = CommSendData(self->comm, self->txBuffer, MTR_TX_MESSAGE);	
	if(commRet == COMM_OK){
	  commRet = CommRecvData(self->comm, self->rxBuffer, MTR_RX_MESSAGE, MTR_CMD_TIMEOUT);
  		if(commRet == COMM_OK) {
			memcpy(&uData, &self->rxBuffer[2], sizeof(uint8_t)*4);
			Log(self->log, DIAG, "Rx:0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
				self->rxBuffer[0],self->rxBuffer[1],self->rxBuffer[2],self->rxBuffer[3],
				self->rxBuffer[4],self->rxBuffer[5]);
			Log(self->log, INFO, "Completed command %s, ret %d", cmdDict[cmd], uData.i32);
			return MTR_OK;
		}
		else if(commRet == COMM_ERR_TIMEOUT) {
			Log(self->log, WARNING, "Error %d, recv timed out", commRet);
			return MTR_ERR_TIMEOUT;
		}	
	}
	Log(self->log, WARNING, "Error %d, could not send data to comm", commRet);
	return MTR_ERR_COMM; 
}

mtr_t* MtrAlloc(void) {
	return (mtr_t*) malloc(sizeof(mtr_t));
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

mtrStatus_t MtrInit(mtr_t* self, comm_t *comm, mtrID_t id, uint8_t addr) {
	int i;

	self->address = addr;

	memset(self->txBuffer, 0, MTR_BUF_SIZE);
	memset(self->rxBuffer, 0, MTR_BUF_SIZE);

	self->comm = comm;

	self->mtrID = id;
	
	self->log = LogAlloc();
	LogInit(self->log, STDOUT, "MTR");
	self->log->levels = ERROR | WARNING | INFO | DIAG;


	Log(self->log, INFO, "Initialized motor (%p), addr:0x%02x", self, self->address);

	return MTR_OK;
}

mtrStatus_t MtrSimpleIf(mtr_t* self, cmdID_t id, uint32_t *data){
	mtrStatus_t ret;

	if(stat[id].get) {
		ret = MtrSendCmd(self, id, 0);
		*data = uData.ui32;
	}
	else {
		ret = MtrSendCmd(self, id, *data);
		if(ret == MTR_OK){
			if(uData.ui32 != 0){
				Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
				ret = uData.ui32;
				*data = uData.ui32;
			}
		}
	}	

	return ret;
}
//Get for the MTR module
mtrStatus_t MtrGetStatus(mtr_t* self, uint32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_STATUS, 0);
	*data = uData.ui32;

	return ret;
}

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

mtrStatus_t MtrGetEncSign(mtr_t* self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_MTR_ENC_SIGN, 0);
	*data = uData.ui32;

	return ret;
}

mtrStatus_t MtrSetPos(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_MTR_POS, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrSetVoltage(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_MTR_VOLTAGE, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrSetBusVoltage(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_MTR_BUS_VOLTAGE, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrSetVoltLimit(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_MTR_VOLT_LIM, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}

mtrStatus_t MtrSetVelLimit(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_MTR_VEL_LIM, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}

mtrStatus_t MtrSetEncSign(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_MTR_VEL_LIM, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrEnableMotor(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	if(data == 1){
		ret = MtrSendCmd(self, SET_MTR_ENABLE, data);
	}
	else {
		ret = MtrSendCmd(self, SET_MTR_DISABLE, data);
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
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrSetKd(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PID_KD, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrSetKi(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PID_KI, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrSetIntLimit(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PID_INT_LIM, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrSetPidOutputLimit(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PID_OUT_LIM, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrSetGainShift(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PID_GAIN_SHIFT, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

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

mtrStatus_t MtrGetPosLim(mtr_t *self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PRO_POS_LIM, 0);
	*data = uData.i32;

	return ret;
}

mtrStatus_t MtrGetNegLim(mtr_t *self, int32_t *data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, GET_PRO_NEG_LIM, 0);
	*data = uData.i32;

	return ret;
}

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
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrSetMaxVel(mtr_t* self, uint32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_MAX_VEL, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrSetPosFinal(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_POS_FINAL, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrSetVelFinal(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_VEL_FINAL, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}

mtrStatus_t MtrSetPosLim(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_POS_LIM, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}

mtrStatus_t MtrSetNegLim(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_NEG_LIM, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}

mtrStatus_t MtrLoadMove(mtr_t* self){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_LOAD, 0);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrStartMove(mtr_t* self){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_START, 0);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}
mtrStatus_t MtrMoveNow(mtr_t* self, int32_t data){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_LOAD_AND_GO, data);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}

mtrStatus_t MtrStopMove(mtr_t* self){
	mtrStatus_t ret;

	ret = MtrSendCmd(self, SET_PRO_STOP, 0);
	if(ret == MTR_OK){
		if(uData.ui32 != 0){
			Log(self->log, WARNING, "Motor(0x%02x) error code: %02x",self->address, uData.ui32);
			ret = uData.ui32;
		}
	}

	return ret;
}


