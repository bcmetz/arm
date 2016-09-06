#include <stdio.h>
#include <stdint.h>
#include <zmq.h>
#include <unistd.h>
#include "Motor.h"

int main(int argc, char **argv) {
	int az_cmd, el_cmd, elb_cmd, wrst_cmd;
	int az_pos, el_pos, elb_pos, wrst_pos;
		
	void *context = zmq_ctx_new ();
	void *req = zmq_socket (context, ZMQ_REQ);
	zmq_connect (req, "tcp://192.168.2.153:5555");

	Motor az(req,0);
	Motor el(req,1);
	Motor elbow(req,2);
	Motor wrst(req,3);

	printf("Setting up motors...\n");
	az.disable();
	el.disable();
	elbow.disable();
	wrst.disable();

	az.setupBrushedMotor(12000,12000,REVERSED);
	el.setupBrushedMotor(12000,12000,STANDARD);
	elbow.setupBrushedMotor(12000,12000,STANDARD);
	wrst.setupBrushedMotor(12000,12000,STANDARD);

  //Setting Kp, Ki, Kd
  az.setupPID(50, 0, 0);
  el.setupPID(50, 0, 0);
  elbow.setupPID(50.0, 0, 0);
  wrst.setupPID(10, 0, 0);

  //Setting stop deceleration rate, accel/decel rate, max speed
  az.setupTrap(50000, 20000, 10000);
  el.setupTrap(50000, 20000, 10000);
  elbow.setupTrap(50000, 20000, 10000);
  wrst.setupTrap(50000, 20000, 10000);

	az.setPosition(0);
	el.setPosition(0);
	elbow.setPosition(0);
	wrst.setPosition(0);

	az_pos=0;
	el_pos=0;
	elb_pos=0;
	wrst_pos=0;

	printf("Enabling motors...\n");
	az.enable();
	el.enable();
	elbow.enable();
	wrst.enable();

	while(1) {
		//Grab relative motions
		printf("az,el,elb,wst: ");
		scanf("%d,%d,%d,%d", &az_cmd, &el_cmd, &elb_cmd, &wrst_cmd); 

		az_pos += az_cmd;
		el_pos += el_cmd;
		elb_pos += elb_cmd;
		wrst_pos += wrst_cmd;

		az.move(az_pos);
		el.move(el_pos);
		elbow.move(elb_pos);
		wrst.move(wrst_pos);
	}
	
}
