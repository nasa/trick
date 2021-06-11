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
		
		const static float G_STRENGTH = 600;
		// external graviational force
		const static float G[3] = {0.f, G_STRENGTH * -9.8f, 0.f};
		const static float REST_DENS = 1000.f;
		const static float GAS_CONST = 100.f;
		// kernel radius
		const static float H = 16.f;
		const static float HSQ = H * H;
		// assume all particles have the same mass
		const static float MASS = 65.f;
		// viscosity constant
		const static float VISC = 700.f;
		// timestep size
		const static float DT = 0.001f;

		// smoothing kernels and their gradients
		const static float POLY6 = 315.f / (64.f * M_PI * pow(H, 9.f));
		const static float SPIKY_GRAD = -45.f / (M_PI * pow(H, 6.f));
		const static float VISC_LAP = 45.f / (M_PI * pow(H, 6.f));

		// number of particles on the edge of the square
		const static int EDGE_NUM_PARTICLES = 32;
		const static int PARTICLE_DEPTH = 4;
		const static int NUM_PARTICLES = PARTICLE_DEPTH * EDGE_NUM_PARTICLES * EDGE_NUM_PARTICLES;
		const static float PARTICLE_DIST = 5;


		// boundary parameters 
		const static float EPS = H; // boundary epsilon
		const static float BOUND_DAMPING = -.9f;

		const static float BOUND = 100;
};

#endif