
#ifndef MTRCMD_H_
#define MTRCMD_H_

#define MTR_CMD_TIMEOUT 500 //ms
#define MTR_BUF_SIZE 16
#define MTR_TX_MESSAGE 8
#define MTR_RX_MESSAGE 6
typedef enum {
	MTR_AZ=0,
	MTR_EL,
	MTR_ELB,
	MTR_WRST,

	NUM_MOTORS
} mtrID_t;

typedef enum {
				MTR_OK=0,
				//Generic Errors
				MTR_ERR_NOT_ENABLED,
				MTR_ERR_RANGE,
				MTR_ERR_NULL_SRC,		//Non initialized pointer
				//Motor Errors
				MTR_ERR_EN_ESTOP,		//Can not enable motor, estop depressed
				//Profiler Errors
				MTR_ERR_NO_SLN,			//Profiler could not find a solution
				MTR_ERR_BAD_TIME,		//Calculated time was less than 0
				MTR_ERR_INVALID_MOVE,	//Either acceleration or vMax is <=0
				MTR_ERR_MOVE_OOB, 		//Move is out of the set position limits
				MTR_ERR_EMPTY_BUFFER,	//Cannot start move on empty buffer
				MTR_ERR_BUF_FAULT,		//Shouldn't be able to get this fault
				MTR_ERR_BUFFERS_FULL, 	//Cannot load move both buffers are full
				//Command Errors
				MTR_ERR_NULL_CMD,		//Some how a NULL command got through
				MTR_ERR_UNKNOWN_CMD,	//I dont know what command ID that is


				MTR_ERR_COMM,			//Error with sending command
				MTR_ERR_TIMEOUT		//Timed out waiting for reply
			} mtrStatus_t;


typedef enum {
	SET_MTR_POS=0,
	SET_MTR_BUS_VOLTAGE,
	SET_MTR_VOLTAGE,
	SET_MTR_VOLT_LIM,
	SET_MTR_VEL_LIM,
	SET_MTR_ENC_SIGN,
	SET_MTR_ENABLE,
	SET_MTR_DISABLE,
	GET_MTR_POS,
	GET_MTR_VEL,
	GET_MTR_BUS_VOLTAGE,
	GET_MTR_VOLTAGE,
	GET_MTR_VOLT_LIM,
	GET_MTR_VEL_LIM,
	GET_MTR_ENC_SIGN,
	SET_PID_KP,
	SET_PID_KI,
	SET_PID_KD,
	SET_PID_INT_LIM,
	SET_PID_OUT_LIM,
	SET_PID_GAIN_SHIFT,
	SET_PID_ENABLE,
	SET_PID_DISABLE,
	GET_PID_KP,
	GET_PID_KI,
	GET_PID_KD,
	GET_PID_INT_LIM,
	GET_PID_OUT_LIM,
	GET_PID_FOLLOWING_ERROR,
	GET_PID_OUTPUT,
	GET_PID_INT_ERROR,
	GET_PID_GAIN_SHIFT,
	SET_PRO_ACCEL,
	SET_PRO_STOP_ACCEL,
	SET_PRO_MAX_VEL,
	SET_PRO_VEL_FINAL,
	SET_PRO_POS_FINAL,
	SET_PRO_POS_LIM,
	SET_PRO_NEG_LIM,
	SET_PRO_LOAD,
	SET_PRO_LOAD_AND_GO,
	SET_PRO_STOP,
	SET_PRO_START,
	GET_PRO_CMD_POS,
	GET_PRO_CMD_VEL,
	GET_PRO_CMD_ACCEL,
	GET_PRO_ACCEL,
	GET_PRO_STOP_ACCEL,
	GET_PRO_MAX_VEL,
	GET_PRO_VEL_FINAL,
	GET_PRO_POS_FINAL,
	GET_PRO_POS_LIM,
	GET_PRO_NEG_LIM,
	GET_STATUS,

	NUM_COMMANDS
} cmdID_t;


typedef struct {
	mtrID_t mtrID;
	uint8_t	address;
	uint8_t	rxBuffer[MTR_BUF_SIZE];  //<ADDR><RXLEN><DATA[0]>..<DATA[3]>
	uint8_t	txBuffer[MTR_BUF_SIZE];  //<ADDR><TXLEN><RXLEN><DATA[0]>..<DATA[3]>
	log_t	*log;
	comm_t *comm;
}mtr_t;

