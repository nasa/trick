/*
 * Author  : Peter Mosbach
 * Date    : August 2003
 * Desc    : Function for matching an error code with error message
 *           Note: User must free() msg string passed back
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eqparse_protos.h"

/* Global error number */
extern int eqp_errno ;

char *eqperror(int code)
{
	char* errorprintout ;

       errorprintout = ( char* ) malloc ( 128 ) ;

	sprintf(errorprintout,"\nError %d in eqparse.c:",code) ;
	switch (code){
		case 1:
			strcat(errorprintout,"\nSyntax Error: ) Expected") ;
			break ;
		case 2:
                        strcat(errorprintout,"\nSyntax Error: ] Expected") ;
			break ;
		case 3:
                        strcat(errorprintout,"\nSyntax Error: } Expected") ;
			break ;
		case 4:
                        strcat(errorprintout,"\nSyntax Error: Operand without"
                                             "operator") ;
			break ;
		case 5:
                        strcat(errorprintout,"\nSyntax Error: Operator without "
                                             "operand") ;
			break ;
		case 6:
                        strcat(errorprintout,"\nMath Error: Division by Zero");
			break ;
		case 7:
                        strcat(errorprintout,"\nMath Error: Non-convergent") ;
			break ;
		case 8:
                        strcat(errorprintout,"\nMath Error: The domain of "
                                             "besy0() is (0,inf)") ;
			break ;
		case 9:
                        strcat(errorprintout,"\nMath Error: Invalid argument "
                                             "for acos()") ;
			break ;
		case 10:
                        strcat(errorprintout,"\nMath Error: Invalid argument "
                                             "for asin()") ;
			break ;
		case 11:
                        strcat(errorprintout,"\nMath Error: Invalid argument "
                                             "for atanh()") ;
			break ;
		case 12:
                        strcat(errorprintout,"\nStack Error: Out of memory") ;
			break ;
		case 13:
                        strcat(errorprintout,"\nStack Error: Stack is empty") ;
			break ;
		case 14:
                        strcat(errorprintout,"\nParse Error: Can't find "
                                             "function") ;
			break ;
		case 15:
			strcat(errorprintout,"\nParse Error: Illegal character "
                                          "in equation") ;
			break ;
		case 16:
                        strcat(errorprintout,"\nMath Error: The domain of "
                                             "invnorm() is (0,1)") ;
			break ;
		case 17:
                        strcat(errorprintout,"\nMath Error: Missing argument") ;
			break ;
		case 18:
                        strcat(errorprintout,"\nMath Error: The domain of "
                                             "besy1() is (0,inf)") ;
			break ;
		case 19:
                        strcat(errorprintout,"\nMath Error: The domain of "
                                             "inverf() is (-1,1)") ;
			break ;
		case 20:
                        strcat(errorprintout,"\nMath Error: The domain of "
                                             "log10() is (0,inf)") ;
			break ;
		case 21:
                        strcat(errorprintout,"\nMath Error: The domain of "
                                             "log() is (0,inf)") ;
			break ;
		case 22:
                        strcat(errorprintout,"\nInput Error: NULL equation"
			                        "string.") ;
			break ;
		case 23:
			strcat(errorprintout,"\nSyntax Error: Missing (") ;
			break ;
		case 24:
			strcat(errorprintout,"\nSyntax Error: Missing [") ;
			break ;
		case 25:
			strcat(errorprintout,"\nSyntax Error: Missing {") ;
			break ;
	}

	return (errorprintout) ;

}

void error(char code)
{
	switch (code){
	case 0:
	       eqp_errno = 1 ;
	       break;
	case 1:
	       eqp_errno = 2 ;
	       break;
	case 2:
	       eqp_errno = 3 ;
	       break;

	case 3:
	       eqp_errno = 4 ;
	       break;
	case 4:
	       eqp_errno = 5 ;
	       break;
	case 5:
		eqp_errno = 23 ;
		break;
	case 6:
		eqp_errno = 24 ;
		break;
	case 7:
		eqp_errno = 25 ;
		break;
	}

}
