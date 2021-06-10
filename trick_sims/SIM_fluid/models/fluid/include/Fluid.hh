/********************************* TRICK HEADER *******************************
PURPOSE: (Simulate a fluid using smoothed particle hydrodynamics (SPH).)
**************************************************************************/
#ifndef _fluid_hh_
#define _fluid_hh_
#include "Particle.hh"
#include <vector>


class Fluid {
	public:
		Fluid(){}
		std::vector<Particle> particles;
		unsigned int n_particles;
		
		int default_data();
		int update_SPH();
};

#endif