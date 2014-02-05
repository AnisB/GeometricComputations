#include "HermiteSpline.h"
#ifdef	_WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <iostream>

using namespace std;

unsigned int	HermiteSpline::ControlPoint::IdCounter = 0xFFFF;

// Cette fonction permets de résoudre le systeme triangulaire que définissent les courbes d'hermite
bool solveSystem(float * a, float * b, float * c, CVector3 * u, CVector3 * r, int parSystemSize)
{
	// Declaration desz variables tampon
	float bet, *gam;
	// Allocation de la mémoire
	gam = new float[parSystemSize];
	// Verification sur la valeur de b0
	if(b[0] == 0.0)
	{
		std::cout<<"Error 1"<<std::endl;
		return false;
	}
	//Initialisation
	bet = b[0];
	u[0] = r[0]/b[0];

	for(int counter = 1; counter<parSystemSize; ++counter)
	{
		gam[counter] = c[counter-1]/bet;
		bet = b[counter]-a[counter]*gam[counter];
		if(bet == 0.0)
		{
			std::cout<<"Error 2 "<<std::endl;
			return false;
		}
		u[counter] = (r[counter]-a[counter]*u[counter-1])/bet;

	}
	for(int counter = (parSystemSize-2); counter>=0;--counter)
	{
		u[counter] -=gam[counter+1]*u[counter+1];
		
	}
	// on libère la mémoire
	delete [] gam;
	return true;
}

// Cette fonction permet de resoudre le systeme cyclique que défini la contrainte de courbure 
// fermée par courbes d'hermite
bool solveSystemCirc(float * a, float * b, float * c, float alpha, float beta, CVector3 * x, CVector3 * r, int parSystemSize)
{
	float gamma, *bb;
	CVector3 fact, *z, *u;
	if(parSystemSize<=2)
	{
		std::cout<<"N trop petit"<<std::endl;
		return false;
	}
	bb = new float[parSystemSize];
	z = new CVector3[parSystemSize];
	u = new CVector3[parSystemSize];
	gamma =-b[0];
	bb[0] = b[0]-gamma;
	bb[parSystemSize-1] = b[parSystemSize-1]-alpha*beta/gamma;
	for(int i = 1; i< (parSystemSize-1); ++i)
	{
		bb[i] = b[i];
	}
	solveSystem(a, bb, c, x, r, parSystemSize);
	u[0] = CVector3(gamma, gamma, gamma);
	u[parSystemSize-1] = CVector3(alpha, alpha,alpha);
	for(int i = 1; i< (parSystemSize-1); ++i)
	{
		u[i] = CVector3();
	}
	solveSystem(a, bb, c, z, u, parSystemSize);
	fact.x = (x[0].x+beta*x[parSystemSize-1].x/gamma)/(1.0+z[0].x+beta*z[parSystemSize-1].x/gamma);
	fact.y = (x[0].y+beta*x[parSystemSize-1].y/gamma)/(1.0+z[0].y+beta*z[parSystemSize-1].y/gamma);
	fact.z = (x[0].z+beta*x[parSystemSize-1].z/gamma)/(1.0+z[0].z+beta*z[parSystemSize-1].z/gamma);
	for(int i =0; i< parSystemSize; ++i)
	{
		x[i].x -=  fact.x*z[i].x; 
		x[i].y -=  fact.x*z[i].y; 
		x[i].z -=  fact.x*z[i].z; 
	}
	delete [] z;
	delete [] bb;
	delete [] u;
	return true;
}

CVector3 HermiteSpline::ComputePoint(ControlPoint parP1, ControlPoint parP2, float parT) const
{
	float u = parT;
	float u2 = u*u;
	float u3 = u2*u;
	return ((2*u3-3*u2+1)*parP1.p+ (-2*u3+3*u2)*parP2.p+(u3-2*u2+u)*parP1.t+(u3-u2)*parP2.t);
}

HermiteSpline::ControlPoint::ControlPoint( const CVector3& _p, const CVector3& _t ) :
	Selected( false ),
	Id( ++IdCounter ),
	p( _p ),
	t( _t )
{
}


