#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <zmq.h>
#include "../i2c_zmq/mtr_srv.h"
#include "Motor.h"


Motor::Motor(void* req, uint8_t id) {
	_id = id; 
	_mtrSet = 0;
	_gainSet = 0;
	_trapSet = 0;
	_req = req;
}

//Motor position
int32_t Motor::getPosition() {
	return (int32_t) SendCommand(_id, CMD_GET_MTR_POS, 0); 
}
uint32_t Motor::setPosition(int32_t data) {
	return (uint32_t) SendCommand(_id, CMD_SET_MTR_POS, data); 
}

//Motor Velocity
int32_t Motor::getVelocity() {
	return (int32_t) SendCommand(_id, CMD_GET_MTR_VEL, 0); 
}

// Bus Voltage 
uint32_t Motor::getBusVoltage() {
	return (uint32_t) SendCommand(_id, CMD_GET_MTR_BUS_VOLTAGE, 0); 
}
uint32_t Motor::setBusVoltage(int32_t data) {
	return (uint32_t) SendCommand(_id, CMD_SET_MTR_BUS_VOLTAGE, data); 
}

// Voltage 
int32_t Motor::getMtrVoltage() {
	return (int32_t) SendCommand(_id, CMD_GET_MTR_VOLTAGE, 0); 
}
uint32_t Motor::setMtrVoltage(int32_t data) {
	return (uint32_t) SendCommand(_id, CMD_SET_MTR_VOLTAGE, data); 
}

// Voltage Limit 
uint32_t Motor::getMtrVoltageLimit() {
	return (uint32_t) SendCommand(_id, CMD_GET_MTR_VOLT_LIM, 0); 
}
uint32_t Motor::setMtrVoltageLimit(uint32_t data) {
	return (uint32_t) SendCommand(_id, CMD_SET_MTR_VOLT_LIM, data); 
}


// Setup Motor Brushed
uint32_t Motor::setupBrushedMotor(uint32_t busVoltage, uint32_t maxVoltage, feedback_t polarity) {
	uint32_t ret;

	ret = SendCommand(_id, CMD_SET_MTR_TYPE, (uint32_t) BRUSHED);
	ret |= SendCommand(_id, CMD_SET_MTR_BUS_VOLTAGE, busVoltage);
	ret |= SendCommand(_id, CMD_SET_MTR_VOLT_LIM, maxVoltage);
	ret |= SendCommand(_id, CMD_SET_MTR_ENC_SIGN, (uint32_t) polarity);
	if(ret) {
			printf("Error setting up motor, ret:%d\n",ret);
		return ret;
	}
	_mtrSet = 1;
	return ret;
}

// Setup Motor Brushless
uint32_t Motor::setupThreePhaseMotor(uint32_t busVoltage, uint32_t maxVoltage, hallWiring_t hallWiring, feedback_t polarity) {
	uint32_t ret;
	ret = SendCommand(_id, CMD_SET_MTR_TYPE, (uint32_t) THREE_PHASE);
	ret |= SendCommand(_id, CMD_SET_MTR_BUS_VOLTAGE, busVoltage);
	ret |= SendCommand(_id, CMD_SET_MTR_VOLT_LIM, maxVoltage);
	ret |= SendCommand(_id, CMD_SET_MTR_HALL_WIRING, (uint32_t) hallWiring);
	ret |= SendCommand(_id, CMD_SET_MTR_ENC_SIGN, (uint32_t) polarity);
	if(ret) {
			printf("Error setting up motor, ret:%d\n",ret);
		return ret;
	}
	_mtrSet = 1;
	return ret;
}

