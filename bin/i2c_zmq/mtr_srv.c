#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <zmq.h>

#include "../../log/log.h"
#include "mtr_srv.h"


//Global Log
log_t *logMain;


uint32_t SendCommandi2c(int32_t file, uint8_t id, uint8_t cmd, uint32_t data) {
  uint8_t buf[5];
	uint32_t ret;

	if (ioctl(file,I2C_SLAVE,BASE_ADDR+id+1) < 0) {
        Log(logMain, ERROR, "Failed to acquire bus access and/or talk to slave.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }   

    buf[0] = cmd; //get enc, change to //CMD_GET_STATUS
    buf[1] = (uint8_t) ((0x000000FF & data));
    buf[2] = (uint8_t) ((0x0000FF00 & data) >> 8);
    buf[3] = (uint8_t) ((0x00FF0000 & data) >> 16);
    buf[4] = (uint8_t) ((0xFF000000 & data) >> 24);

		//Sending out command
		Log(logMain, DIAG, "Sending addr:0x%00x cmd:0x%00x data:0x%00x 0x%00x 0x%00x 0x%00x",
				BASE_ADDR+id+1, buf[0], buf[1], buf[2], buf[3], buf[4]); 
    if (write(file,buf,5) != 5) {
        /* ERROR HANDLING: i2c transaction failed */
        Log(logMain, WARNING, "Failed to write to address 0x%00x",BASE_ADDR+id+1);
				return -1;
    }

		//Reading back reply
    if (read(file,buf,4) != 4) {
      /* ERROR HANDLING: i2c transaction failed */
      Log(logMain, WARNING, "Failed to read from address 0x%00x",BASE_ADDR+id+1);
			return -1;
    }
		Log(logMain, DIAG, "Received addr:0x%00x data:0x%00x 0x%00x 0x%00x 0x%00x",
				BASE_ADDR+id+1, buf[0], buf[1], buf[2], buf[3]); 
	
		ret = (uint32_t) (buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24);

		return ret;
}

uint8_t FindMotors(int file) {
  uint8_t i=1;
  uint8_t buf[5];

	//See if there are motors already assigned
	Log(logMain, INFO, "Looking for preassigned motors on the bus...");
	while(1){
		if (ioctl(file,I2C_SLAVE,BASE_ADDR+i) < 0) {
        Log(logMain, ERROR, "Failed to acquire bus access and/or talk to slave.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }   

    buf[0] = 0x09; //get enc, change to //CMD_GET_STATUS
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;

    if (write(file,buf,5) != 5) {
        /* ERROR HANDLING: i2c transaction failed */
        Log(logMain, INFO, "No motor found at addr: 0x%00x, stopping search",BASE_ADDR+i);
        break;
    } 
		//Now read the data to not make controllers angry
    if (read(file,buf,4) != 4) {
      /* ERROR HANDLING: i2c transaction failed */
      Log(logMain, ERROR, "Failed to read from the i2c bus. Something is wrong");
			exit(1);
    } 
		Log(logMain, INFO, "Found Motor[%d] at addr: 0x%00x", i, BASE_ADDR+i);
		i++;
	}

	Log(logMain, INFO, "Looking for any new motors on the bus...");
  while(1) {
    if (ioctl(file,I2C_SLAVE,BASE_ADDR) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }   

    buf[0] = 0x00; //Change Address
    buf[1] = (BASE_ADDR+i)<<1;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;

    if (write(file,buf,5) != 5) {
        /* ERROR HANDLING: i2c transaction failed */
        Log(logMain, INFO, "Failed to find any more motors");
        break;
    }   

    if (ioctl(file,I2C_SLAVE,BASE_ADDR+i) < 0) {
        Log(logMain, ERROR, "Failed to acquire bus access and/or talk to slave.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }   

    if (read(file,buf,4) != 4) {
      /* ERROR HANDLING: i2c transaction failed */
      Log(logMain, ERROR, "Failed to read from device at new address.");
      exit(1);
    }   
    else {
    	Log(logMain, INFO, "Motor[%d] found, assigned addr 0x%00x\n",i,BASE_ADDR+i); 
    }   
    i++;
  }
  return i-1;
}

int main(int argc, char **argv) {
	int file;
  char filename[40];
	int i;
	uint8_t numMotors;

	mtrCmdReq_t mtrCmdReq;
	mtrCmdRep_t mtrCmdRep;
	
	void *context = zmq_ctx_new ();
	void *responder = zmq_socket (context, ZMQ_REP);
	int rc = zmq_bind (responder, "tcp://*:5555");
	assert (rc == 0);

	if(argc != 2) {
		printf("Incorrect usage:\n");
		printf("%s [i2c port#]\n", argv[0]);
		exit(1);
	}

	logMain = LogAlloc();
	LogInit(logMain,STDOUT,"SERV"); 
	Log(logMain, INFO, "Starting motor interface server, port 5555");

	//Open the i2c port
	Log(logMain, INFO, "Opening i2c port /dev/i2c-%s", argv[1]);
	sprintf(filename,"/dev/i2c-%s", argv[1]);
	if ((file = open(filename,O_RDWR)) < 0) {
			Log(logMain, ERROR, "Failed to open the bus.");
			/* ERROR HANDLING; you can check errno to see what went wrong */
			exit(1);
	}

	numMotors = FindMotors(file);

  Log(logMain, INFO, "Found %d motors on the bus", numMotors);

	Log(logMain, INFO, "Motor server ready");

	while(1) {
		zmq_recv (responder, &mtrCmdReq, sizeof(mtrCmdReq_t), 0);
		mtrCmdRep.data = SendCommandi2c(file, mtrCmdReq.mtrID, mtrCmdReq.cmdID, mtrCmdReq.data);
		zmq_send (responder, &mtrCmdRep, sizeof(mtrCmdRep_t), 0);
	}	

	return 0;
}
