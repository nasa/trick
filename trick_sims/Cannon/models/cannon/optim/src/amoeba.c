/*********************************** TRICK HEADER **************************
PURPOSE:                     ( Amoeba )
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/amoeba.h"

double my_func( double* point ) {

        /* In the case of the simulation, the function value is
         * actually stored in the point
         */
        int num_dims = 4 ;

        return( point[num_dims] ) ;
}

void amoeba_go( AMOEBA* A ) {

        int step ;

        step = 1 ;
        while( ! amoeba_satisfied(A) && step < A->max_steps ) {

                step++ ;

                amoeba_order( A ) ;

                if ( amoeba_reflect( A ) ) {
                        continue ;
                }
                if ( amoeba_expand( A ) ) {
                        continue ;
                }
                if ( amoeba_contract( A ) ) {
                        continue ;
                }

                amoeba_shrink( A )  ;
        }
        amoeba_order( A ) ;

        fprintf(stderr,"\n\nAmoeba search concluded.\n") ;
        fprintf(stderr,"Number iterations: %d \n", step) ;
        fprintf(stderr,"x=(");
        amoeba_print_point( A->num_dims, A->vertices[0] ) ;
        fprintf(stderr,")\n");
        fprintf(stderr,"F(x)=%.6lf\n\n", my_func(A->vertices[0]));
}

void amoeba_shrink( AMOEBA* A ) {

        int ii, jj ;

        double* x ;
        double* x_0 ;

        x_0 = A->vertices[0] ;

        if ( A->debug ) {
                fprintf(stderr,"Shrinking simplex...\n");
                amoeba_print( A ) ;
                fprintf(stderr,"\n\n");
        }

        for ( ii = 0 ; ii < A->num_vertices ; ii++ ) {
                x = A->vertices[ii] ;
                for ( jj = 0 ; jj < A->num_dims ; jj++ ) {
                        x[jj] = x_0[jj] + AMOEBA_ETA*( x[jj] - x_0[jj] ) ;
                }
        }

        if ( A->debug ) {
                fprintf(stderr,"Shrank simplex.\n");
                amoeba_print( A ) ;
                fprintf(stderr,"\n\n");
        }
}

void amoeba_calc_contraction_point( AMOEBA* A ) {

        int ii ;
        double* x_c ;
        double* x_n ;
        double* x_cont ;

        /* Short hand */
        x_c = A->x_cent ;
        x_n = A->vertices[A->num_vertices-1] ;
        x_cont = A->x_cont ;

        /* Calculate contraction point
         * x_contract = x_center + zeta*(x_center - x_worst)
         */
        for ( ii = 0 ; ii < A->num_dims ; ii++ ) {
                x_cont[ii] = x_c[ii] + AMOEBA_ZETA*(x_c[ii] - x_n[ii]) ;
        }
}


int amoeba_contract( AMOEBA* A ) {

        int ii ;
        double* x_r ;
        double* x_n ;
        double* x_n_1 ;
        double* x_c ;
        double* x_cont ;

        /* Short hand */
        x_c = A->x_cent ;
        x_r = A->x_refl ;
        x_n = A->vertices[A->num_vertices-1] ;
        x_n_1 = A->vertices[A->num_vertices-2] ;
        x_cont = A->x_cont ;

        if ( A->debug ) {
                fprintf(stderr,">> Try contraction.\n");
        }

        /* Contract if F(x_r) < F(x_n-1) */
        if ( my_func(x_r) >= my_func(x_n_1) ) {
                if ( A->debug ) {
                        fprintf(stderr,">> Reject contraction.\n");
                }
                return 0 ;
        }

        /* Calculate contraction point
         * x_contract = x_center + zeta*(x_center - x_worst)
         */
        for ( ii = 0 ; ii < A->num_dims ; ii++ ) {
                x_cont[ii] = x_c[ii] + AMOEBA_ZETA*(x_c[ii] - x_n[ii]) ;
        }

        if ( my_func(x_cont) >= my_func(x_n) ) {
                /* Accept contraction point */

                if ( A->debug ) {
                        fprintf(stderr,">> Accept contraction about x_cont(");
                        amoeba_print_point( A->num_dims, x_cont ) ;
                        fprintf(stderr,")  F(x_cont)=%.2lf \n",
                                my_func(x_cont) ) ;
                }

                for ( ii = 0 ; ii < A->num_dims+1 ; ii++ ) {
                        x_n[ii] = x_cont[ii] ;
                }

                return 1 ;

        } else {
                /* Reject contraction point */
                if ( A->debug ) {
                        fprintf(stderr,">> Reject contraction about x_cont(");
                        amoeba_print_point( A->num_dims, x_cont ) ;
                        fprintf(stderr,")  F(x_cont)=%.2lf \n",
                                my_func(x_cont) ) ;
                }

                return 0 ;
        }

}

