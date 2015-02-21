/*
 * status.h
 *
 *  Created on: Jan 10, 2015
 *      Author: metz
 */

#ifndef STATUS_H_
#define STATUS_H_

typedef enum {
				MTR_OK=0,
				//Generic Errors
				MTR_ERR_NOT_ENABLED,
				MTR_ERR_RANGE,
				MTR_ERR_NULL_SRC,		//Non initialized pointer
				//Motor Errors
				MTR_ERR_EN_ESTOP,		//Can not enable motor, estop depressed
				//Profiler Errors
				MTR_ERR_NO_SLN,			//Profiler could not find a solution
				MTR_ERR_BAD_TIME,		//Calculated time was less than 0
				MTR_ERR_INVALID_MOVE,	//Either acceleration or vMax is <=0
				MTR_ERR_MOVE_OOB, 		//Move is out of the set position limits
				MTR_ERR_EMPTY_BUFFER,	//Cannot start move on empty buffer
				MTR_ERR_BUF_FAULT,		//Shouldn't be able to get this fault
				MTR_ERR_BUFFERS_FULL, 	//Cannot load move both buffers are full
				//Command Errors
				MTR_ERR_NULL_CMD,		//Some how a NULL command got through
				MTR_ERR_UNKNOWN_CMD,	//I dont know what command ID that is


				MTR_TIMEOUT				//Timed out waiting for reply
			} mtrStatus_t;



#endif /* STATUS_H_ */
