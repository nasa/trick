/********************************* TRICK HEADER *******************************
PURPOSE: (Simulate a fluid using smoothed particle hydrodynamics (SPH).)
**************************************************************************/
#ifndef _fluid_hh_
#define _fluid_hh_
#include "Particle.hh"
#include <vector>
#include <cmath>

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
		
		
		int default_data();
		int update_SPH();
		
		float G_STRENGTH = 600;
		// external graviational force
		float G[3] = {0.f, G_STRENGTH * -9.8f, 0.f};
		float REST_DENS = 1000.f;
		float GAS_CONST = 100.f;
		// kernel radius
		float H = 16.f;
		float HSQ = H * H;
		// assume all particles have the same mass
		float MASS = 65.f;
		// viscosity constant
		float VISC = 700.f;
		// timestep size
		float DT = 0.001f;

		// smoothing kernels and their gradients
		float POLY6 = 315.f / (64.f * M_PI * std::pow(H, 9.f));
		float SPIKY_GRAD = -45.f / (M_PI * std::pow(H, 6.f));
		float VISC_LAP = 45.f / (M_PI * std::pow(H, 6.f));

		// number of particles on the edge of the square
		int EDGE_NUM_PARTICLES = 20;
		int PARTICLE_DEPTH = 1;
		int NUM_PARTICLES = PARTICLE_DEPTH * EDGE_NUM_PARTICLES * EDGE_NUM_PARTICLES;
		float PARTICLE_DIST = 5;

		Particle* particlesArr;

		// boundary parameters 
		float EPS = H; // boundary epsilon
		float BOUND_DAMPING = -.9f;

		float BOUND = 400;
};

#endif