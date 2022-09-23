#if ( __APPLE__ )
#       include <GLUT/glut.h>
#else
#       include <GL/glut.h>
#endif
#include <math.h>
#include <stdio.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

#include "earth_texture.i"
static GLuint earthTextureID = 0;

static GLuint BallListID = 0;

void Ball_initialize() {

    int n_lats =  90;
    int n_lons = 180;
    int ii,jj;

    earthTextureID = MakeTexture(Earth_texture, EARTH_TEXTURE_WIDTH, EARTH_TEXTURE_HEIGHT);

    if (BallListID == 0) {

        BallListID = glGenLists(1);

        glNewList( BallListID, GL_COMPILE);

        glEnable(GL_LIGHTING);
        glBindTexture(GL_TEXTURE_2D, earthTextureID);
        glEnable(GL_TEXTURE_2D);


        for(ii = 0; ii <= n_lats; ii++) {

            double lat0 = PI * (-0.5 + (double) (ii - 1) / n_lats);
            double z0  = sin(lat0);
            double zr0 = cos(lat0);

            double lat1 = PI * (-0.5 + (double) ii / n_lats);
            double z1 = sin(lat1);
            double zr1 = cos(lat1);

            glBegin(GL_QUAD_STRIP);
            for(jj = 0; jj <= n_lons; jj++) {
                double lon = 2 * PI * (double) (jj - 1) / n_lons;
                double x = cos(lon);
                double y = sin(lon);

                glTexCoord2d( 0.0, 0.0);
                glNormal3d(x * zr0, y * zr0, z0);
                glVertex3d(x * zr0, y * zr0, z0);

                glTexCoord2d( 1.0, 1.0);
                glNormal3d(x * zr1, y * zr1, z1);
                glVertex3d(x * zr1, y * zr1, z1);
            }
            glEnd();
        }
        glEndList();
    }
}

void Ball_display(void) {

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glScaled(6367500.0, 6367500.0, 6367500.0);
    glCallList(BallListID);
    glPopMatrix();

}