HermiteSpline::HermiteSpline()
{
	m_SelectedControlPoint	= m_ControlPoints.end();
}


HermiteSpline::~HermiteSpline()
{
}


unsigned int HermiteSpline::AddControlPoint( const CVector3& p, const CVector3& t )
{
	// On ajoute le point de controle a la fin du vecteur.
	m_ControlPoints.push_back( ControlPoint( p, t ) );

	return m_ControlPoints.back().Id;
}


void HermiteSpline::C2WithImposedTangents( const CVector3& T1, const CVector3& Tn )
{
	// Si on a moins de 3 points de controle, on sort
	// car il n'y a pas de tangente inconnue.
	if( m_ControlPoints.size() < 3 )
		return;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A FAIRE:	Resoudre le systeme d'equations lineaires permettant de trouver
	//			les tangentes interieures pour avoir une continuite C2 lorsque
	//			les tangentes aux extremites sont specifiees.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Nombre d'equations a résoudre
	int nbEquations = m_ControlPoints.size()-2;
	// Création des buffers de resolution
	float * a  = new float[nbEquations];
	float * b  = new float[nbEquations];
	float * c  = new float[nbEquations];
	CVector3 * u  = new CVector3[nbEquations];
	CVector3 * r  = new CVector3[nbEquations];
	// Remplissage des buffers de données
	a[0]=0;
	b[0]=4;
	c[0]=1;
	for(int counter = 1; counter<(nbEquations-1); ++counter)
	{
		a[counter] = 1;
		b[counter] = 4;
		c[counter] = 1;
	}
	a[nbEquations-1]=1;
	b[nbEquations-1]=4;
	c[nbEquations-1]=0;

	r[0] = 3*(m_ControlPoints[2].p -m_ControlPoints[0].p - T1/3);
	for(int counter = 1; counter<(nbEquations-1); ++counter)
	{
		r[counter] =3*( m_ControlPoints[counter+2].p - m_ControlPoints[counter].p);
	}
	r[nbEquations-1] = 3*(m_ControlPoints[m_ControlPoints.size()-1].p -m_ControlPoints[m_ControlPoints.size()-3].p - Tn/3);

	// Resolution du système
	if(!solveSystem(a,b,c,u,r,nbEquations))
		std::cout<<"Error while solving the system"<<std::endl;

	// Copie des tangeantes
	m_ControlPoints[0].t = T1;
	for(int counter = 1; counter<(m_ControlPoints.size()-1); ++counter)
	{
		m_ControlPoints[counter].t = u[counter-1];
	}
	m_ControlPoints[m_ControlPoints.size()-1].t = Tn;

	delete [] a;
	delete [] b;
	delete [] c;
	delete [] u;
	delete [] r;
}


void HermiteSpline::C2WithImposedCurvature( const CVector3& C1, const CVector3& Cn )
{
	// Si on a moins de 3 points de controle, on sort
	// car il n'y a pas de tangente inconnue.
	if( m_ControlPoints.size() < 3 )
		return;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A FAIRE:	Resoudre le systeme d'equations lineaires permettant de trouver
	//			les tangentes interieures pour avoir une continuite C2 lorsque
	//			les courbures aux extremites sont specifiee.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Nombre d'equations a résoudre
	int nbEquations = m_ControlPoints.size();
	// Création des buffers de resolution
	float * a  = new float[nbEquations];
	float * b  = new float[nbEquations];
	float * c  = new float[nbEquations];
	CVector3 * u  = new CVector3[nbEquations];
	CVector3 * r  = new CVector3[nbEquations];
	// Remplissage des buffers de données
	a[0]=0;
	b[0]=2;
	c[0]=1;
	for(int counter = 1; counter<(nbEquations-1); ++counter)
	{
		a[counter] = 1;
		b[counter] = 4;
		c[counter] = 1;
	}
	a[nbEquations-1]=1;
	b[nbEquations-1]=2;
	c[nbEquations-1]=0;

	r[0] =3*( m_ControlPoints[1].p - m_ControlPoints[0].p) -C1/2;
	for(int counter = 1; counter<(nbEquations-1); ++counter)
	{
		r[counter] =3*( m_ControlPoints[counter+1].p - m_ControlPoints[counter-1].p);
	}
	r[nbEquations-1] =3*( m_ControlPoints[nbEquations-1].p - m_ControlPoints[nbEquations-2].p)-Cn/2;
	// Resolution du système
	if(!solveSystem(a,b,c,u,r,nbEquations))
		std::cout<<"Error while solving the system"<<std::endl;
	
	// Copie des tangeantes
	for(int counter = 0; counter<(m_ControlPoints.size()); ++counter)
	{
		m_ControlPoints[counter].t = u[counter];
	}

	delete [] a;
	delete [] b;
	delete [] c;
	delete [] u;
	delete [] r;
}