//Input: mtr_t pointer, motorID, motor address
mtrStatus_t MtrInit(mtr_t*, comm_t*, mtrID_t, uint8_t);

mtrStatus_t MtrSimpleIf(mtr_t*, cmdID_t, uint32_t*);

//Get for the MTR module
mtrStatus_t MtrGetStatus(mtr_t*, uint32_t*);
mtrStatus_t MtrGetPos(mtr_t*, int32_t*);
mtrStatus_t MtrGetVel(mtr_t*, int32_t*);
mtrStatus_t MtrGetVoltage(mtr_t*, int32_t*);
mtrStatus_t MtrGetBusVoltage(mtr_t*, uint32_t*);
mtrStatus_t MtrGetVoltLimit(mtr_t*, uint32_t*);
mtrStatus_t MtrGetVelLimit(mtr_t*, uint32_t*);
mtrStatus_t MtrGetEncSign(mtr_t*, int32_t*);

mtrStatus_t MtrSetPos(mtr_t*, int32_t);
mtrStatus_t MtrSetVoltage(mtr_t*, int32_t);
mtrStatus_t MtrSetBusVoltage(mtr_t*, uint32_t);
mtrStatus_t MtrSetBusVoltage(mtr_t*, uint32_t);
mtrStatus_t MtrSetVoltLimit(mtr_t*, uint32_t);
mtrStatus_t MtrSetVelLimit(mtr_t*, uint32_t);
mtrStatus_t MtrSetEncSign(mtr_t*, int32_t);

mtrStatus_t MtrEnableMotor(mtr_t*, uint32_t);

mtrStatus_t MtrGetKp(mtr_t*, uint32_t*);
mtrStatus_t MtrGetKd(mtr_t*, uint32_t*);
mtrStatus_t MtrGetKi(mtr_t*, uint32_t*);
mtrStatus_t MtrGetIntLimit(mtr_t*, uint32_t*);
mtrStatus_t MtrGetPidOutputLimit(mtr_t*, uint32_t*);
mtrStatus_t MtrGetFollowingError(mtr_t*, int32_t*);
mtrStatus_t MtrGetPidOutput(mtr_t*, int32_t*);
mtrStatus_t MtrGetIntError(mtr_t*, int32_t*);
mtrStatus_t MtrGetGainShift(mtr_t*, uint32_t*);

mtrStatus_t MtrSetKp(mtr_t*, int32_t);
mtrStatus_t MtrSetKd(mtr_t*, int32_t);
mtrStatus_t MtrSetKi(mtr_t*, int32_t);
mtrStatus_t MtrSetIntLimit(mtr_t*, uint32_t);
mtrStatus_t MtrSetPidOutputLimit(mtr_t*, uint32_t);
mtrStatus_t MtrSetGainShift(mtr_t*, uint32_t);

mtrStatus_t MtrPidEnable(mtr_t*, uint32_t);

mtrStatus_t MtrGetAccel(mtr_t*, uint32_t*);
mtrStatus_t MtrGetMaxVel(mtr_t*, uint32_t*);
mtrStatus_t MtrGetPosFinal(mtr_t*, int32_t*);
mtrStatus_t MtrGetVelFinal(mtr_t*, int32_t*);
mtrStatus_t MtrGetPosLim(mtr_t*, int32_t*);
mtrStatus_t MtrGetNegLim(mtr_t*, int32_t*);

mtrStatus_t MtrGetCmdPos(mtr_t*, int32_t*);
mtrStatus_t MtrGetCmdVel(mtr_t*, int32_t*);
mtrStatus_t MtrGetCmdAccel(mtr_t*, int32_t*);

mtrStatus_t MtrSetAccel(mtr_t*, uint32_t);
mtrStatus_t MtrSetMaxVel(mtr_t*, uint32_t);
mtrStatus_t MtrSetPosFinal(mtr_t*, int32_t);
mtrStatus_t MtrSetVelFinal(mtr_t*, int32_t);
mtrStatus_t MtrSetPosLim(mtr_t*, int32_t);
mtrStatus_t MtrSetNegLim(mtr_t*, int32_t);

mtrStatus_t MtrLoadMove(mtr_t*);
mtrStatus_t MtrStartMove(mtr_t*);
mtrStatus_t MtrMoveNow(mtr_t*, int32_t);

mtrStatus_t MtrStopMove(mtr_t*);

#endif
