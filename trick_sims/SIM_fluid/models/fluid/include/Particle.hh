/********************************* TRICK HEADER *******************************
PURPOSE: (Simulate fluid particles in SPH)
*******************************************************************************/
#ifndef _particle_hh_
#define _particle_hh_

struct Particle {
	Particle(float x_, float y_, float z_) :
		pos {x_, y_, z_},
		velocity {0.f, 0.f, 0.f},
		force {0.f, 0.f, 0.f},
		rho(0),
		pressure(0.f) {}
	float pos[3];
	float velocity[3];
	float force[3];
	float rho;
	float pressure;
	
};

#endif
