
#ifndef MTR_SRV_H_
#define MTR_SRV_H_

#define NUM_MTRS 4
#define TIMEOUT 100	//100 * 10us = 1ms

typedef enum {
	IDLE=1,	//Ready for command
	CLIENT_REQ=2, //Client will set this first to alert other clients
	SRV_GRANT=4, //Server grants request by setting client ID = reqID
	RUN_CMD=8, //Client sets cmd id/data then sets flag to this state
	BUSY=16,		//Server busy processing command
	DATA_READY=32 //Signal the client to read the data, client must set back to ready
} srvState_t;

typedef struct {
	mtrID_t 		mtrID;
	cmdID_t 		cmdID;
	srvState_t	state;
	uint32_t		data;
	uint32_t		reqID;
	uint32_t		grantID;	
} mtrCmdIf_t;

typedef struct {
	int32_t	pos[NUM_MTRS];
	int32_t	cmdPos[NUM_MTRS];
	int32_t	vel[NUM_MTRS];
	int32_t	voltage[NUM_MTRS];
	uint32_t	status[NUM_MTRS];
} mtrTlmIf_t;

#endif
