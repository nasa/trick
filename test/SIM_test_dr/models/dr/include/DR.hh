/********************************* TRICK HEADER *******************************
*	PURPOSE:                     ( Data record test )
*	REFERENCES:                  ( None )
*	ASSUMPTIONS AND LIMITATIONS: ( None )
*	PROGRAMMERS:                 ( (Lindsay Landry) (L3) (7-22-2013) )
********************************************************************************/
#include <wchar.h>
#include <string>


#ifndef DR_HH
#define DR_HH

typedef enum {
	ONE			= 1,
	TWO			= 2,
	THREE		= 3,
	FOUR		= 4
} NUM_DEFS;

typedef struct {
    unsigned int var1 :14;
    unsigned int var2 :5;
    unsigned int var3 :12;
    unsigned int var4 :1;
} UINT_BITS;

typedef struct {
    int var1 :7;
    int var2 :16;
    int var3 :3;
    int var4 :6;
} INT_BITS;

typedef struct {
    unsigned char var1 :3;
    unsigned char var2 :2;
    unsigned char var3 :2;
    unsigned char var4 :1;
} UCHAR_BITS;

typedef struct {
    char var1 :2;
    char var2 :2;
    char var3 :2;
    char var4 :2;
} CHAR_BITS;

typedef struct {
    unsigned short var1 :4;
    unsigned short var2 :2;
    unsigned short var3 :9;
    unsigned short var4 :1;
} USHORT_BITS;

typedef struct {
    short var1 :8;
    short var2 :1;
    short var3 :2;
    short var4 :5;
} SHORT_BITS;

/* this struct should be the size of the largest member
 * (in this case, unsigned int) */
typedef struct {
	unsigned char var1 :3;
 	short var2 :8;
    int var3 :9;
    unsigned int var4 :12;
} MIX_BITS;


class DRTypes {
	public:

		char 				a;
		unsigned char 		b;
		short				c;
		unsigned short  	d;
		int 				e;
		unsigned int 		f;
		long				g;
		unsigned long		h;
		float				i;
		double				j;
		long long			k;
		unsigned long long	l;
		bool                m;
		NUM_DEFS			n;

		UINT_BITS uintB;
		INT_BITS intB;
		UCHAR_BITS ucharB;
		CHAR_BITS charB;
		USHORT_BITS ushortB;
		SHORT_BITS shortB;
		MIX_BITS mixB;


		DRTypes();
		~DRTypes();

		int init();

	private:
		void bitfieldInit(int bitSizes[], bool sign);
};

#endif

