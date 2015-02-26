/*
 * The reflection code was in part taken from:
 * http://nehe.gamedev.net : Lesson 26
 * Other stuff I gathered from the OpenGL Programming Guide
 * (a.k.a. the red book)
 *
 * Image textures were made with gimp
 *
 * Tested on Redhat 9.0 with Nvidia card
 */

#if ( __APPLE__ ) 
#       include <GLUT/glut.h>
#else
#       include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <unistd.h>
#include "texture.h"

#include "trick_utils/comm/include/tc.h"

void draw_dirt();
void draw_cannon() ;
void draw_ball() ;
void draw_wheel() ;
void draw_barrel_leg() ;
void move_ball();

GLUquadricObj *q_ball;

typedef struct {
        double x ;
        double y ;
        double z ;
} Point ;

static GLuint texture_dirt ;
static GLuint texture_mars ;
static GLuint texture_granite ;
static GLfloat light_position[3] ;
static GLfloat camera[3] = { 5.0, 10, 4.0 } ;

GLfloat mat_ambient[4] = { 0.25, 0.25, 0.25, 1.0 };
GLfloat mat_diffuse[4] = { 0.4, 0.4, 0.4, 1.0 };
//GLfloat mat_specular[4] = {0.774597, 0.774597, 0.774597,1.0 };
GLfloat mat_specular[4] = {0.0, 0.0, 0.0, 1.0 };
GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat face_shininess[] = { 76.8 };
GLfloat fog_color[4] = { 0.0, 0.0, 0.0, 0.0 } ;

TCDevice listen_device ;
TCDevice connection ;
Point ball_position ;

int ball_spin_angle = 0;

void display(void)
{
        static float angle = 0.0 ;

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   
        glLoadIdentity();

        gluLookAt (camera[0], camera[1], camera[2], 
                         (GLdouble) ball_position.x,  0.0, 
                         (GLdouble) ball_position.y, 
                         0.0,       0.0,       1.0);

        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

        glColorMask(1,1,1,1);
        glEnable(GL_LIGHTING); 
        glEnable(GL_BLEND);

        draw_dirt();
        
        glPushMatrix();
        glTranslatef ( 2.0 , 0.0 , 0 );
        draw_cannon();
        glPopMatrix();

        glPushMatrix();
        glTranslatef ( ball_position.x , 0.0 , ball_position.y );
        if ( ball_position.y > 1.0 ) {
                 angle++;
                 glRotatef (angle, 0.0f, 0.0f, 1.0f);
        }
        draw_ball() ;
        glPopMatrix();

        glutSwapBuffers();
}

void draw_ball() {

        gluQuadricNormals(q_ball, GL_SMOOTH);       
        gluQuadricTexture(q_ball, GL_TRUE);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_mars);
            glEnable(GL_TEXTURE_2D);
            gluSphere(q_ball, 0.5, 30, 30);
        glDisable(GL_TEXTURE_2D);
}        

void draw_cannon() {

        
        GLUquadricObj *quadric;
        quadric = gluNewQuadric();

        gluQuadricDrawStyle(quadric, GLU_FILL);
        gluQuadricTexture(quadric, GL_TRUE);

        /* Wheel 1 */
        glPushMatrix();
        glTranslatef ( 0.0 , 1.0 , 0.0 );
        draw_wheel() ;
        glPopMatrix();

        /* Wheel 2 */
        glPushMatrix();
        glTranslatef ( 0.0 , -1.0 , 0.0 );
        draw_wheel() ;
        glPopMatrix();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_granite);

        /* Barrel sits on this thing */
        glPushMatrix();
        glRotatef (90, 1.0, 0.0, 0.0);
        glTranslatef ( -3.0 , 0.0 , -0.6 );
        draw_barrel_leg() ;
        glPopMatrix();

        glPushMatrix();
        glRotatef (90, 1.0, 0.0, 0.0);
        glTranslatef ( -3.0 , 0.0 , 0.6 );
        draw_barrel_leg() ;
        glPopMatrix();

        /* Axle */
        glPushMatrix();
        glRotatef (90, 1.0, 0.0, 0.0);
        glTranslatef ( 0.0 , 1.0 , 0.0 );
        glTranslatef ( 0.0 , 0.0 , -1.0 );
        gluCylinder(quadric, 0.25, 0.25, 2.00, 30, 30);
        glPopMatrix();

        /* Barrel */
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_granite);
        glPushMatrix();
            glTranslatef ( -2.0 , 0.0 , 0.0 );
            glTranslatef ( 0.0 , 0.0 , 1.0 );
            glRotatef ((GLfloat) 60.0, 0.0, 1.0, 0.0);
            gluCylinder(quadric, 0.5, 0.5, 3.00, 30, 30);
            gluSphere(quadric, 0.52 , 30, 30);
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);

        gluDeleteQuadric(quadric);
}

