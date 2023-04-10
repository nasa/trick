/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Variable server test )
REFERENCES:                  ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
PROGRAMMERS:                 ( (Lindsay Landry) (L3) (9-12-2013) ) ( (Jackie Deans) (CACI) (11-30-2022) )
********************************************************************************/
#include <wchar.h>
#include <string.h>
#include <string>
#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/tc_proto.h"

#ifndef VS_HH
#define VS_HH

typedef struct {
	unsigned char var1 :3;
 	short var2 :8;
    int var3 :9;
    unsigned int var4 :12;
} bitfield;


class VSTest {
	public:
		char 				a;
		unsigned char 		b;
		short				c;
		unsigned short  	d;
		int 				e; 	/*  m xy-position */
		unsigned int 		f;
		long				g;
		unsigned long		h;
		float				i;
		double				j;
		long long			k;
		unsigned long long	l;
		bool                m;
		int 				n[5];
		std::string 		o;
		char * 				p;
		wchar_t *			q; 	/**< trick_chkpnt_io(**) */

		bitfield 			my_bitfield;	

		int large_arr[4000];

		int blocked_from_input; 	/** trick_io(*o) */
		int blocked_from_output; 	/** trick_io(*i) */

		int status;

		VSTest();
		~VSTest();

		int init();
		int default_vars();
		
		int shutdown();

		int success();
		int fail();

		void throw_exception();

		const char *status_messages[3] = {
			"Variable Server Test Success",
			"Variable Server Test Failure",
			"Client failed to connect"
		};

};

#endif

