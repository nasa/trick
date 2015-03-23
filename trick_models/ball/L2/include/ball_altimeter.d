/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model altitude sensor parameter default initialization data.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((Random Gaussian noise)
     (constant bias))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

BALT.input.add_noise = Yes ;
BALT.input.add_bias  = Yes ;
BALT.input.noise {M} = 0.01 ;
BALT.input.bias {M}  = 0.01 ;

