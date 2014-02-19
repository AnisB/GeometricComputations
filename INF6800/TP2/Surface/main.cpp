#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include "BSurface.h"


using namespace std;


float			g_Yaw	= 0.0f;
float			g_Pitch	= 0.0f;
float			g_Zoom	= 2.0f;
float			g_PointZ;
int				g_MouseX, g_MouseY, g_Button;
BSurface		g_BSurface;
enum			Mode { MODE_VIEW, MODE_POINT } g_Mode = MODE_VIEW;
int				g_SelectedControlPoint	= -1;
bool			g_Wire	= false;


void	Display()
{
	int		i, j;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	float x	= g_Zoom * cos( g_Pitch ) * sin( g_Yaw );
	float y	= g_Zoom * sin( g_Pitch );
	float z	= g_Zoom * cos( g_Pitch ) * cos( g_Yaw );
	gluLookAt( x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

	// Dessine la courbe.
	glEnable( GL_LIGHTING );
	g_BSurface.Draw( 10, 10 );
	glDisable( GL_LIGHTING );

	// Dessine le polygone de controle.
	glColor3f( 1.0f, 0.5f, 0.5f );
	for( j = 0; j < g_BSurface.GetControlPointNb( BSurface::DIR_V ); j++ )
	{
		glBegin( GL_LINE_STRIP );
		for( i = 0; i < g_BSurface.GetControlPointNb( BSurface::DIR_U ); i++ )
		{
			CVector3	CP = g_BSurface.GetControlPoint( i, j );
			glVertex3f( CP.x, CP.y, CP.z );
		}
		glEnd();
	}

	for( i = 0; i < g_BSurface.GetControlPointNb( BSurface::DIR_U ); i++ )
	{
		glBegin( GL_LINE_STRIP );
		for( j = 0; j < g_BSurface.GetControlPointNb( BSurface::DIR_V ); j++ )
		{
			CVector3	CP = g_BSurface.GetControlPoint( i, j );
			glVertex3f( CP.x, CP.y, CP.z );
		}
		glEnd();
	}

	// Dessine les points de controle.
	glBegin( GL_POINTS );
	for( i = 0; i < g_BSurface.GetControlPointNb( BSurface::DIR_U ); i++ )
	{
		for( j = 0; j < g_BSurface.GetControlPointNb( BSurface::DIR_V ); j++ )
		{
			if( g_SelectedControlPoint == ( j * g_BSurface.GetControlPointNb( BSurface::DIR_U ) + i ) )
				glColor3f( 0.0f, 0.0f, 0.0f );
			else
				glColor3f( 0.7f, 0.7f, 0.7f );

			CVector3	CP = g_BSurface.GetControlPoint( i, j );
			glVertex3f( CP.x, CP.y, CP.z );
		}
	}
	glEnd();

	// Dessine les axes.
	glBegin( GL_LINES );
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 1.0f, 0.0f, 0.0f );
		
		glColor3f( 0.0f, 1.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 1.0f, 0.0f );

		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 1.0f );
	glEnd();

	glutSwapBuffers();
}


void	Reshape( int width, int height )
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60.0, (double)width / height, 0.1, 100.0 );
	glMatrixMode( GL_MODELVIEW );
	glViewport( 0, 0, width, height );
}


