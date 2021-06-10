/********************************* TRICK HEADER *******************************
PURPOSE: (Simulate a fluid using smoothed particle hydrodynamics (SPH).)
LIBRARY DEPENDENCY:
	((Fluid.o)
	 (Particle.o))
*******************************************************************************/
#include "../include/Fluid.hh"

int Fluid::default_data() {
	n_particles = 1024;
	
	const float PARTICLE_DIST = 5.f;
	for(int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			particles.push_back(Particle(PARTICLE_DIST * i, PARTICLE_DIST * j, 0));
		}
		
	}
	
	return 0;
}

int Fluid::update_SPH() {
	//updateSPH_GPU(particles);
	return 0;

}