#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

#include "../../log/log.h"
#include "../../comm/comm.h"
#include "../../mtr_if/mtr_if.h"
#include "../mtr_srv/mtr_srv.h"
#include "../../mtr_clnt/mtr_clnt.h"

#define CLNT_ID 0x1234

int main(int argc, char **argv) {
	int i;
	FILE *fp;
	double time;
	struct timespec ts;
	int32_t pos[NUM_MOTORS];
	int32_t vel[NUM_MOTORS];
	int32_t cmd_pos[NUM_MOTORS];
	int32_t pid_out[NUM_MOTORS];
	uint32_t status[NUM_MOTORS];
	key_t key;
   mtrCmdIf_t *mtrCmdIf;
	log_t *logMain;
   key = 0x4848;

	if(argc != 2){
		printf("usage: %s filename\n", argv[0]);
		return 1;
	}
	
	//Start logging 
	logMain = LogAlloc();
	LogInit(logMain,STDOUT,"MAIN"); 
	Log(logMain, INFO, "Starting data recording now");

	MtrClntShmat(&mtrCmdIf, key);
	
	fp = fopen(argv[1],"w");
	if(fp == NULL){
		printf("error: cannot create file %s\n", argv[1]);
		return 1;
	}

	//print header
	fprintf(fp,"time,az_pos,el_pos,elb_pos,wrst_pos,az_vel,el_vel,elb_vel,wrst_vel,"
			"az_cmd_pos,el_cmd_pos,elb_cmd_pos,wrst_cmd_pos,az_pid_out,el_pid_out,elb_pid_out,wrst_pid_out,"
			"az_status,el_status,elb_status,wrst_status\n");

	while(1){
		clock_gettime(CLOCK_REALTIME, &ts);
		time = (double)(ts.tv_sec + ts.tv_nsec / 1000000000.0);
		for(i=0;i<NUM_MOTORS;i++){
			MtrClntSendCmd(mtrCmdIf, i,   GET_MTR_POS,     &pos[i], TIMEOUT,CLNT_ID);
			MtrClntSendCmd(mtrCmdIf, i,   GET_MTR_VEL,     &vel[i], TIMEOUT,CLNT_ID);
			MtrClntSendCmd(mtrCmdIf, i,   GET_PRO_CMD_POS, &cmd_pos[i], TIMEOUT,CLNT_ID);
			MtrClntSendCmd(mtrCmdIf, i,   GET_PID_OUTPUT,  &pid_out[i], TIMEOUT,CLNT_ID);
			MtrClntSendCmd(mtrCmdIf, i,   GET_STATUS,      &status[i], TIMEOUT,CLNT_ID);
		}
		fprintf(fp,"%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
				time, pos[MTR_AZ],pos[MTR_EL],pos[MTR_ELB],pos[MTR_WRST],
				vel[MTR_AZ],vel[MTR_EL],vel[MTR_ELB],vel[MTR_WRST],
				cmd_pos[MTR_AZ],cmd_pos[MTR_EL],cmd_pos[MTR_ELB],cmd_pos[MTR_WRST],
				pid_out[MTR_AZ],pid_out[MTR_EL],pid_out[MTR_ELB],pid_out[MTR_WRST],
				status[MTR_AZ],status[MTR_EL],status[MTR_ELB],status[MTR_WRST]);
		usleep(20000); //10Hz
	}


	return 1;
}
