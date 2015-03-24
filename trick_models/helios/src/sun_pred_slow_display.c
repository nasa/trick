/********************************* TRICK HEADER *******************************
PURPOSE:
      (Predict the path of the SUN across the sky.)
LIBRARY DEPENDENCY:
      ((JD.o))
PROGRAMMERS:
      (((John M. Penn) (L-3 Titan) (Oct 8, 2006) (Trick Demo) ))
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "sun_pred.h"

#define HOURS_PER_DAY    24.0
#define MINUTES_PER_DAY  1440.0
#define SECONDS_PER_DAY  86400.0

#define DEGREE_SIGN                 0x00B0
#define SUN                         0x2609
#define POSITION_INDICATOR          0x2316
#define CLOCK                       0x231A

#define GREEK_SMALL_LETTER_ALPHA    0x03B1
#define GREEK_SMALL_LETTER_BETA     0x03B2
#define GREEK_SMALL_LETTER_GAMMA    0x03B3
#define GREEK_SMALL_LETTER_DELTA    0x03B4
#define GREEK_SMALL_LETTER_EPSILON  0x03B5
#define GREEK_SMALL_LETTER_ZETA     0x03B6
#define GREEK_SMALL_LETTER_ETA      0x03B7
#define GREEK_SMALL_LETTER_THETA    0x03B8
#define GREEK_SMALL_LETTER_IOTA     0x03B9
#define GREEK_SMALL_LETTER_KAPPA    0x03BA
#define GREEK_SMALL_LETTER_LAMBDA   0x03BB
#define GREEK_SMALL_LETTER_MU       0x03BC
#define GREEK_SMALL_LETTER_NU       0x03BD
#define GREEK_SMALL_LETTER_XI       0x03BE
#define GREEK_SMALL_LETTER_OMICRON  0x03BF
#define GREEK_SMALL_LETTER_PI       0x03C0
#define GREEK_SMALL_LETTER_RHO      0x03C1
#define GREEK_SMALL_LETTER_SIGMA    0x03C3
#define GREEK_SMALL_LETTER_TAU      0x03C4
#define GREEK_SMALL_LETTER_UPSILON  0x03C5
#define GREEK_SMALL_LETTER_PHI      0x03C6
#define GREEK_SMALL_LETTER_CHI      0x03C7
#define GREEK_SMALL_LETTER_PSI      0x03C8
#define GREEK_SMALL_LETTER_OMEGA    0x03C9


/* Prototype */
int send_hs(FILE * fp, char *format, ...);

int sun_pred_slow_display(
      SUN_PRED* S )
{
	char tmp_s[20];
	char message[500];

	message[0] = '\0';
	sprintf(tmp_s,"%lc",POSITION_INDICATOR);
	strcat(message,tmp_s);
	sprintf(tmp_s,"  %lc %5.2f%lc",GREEK_SMALL_LETTER_PHI, S->observer_latitude, DEGREE_SIGN);
	strcat(message,tmp_s);
	sprintf(tmp_s,"  L %5.2f%lc", S->observer_longitude, DEGREE_SIGN);
	strcat(message,tmp_s);
	sprintf(tmp_s,"  %lc %5.2f%lc",GREEK_SMALL_LETTER_ALPHA, S->right_ascension, DEGREE_SIGN);
	strcat(message,tmp_s);
	sprintf(tmp_s,"  %lc %5.2f%lc",GREEK_SMALL_LETTER_DELTA, S->declination, DEGREE_SIGN);
	strcat(message,tmp_s);
	send_hs(stdout,"%s\n",message);

	return 0 ;
}

