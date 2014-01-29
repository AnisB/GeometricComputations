#include "BezierSpline.h"
#ifdef	_WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

using namespace std;


unsigned int	BezierSpline::ControlPoint::IdCounter = 0xFFFF;



BezierSpline::ControlPoint::ControlPoint( const CVector4& _p ) :
	Selected( false ),
	Id( ++IdCounter ),
	p( _p ),
	IsOutDated(true)
{
}


BezierSpline::BezierSpline()
{
	m_SelectedControlPoint	= m_ControlPoints.end();
}


BezierSpline::~BezierSpline()
{
}


unsigned int BezierSpline::AddControlPoint( const CVector4& p )
{
	// On ajoute le point de controle a la fin du vecteur.
	m_ControlPoints.push_back( ControlPoint( p ) );

	return m_ControlPoints.back().Id;
}


void BezierSpline::SetSelectedPosition( const CVector4& p )
{
	if( m_SelectedControlPoint != m_ControlPoints.end() )
		m_SelectedControlPoint->p = p;
}


CVector4 BezierSpline::GetSelectedPosition() const
{
	if( m_SelectedControlPoint != m_ControlPoints.end() )
		return m_SelectedControlPoint->p;
	else
		return CVector4::ZERO;
}


void BezierSpline::SelectControlPoint( const unsigned int& Id )
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


void BezierSpline::SelectControlPointByPosition( const int Pos )
{
	m_SelectedControlPoint	= m_ControlPoints.begin() + Pos;
}


void BezierSpline::UnselectAll()
{
	CPIterator	it;

	for( it = m_ControlPoints.begin(); it != m_ControlPoints.end(); it++ )
		it->Selected = false;

	m_SelectedControlPoint = m_ControlPoints.end();
}


void BezierSpline::DeleteSelected()
{
	if( m_SelectedControlPoint != m_ControlPoints.end() )
	{
		m_ControlPoints.erase( m_SelectedControlPoint );
		m_SelectedControlPoint = m_ControlPoints.end();
	}
}


bool BezierSpline::IsSelection()
{
	return m_SelectedControlPoint != m_ControlPoints.end() ? true : false;
}

CVector3 BezierSpline::ComputePoint(const CPVector& parPoints, float parT) const
{
    int nbPoints = parPoints.size();
    std::cout<<"Nombre de points"<<nbPoints<<std::endl;
    CVector3 ** baseVal = new CVector3*[nbPoints];
    for(int i = 0; i<nbPoints;++i)
    {
    	baseVal[i] = new CVector3[nbPoints];
    }
    for(int i = 0; i<nbPoints;++i)
    {
    	baseVal[0][i] = parPoints[i].p;
    }
    for(int j = 1; j < nbPoints; j++)
    {
    	std::cout<<"J "<<j<<std::endl;
        for(int i = 0; i < nbPoints - j; i++)
        {
        	std::cout<<"I "<<i<<std::endl;
        	std::cout<<"baseVal[j][i].p "<<baseVal[j][i]<<std::endl;
        	std::cout<<"baseVal[j-1][i].p "<<baseVal[j-1][i]<<std::endl;
        	std::cout<<"baseVal[j-1][i+1].p "<<baseVal[j-1][i+1]<<std::endl;
            baseVal[j][i] = baseVal[j-1][i] * (1-parT) + baseVal[j-1][i+1] * parT;
        }
    }
    CVector3 result = baseVal[nbPoints-1][0];
    delete [] baseVal;
    return result;	
}

void BezierSpline::Draw( bool SelectMode ) const
{
	int			i;

	// Si on a moins de 2 points de controle, on sort
	// car il n'y a pas de courbe a dessiner.
	if( m_ControlPoints.size() < 2 )
		return;

	// Dessine les points de controle.
	for( i = 0; i < (int)m_ControlPoints.size(); i++ )
	{
		// Projette le point de controle.
		// (Le constructeur d'un CVector3 prenant
		// comme argument un CVector4 effectue
		// la division de projection, donc:
		// CVector3	v3( v4 );
		// ou v4 est un CVector4 est equivalent a:
		// v3 = CVector3( v4.x / v4.w, v4.y / v4.w, v4.z / v4.w );
		CVector3	pt(  m_ControlPoints[i].p );

		glPushName( m_ControlPoints[i].Id );
		glBegin( GL_POINTS );
		// Si le point est selectionne, on l'affiche en noir.
		if( m_ControlPoints[i].Selected )
			glColor3f( 0.0f, 0.0f, 0.0f );
		// Sinon en violet.
		else
			glColor3f( 1.0f, 0.2f, 0.8f );
		glVertex3f( pt.x, pt.y, pt.z );
		glEnd();
		glPopName();
	}

	if( !SelectMode )
	{
		// Dessine le polygone de controle.
		glColor3f( 0.1f, 0.1f, 1.0f );
		glBegin( GL_LINE_STRIP );
		for( i = 0; i < (int)m_ControlPoints.size(); i++ )
		{
			// Projette la position du point de controle.
			CVector3	pt(  m_ControlPoints[i].p );

			glVertex3f( pt.x, pt.y, pt.z );
		}
		glEnd();

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// A FAIRE:	Dessiner la courbe de Bezier rationnelle a l'aide de
		//			l'algorithme de deCasteljau.
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
	glBegin( GL_LINE_STRIP );
	int tessFactor = 10;
	for(int i = 0;i <= tessFactor; ++i)
	{
		CVector3 point = ComputePoint(m_ControlPoints,i/tessFactor);
		glVertex3f( point.x, point.y, point.z );
	}
	glEnd();
	
}
