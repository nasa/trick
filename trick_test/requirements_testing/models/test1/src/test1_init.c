/*******************************************************************************
PURPOSE: (Initialize test 1.)
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "../include/test1.h"

int test1_init(TEST1* T) {
	printf("*****************Initialization routine*****************\n");
	printf("Changing i from %d to %d\n", T->i, 11);
	T->i = 11;
	printf("Changing ui from %u to %u\n", T->ui, 22);
	T->ui = 22;
	printf("Changing d from %f to %f\n", T->d, 33.0);
	T->d = 33.0;
	printf("Changing ss.si from %d to %d\n", T->ss.si, 44);
	T->ss.si = 44;
	printf("Changing ip from %d to %d\n", *(T->ip), T->i55);
	T->ip = &T->i55;
	printf("Changing da from {%f, %f, %f, %f, %f} to {%f, %f, %f, %f, %f}\n",
		T->da[0], T->da[1], T->da[2], T->da[3], T->da[4],
		11.0, 22.0, 33.0, 44.0, 55.0);
	T->da[0] = 11.0;
	T->da[1] = 22.0;
	T->da[2] = 33.0;
	T->da[3] = 44.0;
	T->da[4] = 55.0;
	printf("Changing dap from %f to %f\n", *(T->dap), T->da[1]);
	T->dap = &T->da[1];
	printf("Changing s from '%s' to '%s'\n", T->s, "initialization");
	strcpy(T->s, "initialization");
	return(0);
}