void	OnMouseButton( int Button, int State, int x, int y )
{
	unsigned int	Selection[4] = { 0, 0, 0, 0 }, Hits;
	int				i, j, Viewport[4];

	if( State == GLUT_DOWN )
	{
		glGetIntegerv( GL_VIEWPORT, Viewport );

		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		gluPickMatrix( (double)x, (double)(Viewport[3] - y), 10.0, 10.0, Viewport );
		gluPerspective( 60.0, (double)Viewport[2] / (double)Viewport[3], 0.1, 100.0 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		float xv	= g_Zoom * cos( g_Pitch ) * sin( g_Yaw );
		float yv	= g_Zoom * sin( g_Pitch );
		float zv	= g_Zoom * cos( g_Pitch ) * cos( g_Yaw );
		gluLookAt( xv, yv, zv, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

		glSelectBuffer( 4, Selection );
		glRenderMode( GL_SELECT );
		glInitNames();
		
		for( j = 0; j < g_BSurface.GetControlPointNb( BSurface::DIR_V ); j++ )
		{
			for( i = 0; i < g_BSurface.GetControlPointNb( BSurface::DIR_U ); i++ )
			{
				CVector3	CP	= g_BSurface.GetControlPoint( i, j );

				glPushName( (unsigned int)( j * g_BSurface.GetControlPointNb( BSurface::DIR_U ) + i ) );
				glBegin( GL_POINTS );
				glVertex3f( CP.x, CP.y, CP.z );
				glEnd();
				glPopName();
			}
		}

		Hits = glRenderMode( GL_RENDER );

		if( Hits != 0 )
		{
			g_SelectedControlPoint	= Selection[3];
			g_PointZ = (float)( Selection[1] ) / (float)( 0xffffffff );

			g_Mode	= MODE_POINT;
		}
		else
		{
			g_SelectedControlPoint	= -1;
			g_Mode			= MODE_VIEW;
		}


		glMatrixMode( GL_PROJECTION );
		glPopMatrix();

		glMatrixMode( GL_MODELVIEW );
	}
	else
	{
		g_Mode	= MODE_VIEW;
	}

	g_MouseX	= x;
	g_MouseY	= y;
	g_Button	= Button;
}


void	OnMouseMove( int x, int y )
{
	float	DiffX = g_MouseX - x;
	float	DiffY = g_MouseY - y;

	switch( g_Mode )
	{
	case MODE_VIEW:
		{
			if( g_Button == GLUT_LEFT_BUTTON )
			{
				g_Pitch	+= DiffY * 0.01f;
				g_Yaw	+= DiffX * 0.01f;
			}
			else if( g_Button == GLUT_RIGHT_BUTTON )
			{
				float	k;
				if( DiffX + DiffY > 0 )
					k = sqrt( DiffX * DiffX + DiffY * DiffY );
				else
					k = -sqrt( DiffX * DiffX + DiffY * DiffY );

				if( g_Zoom * ( 1 + k * 0.01 ) > 0.0 )
					g_Zoom	*= 1 + k * 0.01;
			}
		}
		break;

	case MODE_POINT:
		{
			double	ModelMatrix[16], ProjectionMatrix[16];
			double	px, py, pz;
			int		Viewport[4];
			
			if( g_SelectedControlPoint != -1 )
			{
				glMatrixMode( GL_MODELVIEW );
				glLoadIdentity();
				float xv	= g_Zoom * cos( g_Pitch ) * sin( g_Yaw );
				float yv	= g_Zoom * sin( g_Pitch );
				float zv	= g_Zoom * cos( g_Pitch ) * cos( g_Yaw );
				gluLookAt( xv, yv, zv, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

				glGetDoublev( GL_MODELVIEW_MATRIX, ModelMatrix );
				glGetDoublev( GL_PROJECTION_MATRIX, ProjectionMatrix );
			}

			glGetIntegerv( GL_VIEWPORT, Viewport );

			gluUnProject( (double)x, (double)(Viewport[3] - y), g_PointZ, ModelMatrix, ProjectionMatrix, Viewport, &px, &py, &pz );

			if( g_SelectedControlPoint != -1 )
			{
				int		i, j;

				i	= g_SelectedControlPoint % g_BSurface.GetControlPointNb( BSurface::DIR_U );
				j	= g_SelectedControlPoint / g_BSurface.GetControlPointNb( BSurface::DIR_U );

				g_BSurface.SetControlPoint( i, j, CVector3( px, py, pz ) );
			}
		}
		break;
	}

	g_MouseX = x;
	g_MouseY = y;
}


void	OnKeyPressed( unsigned char Key, int x, int y )
{
	switch( Key )
	{
	case 'w':
	case 'W':
		g_Wire	= !g_Wire;

		if( g_Wire )
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		else
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		break;

	case 27: // ESC.
		exit( 0 );
	}
}


void	Init()
{
	float	a[4]	=	{ 0.2f, 0.2f, 0.2f, 1.0f };
	float	d[4]	=	{ 0.1f, 0.1f, 0.6f, 1.0f };
	float	s[4]	=	{ 0.4f, 0.4f, 0.4f, 1.0f };
	float	lp[4]	=	{ 0.0f, 5.0f, 0.0f, 1.0f };

	glLightfv( GL_LIGHT0, GL_POSITION, lp );
	glEnable( GL_LIGHT0 );

	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, a );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, d );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, s );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 50.0f );

	glShadeModel( GL_SMOOTH );
	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glEnable( GL_NORMALIZE );
	glPointSize( 5.0f );
	
	int		i, j;

	g_BSurface.Create( 3, 3, 8, 8 );

	for( i = 0; i < 4; i++ )
	{
		g_BSurface.SetKnot( BSurface::DIR_U, i, 0.0f );
		g_BSurface.SetKnot( BSurface::DIR_V, i, 0.0f );
	}

	g_BSurface.SetKnot( BSurface::DIR_U, 4, 0.5f );
	g_BSurface.SetKnot( BSurface::DIR_V, 4, 0.5f );

	for( i = 5; i <= 8; i++ )
	{
		g_BSurface.SetKnot( BSurface::DIR_U, i, 1.0f );
		g_BSurface.SetKnot( BSurface::DIR_V, i, 1.0f );
	}

	for( j = 0; j <= 4; j++ )
		for( i = 0; i <= 4; i++ )
			g_BSurface.SetControlPoint( i, j, CVector3( (float)i / 4 - 0.5f, (float)j / 4 - 0.5f, 0.0f ) );
}


int	main( int argc, char** argv )
{
	try
	{
		glutInit( &argc, argv );
		glutInitWindowSize( 800, 600 );
		glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
		glutCreateWindow( "B-Splines" );
		Init();
		glutDisplayFunc( Display );
		glutMouseFunc( OnMouseButton );
		glutMotionFunc( OnMouseMove );
		glutKeyboardFunc( OnKeyPressed );
		glutIdleFunc( Display );
		glutReshapeFunc( Reshape );
		glutMainLoop();
	}
	catch( Exception e )
	{
		cout << e.GetMessage() << endl;
	}

	return	0;
}
