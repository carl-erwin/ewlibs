/* gears.c */

/*
 * 3-D gear wheels.  This program is in the public domain.
 *
 * Brian Paul
 */

/* Conversion to GLUT by Mark J. Kilgard */

#include <math.h>
#include <stdlib.h>


#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"


#ifndef M_PI
#define M_PI 3.14159265f
#endif

namespace ew
{
namespace test
{

using namespace ew::implementation::graphics::rendering::opengl;

/**

   Draw a gear wheel.  You'll probably want to call this function when
   building a display list since we do a lot of trig here.

   Input:  inner_radius - radius of hole at center
   outer_radius - radius at center of teeth
   width - width of gear
   teeth - number of teeth
   tooth_depth - depth of tooth

**/

void
gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
     GLint teeth, GLfloat tooth_depth)
{
	GLint i;
	GLfloat r0, r1, r2;
	GLfloat angle, da;
	GLfloat u, v, len;

	r0 = inner_radius;
	r1 = outer_radius - tooth_depth / 2.0f;
	r2 = outer_radius + tooth_depth / 2.0f;

	da = 2.0f * M_PI / teeth / 4.0f;

	glShadeModel(GL_FLAT);

	glNormal3f(0.0f, 0.0f, 1.0f);

	/* draw front face */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5f);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5f);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5f);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5f);
	}
	glEnd();


	/* draw front sides of teeth */
	glBegin(GL_QUADS);
	da = 2.0f * M_PI / teeth / 4.0f;
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;

		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5f);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5f);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5f);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5f);
	}
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);

	/* draw back face */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5f);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5f);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5f);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5f);
	}
	glEnd();


	/* draw back sides of teeth */
	glBegin(GL_QUADS);
	da = 2.0f * M_PI / teeth / 4.0f;
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;

		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5f);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5f);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5f);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5f);
	}
	glEnd();


	/* draw outward faces of teeth */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;

		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5f);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5f);
		u = r2 * cos(angle + da) - r1 * cos(angle);
		v = r2 * sin(angle + da) - r1 * sin(angle);
		len = sqrt(u * u + v * v);
		u /= len;
		v /= len;
		glNormal3f(v, -u, 0.0f);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5f);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5f);
		glNormal3f(cos(angle), sin(angle), 0.0f);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5f);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5f);
		u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
		v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
		glNormal3f(v, -u, 0.0f);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5f);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5f);
		glNormal3f(cos(angle), sin(angle), 0.0f);
	}

	glVertex3f(r1 * cos(0.0f), r1 * sin(0.0f), width * 0.5f);
	glVertex3f(r1 * cos(0.0f), r1 * sin(0.0f), -width * 0.5f);

	glEnd();


	glShadeModel(GL_SMOOTH);

	// BUG under valgrind and nvidia OpenGL version 1.5.8 NVIDIA 96.31
	/* draw inside radius cylinder */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;
		GLfloat x = -cos(angle);
		GLfloat y = -sin(angle);

		//    std::cerr << "x = " << x << "\n";
		//    std::cerr << "y = " << y << "\n";
		//           x = (x < -1.0f) ? -1.0f : x;
		//           x = (x > +1.0f) ? 1.0f : x;
		//           y = (y < -1.0f) ? -1.0f : y;
		//           y = (y > +1.0f) ? +1.0f : y;
		glNormal3f(x, y, 0.0f);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5f);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5f);
	}
	glEnd();
}



GLfloat view_rotx = 20.0f, view_roty = 30.0f, view_rotz = 0.0f;
static GLint gear1, gear2, gear3;

TLS_DECL GLfloat angle = 0.0f;

