/*******************************************************************************
PURPOSE: (Change the data being recorded.)
*******************************************************************************/

#include "../include/test3.h"
#include <math.h>

int test3_sched(TEST3  *T,
                double *time) {

    int integer_time;

    T->s++;           /* Changes every time */
    T->i++;
    T->ui++;
    T->l++;
    T->f++;
    T->d1 = sin( 3.14159265 * *time);

    integer_time = (int)*time ;

    if ( integer_time % 2 == 0 ) {
        T->d2 = *time - integer_time ; 
    } else {
        T->d2 = 0.0 ;
    }



}
