
#ifndef MTR_SRV_H_
#define MTR_SRV_H_

#define NUM_MTRS NUM_MOTORS

#define TRUE 1
#define FALSE 0

typedef struct {
	mtrID_t mtrID;  //Motor ID
	cmdID_t cmdID;	 //command ID 
	uint32_t data; 	 //The data to tx with the command ID
} mtrCmdReq_t;

typedef struct {
	mtrStatus_t ret;
	uint32_t data;
} mtrCmdRep_t;

#endif