void amoeba_calc_expansion_point( AMOEBA* A ) {

        int ii ;
        double* x_r ;
        double* x_c ;
        double* x_e ;

        /* Short hand */
        x_r = A->x_refl ;
        x_c = A->x_cent ;
        x_e = A->x_expa ;

        /* Calculate expansion point
         * x_expa = x_refl + beta*(x_refl - x_cent)
         */
        for ( ii = 0 ; ii < A->num_dims ; ii++ ) {
                x_e[ii] = x_r[ii] + AMOEBA_BETA*(x_r[ii] - x_c[ii]) ;
        }
}


int amoeba_expand( AMOEBA* A ) {

        int ii ;
        double* x_0 ;
        double* x_r ;
        double* x_n ;
        double* x_e ;

        /* Short hand */
        x_0 = A->vertices[0] ;
        x_r = A->x_refl ;
        x_n = A->vertices[A->num_vertices-1] ;
        x_e = A->x_expa ;

        if ( A->debug ) {
                fprintf(stderr,">> Try expansion.\n");
        }

        /* Expand if F(x_r) > F(x_0) */
        if ( my_func(x_r) <= my_func(x_0) ) {
                if ( A->debug ) {
                        fprintf(stderr,">> Reject expansion.\n");
                }
                return 0 ;
        }

        if ( A->debug ) {
                fprintf(stderr,">> Accept expansion.\n");
        }

        if ( my_func(x_e) >= my_func(x_r) ) {
                /* Accept expansion point */
                if ( A->debug ) {
                        fprintf(stderr,"Accept x_e(");
                        amoeba_print_point( A->num_dims, x_e ) ;
                        fprintf(stderr,")  F(x_e)=%.2lf \n", my_func(x_e) ) ;
                }

                for ( ii = 0 ; ii < A->num_dims+1 ; ii++ ) {
                        x_n[ii] = x_e[ii] ;
                }
        } else {
                /* Accept reflection point */
                if ( A->debug ) {
                        fprintf(stderr,"Accept x_r(");
                        amoeba_print_point( A->num_dims, x_r ) ;
                        fprintf(stderr,")  F(x_r)=%.2lf \n", my_func(x_r) ) ;
                }
                for ( ii = 0 ; ii < A->num_dims+1 ; ii++ ) {
                        x_n[ii] = x_r[ii] ;
                }
        }

        return 1 ;
}

double amoeba_distance_between( int num_dims, double* x, double* y ) {

        int ii ;
        double sum_squares ;

        sum_squares = 0 ;
        for ( ii = 0 ; ii < num_dims ; ii++ ) {
                sum_squares += (x[ii] - y[ii])*(x[ii] - y[ii]);
        }

        return ( sqrt(sum_squares) ) ;
}

int amoeba_satisfied( AMOEBA* A ) {

        /*
         * Find max distance between points in the simplex
         * If under epsilon, amoeba satisfied
         * If this isn't satisfied, see if F(x) satisfied within epsilon
         */

        int ii, jj ;
        double dist, max_dist ;
        double* x ;
        double* y ;
        double f1 ;
        double f2 ;

        /*
         * Check max dist between points in simplex
         * If the amoeba gets too small, no point in shrinking
         * infintessimally (sp?).
         */
        max_dist = 0.0 ;
        for ( ii = 0 ; ii < A->num_vertices - 1 ; ii++ ) {
                x = A->vertices[ii] ;
                for ( jj = ii+1 ; jj < A->num_vertices ; jj++ ) {
                        y = A->vertices[jj] ;
                        dist = amoeba_distance_between( A->num_dims, x, y ) ;
                        if ( dist > max_dist ) {
                                max_dist = dist ;
                        }
                }
        }
        if ( max_dist < A->epsilon ) {
                return 1 ;
        }

        /*
         * See if diff between function vals less than epsilon
         */
        max_dist = 0 ;
        for ( ii = 0 ; ii < A->num_vertices - 1 ; ii++ ) {
                f1 = A->vertices[ii][A->num_dims] ;
                for ( jj = ii+1 ; jj < A->num_vertices ; jj++ ) {
                        f2 = A->vertices[jj][A->num_dims] ;
                        dist = fabs( f1 - f2 ) ;
                        if ( dist > max_dist ) {
                                max_dist = dist ;
                        }
                }
        }
        if ( max_dist < A->epsilon ) {
                return 1 ;
        }

        /*
         * Unsatisfied amoeba :-(
         */
        return 0 ;
}

