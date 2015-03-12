//What do I want this code to do?
// - So given is forward and reverse kinematics
// - I think the best place to put the way point generation and calculation is here
//   - come up with waypoint struct

#ifndef _KIN_H_
#define _KIN_H_


typedef struct {
	double x;
	double y;
	double z;
	double w;
	//ik solutions for that way point
} waypoint_t;

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
	gsl_matrix *rot;
} eeFrame;



//Given joint angles calc end effector coordinate frame
int KinCalcFowardKinematics(jointAngles_t*, eeFrame_t*);

//Given ee coord frame calc joint angles
int KinCalcReverseKinematics(eeFrame_t*, jointAngles_t*);

//Convert from quaternion to rotation matrix
gsl_matrix* KinQuat2Rot(double x, double y, double z, double w);









#endif
