#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <time.h>

#include <mach/clock.h>
#include <mach/mach.h>

#include "../../log/log.h"
#include "../../comm/comm.h"
#include "../../mtr_if/mtr_if.h"
#include "../../mtr_clnt/mtr_clnt.h"
#include "../mtr_srv/mtr_srv.h"

int main(int argc, char **argv) {
	int i;
	FILE *fp;
	double time=0;
	struct timespec ts;
	int32_t pos[NUM_MOTORS];
	int32_t vel[NUM_MOTORS];
	int32_t cmd_pos[NUM_MOTORS];
	int32_t pid_out[NUM_MOTORS];
	uint32_t status[NUM_MOTORS];
	log_t *logMain;

	void *context = zmq_ctx_new ();
	void *req = zmq_socket (context, ZMQ_REQ);


	clock_serv_t cclock;
	mach_timespec_t mts;

	if(argc != 2){
		printf("usage: %s filename\n", argv[0]);
		return 1;
	}
	
	zmq_connect (req, "tcp://localhost:5555");


	//Start logging 
	logMain = LogAlloc();
	LogInit(logMain,STDOUT,"MAIN"); 
	Log(logMain, INFO, "Starting data recording now");

	
	fp = fopen(argv[1],"w");
	if(fp == NULL){
		printf("error: cannot create file %s\n", argv[1]);
		return 1;
	}

	//print header
	fprintf(fp,"time,"
						 "wrst_pos,"
						 "wrst_vel,"
						 "wrst_cmd_pos,"
						 "wrst_pid_out,"
						 "wrst_status\n");

	while(1){
/*		host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
		clock_get_time(cclock, &mts);
		mach_port_deallocate(mach_task_self(), cclock);
		ts.tv_sec = mts.tv_sec;
		ts.tv_nsec = mts.tv_nsec; */
		time = time + 1; //(double)(ts.tv_sec + ts.tv_nsec / 1000000000.0);
		for(i=0;i<NUM_MOTORS;i++){
			SendCommand(req, i,   GET_MTR_POS,     &pos[i]);
			SendCommand(req, i,   GET_MTR_VEL,     &vel[i]);
			SendCommand(req, i,   GET_PRO_CMD_POS, &cmd_pos[i]);
			SendCommand(req, i,   GET_PID_OUTPUT,  &pid_out[i]);
			SendCommand(req, i,   GET_STATUS,      &status[i]);
		}
		
		fprintf(fp,"%f,"
							 "%d,"
							 "%d,"
							 "%d,"
							 "%d,"
							 "%d\n",
				time, 
				pos[MTR_WRST],
				vel[MTR_WRST],
				cmd_pos[MTR_WRST],
				pid_out[MTR_WRST],
				status[MTR_WRST]);
		fflush(fp);

/*		fprintf(fp,"%f,"
							 "%d,%d,%d,%d,"
							 "%d,%d,%d,%d,"
							 "%d,%d,%d,%d,"
							 "%d,%d,%d,%d,"
							 "%d,%d,%d,%d\n",
				time, 
				pos[MTR_AZ],pos[MTR_EL],pos[MTR_ELB],pos[MTR_WRST],
				vel[MTR_AZ],vel[MTR_EL],vel[MTR_ELB],vel[MTR_WRST],
				cmd_pos[MTR_AZ],cmd_pos[MTR_EL],cmd_pos[MTR_ELB],cmd_pos[MTR_WRST],
				pid_out[MTR_AZ],pid_out[MTR_EL],pid_out[MTR_ELB],pid_out[MTR_WRST],
				status[MTR_AZ],status[MTR_EL],status[MTR_ELB],status[MTR_WRST]);
*/
				usleep(20000); //10Hz
	}


	return 1;
}