void draw_barrel_leg() {

        GLUquadricObj *quadric;
        quadric = gluNewQuadric();

        GLfloat a[3] = { 0.0,  0.0,  0.0 } ; 
        GLfloat b[3] = { 0.0,  0.5,  0.0 } ;
        GLfloat c[3] = { 4.0,  1.75, 0.0 } ;
        GLfloat d[3] = { 4.0,  1.0,  0.0 } ;
        GLfloat e[3] = { 1.0, 0.0,  0.0 } ;

        GLfloat a1[3] = { 0.0,  0.0,  0.2 } ; 
        GLfloat b1[3] = { 0.0,  0.5,  0.2 } ;
        GLfloat c1[3] = { 4.0,  1.75, 0.2 } ;
        GLfloat d1[3] = { 4.0,  1.0,  0.2 } ;
        GLfloat e1[3] = { 1.0,  0.0,  0.2 } ;

        GLfloat n0[3] = { 0.0, 0.0, -1.0 } ;
        GLfloat n1[3] = { 0.0, 0.0, 1.0 } ;
        GLfloat n2[3] = { 0.0, -1.0, 0.0 } ;
        GLfloat n3[3] = { 0.0, 1.0, 0.0 } ;
        GLfloat n4[3] = { 1.0, 0.0, 0.0 } ;
        GLfloat n5[3] = { 0.0, -1.0, 0.0 } ;
        GLfloat n6[3] = { 0.0, -1.0, 1.0 } ;


        glBegin(GL_POLYGON) ; 
           glNormal3fv( n0 ) ;
           glTexCoord2f(0.0, 0.0);
           glTexCoord2f(0.0, 10.0);
           glTexCoord2f(10.0, 10.0);
           glTexCoord2f(10.0, 0.0);
           glVertex3fv( a ) ;
           glVertex3fv( b ) ;
           glVertex3fv( c ) ;
           glVertex3fv( d ) ;
           glVertex3fv( e ) ;
        glEnd();

        glBegin(GL_POLYGON) ; 
           glNormal3fv( n1 ) ;
           glVertex3fv( a1 ) ;
           glVertex3fv( b1 ) ;
           glVertex3fv( c1 ) ;
           glVertex3fv( d1 ) ;
           glVertex3fv( e1 ) ;
        glEnd();

        glBegin(GL_POLYGON) ; 
           glNormal3fv( n2 ) ;
           glVertex3fv( a ) ;
           glVertex3fv( b ) ;
           glVertex3fv( b1 ) ;
           glVertex3fv( a1 ) ;
        glEnd();

        glBegin(GL_POLYGON) ; 
           glNormal3fv( n3 ) ;
           glVertex3fv( d ) ;
           glVertex3fv( c ) ;
           glVertex3fv( c1 ) ;
           glVertex3fv( d1 ) ;
        glEnd();

        glBegin(GL_POLYGON) ; 
           glNormal3fv( n4 ) ;
           glVertex3fv( b ) ;
           glVertex3fv( b1 ) ;
           glVertex3fv( c1 ) ;
           glVertex3fv( c ) ;
        glEnd();

        glBegin(GL_POLYGON) ; 
           glNormal3fv( n5 ) ;
           glVertex3fv( e ) ;
           glVertex3fv( a ) ;
           glVertex3fv( a1 ) ;
           glVertex3fv( e1 ) ;
        glEnd();

        glBegin(GL_POLYGON) ; 
           glNormal3fv( n6 ) ;
           glVertex3fv( d ) ;
           glVertex3fv( e ) ;
           glVertex3fv( e1 ) ;
           glVertex3fv( d1 ) ;
        glEnd();

}

