#include <stdio.h>
#include <stdlib.h>
//#include <gsl_matrix.h>


#define IKFAST_HAS_LIBRARY // Build IKFast with API functions
#define IKFAST_NO_MAIN // Don't include main() from IKFast

#include "ikfast.cpp"
#include "kin.h"

int KinCalcFowardKinematics(jointAngles_t* angles, eeCoord_t* ee) {
	int i;
	double eetrans[3];
	double eerot[9];

	for(i=0;i<9;i++){
		eerot[i] = -48;
	}

	ComputeFk((double*)angles,eetrans,eerot);

	printf("Found fk solution for end frame: \n\n");

	printf("  Translation:  x: %f  y: %f  z: %f  \n", eetrans[0], eetrans[1], eetrans[2] );
	printf("\n");
	printf("     Rotation     %f   %f   %f  \n", eerot[0], eerot[1], eerot[2] );
	printf("       Matrix:    %f   %f   %f  \n", eerot[3], eerot[4], eerot[5] );
	printf("                  %f   %f   %f  \n", eerot[6], eerot[7], eerot[8] );
	printf("\n");	
	

	return 0;
}



#ifdef STANDALONE

int main(int argc, char **argv){
	int i;
	if(argc != 9) {
		printf("usage: %s x1 y1 z1 w1 x2 y2 z2 w2\n", argv[0]);
		return 1;
	}

	const int points = 100;

	waypoint_t waypoint[points];

	waypoint[0].x = atof(argv[1]);
	waypoint[0].y = atof(argv[2]);
	waypoint[0].z = atof(argv[3]);
	waypoint[0].w = atof(argv[4]);
	waypoint[points-1].x = atof(argv[5]);
	waypoint[points-1].y = atof(argv[6]);
	waypoint[points-1].z = atof(argv[7]);
	waypoint[points-1].w = atof(argv[8]);
	

	KinCalcFowardKinematics(&angles, &ee);

}

#endif



