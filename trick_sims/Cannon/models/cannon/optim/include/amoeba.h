/*
 * Vetter's implementation of the Nelder-Mead "amoeba" algorithm.
 * Was made strictly for Trick tutorial.  Was a whim.
 * Reference for algorithm (not code):
 * http://www.research.ibm.com/infoecon/paps/html/amec99_bundle/node8.html
 */

#ifndef AMOEBA_H
#define AMOEBA_H

#define AMOEBA_ALPHA 1.0     /* reflection constant */
#define AMOEBA_BETA  1.0     /* expansion constant */
#define AMOEBA_ZETA  0.5     /* contraction constant */
#define AMOEBA_ETA   0.5     /* shrinkage constant */

typedef enum {

        VERTICES,
        CALC_CENTROID_POINT,
        CALC_REFLECTION_POINT,
        REFLECT,
        EXPAND,
        CONTRACT,
        SHRINK,

} AMOEBA_STATE ;

typedef struct {
        int debug ;             /* -- Turn on some printing */
        int max_steps ;         /* -- Even if we don't find solution, break */
        int num_dims ;          /* -- Num dims of each vertice */
        int num_vertices ;      /* -- Num vertices in simplex */
        double epsilon ;        /* -- Bail if max dist simplex < epsilon */
        double** vertices ;     /* -- Rows are vectors of num_dims */
        double** init_simplex ; /* -- Initial simplex */
        double*  x_cent ;       /* -- Center of vertices-exclude worst vertex */
        double*  x_refl ;       /* -- Point of reflection */
        double*  x_expa ;       /* -- Point of expansion */
        double*  x_cont ;       /* -- Point of contraction */
        double*  curr_point ;   /* -- Current point of interest for sim use:
                                      Simplex vertice, centroid, reflection,
                                      expansion or contraction point */
        int curr_vertex ;       /* -- Current vertex for sim state machine */
        AMOEBA_STATE state ;    /* -- For sim's amoeba state machine */
} AMOEBA ;

/* Prototypes */
#ifdef __cplusplus
extern "C" {
#endif
void amoeba_init( AMOEBA* A, int num_dims, double epsilon, int max_steps,
                  double* simplex_point, double simplex_size ) ;
void amoeba_print( AMOEBA* A ) ;
void amoeba_print_point( int num_dims, double* point ) ;
void amoeba_go( AMOEBA* A ) ;
double my_func( double* point ) ;
void amoeba_quit( AMOEBA* A ) ;
void amoeba_order( AMOEBA* A ) ;
int amoeba_reflect( AMOEBA* A ) ;
void amoeba_calc_reflection_point( AMOEBA* A ) ;
int amoeba_expand( AMOEBA* A ) ;
void amoeba_calc_expansion_point( AMOEBA* A ) ;
int amoeba_contract( AMOEBA* A ) ;
void amoeba_calc_contraction_point( AMOEBA* A ) ;
void amoeba_shrink( AMOEBA* A ) ;
int amoeba_satisfied( AMOEBA* A ) ;
double amoeba_distance_between( int num_dims, double* x, double* y) ;
int amoeba_compare_vertices( const void* a, const void* b) ;
#ifdef __cplusplus
}
#endif
#endif
