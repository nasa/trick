/******************************************************************************
PURPOSE: (Tests requirements: 3.1.2.9, 3.1.2.9.1-5)
******************************************************************************/

#ifndef _test1_h_
#define _test1_h_

typedef struct {
	int si;
} SS;

typedef struct {
	int i;
	unsigned int ui;
	double d;
	SS ss;
	int* ip;
	double da[5];
	double* dap;
	char s[15];
	int i5;
	int i55;
	int i555;
} TEST1;

#endif
