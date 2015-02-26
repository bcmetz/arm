
#ifndef MTR_SRV_H_
#define MTR_SRV_H_

#define NUM_MTRS 4

typedef enum {
	IDLE=0,	//Ready for command
	CLIENT_REQ, //Client will set this first to alert other clients
	RUN_CMD, //Client sets cmd id/data then sets flag to this state
	BUSY,		//Server busy processing command
	DATA_READY //Signal the client to read the data, client must set back to ready
} srvState_t;

typedef struct {
	mtrID_t 		mtrID;
	cmdID_t 		cmdID;
	srvState_t	state;
	uint32_t		data;	
} mtrCmdIf_t;

typedef struct {
	int32_t	pos[NUM_MTRS];
	int32_t	cmdPos[NUM_MTRS];
	int32_t	vel[NUM_MTRS];
	int32_t	voltage[NUM_MTRS];
	uint32_t	status[NUM_MTRS];
} mtrTlmIf_t;


#endif
