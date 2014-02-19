#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include "BSpline.h"
#include <math.h>
#include <GL/gl.h>


#define EPSILON 0.0000001f

int		BSpline::FindSpan( const float u ) const
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A COMPLETER
	//
	// Remplacer le code qui suit du code pertinent!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Pour accelerer la recherche dans la liste triée on effectue une recherche dichotomie
	// Liste triée => recherche dichotomique
	// Nurbs book page 68!
	// Il faut simplement traiter le cas limite avant
	if ( fabs(u - m_Knots[m_Knots.size()-1-m_p]) <= EPSILON )
	{
		return m_Knots.size() - 1 - m_p-1; 
	}
	// Recherche par dichotomie dans la liste
	int begin = m_p, end = m_Knots.size() - 1;
	int mid = (begin + end) / 2;
	while( u <m_Knots[mid] || u >m_Knots[mid+1])
	{

		if ( u < m_Knots[mid] )
		{
			end = mid;
		}
		else 
		{
			begin = mid;
		}
		mid = (begin + end) / 2;
	}
	return mid; 
}


void BSpline::BasisFunctions( const int i, const float u, float *N ) const
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A COMPLETER
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Algorithme de calcul de bases b-spline page 70 du NURBS book (référencé par le cours)
	// le calcul se fait de manière itérative et non récursive!
	N[0] = 1.0;
	float* left = new float[m_p+1];
	float* right = new float[m_p+1];
	for(int j = 1; j <= m_p; ++j)
	{
		left[j] = u - m_Knots[i + 1 - j];
		right[j] = m_Knots[i + j] - u;
		float saved = 0.0;
		for(int r = 0; r < j; ++r)
		{
			float temp = N[r]/( right[r + 1] + left[j - r] );
			N[r] = saved + right[r + 1]*temp;
			saved = left[j - r] * temp;
		}
		N[j] = saved;
	}
	delete [] left;
	delete [] right;
}


CVector3	BSpline::Evaluate( const float u ) const
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A COMPLETER
	//
	// Remplacer le code qui suit du code pertinent!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Récupération de l'intervalle correspondant à notre valeur de U
	int  valSpan = FindSpan(u);
	std::vector<float> baseN(m_p+1, 0);
	// Calcul des fonction de base b-spline
	BasisFunctions(valSpan, u, &baseN[0]);
	// Initialisation de la variable de retour
	CVector3 point (CVector3::ZERO);

	// Calcul du point resultat par (Formule du cours)
	for (int i = 0; i <= m_p; ++i)
	{
		point += m_ControlPoints[valSpan - (m_p) + i] * baseN[i]; 
	}
	// Libération de la mémoire
	return point;
}


void	BSpline::DrawSpline( const int Segments ) const
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A COMPLETER
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	glColor3f(0.0, 0.0, 0.0);
	
	// On récupère la taille de l'intervale
	float intervalSize = m_Knots[m_Knots.size() - 1] - m_Knots[0];
	// ON utilise gl lines pour ne pas fermer la courbe!
	glBegin(GL_LINES);
	// on calcule le premier nombre en dehors de la courbe
	CVector3 resultPoint; 
	resultPoint = Evaluate(m_Knots[0]);
	// Pour chacun des valeurs de l'intervalle [1,Segments]
	for(int i = 1; i <= Segments; ++i)
	{
		// On dessine le point précedent
		glVertex3f(resultPoint.x, resultPoint.y,  resultPoint.z);
		// On calcule la nouvelle etape U et la donnons en paramètre la Evaluate qui va determiner le point 
		// resultat
		resultPoint = Evaluate(m_Knots[0] + i * intervalSize/(float)Segments);
		//On dessine le point suivant
		glVertex3f(resultPoint.x, resultPoint.y,  resultPoint.z);
	}
	glEnd();
}


void	BSpline::DrawBasisFunctions( const int Segments ) const
{
	int		i, j, Span, LastSpan;
	float	*N		= new float[m_p + 1];
	float	*LastN	= new float[m_p + 1];
	float	LastU;

	Span		= FindSpan( 0.0f );
	LastSpan	= FindSpan( 1.0f );

	glBegin( GL_LINES );
	while( Span <= LastSpan )
	{
		if( Span % 2 )
			glColor3f( 0.8f, 0.8f, 0.8f );
		else
			glColor3f( 0.0f, 0.0f, 0.0f );

		LastU	= m_Knots[Span];
		BasisFunctions( Span, m_Knots[Span], LastN );

		for( i = 1; i <= Segments; i++ )
		{
			float	u	= (float)i / Segments * ( m_Knots[Span + 1] - m_Knots[Span] ) + m_Knots[Span];

			BasisFunctions( Span, u, N );

			for( j = 0; j <= m_p; j++ )
			{
				glVertex2f( LastU, LastN[j] );
				glVertex2f( u, N[j] );
				LastN[j]	= N[j];
			}

			LastU		= u;
		}

		do
		{
			Span++;
		}
		while( m_Knots[Span] == m_Knots[Span + 1] );
	}
	glEnd();

	delete[]	N;
	delete[]	LastN;
}


BSpline::BSpline() :
	m_p( 0 ),
	m_Knots( 0 ),
	m_ControlPoints( 0 )
{
}


