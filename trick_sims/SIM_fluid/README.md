# Summer 2021 Intern Project for Nate Hill
TSPH is a robust particle-based fluid simulation within the Trick simulation framework.

## Applications

- Serves as an example of how to use GPU hardware acceleration within Trick
- Serves as an example of how to write shared memory graphics for Trick simulations using OpenGL
- Serves as an example of using scheduled time integration outside of the Trick integration system. (TSPH uses Velocity Verlet)
- Demonstrates performing time integration in smaller durations than real time. By default, TSPH is scheduled to simulate one time step of the fluid every 0.02 seconds. Since the time step size of the fluid simulation is 0.001 seconds, every second TSPH simulates 0.05 seconds of the fluid.
- Demonstrates the application of a popular computer graphics/physical simulation research topic within Trick
- Can be used to simulate a wide range of fluids in a cuboidal region of space in quasi-real time. (Quasi-real time meaning that the complete evolution of the fluid state can be observed in a small amount of time. See above.) TSPH users have control over the viscosity, kernel radius, and other physical parameters of the fluid.
- User can create various initial conditions of the fluid particles from the Python input file
## Future Applications

- If physically correct rigid body-fluid interaction is implemented in the future, then TSPH can be used for the following simulations:
    - Splashdown simulation of spacecraft in the ocean
    - Simulate interactions between spacecraft and air fluid particles
    - Simulate fluids within tank of a rocket
    - Could serve as a starting point for future particle-based fluid simulation research

## Demos
- TSPH Particle Real Time: https://youtu.be/i3YgT9ZVXLM
- TSPH Marching Cubes Real Time: https://youtu.be/7kZGGRVzTts
- TSPH Paraboloid: https://youtu.be/yvON0uI0Zpo
- TSPH Marching Cubes Dam Break: https://youtu.be/aN4TR_0ftZI
- TSPH Particle Dam Break: https://youtu.be/OyaTB79UXFE