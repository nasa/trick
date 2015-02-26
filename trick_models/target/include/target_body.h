/********************************* TRICK HEADER *******************************
PURPOSE:
   (Simple taget body tagetting information.)
REFERENCES:
   ((None))
ASSUMPTIONS AND LIMITATIONS:
   ((Purely Keplerian dynamics.))
PROGRAMMERS:
   (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

#ifndef _TARGET_BODY_H_
#define _TARGET_BODY_H_

#ifdef __cplusplus
extern "C" {
#endif



/* Target Body initialization parameters. */
typedef struct { /* TargetBodyPlanet ------------------------------------ */

   double mu; /* (m3/s2) Planetary gravitational constant. */

} TargetBodyPlanet; /* -------------------------------------------------- */


/* Target Body initialization parameters. */
typedef struct { /* TargetBodyInit -------------------------------------- */

   double position[3]; /* (m)   Position vector of the target body. */
   double velocity[3]; /* (m/s) Velocity vector of the target body. */

} TargetBodyInit; /* ---------------------------------------------------- */


/* Target Body state parameters. */
typedef struct { /* TargetBodyState -------------------------------------- */

   double position[3];     /* (m)     Position vector of the target body.     */
   double velocity[3];     /* (m/s)   Velocity vector of the target body.     */
   double acceleration[3]; /* (m/s2)  Acceleration vector of the target body. */

} TargetBodyState; /* ---------------------------------------------------- */


/* Target Body itereation parameters. */
typedef struct { /* TargetBodyIteration --------------------------------- */

   int    iter_max;     /* cnt   Maximum number of iterations allowed.      */
   int    iter_cnt;     /* cnt   Iteration count.                           */
   double tolerance;    /* (m)   Tolerance for iteration converence.        */

   double v_init[3];    /* (m/s) Initial velocity for this iteration.       */

   double delta_r[3];   /* (m)   Vector distance from the target location.  */
   double delta_r_mag;  /* (m)   Total distance from the target location.   */
   double delta_v[3];   /* (m/s) Change in initial velocity for targeting.  */

} TargetBodyIteration; /* ----------------------------------------------- */


/* Target Body data parameters. */
typedef struct { /* TargetBodyData -------------------------------------- */

   double position[3];  /* (m) Vector to target position.                   */

   double phi[6][6];    /* --  State transition matrix.                     */
   double phidot[6][6]; /* --  State transition matrix time derivative.     */

   double F[6][6];      /* --  Working area.                                */
   double Gmat[3][3];   /* --  Working area.                                */

} TargetBodyData; /* ---------------------------------------------------- */


#ifdef __cplusplus
}
#endif

#endif /* _TARGET_BODY_H_ : Do NOT put anything after this line! */

