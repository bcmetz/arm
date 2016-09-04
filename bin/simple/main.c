#include <stdio.h>
#include <stdint.h>
#include <zmq.h>
#include "Motor.h"

int main(int argc, char **argv) {

	void *context = zmq_ctx_new ();
	void *req = zmq_socket (context, ZMQ_REQ);
	zmq_connect (req, "tcp://192.168.2.153:5555");

	Motor az(req,0);
	Motor el(req,1);
	Motor wrst(req,2);

	printf("Setting up motors...\n");
	az.setupBrushedMotor(12000,12000,REVERSED);
	el.setupBrushedMotor(12000,12000,REVERSED);
	wrst.setupBrushedMotor(12000,12000,REVERSED);
  //Setting Kp, Ki, Kd
  az.setupPID(50.0, 0, 0.1);
  el.setupPID(50.0, 0, 0.1);
  wrst.setupPID(50.0, 0, 0.1);
  //Setting stop deceleration rate, accel/decel rate, max speed
  az.setupTrap(50000, 20000, 10000);
  el.setupTrap(50000, 20000, 10000);
  wrst.setupTrap(50000, 20000, 10000);

	printf("Enabling motors...\n");
	az.enable();
	el.enable();
	wrst.enable();
	
}