BSpline::BSpline( const int p, const int m ) :
	m_p( p ),
	m_Knots( m + 1 ),
	m_ControlPoints( m - p )
{
	if( m + 1 < ( p + 1 ) * 2 )
		throw Exception( "Erreur: parametres de creation de la spline invalides" );
}


void	BSpline::Create( const int p, const int m )
{
	if( m_p != p || m_Knots.size() != m + 1 )
	{
		if( m + 1 < ( p + 1 ) * 2 )
			throw Exception( "Erreur: parametres de creation de la spline invalides" );

		m_p	= p;
		m_Knots.resize( m + 1 );
		m_ControlPoints.resize( m - p );
	}
}


void	BSpline::Destroy()
{
	m_p	= 0;
	m_Knots.clear();
	m_ControlPoints.clear();
}


BSpline::~BSpline()
{
}


// Ajoute un point de controle et un noeud
void	BSpline::AddControlPoint( const int Index, const CVector3& ControlPoint )
{
	// Verification que la position est valide
	if( ( Index > (int)m_ControlPoints.size() ) || ( Index < 0 ) )
		throw Exception( "Erreur: ajout d'un point de controle a une position invalide" );

	// Insert le point de controle
	m_ControlPoints.insert( m_ControlPoints.begin() + Index, ControlPoint );
	
	// Insert un noeud entre le noeud [p+1] et le noeud [p+2]
	m_Knots.insert( m_Knots.begin() + m_p + 1, m_Knots[m_p + 1] / 2 );
}


// Ajuste un point de controle
void	BSpline::SetControlPoint( const int Index, const CVector3& ControlPoint )
{
	// Verification que la position est valide
	if( ( Index >= (int)m_ControlPoints.size() ) || ( Index < 0 ) )
		throw Exception( "Erreur: ajustement d'un point de controle a une position invalide" );

	m_ControlPoints[Index] = ControlPoint;
}


// Retourne un point de controle
CVector3	BSpline::GetControlPoint( const int Index ) const
{
	// Verification que la position est valide
	if( ( Index >= (int)m_ControlPoints.size() ) || ( Index < 0 ) )
		throw Exception( "Erreur: requete d'un point de controle a une position invalide" );

	return m_ControlPoints[Index];
}


// Efface un point de controle et un noeud
void	BSpline::DeleteControlPoint( const int Index )
{
	// Verification que la position est valide
	if( ( Index > (int)m_ControlPoints.size() - 1 ) || ( Index < 0 ) )
		throw Exception( "Erreur: effacement d'un point de controle a une position invalide" );

	// Efface le point de controle
	m_ControlPoints.erase( m_ControlPoints.begin() + Index );
	// Efface le noeud [p+1]
	m_Knots.erase( m_Knots.begin() + m_p + 1 );
}


// Retourne un noeud du vecteur nodal
float	BSpline::GetKnot( const int Index ) const
{
	// Verification que la position est valide
	if( ( Index < 0 ) || ( Index > (int)m_Knots.size() ) )
		throw Exception( "Erreur: requete d'un noeud a une position invalide" );

	return m_Knots[Index];
}


// Ajuste la valeur d'un noeud du vecteur nodal
void	BSpline::SetKnot( const int Index, const float Knot )
{
	// Verification que la position est valide
	if( ( Index < 0 ) || ( Index > (int)m_Knots.size() ) )
		throw Exception( "Erreur: ajustement d'un noeud a une position invalide" );

	m_Knots[Index] = Knot;
}


// Change le degre
void	BSpline::SetDegree( const int p )
{
	const int	DeltaP	= p - m_p;
	
	// Verification que le degre est valide
	if( ( p < 1 ) || ( (int)m_ControlPoints.size() <= p ) )
		throw Exception( "Erreur: changement de degre invalide" );
	else if( p == m_p )
		return;

	// Si on augmente l'ordre, on doit ajouter des noeuds au vecteur nodal
	if( DeltaP > 0 )
	{
		// On ajoute les noeuds manquant en alternant a partir du debut ou de la fin
		while( m_p < p )
		{
			// Change la valeur du premier noeud != 0.0 pour 0.0
			m_Knots[m_p + 1] = 0.0f;
			// Ajoute un 1.0 a la fin du vecteur nodal
			m_Knots.push_back( 1.0f );

			m_p++;

			if( m_p < p )
			{
				// Change la valeur du dernier noeud != 1.0 pour 1.0
				m_Knots[m_Knots.size() - m_p - 2] = 1.0f;
				// Ajoute un 0.0 au debut du vecteur nodal
				m_Knots.insert( m_Knots.begin(), 0.0f );

				m_p++;
			}
		}
	}
	// Sinon on doit en enlever
	else
	{
		// On enleve les noeuds superflus en alternant a partir du debut ou de la fin
		while( m_p > p )
		{
			// Enleve le dernier noeud
			m_Knots.pop_back();
			// Remplace le dernier noeud == 0.0
			m_Knots[m_p] = m_Knots[m_p + 1] / 2.0f;

			m_p--;

			if( m_p > p )
			{
				// Enleve le premier noeud
				m_Knots.erase( m_Knots.begin() );
				// Remplace le premier noeud == 1.0
				m_Knots[m_Knots.size() - m_p - 1] = 1.0f - m_Knots[m_Knots.size() - m_p - 2] / 2.0f;
				m_p--;
			}
		}
	}
}
