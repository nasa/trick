/******************************TRICK HEADER*************************************
PURPOSE:                     ( Test variable server )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( scheduled )
LIBRARY DEPENDENCY:          ( VS.o )
PROGRAMMERS:                 ( (Lindsay Landry) (L3) (9-12-2013) )
*******************************************************************************/
#include <iostream>
#include "../include/VS.hh"
#include "sim_services/VariableServer/include/variable_server_proto.h"

int VSTest::default_vars() {

	a = 'a';            //char
	b = 'b';            //unsigned char
	c = -1234;          //short
	d = 1234;           //unsigned short
	e = -123456;        //int
	f = 123456;         //unsigned int
	g = -1234567;       //long
	h = 123456789;      //unsigned long
	i = 1234.5678;      //float
	j = -1234.567890;   //double
	k = -12345678912345;//long long
	l = 12345678912345; //unsigned long
	m = false;          //boolean
	for (int i = 0; i < 5; i++) {
		n[i] = i;
	}
	o = std::string("You will rejoice to hear that no disaster has accompanied the commencement of an enterprise which you have regarded with such evil forebodings. I arrived here yesterday, and my first task is to assure my dear sister of my welfare and increasing confidence in the success of my undertaking.");
	p = "I am already far north of London, and as I walk in the streets of Petersburgh, I feel a cold northern breeze play upon my cheeks, which braces my nerves and fills me with delight. Do you understand this feeling?";
}
