/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Variable server test )
REFERENCES:                  ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
PROGRAMMERS:                 ( (Lindsay Landry) (L3) (9-12-2013) )
********************************************************************************/
#include <wchar.h>
#include <string.h>
#include <string>
#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/tc_proto.h"

#ifndef VS_HH
#define VS_HH

class VSTest {
	public:

		unsigned short port_num;
		const char * hostest;
		char got_read[80];
		double cycle_rate;

		TCDevice comm_device;

		char 				a;
		unsigned char 		b;
		short				c;
		unsigned short  	d;
		int 				e; /*  m xy-position */
		unsigned int 		f;
		long				g;
		unsigned long		h;
		float				i;
		double				j;
		long long			k;
		unsigned long long	l;
		bool                m;
		int 				n[5];
		std::string o;
		char * p;

		VSTest();
		~VSTest();

		int init();
		int default_vars();
		int vs_write(char* cmd_buffer);
		int vs_read();
		void printLine();

		int shutdown();

		// TEST FUNCTIONS
		int testAddRemove();
		int testExists();
		int testPause();
		int testSendOnce();
		int testUnits();
		int testStrings();

	private:
		int get_line(char* thing);
		int strcmp_IgnoringWhiteSpace(std::string s1, const char* s2);
};

#endif