void draw_wheel() {

        int ii ;
        int angle ;

        GLUquadricObj *quadric;

        quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);
        gluQuadricTexture(quadric, GL_TRUE);

        /* Spokes */
        angle = 0 ;
        for ( ii = 0 ; ii < 14 ; ii++ ) {

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texture_granite);
                glPushMatrix();
                glTranslatef ( 0.0 , 0.0 , 1.0 );
                glRotatef ((GLfloat) angle, 0.0, 1.0, 0.0);
                gluCylinder(quadric, 0.05, 0.05, 1.0, 10, 10);
                glPopMatrix();
                glDisable(GL_TEXTURE_2D);

                angle += 25 ;
        }

        /* 
         * Tire and cap 
         */
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_granite);

        glPushMatrix();
        glTranslatef ( 0.0 , 0.0 , 1.0 );
        glRotatef ((GLfloat) 90, 0.0, 1.0, 0.0);
        glRotatef ((GLfloat) 90, 1.0, 0.0, 0.0);

        glTranslatef ( 0.0 , 0.0 , 0.1 ); 
        gluDisk(quadric, 1.0, 1.05, 30, 30);              /* Tire side */
        gluDisk(quadric, 0.00, 0.25, 30, 30);             /* Cap side */

        glTranslatef ( 0.0 , 0.0 , -0.2 );          
        gluDisk(quadric, 1.0, 1.05, 30, 30);              /* Tire side */
        gluDisk(quadric, 0.00, 0.25, 30, 30);             /* Cap side */

        gluCylinder(quadric, 1.0, 1.0, 0.2, 30, 30);      /* Inner Tire */
        gluCylinder(quadric, 1.05, 1.05, 0.2, 30, 30);    /* Tire tread */
        gluCylinder(quadric, 0.25, 0.25, 0.2, 30, 30);    /* Cap */

        glPopMatrix();
        glDisable(GL_TEXTURE_2D);

        gluDeleteQuadric(quadric);

}
void draw_dirt() {

        glPolygonMode(GL_FRONT, GL_FILL );
        glEnable(GL_POLYGON_SMOOTH);
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D, texture_dirt);
        glBegin(GL_POLYGON);
           glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
           glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
           glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
           glMaterialfv(GL_FRONT, GL_SHININESS, face_shininess);
           glNormal3f( 0.0, 0.0, 1.0 ) ;
           glTexCoord2f(0.0, 0.0) ; glVertex3f( -500.0, 500.0, -0.5 ) ;
           glTexCoord2f(250.0, 0.0) ; glVertex3f( 500.0, 500.0, -0.5 ) ;
           glTexCoord2f(250.0, 250.0) ; glVertex3f( 500.0, -500.0, -0.5 ) ;
           glTexCoord2f(0.0, 250.0) ; glVertex3f( -500.0, -500.0, -0.5 ) ;
        glEnd();
        glDisable(GL_TEXTURE_2D);
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 500.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt (camera[0], camera[1], camera[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void move_ball() {
        int nbytes ; 

        if ( tc_isValid(&connection) && tc_pending(&connection) ) {

                nbytes = tc_read(&connection, (char*) &ball_position.x, 
                                 sizeof(double));
                nbytes = tc_read(&connection, (char*) &ball_position.y, 
                                 sizeof(double));
        }

        glutPostRedisplay();
}

void glInit() {

   /* Init glut */
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Trick Cannon");

   /* Textures */
   texture_dirt = LoadBitmap("images/dirt.bmp");
   texture_mars = LoadBitmap("images/MarsMap.bmp");
   texture_granite = LoadBitmap("images/granite.bmp");

   /* Quadric for ball */
   q_ball = gluNewQuadric();

   //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);    
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

   // Here is the light/shading init
   light_position[0] = 4.0 ;
   light_position[1] = 0.0 ;
   light_position[2] = 0.0 ;
   light_position[3] = 1.0 ;
   GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };

   glClearColor (0.0, 0.0, 0.0, 1.0);
   glShadeModel (GL_SMOOTH);

   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);

   glDepthFunc(GL_LEQUAL); 
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

   // Fog
   glEnable(GL_FOG);
   glFogi(GL_FOG_MODE, GL_EXP2) ;
   glFogfv(GL_FOG_COLOR, fog_color);
   glFogf(GL_FOG_DENSITY, 0.005) ;
   glFogf(GL_FOG_START, 10.0);
   glFogf(GL_FOG_END, 10.0);

   // Add specular effect -after- texturing calculations
   glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,
                 GL_SEPARATE_SPECULAR_COLOR);

   /* Call backs */
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
}

void keyboard(unsigned char key, int x, int y) {

    switch (key) {
      case 'u':
         camera[0] -= 0.5 ;
         break;
      case 'i':
         camera[0] += 0.5 ;
         break;
      case 'j':
         camera[1] -= 0.5 ;
         break;
      case 'k':
         camera[1] += 0.5 ;
         break;
      case 'n':
         camera[2] -= 0.5 ;
         break;
      case 'm':
         camera[2] += 0.5 ;
         break;
      case 'x':
         light_position[0] -= 0.5 ;
         break;
      case 'y':
         light_position[1] -= 0.5 ;
         break;
      case 'z':
         light_position[2] -= 0.5 ;
         break;
      case 'X':
         light_position[0] += 0.5 ;
         break;
      case 'Y':
         light_position[1] += 0.5 ;
         break;
      case 'Z':
         light_position[2] += 0.5 ;
         break;
    }
}

void trickCommInit( ) {

        memset(&listen_device, '\0', sizeof(TCDevice));
        memset(&connection,    '\0', sizeof(TCDevice));

        listen_device.port = 9000 ;
        connection.blockio_type = TC_COMM_NOBLOCKIO ;
        tc_init(&listen_device) ;
        tc_accept(&listen_device, &connection);
}


int main(int argc, char** argv)
{
        glutInit(&argc, argv);
        glInit();
        trickCommInit();
        glutIdleFunc(move_ball);
        glutKeyboardFunc(keyboard);
        glutMainLoop();
        return 0;
}
