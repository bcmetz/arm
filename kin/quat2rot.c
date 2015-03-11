#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv){
	double eerot[9],eetrans[3];

	// Convert input effector pose, in w x y z quaternion notation, to rotation matrix. 
	// Must use doubles, else lose precision compared to directly inputting the rotation matrix.
	double qw;// = atof(argv[5]);
	double qx;// = atof(argv[6]);
	double qy;// = atof(argv[7]);
	double qz;// = atof(argv[8]);
	double n;// = 1.0f/sqrt(qx*qx+qy*qy+qz*qz+qw*qw);
	
	if(argc != 5){
		printf("usage: %s <rot> <x> <y> <Z>\n", argv[0]);
		return 1;
	}

	qw = atof(argv[1]);
	qx = atof(argv[2]);
	qy = atof(argv[3]);
	qz = atof(argv[4]);

	n = 1.0f/sqrt(qx*qx+qy*qy+qz*qz+qw*qw);
	qw *= n;
	qx *= n;
	qy *= n;
	qz *= n;
	eerot[0] = 1.0f - 2.0f*qy*qy - 2.0f*qz*qz;  eerot[1] = 2.0f*qx*qy - 2.0f*qz*qw;         eerot[2] = 2.0f*qx*qz + 2.0f*qy*qw;
	eerot[3] = 2.0f*qx*qy + 2.0f*qz*qw;         eerot[4] = 1.0f - 2.0f*qx*qx - 2.0f*qz*qz;  eerot[5] = 2.0f*qy*qz - 2.0f*qx*qw;
	eerot[6] = 2.0f*qx*qz - 2.0f*qy*qw;         eerot[7] = 2.0f*qy*qz + 2.0f*qx*qw;         eerot[8] = 1.0f - 2.0f*qx*qx - 2.0f*qy*qy;

	// For debugging, output the matrix

	printf("     Rotation     %f   %f   %f  \n", eerot[0], eerot[1], eerot[2] );
	printf("                  %f   %f   %f  \n", eerot[3], eerot[4], eerot[5] );
	printf("                  %f   %f   %f  \n", eerot[6], eerot[7], eerot[8] );
	printf("\n");


	return 0;
}
