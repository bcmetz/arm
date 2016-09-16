#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <zmq.h>

#include "Motor.h"
#include "Coord.h"

Coord::Coord() {
	_mtrIndex=0;
}

uint32_t Coord::addMove(Motor *mtr, double pos) {
	_mtr[_mtrIndex] = mtr;
	_pos[_mtrIndex] = pos;
	_mtrIndex++;
	return _mtrIndex;
}

uint32_t Coord::startCoordMove() {
	uint32_t i;
	double 	 t[127];
	double	 tMax;
	profileMove move[127];

	//For right now cannot do a coord move while motions are current in process
	for(i=0; i<_mtrIndex; i++) {
		_mtr[i]->getStatus(); 
		if(_mtr[i]->status.profileState != PROFILE_IDLE) {
			printf("Error cannot preform coordinated motion while other motions are in progress\n");
			return -1;
		}
	}
	
	for(i=0; i<_mtrIndex; i++) {
		move[i].a = _mtr[i]->_a;
		move[i].vMax = _mtr[i]->_vMax;
		move[i].v0 = 0;
		move[i].vf = 0;
		move[i].x0 = (double)_mtr[i]->getPosition();
		move[i].xf = _pos[i];	
		t[i] = motionTime(move[i]);
		if(t[i] < 0) {
			printf("Error finding motion time on index %d\n", i);
			return -1; 
		}
	}

	//Find move with longest time
	tMax=0;
	for(i=0;i<_mtrIndex;i++){
		tMax = (t[i]>tMax) ? t[i] : tMax;
	}

	//Find all the new Vmax	
	for(i=0;i<_mtrIndex;i++){
		if(t[i] < tMax) {
			if(calcVmax(&move[i],tMax)) {
				printf("Error solving for vMax on index %d\n", i);
				return -1;
			}
		}
	}


	//Load all the motions	
	for(i=0;i<_mtrIndex;i++){
		_mtr[i]->loadProfileMove(move[i]);
	}

	//Kick off the motions
	for(i=0;i<_mtrIndex;i++){
		_mtr[i]->start();
	}


	_mtrIndex=0;

	return 1;
}