void amoeba_calc_reflection_point( AMOEBA* A ) {

        int ii ;
        double* x_r ;
        double* x_n ;
        double* x_c ;

        /* Short hand */
        x_r = A->x_refl ;
        x_n = A->vertices[A->num_vertices-1] ;
        x_c = A->x_cent ;

        /* Calculate reflection point
         * x_refl = x_cent + alpha*(x_cent - x_worst)
         */
        for ( ii = 0 ; ii < A->num_dims ; ii++ ) {
                x_r[ii] = x_c[ii] + AMOEBA_ALPHA*(x_c[ii] - x_n[ii]) ;
        }
}


int amoeba_reflect( AMOEBA* A ) {

        int ii ;
        double* x_0 ;
        double* x_r ;
        double* x_n ;

        if ( A->debug ) {
                fprintf(stderr,">> Try reflection.\n");
        }

        /* Short hand */
        x_0 = A->vertices[0] ;
        x_r = A->x_refl ;
        x_n = A->vertices[A->num_vertices-1] ;

        if ( my_func(x_0) >= my_func(x_r) && my_func(x_r) > my_func(x_n) ) {
                /* Accept reflection point --- replace worst point with x_r */
                if ( A->debug ) {
                        fprintf(stderr,"Accept reflection x_r(");
                        amoeba_print_point( A->num_dims, x_r ) ;
                        fprintf(stderr,")  F(x_r)=%.2lf \n", my_func(x_r) ) ;
                }
                for ( ii = 0 ; ii < A->num_dims+1 ; ii++ ) {
                        x_n[ii] = x_r[ii] ;
                }
                return 1 ;
        } else {
                if ( A->debug ) {
                        fprintf(stderr,"Reject reflection x_r(");
                        amoeba_print_point( A->num_dims, x_r ) ;
                        fprintf(stderr,")  F(x_r)=%.2lf \n", my_func(x_r) ) ;
                }
                return 0 ;
        }
}

void amoeba_order( AMOEBA* A ) {

        int ii, jj ;

        /* Order vertices based on function results.
         * Order from best to worst (i.e. max to min)
         */
        qsort( A->vertices,
               A->num_vertices, sizeof(double*),
               amoeba_compare_vertices) ;

        /* Calculate centroid --- excluding worst point */
        for ( jj = 0 ; jj < A->num_dims ; jj++ ) {
                A->x_cent[jj] = 0 ;
        }
        for ( jj = 0 ; jj < A->num_dims ; jj++ ) {
                for ( ii = 0 ; ii < A->num_vertices - 1 ; ii++ ) {
                        A->x_cent[jj] +=  A->vertices[ii][jj] ;
                }
                A->x_cent[jj] /= (double)(A->num_vertices - 1.0 ) ;
        }

        if ( A->debug ) {
                fprintf(stderr,"Ordered Amoeba.\n");
                amoeba_print( A ) ;
                fprintf(stderr,"\n");
        }
}

int amoeba_compare_vertices( const void* ap, const void* bp ) {

        double a, b ;

        a = my_func( *((double**) ap) ) ;
        b = my_func( *((double**) bp) ) ;

        if ( a > b ) {
                return -1 ;
        } else if ( a < b ) {
                return 1 ;
        } else {
                return 0 ;
        }
}

/*
 * num_dims: Number of dimensions in domain space.
 *           Example: If the function we are maximizing is F(x,y,z)
 *                    The number of dims is 3.
 *
 * epsilon: The error tolerance before amoeba algorithm stops
 *          A good number is something like: 1.0e-9
 *
 * max_steps: Even if we don't reach a solution by "max_steps", stop.
 *
 * We have to create a first simplex to begin amoeba algorithm.
 * There are two ways to do this.
 * First :  Send simplex_point and simplex_size
 *          This defines a simplex with one vertex at "simplex_point"
 *          All other points are spaced "simplex_size" in the unit vec direction
 * Second : If "simplex_point" is NULL, it is assumed user setup
 *          A->init_simplex before entering amoeba_init.
 */