// Motor Gains
uint32_t Motor::setupPID(float kp, float ki, float kd) {
	uint32_t ret;

	//We are using unions to avoid the compiler trying to cast the floats to integers
	// and this is cheaper than doing a memcopy
	float2int_t uKp, uKi, uKd;

	uKp.floatData = kp;
	uKi.floatData = ki;
	uKd.floatData = kd;

	ret  = SendCommand(_id, CMD_SET_PID_KP, uKp.intData); 	
	ret |= SendCommand(_id, CMD_SET_PID_KI, uKi.intData); 	
	ret |= SendCommand(_id, CMD_SET_PID_KD, uKd.intData); 	

	if(ret) {
			printf("Error setting up gains, ret:%d\n",ret);
		return ret;
	}
	_gainSet = 1;
	return (uint32_t) ret; 
}

uint32_t Motor::setupPID(float kp, float ki, float kd, uint32_t intLimit) {
	uint32_t ret;
	ret = setupPID(kp, ki, kd);
	ret |= SendCommand(_id, CMD_SET_PID_INT_LIM, intLimit); 	

	return ret; 
}

//Motion trap settings
uint32_t Motor::setupTrap(uint32_t stopAccel, uint32_t accel, uint32_t maxVel) {
	uint32_t ret;
	ret = SendCommand(_id, CMD_SET_PRO_STOP_ACCEL, stopAccel); 	
	ret |= SendCommand(_id, CMD_SET_PRO_ACCEL, accel); 	
	ret |= SendCommand(_id, CMD_SET_PRO_MAX_VEL, maxVel); 
	if(ret) {
			printf("Error setting up trap parms, ret:%d\n",ret);
		return ret;
	}
	_trapSet = 1;
	return ret;	
}

uint32_t Motor::enable() {
	uint32_t ret; 
	
	ret = SendCommand(_id, CMD_SET_MTR_ENABLE, 0);
	ret |= SendCommand(_id, CMD_SET_PID_ENABLE, 0); 	
	
	return ret;
} 

uint32_t Motor::disable() {
	uint32_t ret; 
	
	ret = SendCommand(_id, CMD_SET_MTR_DISABLE, 0);
	ret |= SendCommand(_id, CMD_SET_PID_DISABLE, 0); 	
	
	return ret;
}

uint32_t Motor::move(int32_t pos) {
	uint32_t ret; 

	if(!_mtrSet) {
			printf("Motor parms must be set before motion can start\n");
		return -1;
	}
	if(!_gainSet) {
			printf("Gains must be set before motion can start\n");
		return -1;
	}
	if(!_trapSet) {
			printf("Trap parms must be set before motion can start\n");
		return -1;
	} 

	return (uint32_t) SendCommand(_id, CMD_SET_PRO_LOAD_AND_GO, pos);	
}

uint32_t Motor::move(int32_t pos, uint32_t maxVel) {
	uint32_t ret; 

	ret = SendCommand(_id, CMD_SET_PRO_MAX_VEL, maxVel); 
	ret |= move(pos);	

	return ret;
}

uint32_t Motor::stop() {
	return SendCommand(_id, CMD_SET_PRO_STOP, 0);
}

uint32_t Motor::softLimitsPosNeg(int32_t pos, int32_t neg) {
	uint32_t ret;
	
	ret = SendCommand(_id, CMD_SET_PRO_POS_LIM, pos); 
	ret |= SendCommand(_id, CMD_SET_PRO_NEG_LIM, neg);	

	return ret;
}

uint32_t Motor::getStatus() {
	int2status_t stat;
	stat.intData = SendCommand(_id, CMD_GET_STATUS, 0);
	status = stat.statusData;
	return stat.intData;
}

uint32_t Motor::SendCommand(uint8_t id, cmdID_t cmd, uint32_t data) {
	mtrCmdReq_t mtrCmdReq;
  mtrCmdRep_t mtrCmdRep;
  
	mtrCmdReq.mtrID=id;
	mtrCmdReq.cmdID = (uint8_t)cmd;
	mtrCmdReq.data = data;		
	zmq_send (_req, &mtrCmdReq, sizeof(mtrCmdReq_t), 0);	
	zmq_recv (_req, &mtrCmdRep, sizeof(mtrCmdRep_t), 0);
	return mtrCmdRep.data;
}
