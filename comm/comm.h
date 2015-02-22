
#ifndef COMM_H_
#define COMM_H_

typedef enum {
	OPEN,
	CLOSED
} commState_t;

typedef struct {
	char mode[4];
	int  brate;
	int  cport;
	log_t *log;
	commState_t state;
} comm_t;

typedef enum {
	COMM_OK=0,
	COMM_ERR_NO_PORT,
	COMM_ERR_TIMEOUT
} commStatus_t; 


comm_t* CommAlloc();
void CommFree(comm_t*);
commStatus_t CommInit(comm_t*, int, int, char*);

commStatus_t CommSendData(comm_t*, uint8_t*, uint32_t);
commStatus_t CommRecvData(comm_t*, uint8_t*, uint32_t, uint32_t);









#endif
