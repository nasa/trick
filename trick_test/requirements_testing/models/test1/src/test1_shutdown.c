/*******************************************************************************
PURPOSE: (Shutdown test 1.)
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "../include/test1.h"

int test1_shutdown(TEST1* T) {
	printf("********************Shutdown routine********************\n");
	printf("Changing i from %d to %d\n", T->i, 111);
	T->i = 111;
	printf("Changing ui from %u to %u\n", T->ui, 222);
	T->ui = 222;
	printf("Changing d from %f to %f\n", T->d, 333.0);
	T->d = 333.0;
	printf("Changing ss.si from %d to %d\n", T->ss.si, 444);
	T->ss.si = 444;
	printf("Changing ip from %d to %d\n", *(T->ip), T->i555);
	T->ip = &T->i555;
	printf("Changing da from {%f, %f, %f, %f, %f} to {%f, %f, %f, %f, %f}\n",
		T->da[0], T->da[1], T->da[2], T->da[3], T->da[4],
		111.0, 222.0, 333.0, 444.0, 555.0);
	T->da[0] = 111.0;
	T->da[1] = 222.0;
	T->da[2] = 333.0;
	T->da[3] = 444.0;
	T->da[4] = 555.0;
	printf("Changing dap from %f to %f\n", *(T->dap), T->da[2]);
	T->dap = &T->da[2];
	printf("Changing s from '%s' to '%s'\n", T->s, "shutdown");
	strcpy(T->s, "shutdown");
	return(0);
}
