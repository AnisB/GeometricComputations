#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include "BSpline.h"


using namespace std;


float			g_Yaw	= 0.0f;
float			g_Pitch	= 0.0f;
float			g_Zoom	= 2.0f;
float			g_PointZ;
int				g_MouseX, g_MouseY, g_Button;
BSpline			g_BSpline;
enum			Mode { MODE_VIEW, MODE_POINT } g_Mode = MODE_VIEW;
int				g_SelectedControlPoint	= -1;
int				g_SelectedKnot			= -1;


void	Display()
{
	int			i;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	float x	= g_Zoom * cos( g_Pitch ) * sin( g_Yaw );
	float y	= g_Zoom * sin( g_Pitch );
	float z	= g_Zoom * cos( g_Pitch ) * cos( g_Yaw );
	gluLookAt( x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

	// Dessine la courbe.
	g_BSpline.DrawSpline( 1000 );

	// Dessine le polygone de controle.
	glColor3f( 1.0f, 0.5f, 0.5f );
	glBegin( GL_LINE_STRIP );
	for( i = 0; i < g_BSpline.GetControlPointNb(); i++ )
	{
		CVector3	Point	= g_BSpline.GetControlPoint( i );
		glVertex3f( Point.x, Point.y, Point.z );
	}
	glEnd();

	// Dessine les points de controle.
	glBegin( GL_POINTS );
	for( i = 0; i < g_BSpline.GetControlPointNb(); i++ )
	{
		CVector3	Point	= g_BSpline.GetControlPoint( i );

		if( g_SelectedControlPoint == i )
			glColor3f( 0.0f, 0.0f, 0.0f );
		else
			glColor3f( 0.7f, 0.7f, 0.7f );
		
		glVertex3f( Point.x, Point.y, Point.z );
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

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( -0.1f, 1.1f, -0.5f, 4.0f, -10.0f, 10.0f );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	g_BSpline.DrawBasisFunctions( 100 );
	
	glBegin( GL_LINES );
	for( i = 0; i < g_BSpline.GetKnotsNb(); i++ )
	{
		if( g_SelectedKnot == i )
			glColor3f( 0.0f, 0.0f, 0.0f );
		else
			glColor3f( 0.7f, 0.7f, 0.7f );
		
		glVertex2f( g_BSpline.GetKnot( i ), -0.1f );
		glVertex2f( g_BSpline.GetKnot( i ), 1.1f );
	}
	glEnd();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );

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
	int				i, Viewport[4];

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
		
		for( i = 0; i < g_BSpline.GetControlPointNb(); i++ )
		{
			glPushName( (unsigned int)i );
			glBegin( GL_POINTS );
			glVertex3f( g_BSpline.GetControlPoint( i ).x, g_BSpline.GetControlPoint( i ).y, g_BSpline.GetControlPoint( i ).z );
			glEnd();
			glPopName();
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

			glSelectBuffer( 4, Selection );
			glRenderMode( GL_SELECT );
			glInitNames();

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			gluPickMatrix( (double)x, (double)( Viewport[3] - y ), 10.0, 10.0, Viewport );
			glOrtho( -0.1f, 1.1f, -0.5f, 4.0f, -1.0f, 1.0f );

			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();
		
			for( i = g_BSpline.GetDegree() + 1; i < g_BSpline.GetKnotsNb() - g_BSpline.GetDegree() - 1; i++ )
			{
				glPushName( i );
				
				glBegin( GL_LINES );
				glVertex2f( g_BSpline.GetKnot( i ), -0.1f );
				glVertex2f( g_BSpline.GetKnot( i ), 1.1f );
				glEnd();
				
				glPopName();
			}

			Hits = glRenderMode( GL_RENDER );

			if( Hits != 0 )
			{
				g_SelectedKnot	= Selection[3];
				g_PointZ = (float)( Selection[1] ) / (float)( 0xffffffff );

				g_Mode	= MODE_POINT;
			}
			else
			{
				g_SelectedKnot	= -1;
				g_Mode			= MODE_VIEW;
			}
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
			else if( g_SelectedKnot != -1 )
			{
				glMatrixMode( GL_PROJECTION );
				glPushMatrix();
				glLoadIdentity();
				glOrtho( -0.1f, 1.1f, -0.5f, 10.0f, -10.0f, 10.0f );

				glMatrixMode( GL_MODELVIEW );
				glLoadIdentity();

				glGetDoublev( GL_MODELVIEW_MATRIX, ModelMatrix );
				glGetDoublev( GL_PROJECTION_MATRIX, ProjectionMatrix );

				glMatrixMode( GL_PROJECTION );
				glPopMatrix();
				glMatrixMode( GL_MODELVIEW );
			}

			glGetIntegerv( GL_VIEWPORT, Viewport );

			gluUnProject( (double)x, (double)(Viewport[3] - y), g_PointZ, ModelMatrix, ProjectionMatrix, Viewport, &px, &py, &pz );

			if( g_SelectedControlPoint != -1 )
			{
				g_BSpline.SetControlPoint( g_SelectedControlPoint, CVector3( px, py, pz ) );
			}
			else if( g_SelectedKnot != -1 )
			{
				if( g_BSpline.GetKnot( g_SelectedKnot - 1 ) > px )
					g_BSpline.SetKnot( g_SelectedKnot, g_BSpline.GetKnot( g_SelectedKnot - 1 ) );
				else if( g_BSpline.GetKnot( g_SelectedKnot + 1 ) < px )
					g_BSpline.SetKnot( g_SelectedKnot, g_BSpline.GetKnot( g_SelectedKnot + 1 ) );
				else
					g_BSpline.SetKnot( g_SelectedKnot, px );
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
	case '+':
	case '=':
		if( g_BSpline.GetDegree() + 1 < g_BSpline.GetControlPointNb() )
			g_BSpline.SetDegree( g_BSpline.GetDegree() + 1 );
		break;

	case '-':
		if( g_BSpline.GetDegree() > 1 )
			g_BSpline.SetDegree( g_BSpline.GetDegree() - 1 );
		break;

	case 127: // Delete
		if( ( g_SelectedControlPoint != -1 ) && ( g_BSpline.GetDegree() < g_BSpline.GetControlPointNb() - 1 ) )
			g_BSpline.DeleteControlPoint( g_SelectedControlPoint );
		break;
	case 'j':
		if( g_SelectedControlPoint != -1 )
		{
			int	Pos = ( g_SelectedControlPoint < g_BSpline.GetControlPointNb() - 1 ? g_SelectedControlPoint : g_BSpline.GetControlPointNb() - 2 );
			g_BSpline.AddControlPoint( Pos + 1, ( g_BSpline.GetControlPoint( Pos ) + g_BSpline.GetControlPoint( Pos + 1 ) ) / 2.0f );
		}
		break;
	case 27: // ESC.
		exit( 0 );
	}
}


void	OnSpecialKeyPressed( int Key, int x, int y )
{
	switch( Key )
	{
	// case GLUT_KEY_INSERT:
	// 	if( g_SelectedControlPoint != -1 )
	// 	{
	// 		int	Pos = ( g_SelectedControlPoint < g_BSpline.GetControlPointNb() - 1 ? g_SelectedControlPoint : g_BSpline.GetControlPointNb() - 2 );

	// 		g_BSpline.AddControlPoint( Pos + 1, ( g_BSpline.GetControlPoint( Pos ) + g_BSpline.GetControlPoint( Pos + 1 ) ) / 2.0f );
	// 	}
	// 	break;
	// }
	}
}


void	Init()
{
	glShadeModel( GL_SMOOTH );
	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glEnable( GL_NORMALIZE );
	glPointSize( 5.0f );
	
	int		i;

	g_BSpline.Create( 3, 7 );

	for( i = 0; i < 4; i++ )
		g_BSpline.SetKnot( i, 0.0f );

	for( i = 4; i < 8; i++ )
		g_BSpline.SetKnot( i, 1.0f );

	g_BSpline.SetControlPoint( 0, CVector3( 0.0f, 0.0f, 0.0f ) );
	g_BSpline.SetControlPoint( 1, CVector3( 0.5f, 0.0f, 0.0f ) );
	g_BSpline.SetControlPoint( 2, CVector3( 0.5f, 0.5f, 0.0f ) );
	g_BSpline.SetControlPoint( 3, CVector3( 0.0f, 0.5f, 0.0f ) );
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
		glutSpecialFunc( OnSpecialKeyPressed );
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
