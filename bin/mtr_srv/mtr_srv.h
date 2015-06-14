
#ifndef MTR_SRV_H_
#define MTR_SRV_H_

#define NUM_MTRS NUM_MOTORS

#define TRUE 1
#define FALSE 0


typedef enum {
	MOTION=0,
	PARAMETER
} mtrCmdType_t;

typedef enum {
	BUFFER=0,
	INTERRUPT
} mtrMoveType_t;
//The units are in radians and will be converted to encoder counts here out
typedef struct {
	int32_t  pos;
	uint32_t velMax;
	int32_t  velFinal;
	uint32_t accel;
	mtrMoveType_t moveType;
} mtrMove_t;

typedef struct {
	mtrCmdType_t cmdType;			 //Is this a move or a parameter
	uint8_t	enabled[NUM_MTRS]; //If true that joints command/move will be processed
	mtrMove_t move[NUM_MTRS];   //motion details in radians
	cmdID_t cmdID[NUM_MTRS];	 //command ID for each joint
	uint32_t cmdData[NUM_MTRS]; //The data to tx with the command ID
} mtrCmdRcv_t;

typedef struct {
	mtrStatus_t reply[NUM_MTRS];
} mtrCmdRep_t;


//Messages to publish to subscribers to display record data
typedef struct {
	int32_t	pos[NUM_MTRS];
	int32_t	cmdPos[NUM_MTRS];
	int32_t	vel[NUM_MTRS];
	int32_t	voltage[NUM_MTRS];
	uint32_t	status[NUM_MTRS];
} mtrTlmPub_t;

#endif
