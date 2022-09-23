#if ( __APPLE__ )
#       include <GLUT/glut.h>
#else
#       include <GL/glut.h>
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static GLuint capsuleTextureID = 0;
static GLuint nozzleTextureID = 0;
static GLuint displayListID = 0;

#ifndef PI
#define PI 3.14159265358979323846
#endif

#include "capsule_texture.i"
#include "nozzle_texture.i"

GLuint MakeTexture(GLubyte* data , int width, int height) {

      GLuint texture_name;
      glGenTextures(1, &texture_name);
      glBindTexture(GL_TEXTURE_2D, texture_name);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
      gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
      return(texture_name);
}

void Satellite_initialize(void) {

   capsuleTextureID = MakeTexture(Capsule_texture, CAPSULE_TEXTURE_WIDTH, CAPSULE_TEXTURE_HEIGHT);
   nozzleTextureID = MakeTexture(Nozzle_texture, NOZZLE_TEXTURE_WIDTH, NOZZLE_TEXTURE_HEIGHT);

  if (displayListID == 0) {
      /* Build Display list.*/
      double cosine[33];
      double sine[33];

      double radius1 = 0.254;
      double station1 = 3.5738;

      double radius2 = 1.9558;
      double station2 = 0.8128;

      double radius3 = 1.9558;
      double station3 = -3.1242;

      double radius4 = 0.3048;
      double station4 = -3.1242;

      double radius5 = 0.508;
      double station5 = -3.5560;

      double radius6 = 1.016;
      double station6 = -4.8260;

      double radius7 = 1.27;
      double station7 = -6.0858;

      int ii;
      for(ii=0 ; ii <= 32 ; ii++) {
          double angle = (PI*2*ii)/32.0;
          cosine[ii] = cos(angle);
          sine[ii] = sin(angle);
      }

      displayListID = glGenLists(1);
      glNewList( displayListID, GL_COMPILE);

          glEnable(GL_LIGHTING);

          glBindTexture(GL_TEXTURE_2D, capsuleTextureID);
          glEnable(GL_TEXTURE_2D);

          glBegin(GL_QUAD_STRIP);
          for(ii=0 ; ii <= 32 ; ii++) {

              double t = (double)ii/32.0;

              glTexCoord2d( 0.0, t);
              glNormal3d( (station2-station1)/radius2-radius1, sine[ii], cosine[ii]);
              glVertex3d( station1, sine[ii]*radius1, cosine[ii]*radius1);

              glTexCoord2d( 1.0, t);
              glNormal3d( (station2-station1)/radius2-radius1, sine[ii], cosine[ii]);
              glVertex3d( station2, sine[ii]*radius2, cosine[ii]*radius2);

          }
          glEnd();

          glDisable(GL_TEXTURE_2D);

          glBegin(GL_QUAD_STRIP);
          for(ii=0 ; ii <= 32 ; ii++) {
              glNormal3d( 0.0, sine[ii], cosine[ii]);
              glVertex3d( station2, sine[ii]*radius2, cosine[ii]*radius2);
              glNormal3d( 0.0, sine[ii], cosine[ii]);
              glVertex3d( station3, sine[ii]*radius3, cosine[ii]*radius3);
          }
          glEnd();

          glBegin(GL_QUAD_STRIP);
          for(ii=0 ; ii <= 32 ; ii++) {
              glNormal3d( 0.0, sine[ii], cosine[ii]);
              glVertex3d( station3, sine[ii]*radius3, cosine[ii]*radius3);
              glNormal3d( 0.0, sine[ii], cosine[ii]);
              glVertex3d( station4, sine[ii]*radius4, cosine[ii]*radius4);
          }
          glEnd();

          glBegin(GL_QUAD_STRIP);
          for(ii=0 ; ii <= 32 ; ii++) {
              glNormal3d( 0.0, sine[ii], cosine[ii]);
              glVertex3d( station4, sine[ii]*radius4, cosine[ii]*radius4);
              glNormal3d( 0.0, sine[ii], cosine[ii]);
              glVertex3d( station5, sine[ii]*radius5, cosine[ii]*radius5);
          }
          glEnd();


          glEnable(GL_LIGHTING);
          if (nozzleTextureID != 0) {
              glBindTexture(GL_TEXTURE_2D, nozzleTextureID);
              glEnable(GL_TEXTURE_2D);
          }

          glBegin(GL_QUAD_STRIP);
          for(ii=0 ; ii <= 32 ; ii++) {
              double t = (double)ii/32.0;
              glTexCoord2d( 0.0, t);
              glNormal3d( 0.0, sine[ii], cosine[ii]);
              glVertex3d( station5, sine[ii]*radius5, cosine[ii]*radius5);

              glTexCoord2d( 0.5, t);
              glNormal3d( 0.0, sine[ii], cosine[ii]);
              glVertex3d( station6, sine[ii]*radius6, cosine[ii]*radius6);
          }
          glEnd();

          glBegin(GL_QUAD_STRIP);
          for(ii=0 ; ii <= 32 ; ii++) {
              double t = (double)ii/32.0;
              glTexCoord2d( 0.5, t);
              glNormal3d( 0.0, sine[ii], cosine[ii]);
              glVertex3d( station6, sine[ii]*radius6, cosine[ii]*radius6);
              glTexCoord2d( 1.0, t);
              glNormal3d( 0.0, sine[ii], cosine[ii]);
              glVertex3d( station7, sine[ii]*radius7, cosine[ii]*radius7);
          }
          glEnd();

      glEndList();
  }
}

void Satellite_display(void) {

    glCallList(displayListID);

}
