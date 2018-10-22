/*
 * Author  : Peter Mosbach
 * Date    : August 2003
 * Desc    : Evaluate the postfix form of the infix input
 *           (it's a well known algorithm)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "eqparse_protos.h"


/* Global error number */
extern int eqp_errno;

double eval(stack * stk, stack1 * no)
{

        stack1 nos = NULL;
        double x, y, z;
        char ch;
        ldiv_t ldivt;
        while (!empty(*stk)) {
                *stk = pop(*stk, &ch);
                if (ch == 'N') {
                        *no = pop1(*no, &x);
                        nos = push1(nos, x);
                } else if (ch == (char)0xD8) {
                } else {
                        nos = pop1(nos, &y);
                        if ((ch >= (char)0xA2 && ch <= (char)0xCA) && ch != (char)0xAA && ch != (char)0xCB
                            && ch != (char)0xB9)
                                x = 0;
                        else
                                nos = pop1(nos, &x);

                        switch (ch) {
                        case '+':
                                z = add(x, y);
                                break;
                        case '-':
                                z = dif(x, y);
                                break;
                        case '*':
                                z = mul(x, y);
                                break;

                        case '/':
                                if (y == 0.0) {
                                        eqp_errno = 6;
                                        return (0);
                                }
                                z = divide(x, y);
                                break;
                        case '^':
                                z = pow(x, y);
                                break;
                        case '%':
                                z = mod(x, y);
                                break;

                                /* The inline comments give the equivalent
                                 * gnuplot function
                                 */
                        case (char)0xA2:      /* abs(x) */
                                z = fabs(y);
                                break;
                        case (char)0xA3:      /* acos(x) */
                                if (y > 1 || y < -1) {
                                        /*
                                           eqp_errno = 9 ;
                                           return(0);
                                         */
                                }
                                z = acos(y);
                                break;
                        case (char)0xA4:      /* acosh(x) */
                                z = acosh(y);
                                break;
                        case (char)0xA6:      /* asin(x) */
                                if (y > 1 || y < -1) {
                                        /*
                                           eqp_errno = 10 ;
                                           return(0);
                                         */
                                }
                                z = asin(y);
                                break;
                        case (char)0xA7:      /* asinh(x) */
                                z = asinh(y);
                                break;
                        case (char)0xA8:      /* atan(x) */
                                z = atan(y);
                                break;
                        case (char)0xA9:      /* atanh(x) */
                                if (y > 1 || y < -1) {
                                        eqp_errno = 11;
                                        return (0);
                                }
                                z = atanh(y);
                                break;
                        case (char)0xAA:      /* atan2(y,x) */
                                z = atan2(x, y);
                                break;
                        case (char)0xAB:      /* besj0(x) */
                                z = j0(y);
                                break;
                        case (char)0xAC:      /* besj1(x) */
                                z = j1(y);
                                break;
                        case (char)0xAD:      /* besy0(x) */
                                if (y <= 0) {
                                        eqp_errno = 8;
                                        return (0);
                                }
                                z = y0(y);
                                break;
                        case (char)0xAE:      /* besy1(x) */
                                if (y <= 0) {
                                        eqp_errno = 18;
                                        return (0);
                                }
                                z = y1(y);
                                break;
                        case (char)0xAF:      /* ceil(x) */
                                z = ceil(y);
                                break;
                        case (char)0xB1:      /* cos(x) */
                                z = cos(y);
                                break;
                        case (char)0xB2:      /* cosh(x) */
                                z = cosh(y);
                                break;
                        case (char)0xCB:      /* div(num,denom) */
                                ldivt = ldiv((int) x, (int) y);
                                z = (double) ldivt.quot;
                                break;
                        case (char)0xB3:      /* erf(x) */
                                z = erf(y);
                                break;
                        case (char)0xB4:      /* erfc(x) */
                                z = erfc(y);
                                break;
                        case (char)0xB5:      /* exp(x) */
                                z = exp(y);
                                break;
                        case (char)0xB6:      /* floor(x) */
                                z = floor(y);
                                break;
                        case (char)0xB7:      /* gamma(x) */
                                z = exp(lgamma(y));
                                break;
                        case (char)0xBB:      /* int(x) */
                                z = rint(y);
                                if (z > 0 && z > y)
                                        z--;
                                if (z < 0 && z < y)
                                        z++;
                                break;
                        case (char)0xBC:      /* inverf(x) */
                                if (y <= -1 || y >= 1) {
                                        eqp_errno = 19;
                                        return (0);
                                }
                                z = inverf(y);
                                break;
                        case (char)0xBD:      /* invnorm(x) */
                                if (y <= 0 || y >= 1) {
                                        eqp_errno = 16;
                                        return (0);
                                }
                                z = sqrt(2) * inverf(2 * y - 1);
                                break;
                        case (char)0xBE:      /* lgamma(x) */
                                z = lgamma(y);
                                break;
                        case (char)0xBF:      /* log(x) */
                                if (y <= 0) {
                                        eqp_errno = 21;
                                        return (0);
                                }
                                z = log(y);
                                break;
                        case (char)0xC0:      /* log10(x) */
                                z = log10(y);
                                if (y <= 0) {
                                        eqp_errno = 20;
                                        return (0);
                                }
                                break;
                        case (char)0xC1:      /* norm(x) */
                                z = .5 * (1 + erf(y / sqrt(2)));
                                break;
                        case (char)0xC2:      /* rand(x) */
                                z = ((double) rand()) / RAND_MAX;
                                break;
                        case (char)0xC4:      /* sgn(x) */
                                if (y > 0)
                                        z = 1;
                                else if (y < 0)
                                        z = -1;
                                else
                                        z = 0;
                                break;
                        case (char)0xC5:      /* sin(x) */
                                z = sin(y);
                                break;
                        case (char)0xC6:      /* sinh(x) */
                                z = sinh(y);
                                break;
                        case (char)0xC7:      /* sqrt(x) */
                                z = sqrt(y);
                                break;
                        case (char)0xC8:      /* tan(x) */
                                z = tan(y);
                                break;
                        case (char)0xC9:      /* tanh(x) */
                                z = tanh(y);
                                break;
                        }
                        nos = push1(nos, z);
                }
        }
        nos = pop1(nos, &z);
#ifdef DEBUG
        printf("\n%lf", z);
#endif
        return (z);
}
