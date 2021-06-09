/********************************* TRICK HEADER *******************************
PURPOSE: (Simulate a fluid using smoothed particle hydrodynamics (SPH).)
**************************************************************************/
#ifndef _fluid_hh_
#define _fluid_hh_
#include "Particle.hh"


class Fluid {
	public:
		Fluid(){}
		Particle** particles;
		unsigned int n_particles;
		
		int default_data();
		int update_SPH_GPU();
};

#endif