void HermiteSpline::C2WithClosedCurve()
{
	// Si on a moins de 3 points de controle, on sort
	// car on ne peut pas vraiment fermer la courbe.
	if( m_ControlPoints.size() < 3 )
		return;

	// Si le dernier point n'est pas a la meme place que le premier...
	if( m_ControlPoints.back().p != m_ControlPoints[0].p )
	{
		// ...on le deplace pour qu'il soit confondu avec le premier.
		m_ControlPoints.back().p = m_ControlPoints[0].p;
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A FAIRE:	Resoudre le systeme d'equations lineaires permettant de trouver
	//			les tangentes pour avoir une continuite C2 lorsque
	//			la courbe est fermee.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Nombre d'equations a résoudre
	int nbEquations = m_ControlPoints.size()-1;
	// Création des buffers de resolution
	float * a  = new float[nbEquations];
	float * b  = new float[nbEquations];
	float * c  = new float[nbEquations];
	CVector3 * u  = new CVector3[nbEquations];
	CVector3 * r  = new CVector3[nbEquations];
	// Remplissage des buffers de données
	a[0]=0;
	b[0]=4;
	c[0]=1;
	for(int counter = 1; counter<(nbEquations-1); ++counter)
	{
		a[counter] = 1;
		b[counter] = 4;
		c[counter] = 1;
	}
	a[nbEquations-1]=1;
	b[nbEquations-1]=4;
	c[nbEquations-1]=0;

	r[0] =3*( m_ControlPoints[1].p - m_ControlPoints[nbEquations-1].p);
	for(int counter = 1; counter<(nbEquations-1); ++counter)
	{
		r[counter] =3*( m_ControlPoints[counter+1].p - m_ControlPoints[counter-1].p);
	}
	r[nbEquations-1] =3*( m_ControlPoints[0].p - m_ControlPoints[nbEquations-2].p);
	// Resolution du système
	if(!solveSystemCirc(a,b,c,1,1,u,r,nbEquations))
		std::cout<<"Error while solving the system"<<std::endl;
	// Copie des tangeantes
	for(int counter = 0; counter<nbEquations; ++counter)
	{
		m_ControlPoints[counter].t = u[counter];
	}
	m_ControlPoints[nbEquations].t = u[0];


	delete [] a;
	delete [] b;
	delete [] c;
	delete [] u;
	delete [] r;
}


void HermiteSpline::SetSelectedPosition( const CVector3& p )
{
	if( m_SelectedControlPoint != m_ControlPoints.end() )
		m_SelectedControlPoint->p = p;
}


void HermiteSpline::SetSelectedTangent( const CVector3& t )
{
	if( m_SelectedControlPoint != m_ControlPoints.end() )
		m_SelectedControlPoint->t = t;
}


CVector3 HermiteSpline::GetSelectedPosition() const
{
	if( m_SelectedControlPoint != m_ControlPoints.end() )
		return m_SelectedControlPoint->p;
	else
		return CVector3::ZERO;
}


CVector3 HermiteSpline::GetSelectedTangent() const
{
	if( m_SelectedControlPoint != m_ControlPoints.end() )
		return m_SelectedControlPoint->t;
	else
		return CVector3::ZERO;
}


void HermiteSpline::SelectControlPoint( const unsigned int& Id )
{
	CPIterator	it;

	for( it = m_ControlPoints.begin(); it != m_ControlPoints.end(); it++ )
	{
		if( it->Id == Id )
		{
			it->Selected = true;
			m_SelectedControlPoint = it;
			break;
		}
	}
}


void HermiteSpline::SelectControlPointByPosition( const int Pos )
{
	m_SelectedControlPoint	= m_ControlPoints.begin() + Pos;
}


void HermiteSpline::UnselectAll()
{
	CPIterator	it;

	for( it = m_ControlPoints.begin(); it != m_ControlPoints.end(); it++ )
		it->Selected = false;

	m_SelectedControlPoint = m_ControlPoints.end();
}


void HermiteSpline::DeleteSelected()
{
	if( m_SelectedControlPoint != m_ControlPoints.end() )
	{
		m_ControlPoints.erase( m_SelectedControlPoint );
		m_SelectedControlPoint = m_ControlPoints.end();
	}
}


bool HermiteSpline::IsSelection()
{
	return m_SelectedControlPoint != m_ControlPoints.end() ? true : false;
}


void HermiteSpline::Draw( bool SelectMode ) const
{
	int		i;

	// Si on a moins de 2 points de controle, on sort
	// car il n'y a pas de courbe a dessiner.
	if( m_ControlPoints.size() < 2 )
		return;

	// Dessine les points de controle.
	for( i = 0; i < (int)m_ControlPoints.size(); i++ )
	{
		glPushName( m_ControlPoints[i].Id );
		glBegin( GL_POINTS );
		// Si le point est selectionne, on l'affiche en noir.
		if( m_ControlPoints[i].Selected )
			glColor3f( 0.0f, 0.0f, 0.0f );
		else
		// Sinon en violet.
			glColor3f( 1.0f, 0.2f, 0.8f );
		glVertex3f( m_ControlPoints[i].p.x, m_ControlPoints[i].p.y, m_ControlPoints[i].p.z );
		glEnd();
		glPopName();
	}

	// On affiche le reste si on est pas en mode selection.
	if( !SelectMode )
	{
		// On trouve les points selectionnes.
		for( i = 0; i < (int)m_ControlPoints.size(); i++ )
		{
			// Et on dessine leur tangente.
			if( m_ControlPoints[i].Selected )
			{
				glBegin( GL_LINES );
				glColor3f( 0.0f, 0.0f, 0.0f );
				glVertex3f( m_ControlPoints[i].p.x, m_ControlPoints[i].p.y, m_ControlPoints[i].p.z );
				glVertex3f( m_ControlPoints[i].p.x + m_ControlPoints[i].t.x * 1.0f, m_ControlPoints[i].p.y + m_ControlPoints[i].t.y * 1.0f, m_ControlPoints[i].p.z + m_ControlPoints[i].t.z * 1.0f );
				glEnd();
			}
		}
	}
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A FAIRE:	Afficher la courbe d'Hermite a l'aide d'OpenGL.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Facteur de tesselation pour la ligne
	int tessFactor = 100; 
	for( i = 0; i < (m_ControlPoints.size()-1); i++ )
	{
		glBegin( GL_LINES);
		glColor3f( 0.0f, 0.0f, 1.0f );
		for(int j = 0; j < tessFactor; ++j)
		{
			//On calcule les deux points pour la ligne courante
			CVector3 point1 = ComputePoint(m_ControlPoints[i], m_ControlPoints[i+1], j/(float)tessFactor);
			CVector3 point2 = ComputePoint(m_ControlPoints[i], m_ControlPoints[i+1], (j+1)/(float)tessFactor);
			// On trace la ligne entre ces deux points
			glVertex3f( point1.x, point1.y, point1.z);
			glVertex3f( point2.x, point2.y, point2.z);
		}
		glEnd();
	}
}