void
draw(void)
{
	glPushMatrix();
	glRotatef(view_rotx, 1.0f, 0.0f, 0.0f);
	glRotatef(view_roty, 0.0f, 1.0f, 0.0f);
	glRotatef(view_rotz, 0.0f, 0.0f, 1.0f);

	glPushMatrix();
	glTranslatef(-3.0f, -2.0f, 0.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glCallList(gear1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.1f, -2.0f, 0.0f);
	glRotatef(-2.0f * angle - 9.0f, 0.0f, 0.0f, 1.0f);
	glCallList(gear2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.1f, 4.2f, 0.0f);
	glRotatef(-2.0f * angle - 25.0f, 0.0f, 0.0f, 1.0f);
	glCallList(gear3);
	glPopMatrix();

	glPopMatrix();
}



// key( unsigned char k, int x, int y )
// {
//   switch ( k )
//   {
//   case 'z':
//     view_rotz += 5.0f;
//     break;
//   case 'Z':
//     view_rotz -= 5.0f;
//     break;
//   case 27:    /* Escape */
//     exit( 0 );
//     break;
//   default:
//     return ;
//   }
//   glutPostRedisplay();
// }

/* change view angle */
/* ARGSUSED1 */
// static void
// special( int k, int x, int y )
// {
//   switch ( k )
//   {
//   case GLUT_KEY_UP:
//     view_rotx += 5.0f;
//     break;
//   case GLUT_KEY_DOWN:
//     view_rotx -= 5.0f;
//     break;
//   case GLUT_KEY_LEFT:
//     view_roty += 5.0f;
//     break;
//   case GLUT_KEY_RIGHT:
//     view_roty -= 5.0f;
//     break;
//   default:
//     return ;
//   }
//   glutPostRedisplay();
// }

/* new window size or exposure */
// static void
// reshape( int width, int height )
// {
//   GLfloat h = ( GLfloat ) height / ( GLfloat ) width;

//   glViewport( 0, 0, ( GLint ) width, ( GLint ) height );
//   glMatrixMode( GL_PROJECTION );
//   glLoadIdentity();
//   glFrustum( -1.0f, 1.0f, -h, h, 5.0f, 60.0f );
//   glMatrixMode( GL_MODELVIEW );
//   glLoadIdentity();
//   glTranslatef( 0.0f, 0.0f, -40.0f );
// }

GLfloat pos[ 4 ] = {5.0f, 5.0f, 10.0f, 0.0f};
GLfloat red[ 4 ] = {0.8f, 0.1f, 0.0f, 1.0f};
GLfloat green[ 4 ] = {0.0f, 0.8f, 0.2f, 1.0f};
GLfloat blue[ 4 ] = {0.2f, 0.2f, 1.0f, 1.0f};

void
gears_init(void)
{
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	/* make the gears */
	gear1 = glGenLists(1);
	glNewList(gear1, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
	gear(1.0f, 4.0f, 1.0f, 20, 0.7f);
	glEndList();

	gear2 = glGenLists(1);
	glNewList(gear2, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
	gear(0.5f, 2.0f, 2.0f, 10, 0.7f);
	glEndList();

	gear3 = glGenLists(1);
	glNewList(gear3, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	gear(1.3f, 2.0f, 0.5f, 10, 0.7f);
	glEndList();

	glEnable(GL_NORMALIZE);
}



void
draw_gears_direct(void)
{
	glEnable(GL_NORMALIZE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	/* make the gears */
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
	gear(1.0f, 4.0f, 1.0f, 20, 0.7f);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
	gear(0.5f, 2.0f, 2.0f, 10, 0.7f);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	gear(1.3f, 2.0f, 0.5f, 10, 0.7f);
}


// void
// visible( int vis )
// {
//   if ( vis == GLUT_VISIBLE )
//     glutIdleFunc( idle );
//   else
//     glutIdleFunc( NULL );
// }

// int
// main_2( int argc, char *argv[] )
// {
//   /*   glutInit(&argc, argv); */
//   /*   if (argc > 1) { */
//   /*     /\* do 'n' frames then exit *\/ */
//   /*     limit = atoi(argv[1]) + 1; */
//   /*   } else { */
//   /*     limit = 0; */
//   /*   } */
//   /*   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE); */

//   /*   glutCreateWindow("Gears"); */
//   gears_init();

//   glutDisplayFunc( draw );
//   glutReshapeFunc( reshape );
//   glutKeyboardFunc( key );
//   glutSpecialFunc( special );
//   glutVisibilityFunc( visible );

//   glutMainLoop();
//   return 0;             /* ANSI C requires main to return int. */
// }

}
}
