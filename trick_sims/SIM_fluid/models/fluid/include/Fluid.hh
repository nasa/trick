/********************************* TRICK HEADER *******************************
PURPOSE: (Simulate a fluid using smoothed particle hydrodynamics (SPH).)
**************************************************************************/
#ifndef _fluid_hh_
#define _fluid_hh_
#include "Particle.hh"
#include <vector>


class Fluid {
	private:
		void computeDensityAndPressure(int p_start, int p_end);
		void computeForces(int p_start, int p_end);
		void verletUpdatePosition(int p_start, int p_end);
		void timeIntegration(int p_start, int p_end);
		std::vector<float> getParticlePositions();
	public:
		Fluid(){}
		std::vector<Particle> particles;
		unsigned int n_particles;
		
		int default_data();
		int update_SPH();
		
		const float G_STRENGTH = 600;
		// external graviational force
		const float G[3] = {0.f, G_STRENGTH * -9.8f, 0.f};
		const float REST_DENS = 1000.f;
		const float GAS_CONST = 100.f;
		// kernel radius
		const float H = 16.f;
		const float HSQ = H * H;
		// assume all particles have the same mass
		const float MASS = 65.f;
		// viscosity constant
		const float VISC = 700.f;
		// timestep size
		const float DT = 0.001f;

		// smoothing kernels and their gradients
		const float POLY6 = 315.f / (64.f * M_PI * pow(H, 9.f));
		const float SPIKY_GRAD = -45.f / (M_PI * pow(H, 6.f));
		const float VISC_LAP = 45.f / (M_PI * pow(H, 6.f));

		// number of particles on the edge of the square
		const int EDGE_NUM_PARTICLES = 32;
		const int PARTICLE_DEPTH = 4;
		const int NUM_PARTICLES = PARTICLE_DEPTH * EDGE_NUM_PARTICLES * EDGE_NUM_PARTICLES;
		const float PARTICLE_DIST = 5;


		// boundary parameters 
		const float EPS = H; // boundary epsilon
		const float BOUND_DAMPING = -.9f;

		const float BOUND = 100;
};

#endif