void amoeba_init(  AMOEBA* A, int num_dims, double epsilon, int max_steps,
                  double* simplex_point, double simplex_size ) {

        int ii, jj ;

        if ( A->init_simplex == NULL && simplex_point == NULL ) {
                fprintf(stderr, "Initial simplex undefined by user.\n"
                                "Set A->init_simplex or "
                                "simplex_point and simplex_size\n") ;
                exit(-1) ;
        }

        A->state = VERTICES ;
        A->num_dims = num_dims ;
        A->num_vertices = num_dims + 1 ;
        A->curr_vertex = 0 ;
        A->epsilon = epsilon ;
        A->max_steps = max_steps ;

        A->vertices = (double**) malloc (A->num_vertices*sizeof(double));
        for ( ii = 0 ; ii < A->num_vertices ; ii++ ) {
                A->vertices[ii] = (double*) malloc
                                     ((A->num_dims+1)*sizeof(double));
        }

        if ( simplex_point != NULL ) {
                /*
                 * Use "simplex_point" as one vertice of first simplex. Then
                 * move in "simplex_size" along unit vectors to create the
                 * rest of the first guess simplex.
                 *
                 * First_simplex = simplex_point + simplex_size*unit_vecs
                 */
                for ( jj = 0 ; jj < A->num_dims ; jj++ ) {
                        A->vertices[0][jj] = simplex_point[jj] ;
                }
                /* Load function vals with zero */
                A->vertices[0][jj] = 0.0 ;

                for ( ii = 1 ; ii < A->num_vertices ; ii++ ) {
                        for ( jj = 0 ; jj < A->num_dims ; jj++ ) {
                                if ( jj+1 == ii ) {
                                        A->vertices[ii][jj] =
                                              simplex_point[jj] - simplex_size ;
                                } else {
                                       A->vertices[ii][jj] = simplex_point[jj] ;
                                }
                        }
                        /* Load function vals with zero */
                        A->vertices[ii][jj] = 0.0 ;
                }

        } else {
                /* Assume user has defined first simplex */
                for ( ii = 0 ; ii < A->num_vertices ; ii++ ) {
                        for ( jj = 0 ; jj < A->num_dims ; jj++ ) {
                                A->vertices[ii][jj] = A->init_simplex[ii][jj] ;
                        }
                        /* Load function vals with zero */
                        A->vertices[ii][jj] = 0.0 ;
                }
        }

        A->x_cent = (double*) malloc ( (A->num_dims+1) * sizeof(double) ) ;
        A->x_refl = (double*) malloc ( (A->num_dims+1) * sizeof(double) ) ;
        A->x_expa = (double*) malloc ( (A->num_dims+1) * sizeof(double) ) ;
        A->x_cont = (double*) malloc ( (A->num_dims+1) * sizeof(double) ) ;

        /* Init func results */
        A->x_cent[A->num_dims] = 0.0 ;
        A->x_refl[A->num_dims] = 0.0 ;
        A->x_expa[A->num_dims] = 0.0 ;
        A->x_cont[A->num_dims] = 0.0 ;


        if ( A->debug ) {
                fprintf(stderr,"Initial simplex with function evals.\n");
                amoeba_print( A ) ;
                fprintf(stderr,"\n\n");
        }
}

void amoeba_quit( AMOEBA* A ) {

        int ii ;

        for ( ii = 0 ; ii < A->num_vertices ; ii++ ) {
                free( A->vertices[ii] ) ;
        }
        free( A->vertices ) ;
        free( A->x_cent ) ;
        free( A->x_refl ) ;
        free( A->x_expa ) ;
        free( A->x_cont ) ;
}

void amoeba_print_point( int num_dims, double* point ) {

        int ii;

        for ( ii = 0 ; ii < num_dims-1 ; ii++ ) {
                fprintf(stderr,"%.2lf   ", point[ii]);
        }
        fprintf(stderr,"%.2lf", point[num_dims-1]);
}

void amoeba_print( AMOEBA* A ) {

        int ii, jj ;

        for ( ii = 0 ; ii < A->num_vertices ; ii++ ) {
                for ( jj = 0 ; jj < A->num_dims ; jj++ ) {
                        fprintf(stderr,"%.2lf   ", A->vertices[ii][jj] ) ;
                }
                fprintf(stderr," > %.2lf \n", A->vertices[ii][jj]);
        }

}
