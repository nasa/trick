#ifndef TETRAD_HH
#define TETRAD_HH

class Tetrad
{
public:
	double pos[2];
	double angle;
	double vertices[12][2];
	int vertix_count;
	double com[2];

	double vel[2];
	double rate;                 //clockwise rate

	//double accel[2];
	//double ang_accel;            //clockwise acceleration

private:
	void calc_center_of_mass();

public:
	Tetrad(double* in_pos, double in_angle, double** in_vertices, int in_vertix_count);

};
#endif
