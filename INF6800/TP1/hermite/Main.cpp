#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#ifdef _WIN32
#include <glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <iostream>

using namespace std;


#include "HermiteSpline.h"


// Courbe d'hermite.
HermiteSpline	g_Courbe;

// Variables pour l'orientation de la scene.
float			g_Yaw	= 0.0f;
float			g_Pitch	= 0.0f;
float			g_Zoom	= 3.0f;

// Variables pour gerer le deplacement de la souris.
int				g_MouseX, g_MouseY, g_Button;

// Conserve la profondeur d'un point selectionne.
float			g_PointZ;

// Mode lors du deplacement de la souris:
//  - MODE_VIEW:  change l'orientation de la scene.
//  - MODE_POINT: deplace un point.
enum			Mode { MODE_VIEW, MODE_POINT }	g_Mode = MODE_VIEW;


void	Display()
{
	// Efface le tampon image.
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Place l'observateur.
	glLoadIdentity();
	float x	= g_Zoom * cos( g_Pitch ) * sin( g_Yaw );
	float y	= g_Zoom * sin( g_Pitch );
	float z	= g_Zoom * cos( g_Pitch ) * cos( g_Yaw );
	gluLookAt( x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

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

	// Dessine la courbe.
	glColor3f( 0.0f, 0.0f, 0.0f );
	g_Courbe.Draw();

	// Affiche le tampon image.
	glutSwapBuffers();
}


void	Reshape( int width, int height )
{
	// Ajuste la matrice de projection.
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60.0, (double)width / height, 0.1, 100.0 );
	glMatrixMode( GL_MODELVIEW );
}


void	OnMouseButton( int Button, int State, int x, int y )
{
	unsigned int	Selection[4], Hits;
	int				Viewport[4];

	if( State == GLUT_DOWN )
	{
		// Si on appuie sur un bouton, on verifie s'il y a une selection.
		glSelectBuffer( 4, Selection );
		glRenderMode( GL_SELECT );
		glInitNames();
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		glGetIntegerv( GL_VIEWPORT, Viewport );
		gluPickMatrix( (double)x, (double)(Viewport[3] - y), 10.0, 10.0, Viewport );
		gluPerspective( 60.0, (double)Viewport[2] / (double)Viewport[3], 0.1, 100.0 );
		glMatrixMode( GL_MODELVIEW );
		// Affiche la courbe en mode selection.
		g_Courbe.Draw( true );
		glMatrixMode( GL_PROJECTION );
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
		glFlush();
		Hits = glRenderMode( GL_RENDER );

		// S'il y a eu selection...
		if( Hits != 0 )
		{
			// ...on selectionne le point.
			g_Courbe.UnselectAll();
			g_Courbe.SelectControlPoint( Selection[3] );
			g_PointZ = (float)( Selection[1] ) / (float)( 0xffffffff );
			g_Mode = MODE_POINT;

			// On affiche la scene.
			glutPostRedisplay();
		}
		else
			g_Mode		= MODE_VIEW;
	}
	else
		g_Mode	= MODE_VIEW;

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
			// Bouton de gauche en mode vue...
			if( g_Button == GLUT_LEFT_BUTTON )
			{
				// ...on change l'orientation.
				g_Pitch	+= DiffY * 0.01f;
				g_Yaw	+= DiffX * 0.01f;

				// On affiche la scene.
				glutPostRedisplay();
			}
			// Bouton de droite en mode vue...
			else if( g_Button == GLUT_RIGHT_BUTTON )
			{
				// ...on change la distance a l'origine.
				float	k;
				if( DiffX + DiffY > 0 )
					k = sqrt( DiffX * DiffX + DiffY * DiffY );
				else
					k = -sqrt( DiffX * DiffX + DiffY * DiffY );

				if( g_Zoom * ( 1 + k * 0.01 ) > 0.0 )
					g_Zoom	*= 1 + k * 0.01;

				// On affiche la scene.
				glutPostRedisplay();
			}
		}
		break;

	case MODE_POINT:
		{
			if( g_Courbe.IsSelection() )
			{
				switch( g_Button )
				{
				case GLUT_LEFT_BUTTON:
					{
						// Bouton de gauche en mode point.
						double	ModelMatrix[16], ProjectionMatrix[16];
						double	px, py, pz;
						int		Viewport[4];

						glGetDoublev( GL_MODELVIEW_MATRIX, ModelMatrix );
						glGetDoublev( GL_PROJECTION_MATRIX, ProjectionMatrix );
						glGetIntegerv( GL_VIEWPORT, Viewport );

						gluUnProject( (double)x, (double)(Viewport[3] - y), 
							g_PointZ, ModelMatrix, ProjectionMatrix, 
							Viewport, &px, &py, &pz );

						// On deplace le point selectionne.
						g_Courbe.SetSelectedPosition( CVector3( px, py, pz ) );

						// On affiche la scene.
						glutPostRedisplay();
					}
					break;

				case GLUT_RIGHT_BUTTON:
					{
						// Bouton de droite en mode point.
						CVector3 t = g_Courbe.GetSelectedTangent();

						float	Norm	= t.Norm();
						float	Yaw		= atan2( t.x, t.z );
						float	Pitch	= atan2( t.y, t.z / cos( Yaw ) );

						Pitch	+= DiffY * 0.01f;
						Yaw		+= DiffX * 0.01f;

						float tx	= Norm * cos( Pitch ) * sin( Yaw );
						float ty	= Norm * sin( Pitch );
						float tz	= Norm * cos( Pitch ) * cos( Yaw );

						// On change l'orientation de la tangente.
						g_Courbe.SetSelectedTangent( CVector3( tx, ty, tz ) );

						// On affiche la scene.
						glutPostRedisplay();
					}
					break;

				case GLUT_MIDDLE_BUTTON:
					{
						// Bouton du centre en mode point.
						float	k;
						CVector3 t = g_Courbe.GetSelectedTangent();
						float	Norm;

						// Si le vecteur normal est 0, on le change.
						if( t == CVector3::ZERO )
							t	= CVector3( 0.01f, 0, 0 );

						Norm	= t.Norm();

						if( DiffX + DiffY > 0 )
							k = sqrt( DiffX * DiffX + DiffY * DiffY );
						else
							k = -sqrt( DiffX * DiffX + DiffY * DiffY );

						if( Norm * ( 1 + k * 0.01f ) > 0.01f )
							Norm	*= 1 + k * 0.01f;

						// On change la longueur de la tangente.
						g_Courbe.SetSelectedTangent( t.Normalize() * Norm );

						// On affiche la scene.
						glutPostRedisplay();
					}
				}
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
	case 27:
		// ESC.
		exit( 0 );
	
	case 't':
	case 'T':
		{
			CVector3	T1, Tn;

			// On ajuste trouve la tangente de depart...
			g_Courbe.SelectControlPointByPosition( 0 );
			T1	= g_Courbe.GetSelectedTangent();
			// et la tangente de fin.
			g_Courbe.SelectControlPointByPosition( g_Courbe.GetNbControlPoints() - 1 );
			Tn	= g_Courbe.GetSelectedTangent();

			// On impose la continuite C2 selon les tangentes de debut et de fin.
			g_Courbe.C2WithImposedTangents( T1, Tn);
			glutPostRedisplay();
		}
		break;

	case 'n':
	case 'N':
		{
			// Impose la continuite C2 avec une courbure nulle aux extremites.
			g_Courbe.C2WithImposedCurvature( CVector3::ZERO, CVector3::ZERO );
			glutPostRedisplay();
		}
		break;

	case 'c':
	case 'C':
		{
			// Impose la continuite C2 avec une courbure quelconque aux extremites.
			// Comme il n'est pas evident d'editer la courbure directement dans le 
			// programme, elles sont hardcodee ici.  Vous pouvez essayer differentes
			// valeurs.
			g_Courbe.C2WithImposedCurvature( CVector3( 1, 1, 1 ), CVector3( 2, 2, 2 ) );
			glutPostRedisplay();
		}
		break;

	case 'f':
	case 'F':
		{
			// Impose la continuite C2 en fermant la courbe.
			g_Courbe.C2WithClosedCurve();
			glutPostRedisplay();
		}
		break;

	case 127:
		// Delete.
		{
			// Efface le point selectionne.
			if( g_Courbe.GetNbControlPoints() > 2 )
				g_Courbe.DeleteSelected();
			glutPostRedisplay();
		}
		break;
	}
}


