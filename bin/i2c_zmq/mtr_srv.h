
#ifndef MTR_SRV_H_
#define MTR_SRV_H_

#define TRUE 1
#define FALSE 0

#define BASE_ADDR (0x48>>1)

typedef struct {
	uint8_t mtrID;  //Motor ID
	uint8_t cmdID;	 //command ID 
	uint32_t data; 	 //The data to tx with the command ID
} mtrCmdReq_t;

typedef struct {
	uint32_t data;
} mtrCmdRep_t;

uint32_t SendCommandi2c(int32_t file, uint8_t id, uint8_t cmd, uint32_t data) ;
uint8_t FindMotors(int file) ;

#endif
