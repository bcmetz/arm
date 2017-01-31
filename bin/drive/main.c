#include <stdio.h>
#include <stdint.h>
#include <zmq.h>
#include <unistd.h>
#include "Motor.h"
#include "Coord.h"

int main(int argc, char **argv) {
	int lf_cmd, rf_cmd, lr_cmd, rr_cmd;
	int lf_pos, rf_pos, lr_pos, rr_pos;
		
	void *context = zmq_ctx_new ();
	void *req = zmq_socket (context, ZMQ_REQ);
	zmq_connect (req, "tcp://192.168.8.1:5555");

	Motor lf(req,0);
	Motor rf(req,1);
	Motor lr(req,3);
	Motor rr(req,2);

	Coord coord;

	printf("Setting up motors...\n");
	lf.disable();
	rf.disable();
	lr.disable();
	rr.disable();

	lf.setupBrushedMotor(12000,12000,REVERSED);
	rf.setupBrushedMotor(12000,12000,REVERSED);
	lr.setupBrushedMotor(12000,12000,REVERSED);
	rr.setupBrushedMotor(12000,12000,REVERSED);

  //Setting Kp, Ki, Kd
  lf.setupPID(50, 0, 0);
	rf.setupPID(50, 0, 0);
	lr.setupPID(50, 0, 0);
	rr.setupPID(50, 0, 0);

  //Setting stop decrf.ration rate, accrf.decrf.rate, max speed
  lf.setupTrap(50000, 2000, 5000);
	rf.setupTrap(50000, 2000, 5000);
	lr.setupTrap(50000, 2000, 5000);
	rr.setupTrap(50000, 2000, 5000);

	lf.setPosition(0);
	rf.setPosition(0);
	lr.setPosition(0);
	rr.setPosition(0);

	lf_pos=0;
	rf_pos=0;
	lr_pos=0;
	rr_pos=0;

	printf("Enabling motors...\n");
	lf.enable();
	rf.enable();
	lr.enable();
	rr.enable();

	while(1) {
	//Grab relative motions
		printf("LF,RF,LR,RR: ");
		scanf("%d,%d,%d,%d", &lf_cmd, &rf_cmd, &lr_cmd, &rr_cmd); 

		lf_pos += lf_cmd;
		rf_pos += rf_cmd;
		lr_pos += lr_cmd;
		rr_pos += rr_cmd;

		coord.addMove(&lf, lf_pos);
		coord.addMove(&rf, rf_pos);
		coord.addMove(&lr, lr_pos);
		coord.addMove(&rr, rr_pos);
		coord.startCoordMove();
		/*lf.move(lf_pos);
		rf.move(rf_pos);
		lr.move(lr_pos);
		rr.move(rr_pos);*/
	}
	
}
