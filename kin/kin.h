//What do I want this code to do?
// - So given is forward and reverse kinematics
// - I think the best place to put the way point generation and calculation is here
//   - come up with waypoint struct

#ifndef _KIN_H_
#define _KIN_H_

typedef enum {
	JOINT_AZ=0,
	JOINT_EL,
	JOINT_ELBOW,
	JOINT_WRIST,

	NUM_JOINTS
} joints_t;

typedef struct {
	double az;
	double el;
	double elbow;
	double wrist;
} jointAngles_t;

typedef struct {
	double x;
	double y;
	double z;
	double angle[3]; //wrist angle
} eeCoord_t;

typedef struct {
	double x;
	double y;
	double z;
	double w;
	jointAngles_t joints;
	eeCoord_t ee;
} waypoint_t;


//Given joint angles calc end effector coordinate frame
int KinCalcFowardKinematics(jointAngles_t*, eeCoord_t*);

//Given ee coord frame calc joint angles
int KinCalcInverseKinematics(eeCoord_t*, jointAngles_t*);

//Convert from quaternion to rotation matrix
//gsl_matrix* KinQuat2Rot(double x, double y, double z, double w);









#endif
