#ifndef MOTOR_H_
#define MOTOR_H_

/* Motor.h - Simple motor control interface*/

typedef enum {
	CMD_NULL=-1,
	//Motor Related Commands
	//SET
	CMD_SET_ADDR=0,
	CMD_SET_MTR_POS,
	CMD_SET_MTR_BUS_VOLTAGE,
	CMD_SET_MTR_VOLTAGE,
	CMD_SET_MTR_VOLT_LIM,
	CMD_SET_MTR_VEL_LIM,
	CMD_SET_MTR_ENC_SIGN,
	CMD_SET_MTR_HALL_WIRING,
	CMD_SET_MTR_TYPE,
	CMD_SET_MTR_ENABLE,
	CMD_SET_MTR_DISABLE,
	//GET
	CMD_GET_MTR_POS,
	CMD_GET_MTR_VEL,
	CMD_GET_MTR_BUS_VOLTAGE,
	CMD_GET_MTR_VOLTAGE,
	CMD_GET_MTR_VOLT_LIM,
	CMD_GET_MTR_VEL_LIM,
	CMD_GET_MTR_ENC_SIGN,
	CMD_GET_MTR_HALL_WIRING,
	CMD_GET_MTR_TYPE,
	//PID Related Commands
	//SET
	CMD_SET_PID_KP,
	CMD_SET_PID_KI,
	CMD_SET_PID_KD,
	CMD_SET_PID_INT_LIM,
	CMD_SET_PID_OUT_LIM,
	CMD_SET_PID_ENABLE,
	CMD_SET_PID_DISABLE,
	//GET
	CMD_GET_PID_KP,
	CMD_GET_PID_KI,
	CMD_GET_PID_KD,
	CMD_GET_PID_INT_LIM,
	CMD_GET_PID_OUT_LIM,
	CMD_GET_PID_FOLLOWING_ERROR, //Error before gains
	CMD_GET_PID_OUTPUT,			 //Error after gains
	CMD_GET_PID_INT_ERROR,
	//Profile Related Commands
	//SET
	CMD_SET_PRO_ACCEL,
	CMD_SET_PRO_STOP_ACCEL,
	CMD_SET_PRO_MAX_VEL,
	CMD_SET_PRO_VEL_FINAL,
	CMD_SET_PRO_POS_FINAL,
	CMD_SET_PRO_POS_LIM,
	CMD_SET_PRO_NEG_LIM,
	CMD_SET_PRO_LOAD,
	CMD_SET_PRO_LOAD_AND_GO,
	CMD_SET_PRO_STOP,		//Come to a stop
	CMD_SET_PRO_START,		//Start the move
	//GET
	CMD_GET_PRO_CMD_POS,
	CMD_GET_PRO_CMD_VEL,
	CMD_GET_PRO_CMD_ACCEL,
	CMD_GET_PRO_ACCEL,
	CMD_GET_PRO_STOP_ACCEL,
	CMD_GET_PRO_MAX_VEL,
	CMD_GET_PRO_VEL_FINAL,
	CMD_GET_PRO_POS_FINAL,
	CMD_GET_PRO_POS_LIM,
	CMD_GET_PRO_NEG_LIM,
	//Get status of the system
	CMD_GET_STATUS

} cmdID_t;

typedef enum {BRUSHED, THREE_PHASE, STEPPER} mtrTypes_t;

typedef enum {
	ABC = 0,
	CAB,
	BCA,
	BAC,
	CBA,
	ACB
} hallWiring_t;

typedef enum {
	STANDARD=1,
	REVERSED=-1
} feedback_t;

typedef struct {
	unsigned int enabled : 1 ;
	unsigned int voltageClipped : 1 ;
	unsigned int pidEnabled : 1;
	unsigned int outputClipped : 1;
	unsigned int intClipped : 1;
	unsigned int profileState : 2;
	unsigned int buf1State : 3;
	unsigned int buf2State : 3;
} ctrlStatus_t;

typedef enum {PROFILE_IDLE, MOVING_BUF1, MOVING_BUF2} profilerState_t;
typedef enum {EMPTY, LOADED, READY, BUSY} profileBufferState_t;

typedef struct {
  double a;
  double v0;
  double vf;
  double vMax;
  double x0;
  double xf;
} profileMove;


//We need this because we dont want to cast to uint
typedef union {
	uint32_t intData;
	float floatData;
} float2int_t;

typedef union {
	uint32_t intData;
	ctrlStatus_t statusData;
} int2status_t;

class Motor
{
	public:
		uint8_t 	serialOutput;	
		ctrlStatus_t status;
		double	_aStop;
		double	_a;
		double 	_vMax;

		void			begin();
		Motor(void* req, uint8_t id);
		int32_t 	getPosition();
		uint32_t	setPosition(int32_t data);
		int32_t		getVelocity();
		uint32_t 	getBusVoltage();
		uint32_t 	setBusVoltage(int32_t data);
		int32_t 	getMtrVoltage();
		uint32_t	setMtrVoltage(int32_t data);
		uint32_t	getMtrVoltageLimit();
		uint32_t	setMtrVoltageLimit(uint32_t data);
		uint32_t	setupBrushedMotor(uint32_t busVoltage, uint32_t maxVoltage, feedback_t polarity);
		uint32_t	setupThreePhaseMotor(uint32_t busVoltage, uint32_t maxVoltage, hallWiring_t hallWiring, feedback_t polarity);
		uint32_t	softLimitsPosNeg(int32_t pos, int32_t neg);
		uint32_t	setupPID(float kp, float ki, float kd);
		uint32_t	setupPID(float kp, float ki, float kd, uint32_t intLimit);
		uint32_t	setupTrap(uint32_t stopAccel, uint32_t accel, uint32_t maxVel);
		uint32_t	enable();
		uint32_t	disable();
		uint32_t	move(int32_t pos);
		uint32_t	move(int32_t pos, uint32_t maxVel);
		uint32_t	start();
		uint32_t	stop();
		uint32_t	loadProfileMove(profileMove move);
		uint32_t	getStatus();
	
	private:
		void*   _req;
		uint8_t _id;
		uint8_t _mtrSet;
		uint8_t _gainSet;
		uint8_t _trapSet;
		uint32_t SendCommand(uint8_t id, cmdID_t cmd, uint32_t data);
};


#endif
