
#ifndef MTRCMD_H_
#define MTRCMD_H_

#define MTR_BUF_SIZE 16
typedef enum {
	MTR_AZ,
	MTR_EL,
	MTR_ELB,
	MTR_WRST,

	NUM_MOTORS
} mtrID_t;

typedef enum {
	SET_MTR_POS=0,
	SET_MTR_BUS_VOLTAGE,
	SET_MTR_VOLTAGE,
	SET_MTR_VOLT_LIM,
	SET_MTR_VEL_LIM,
	SET_MTR_ENABLE,
	SET_MTR_DISABLE,
	//GET
	GET_MTR_POS,
	GET_MTR_VEL,
	GET_MTR_BUS_VOLTAGE,
	GET_MTR_VOLTAGE,
	GET_MTR_VOLT_LIM,
	GET_MTR_VEL_LIM,
	//PID Related Commands
	//SET
	SET_PID_KP,
	SET_PID_KI,
	SET_PID_KD,
	SET_PID_INT_LIM,
	SET_PID_OUT_LIM,
	SET_PID_GAIN_SHIFT,
	SET_PID_ENABLE,
	SET_PID_DISABLE,
	//GET
	GET_PID_KP,
	GET_PID_KI,
	GET_PID_KD,
	GET_PID_INT_LIM,
	GET_PID_OUT_LIM,
	GET_PID_FOLLOWING_ERROR, //Error before gains
	GET_PID_OUTPUT,			 //Error after gains
	GET_PID_INT_ERROR,
	GET_PID_GAIN_SHIFT,
	//Profile Related Commands
	//SET
	SET_PRO_ACCEL,
	SET_PRO_MAX_VEL,
	SET_PRO_VEL_FINAL,
	SET_PRO_POS_FINAL,
	SET_PRO_POS_LIM,
	SET_PRO_NEG_LIM,
	SET_PRO_LOAD,
	SET_PRO_LOAD_AND_GO,
	SET_PRO_STOP,		//Come to a stop
	SET_PRO_START,		//Start the move
	//GET
	GET_PRO_CMD_POS,
	GET_PRO_CMD_VEL,
	GET_PRO_CMD_ACCEL,
	GET_PRO_ACCEL,
	GET_PRO_MAX_VEL,
	GET_PRO_VEL_FINAL,
	GET_PRO_POS_FINAL,
	//Get status of the system
	GET_STATUS,

	NUM_COMMANDS
} cmdID_t;


typedef struct {
	mtrID_t mtrID;
	uint8_t	address;
	uint8_t	rxBuffer[MTR_BUF_SIZE];  //<ADDR><RXLEN><DATA[0]>..<DATA[3]>
	uint8_t	txBuffer[MTR_BUF_SIZE];  //<ADDR><TXLEN><RXLEN><DATA[0]>..<DATA[3]>
	log_t	*log;
}mtr_t;

//Input: mtr_t pointer, motorID, motor address
mtrStatus_t MtrInit(mtr_t*, mtrID_t, uint8_t);

//Get for the MTR module
mtrStatus_t MtrGetPos(mtr_t*, int32_t*);
mtrStatus_t MtrGetVel(mtr_t*, int32_t*);
mtrStatus_t MtrGetVoltage(mtr_t*, int32_t*);
mtrStatus_t MtrGetBusVoltage(mtr_t*, uint32_t*);
mtrStatus_t MtrGetVoltLimit(mtr_t*, uint32_t*);
mtrStatus_t MtrGetVelLimit(mtr_t*, uint32_t*);

mtrStatus_t MtrSetPos(mtr_t*, int32_t);
mtrStatus_t MtrSetVoltage(mtr_t*, int32_t);
mtrStatus_t MtrSetBusVoltage(mtr_t*, uint32_t);
mtrStatus_t MtrSetBusVoltage(mtr_t*, uint32_t);
mtrStatus_t MtrSetVoltLimit(mtr_t*, uint32_t);
mtrStatus_t MtrSetVelLimit(mtr_t*, uint32_t);

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

mtrStatus_t MtrGetCmdPos(mtr_t*, int32_t*);
mtrStatus_t MtrGetCmdVel(mtr_t*, int32_t*);
mtrStatus_t MtrGetCmdAccel(mtr_t*, int32_t*);

mtrStatus_t MtrSetAccel(mtr_t*, uint32_t);
mtrStatus_t MtrSetMaxVel(mtr_t*, uint32_t);
mtrStatus_t MtrSetPosFinal(mtr_t*, int32_t);
mtrStatus_t MtrSetVelFinal(mtr_t*, int32_t);

mtrStatus_t MtrLoadMove(mtr_t*);
mtrStatus_t MtrStartMove(mtr_t*);
mtrStatus_t MtrMoveNow(mtr_t*, int32_t);

mtrStatus_t MtrStopMove(mtr_t*);

#endif