void	OnSpecialKeyPressed( int Key, int x, int y )
{
	switch( Key )
	{
	case GLUT_KEY_INSERT:
		{
			// Insere un point a l'origine.
			g_Courbe.AddControlPoint( CVector3::ZERO, CVector3::ZERO );
			glutPostRedisplay();
			break;
		}
	}
}


void	Init()
{
	// Initialisation des etats d'OpenGL.
	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glPointSize( 5.0f );

	const float Sqrt22	= sqrtf( 2.0f ) / 2.0f;

	// Creation de quelques points de controles au depart.
	g_Courbe.AddControlPoint( CVector3( 1.0f, 0.0f, 0.0f ) );
	g_Courbe.AddControlPoint( CVector3( Sqrt22, Sqrt22, 0.0f ) );
	g_Courbe.AddControlPoint( CVector3( 0.0f, 1.0f, 0.0f ) );
	g_Courbe.AddControlPoint( CVector3( -Sqrt22, Sqrt22, 0.0f ) );
	g_Courbe.AddControlPoint( CVector3( -1.0f, 0.0f, 0.0f ) );
	g_Courbe.AddControlPoint( CVector3( -Sqrt22, -Sqrt22, 0.0f ) );
	g_Courbe.AddControlPoint( CVector3( 0.0f, -1.0f, 0.0f ) );
	g_Courbe.AddControlPoint( CVector3( Sqrt22, -Sqrt22, 0.0f ) );
	g_Courbe.AddControlPoint( CVector3( 1.0f, -Sqrt22, 0.0f ) );
}


int	main( int argc, char** argv )
{
	glutInit( &argc, argv );
	glutInitWindowSize( 800, 600 );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutCreateWindow( "Splines" );
	Init();
	glutDisplayFunc( Display );
	glutKeyboardFunc( OnKeyPressed );
	glutSpecialFunc( OnSpecialKeyPressed );
	glutMouseFunc( OnMouseButton );
	glutMotionFunc( OnMouseMove );
	glutReshapeFunc( Reshape );
	glutMainLoop();

	return	0;
}