double Coord::motionTime(profileMove move) {
  double dtTemp1, dtTemp2, dtTemp3;
  double aTemp1, aTemp2, aTemp3;
  double sqrtTemp;
  double vMaxTemp;
  double xTrapCheck;
  double xfNew;
  double x1,x2,A,B,C;
	profileSegment segment[3];

  //In order to calc correct times need to get accel directions right
  if(move.xf - move.x0 < 0) {
    vMaxTemp = -1*move.vMax;
  }
  else {
    vMaxTemp = move.vMax;
  }

  aTemp1 = (move.v0 > vMaxTemp) ? -1*move.a : move.a;
  aTemp2 = (move.vf > vMaxTemp) ? move.a : -1*move.a;

  //Time to accelerate to max velocity
  dtTemp1 = (vMaxTemp - move.v0)/aTemp1;
  //Time to decelerate to final velocity
  dtTemp2 = (move.vf - vMaxTemp)/aTemp2;

  //Check to see if we'll do a trap profile motion
  xTrapCheck = 0.5*aTemp1*pow(dtTemp1,2) + move.v0*dtTemp1 + 0.5*aTemp2*pow(dtTemp2,2) + vMaxTemp*dtTemp2;

  //Filling out the easy data
  segment[0].x0 = move.x0;
  segment[0].v0 = move.v0;

  //Is our check position less than the final position in the time it takes to accelerate from v0 to vMax and decelerate to vf, if so then we need to cruise at vMax for a bit
  if(fabs(xTrapCheck) <= fabs(move.xf - move.x0)) {
    //We're doing a trap profile which consists of three segments
    //We know the accels we're going to need
    segment[0].a = aTemp1;
    segment[1].a = 0;
    segment[2].a = aTemp2;

    //We know accel and decel times
    segment[0].dt = dtTemp1;
    segment[2].dt = dtTemp2;

    //Now we need time for the cruise segment
    segment[1].x0 = 0.5*segment[0].a*pow(segment[0].dt,2) + move.v0*segment[0].dt + move.x0;
    segment[2].x0 = move.xf-0.5*segment[2].a*pow(segment[2].dt,2) - vMaxTemp*segment[2].dt;
    segment[1].dt = (segment[2].x0 - segment[1].x0)/vMaxTemp;

    //Fill in the blanks
    segment[1].v0 = vMaxTemp;
    segment[2].v0 = vMaxTemp;

    //Return # of segments
    return segment[0].dt + segment[1].dt + segment[2].dt;
  }
  if((move.xf <= move.x0 && move.vf >= move.v0) || \
     (move.xf >= move.x0 && move.vf <= move.v0)) {
    aTemp3 = (move.vf-move.v0 < 0) ? -1*move.a : move.a;
    dtTemp3 = (move.vf-move.v0)/aTemp3;
    xfNew = move.xf - (0.5*aTemp3*pow(dtTemp3,2) + move.v0*dtTemp3);

    aTemp1 = (xfNew-move.x0 < 0) ? -1.0*move.a : move.a;
    aTemp2 = -1.0*aTemp1;
    A=0.5*aTemp1; B=move.v0; C=(move.x0-xfNew)*0.5;
    sqrtTemp=sqrt(B*B-4*A*C);
    dtTemp1=(-B+sqrtTemp)/(2*A);
    dtTemp2=(-B-sqrtTemp)/(2*A);

    if(dtTemp1 >= 0 && dtTemp2 >= 0) {
    	dtTemp1 = (dtTemp1 < dtTemp2) ? dtTemp1 : dtTemp2;
    }
    else {
    	dtTemp1 = (dtTemp1>=0) ? dtTemp1 : dtTemp2;
    }

    if(dtTemp1 < 0) {
  	  return -1;
    }

    dtTemp2 = dtTemp1;
  }
  else {
    aTemp1 = (move.vf-move.v0 < 0) ? -1*move.a : move.a;

    dtTemp1 = (move.vf-move.v0)/aTemp1;

    x1=0.5*aTemp1*pow(dtTemp1,2)+move.v0*dtTemp1+move.x0;
    //Find x1 compare to xf, set accel that way
    aTemp2 = (x1 < move.xf) ? move.a : -1*move.a;
    aTemp3=-1*aTemp2;
    x2 = x1 + (move.xf-x1)/2;

    A=0.5*aTemp2; B=move.vf; C=x1-x2;
    sqrtTemp=sqrt(B*B-4*A*C);
    dtTemp2=(-B+sqrtTemp)/(2*A);
    dtTemp3=(-B-sqrtTemp)/(2*A);

    if(dtTemp2 >= 0 && dtTemp3 >= 0) {
    dtTemp2 = (dtTemp2 < dtTemp3) ? dtTemp2 : dtTemp3;
    }
    else {
    dtTemp2 = (dtTemp2>=0) ? dtTemp2 : dtTemp3;
    }

    if(dtTemp2 < 0){
        return -1;
    }

    dtTemp3=dtTemp2;
  }

  //Triangle Profile
  segment[0].dt = dtTemp1;
  segment[1].dt = dtTemp2;
  segment[2].dt = dtTemp3;

  segment[0].a = aTemp1;
  segment[1].a = aTemp2;
  segment[2].a = aTemp3;

  segment[1].x0 = 0.5*segment[0].a*pow(segment[0].dt,2) + segment[0].v0*segment[0].dt + segment[0].x0;
  segment[1].v0 = segment[0].a*segment[0].dt + segment[0].v0;

  segment[2].x0 = 0.5*segment[1].a*pow(segment[1].dt,2) + segment[1].v0*segment[1].dt + segment[1].x0;
  segment[2].v0 = segment[1].a*segment[1].dt + segment[1].v0;

  return segment[0].dt + segment[1].dt + segment[2].dt;
}


int32_t Coord::calcVmax(profileMove *move, double t) {
  double A, B, C;
  double a, v0, vf, x0, xf;
  double sln1, sln2;

  printf("Desired time: %f\n", t);

  a = (move->xf - move->x0 < 0) ? -1*move->a : move->a;

  v0=move->v0;
  vf=move->vf;
  x0=move->x0;
  xf=move->xf;

  A=1;
  B=-1*(a*t+v0+vf);
  C=0.5*(v0*v0+vf*vf)+a*(xf-x0);

  sln1 = (-B+sqrt(B*B-4*A*C))/2/A;
  sln2 = (-B-sqrt(B*B-4*A*C))/2/A;

  printf("Vmax: sln1:%f\tsln2:%f\n",sln1,sln2);


  move->vMax = fabs(sln1);
  if(fabs(t - (this->motionTime(*move))) < 0.001) {
    return 0;
  }

  move->vMax = fabs(sln2);
  if(fabs(t - (this->motionTime(*move))) < 0.001) {
    return 0;
  }

  //No solution
  return -1;

}






