
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*
 * ---------------- www.spacesimulator.net --------------
 *   ---- Space simulators and 3d engine tutorials ----
 *
 * (c) Copyright 2002 Damiano Vitulli <info@spacesimulator.net>
 *
 * ALL RIGHTS RESERVED
 *
 *
 * Tutorial 3: 3d engine - Texture mapping with OpenGL!
 *
 * Include File: texture.cpp
 *
 */

/*
Linux port by Panteleakis Ioannis
mail: pioann@csd.auth.gr

just run: make and you are done.
of course you may need to change the makefile
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if ( __APPLE__ )
#       include <GLUT/glut.h>
#       include <OpenGL/glu.h>
#else
#       include <GL/glut.h>
#       include <GL/glu.h>
#endif
#include "texture.h"        

#include "trick_utils/comm/include/trick_byteswap.h"

/**********************************************************
 *
 * VARIABLES DECLARATION
 *
 *********************************************************/

/*** Counter to keep track of the last loaded texture ***/
int num_texture=-1;



/**********************************************************
 *
 * FUNCTION LoadBitmap(char *)
 *
 * This function loads a bitmap file and return the OpenGL 
 * reference ID to use that texture
 *
 *********************************************************/
int LoadBitmap(char *filename)
{
    FILE * file;
    char temp;
    long i;

    int local_order;
    TRICK_GET_BYTE_ORDER(local_order);

    BITMAPINFOHEADER infoheader;

    num_texture++; // The counter of the current texture is increased

    // Open the file for reading
    if( (file = fopen(filename, "rb"))==NULL) return (-1); 

    fseek(file, 18, SEEK_CUR);  /* start reading width & height */
    fread(&infoheader.biWidth, sizeof(int), 1, file);
    if ( local_order == TRICK_BIG_ENDIAN ) {
            infoheader.biWidth = trick_byteswap_int(infoheader.biWidth);
    }

    fread(&infoheader.biHeight, sizeof(int), 1, file);
    if ( local_order == TRICK_BIG_ENDIAN ) {
            infoheader.biHeight = trick_byteswap_int(infoheader.biHeight);
    }

    fread(&infoheader.biPlanes, sizeof(short int), 1, file);
    if ( local_order == TRICK_BIG_ENDIAN ) {
            infoheader.biPlanes = trick_byteswap_short(infoheader.biPlanes);
    }
    if (infoheader.biPlanes != 1) {
	    printf("Planes from %s is not 1: %u\n", 
                  filename, infoheader.biPlanes);
	    return 0;
    }

    // read the bpp
    fread(&infoheader.biBitCount, sizeof(unsigned short int), 1, file);
    if ( local_order == TRICK_BIG_ENDIAN ) {
            infoheader.biBitCount = trick_byteswap_short(infoheader.biBitCount);
    }
    if (infoheader.biBitCount != 24) {
      printf("Bpp from %s is not 24: %d\n", filename, infoheader.biBitCount);
      return 0;
    }

    fseek(file, 24, SEEK_CUR);

    // read the data.
    infoheader.data = (char *) malloc(infoheader.biWidth * 
                                      infoheader.biHeight * 3);
    if (infoheader.data == NULL) {
	    printf("Error allocating memory for color-corrected image data\n");
	    return 0;
    }

    if ((i = fread(infoheader.data, infoheader.biWidth * 
                                    infoheader.biHeight * 3, 1, file)) != 1) {
	    printf("Error reading image data from %s.\n", filename);
	    return 0;
    }

    for (i=0; i<(infoheader.biWidth * infoheader.biHeight * 3); i+=3) { 
           // reverse all of the colors. (bgr -> rgb)
	    temp = infoheader.data[i];
	    infoheader.data[i] = infoheader.data[i+2];
	    infoheader.data[i+2] = temp;
    }


    fclose(file); // Closes the file stream

    // Bind the ID texture specified by the 2nd parameter
    glBindTexture(GL_TEXTURE_2D, num_texture);

    // The next commands sets the texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); 

    // If the u,v coordinates overflow the range 0,1 the image is repeated
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // The magnification function ("linear" produces better results)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                    GL_LINEAR_MIPMAP_NEAREST); 

    //The minifying function
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                    GL_LINEAR_MIPMAP_NEAREST); 

    // We don't combine the color with the original surface color, 
    // use only the texture map.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 

    // Finally we define the 2d texture
    glTexImage2D( GL_TEXTURE_2D, 0, 3, 
                  infoheader.biWidth, infoheader.biHeight, 
                  0, GL_RGB, GL_UNSIGNED_BYTE, infoheader.data);

    // And create 2d mipmaps for the minifying function
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, 
                       infoheader.biWidth, infoheader.biHeight, 
                       GL_RGB, GL_UNSIGNED_BYTE, infoheader.data);
    
    free(infoheader.data);

    return (num_texture); 
}
