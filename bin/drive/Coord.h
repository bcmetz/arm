
#ifndef COORD_H_
#define COORD_H_

typedef struct {
  double dt;
  double a;
  double v0;
  double x0;
} profileSegment;

class Coord
{
	public:
		Coord();
		uint32_t addMove(Motor *mtr, double pos);
		uint32_t startCoordMove();
	private:
		Motor			*_mtr[127];
		double		_pos[127];
		uint32_t	_mtrIndex;
		double		motionTime(profileMove move);
		int32_t		calcVmax(profileMove *move, double t); 
};

#endif
