#ifdef _WIN32
#include <windows.h>
#endif

#include "BSurface.h"
#include <math.h>
#include <GL/gl.h>

#define EPSILON 0.0000001f



int		BSurface::FindSpan( const Direction Dir, const float uv ) const
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A COMPLETER
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Dans un premier temps, on determine quelle est la direction qui nous 
	// concerne c-a-d le vecteur nodal et le degree correspondant
	// Initialisation des variables
	int degreReel;
	KnotVector noeudReels;
	// Test des vlaeurs
	if (Dir == DIR_U)
	{
		degreReel = m_pv;
		noeudReels = m_VKnots;
	}
	else 
	{
		degreReel = m_pu;
		noeudReels = m_UKnots;
	}

	// Comme pour le cas des bspline courbes, nous allons faire une recherche dichotomique
	// car nous devons trouver une donnée dans une liste ordonnée
	// Nurbs book page 68!
	// Première étape gestion du cas limite
	if ( fabs(uv - noeudReels[noeudReels.size()-1-degreReel]) <= EPSILON )
	{
		return noeudReels.size() - 1 - degreReel-1; 
	}
	// Recherche par dichotomie dans la liste
	int begin = degreReel, end = noeudReels.size() - 1;
	int mid = (begin + end) / 2;
	while( uv <noeudReels[mid] || uv >noeudReels[mid+1])
	{

		if ( uv < noeudReels[mid] )
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


void	BSurface::BasisFunctions( const Direction Dir, const int ij, const float uv, float *N ) const
{
	BasisFunctionDerivatives( Dir, ij, uv, 0, &N );
}


void	BSurface::BasisFunctionDerivatives( const Direction Dir, const int ij, const float uv, const int n, float **dN ) const
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A COMPLETER
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Dans un premier temps, on determine quelle est la direction qui nous 
	// concerne c-a-d le vecteur nodal et le degree correspondant
	// Initialisation des variables
	int degreReel;
	KnotVector noeudReels;
	// Test des vlaeurs
	if (Dir == DIR_U)
	{
		degreReel = m_pv;
		noeudReels = m_VKnots;
	}
	else 
	{
		degreReel = m_pu;
		noeudReels = m_UKnots;
	}
	// Initialisation du conteneur de dérivées
	float** ndu = new float*[degreReel+1];
	for(int i = 0; i < (degreReel+1); ++i)
	{
		ndu[i] = new float[degreReel+1];
	}
	ndu[0][0] = 1.0;
	
	// Initialisation des variables left et right
	float* left = new float[degreReel+1];
	float* right = new float[degreReel+1];
	for(int j = 1; j <= degreReel; ++j)
	{
		left[j] = uv - noeudReels[ ij + 1 - j];
		right[j] = noeudReels[ij + j] - uv;
		float saved = 0.0;
		for(int r = 0; r < j; ++r)
		{
			ndu[j][r] = right[r + 1] + left[j - r];
			float temp = ndu[r][j - 1]/ndu[j][r];

			ndu[r][j] = saved + right[r + 1] * temp;
			saved = left[j - r] * temp;
		}
		ndu[j][j] = saved;
	}

	for(int j = 0; j <= degreReel; j++)
	{
		dN[0][j] = ndu[j][degreReel];
	}

	// Les calcul des dérivées se font ici
	float** a = new float*[2];
	a[0] = new float[degreReel+1];
	a[1] = new float[degreReel+1];

	for(int r = 0; r <= degreReel; r++)
	{
		int s1=0, s2=1;
		a[0][0] = 1.0;
		int j1, j2;
		for(int k = 1; k <= n; ++k)
		{
			float d = 0.0;
			int rk = r-k;
			int pk = degreReel-k;
			if(r >= k)
			{
				a[s2][0] = a[s1][0]/ndu[pk+1][rk];
				d = a[s2][0] * ndu[rk][pk];
			}
			if(rk >= -1) 
			{
				j1 = 1;
			}
			else 
			{
				j1 = -rk;
			}
			if((r-1) <= pk) 
			{
				j2 = k - 1;
			}
			else 
			{
				j2 = degreReel - r;
			}
			for(int j = j1; j <= j2; ++j)
			{
				a[s2][j] = (a[s1][j] - a[s1][j - 1])/ndu[pk+1][rk+j];
				d += a[s2][j]*ndu[rk+j][pk];
			}
			if(r <= pk)
			{
				a[s2][k] = -a[s1][k-1]/ndu[pk+1][r];
				d += a[s2][k] * ndu[r][pk];
			}
			dN[k][r] = d;
			int tmp = s1; 
			s1 = s2; 
			s2 = tmp; 
		}
	}

	//Multiplication par les bons facteurs
	// Eq 2.9
	int r = degreReel;
	for(int k = 1; k <= n; ++k)
	{
		for(int j = 0; j <= degreReel; ++j)
		{
			dN[k][j] *= r;
		}
		r *= (degreReel-k);
	}
	// On libere l'espace memoire associe a ndu et a
	for(int i = 0; i < (degreReel+1); i++)
	{
		delete[] ndu[i];
	}
	delete[] ndu;
	delete[] a[0];
	delete[] a[1];
	delete[] a;
}


CVector3	BSurface::Evaluate( const float u, const float v ) const
{
	CVector3	*p, Point;
	p			= &Point;

	EvaluateDerivatives( u, v, 1, &p );

	return Point;
}


void	BSurface::EvaluateDerivatives( const float u, const float v, const int d, CVector3 **Derivatives ) const
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A COMPLETER
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Implémentation de l'algorithme du Nurbs book page 111 pour calculer les valeurs des dérivées 
	// Pour un point determiné par U et V jusqu'a un ordre donnée en paramètre d
	// Calcul de l'ordre maximal entre u et d
	int du = std::min(d, m_pu);
	// Initialisation à une valeur nulle le tableau des dérivées
	for(int i = m_pu+1; i <= d; ++i)
	{
		const int max = d-i;
		for(int j = 0; j <= max; ++j)
		{
			Derivatives[i][j] = CVector3::ZERO;
		}
	}
	// Calcul de l'ordre maximal entre v et d
	int dv = std::min(d, m_pv);
	// Initialisation à une valeur nulle le tableau des dérivées
	for(int i = m_pv+1; i <= d; ++i)
	{
		const int max = d-i;
		for(int j = 0; j <= max; ++j)
		{
			Derivatives[i][j] = CVector3::ZERO;
		}
	}
	// Récupération de l'intervalle concerné sur la direction U
	int uspan = FindSpan(DIR_U, u);
	// Allocation de la mémoire pour le vecteur des dérivées partielles sur U
	float** dVu = new float*[d+1];
	for(int i = 0; i < d+1; ++i)
	{
		dVu[i] = new float[m_pu+1];
	}
	// Calcul des dérivées de base pour la direction U
	BasisFunctionDerivatives(DIR_U, uspan, u, du, dVu);

	// On effectue la même opération pour la direction V
	int vspan = FindSpan(DIR_V, v);
	float** dVv = new float*[d+1];
	for(int i = 0; i < d+1; ++i)
	{
		dVv[i] = new float[m_pv+1];
	}
	BasisFunctionDerivatives(DIR_V, vspan, v, dv, dVv);

	// Calcul des dérivées
	CVector3* temp = new CVector3[m_pv+1];
	for(int k = 0; k <= du; ++k)
	{
		for(int s = 0; s <= m_pv; ++s)
		{
			temp[s] = CVector3::ZERO;
			for(int r = 0; r <= m_pu; r++)
			{
				temp[s] += dVu[k][r] * m_ControlPoints[uspan - m_pu + r][vspan - m_pv + s];
			}
		}
		int dd = std::min(d-k, dv);
		for(int l = 0; l <= dd; ++l)
		{
			Derivatives[k][l] = CVector3::ZERO;
			for(int s = 0; s <= m_pv; ++s)
			{
				Derivatives[k][l]+= dVv[l][s] * temp[s];
			}
		}
	}

	// Libération de la mémoire allouée pour les calculs
	for(int i = 0; i < d+1; i++)
	{
		delete[] dVu[i];
		delete[] dVv[i];
	}
	delete[] dVu;
	delete[] dVv;
	delete[] temp;
}


void	BSurface::Draw( const int USegments, const int VSegments ) const
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A COMPLETER
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Allocation de la mémoire pour le calcul des dérivées 
	CVector3** derivatives = new CVector3*[2];
	derivatives[0] = new CVector3[2];
	derivatives[1] = new CVector3[2];
	// Initialisation des vecteurs U&V
	float u = 0, v = 0;
	// Précalcul de la taille des intervalles par soucis de performance
	float intervalSizeU = m_UKnots[m_UKnots.size()-1] - m_UKnots[0];
	float intervalSizeV = m_VKnots[m_VKnots.size()-1] - m_VKnots[0];

	// Coleur noire
	glColor3f(0.0, 0.0, 0.0);

	// Pour chacune des valeurs de l'intervalle [0,USegments] 
	for(int i = 0; i < USegments; ++i)
	{
		glBegin(GL_QUAD_STRIP);
		// Pour chacun des valeurs de l'intervalle [0,USegments] 
		for(int j = 0; j < VSegments + 1; ++j)
		{
			// On calcule les nouveaux  U et V pour le point suivant
			u = m_UKnots[0] + i*intervalSizeU/(float)USegments;
			v = m_VKnots[0] + j*intervalSizeV/(float)VSegments;

			// ON évalue le point à (u,v)
			EvaluateDerivatives(u, v, 1, derivatives); 
			// Produit vectoriel des deux dérivées première donne la normale au plan
			CVector3 normale =derivatives[1][0]^derivatives[0][1];
			glNormal3f(normale.x,normale.y,normale.z);
			// // La position du sommet
			glVertex3f(derivatives[0][0].x, derivatives[0][0].y, derivatives[0][0].z);

			u = m_UKnots[0] + (i+1) * (intervalSizeU / (float)USegments) ;
			EvaluateDerivatives(u, v, 1, derivatives); // On ne s'intéresse qu'aux dérivées 1ères
			// Produit vectoriel des deux dérivées première donne la normale au plan
			normale = derivatives[1][0]^derivatives[0][1];
			glNormal3f(normale.x,normale.y,normale.z);
			// // La position du sommet
			glVertex3f(derivatives[0][0].x, derivatives[0][0].y, derivatives[0][0].z);	
		}
		glEnd();
	}

	// On libère la matrice ayant servi aux calculs
	delete[] derivatives[0];
	delete[] derivatives[1];
	delete[] derivatives;
}


BSurface::BSurface() :
	m_pu( 0 ),
	m_pv( 0 ),
	m_UKnots( 0 ),
	m_VKnots( 0 ),
	m_ControlPoints( 0 )
{
}


BSurface::BSurface( const int pu, const int pv, const int mu, const int mv ) :
	m_pu( pu ),
	m_pv( pv ),
	m_UKnots( mu + 1 ),
	m_VKnots( mv + 1 ),
	m_ControlPoints( mu - pu, CPVector( mv - pv ) )
{
	if( ( mu + 1 < ( pu + 1 ) * 2 ) || ( mv + 1 < ( pv + 1 ) * 2 ) )
		throw Exception( "Erreur: parametres de creation de la surface invalides" );
}


void	BSurface::Create( const int pu, const int pv, const int mu, const int mv )
{
	if( m_pu != pu || m_UKnots.size() != mu + 1 || m_pv != pv || m_VKnots.size() != mv + 1 )
	{
		if( ( mu + 1 < ( pu + 1 ) * 2 ) || ( mv + 1 < ( pv + 1 ) * 2 ) )
		throw Exception( "Erreur: parametres de creation de la surface invalides" );

		m_pu	= pu;
		m_pv	= pv;
		m_UKnots.resize( mu + 1 );
		m_VKnots.resize( mv + 1 );
		m_ControlPoints.resize( mu - pu, CPVector( mv - pv ) );
	}
}


void	BSurface::Destroy()
{
	m_pu	= 0;
	m_pv	= 0;
	m_UKnots.clear();
	m_VKnots.clear();
	m_ControlPoints.clear();
}


BSurface::~BSurface()
{
}


// Ajuste un point de controle
void	BSurface::SetControlPoint( const int i, const int j, const CVector3& ControlPoint )
{
	// Verification que la position est valide
	if( ( i >= (int)m_ControlPoints.size() ) || ( i < 0 ) || ( j >= (int)m_ControlPoints[0].size() ) || ( j < 0 ) )
		throw Exception( "Erreur: ajustement d'un point de controle a une position invalide" );

	m_ControlPoints[i][j] = ControlPoint;
}


// Retourne un point de controle
CVector3	BSurface::GetControlPoint( const int i, const int j ) const
{
	// Verification que la position est valide
	if( ( i >= (int)m_ControlPoints.size() ) || ( i < 0 ) || ( j >= (int)m_ControlPoints[0].size() ) || ( j < 0 ) )
		throw Exception( "Erreur: requete d'un point de controle a une position invalide" );

	return m_ControlPoints[i][j];
}


// Retourne un noeud du vecteur nodal
float	BSurface::GetKnot( const Direction Dir, const int Index ) const
{
	const KnotVector	*U;

	if( Dir == DIR_U )
		U	= &m_UKnots;
	else
		U	= &m_VKnots;

	// Verification que la position est valide
	if( ( Index < 0 ) || ( Index > (int)U->size() ) )
		throw Exception( "Erreur: requete d'un noeud a une position invalide" );

	return (*U)[Index];
}


// Ajuste la valeur d'un noeud du vecteur nodal
void	BSurface::SetKnot( const Direction Dir, const int Index, const float Knot )
{
	KnotVector	*U;

	if( Dir == DIR_U )
		U	= &m_UKnots;
	else
		U	= &m_VKnots;

	// Verification que la position est valide
	if( ( Index < 0 ) || ( Index > (int)U->size() ) )
		throw Exception( "Erreur: ajustement d'un noeud a une position invalide" );

	(*U)[Index] = Knot;
}


int	BSurface::GetControlPointNb( const Direction Dir ) const
{
	if( Dir == DIR_U )
		return (int)m_UKnots.size() - m_pu - 1;
	else
		return (int)m_VKnots.size() - m_pv - 1;
}


int	BSurface::GetKnotsNb( const Direction Dir ) const
{
	if( Dir == DIR_U )
		return (int)m_UKnots.size();
	else
		return (int)m_VKnots.size();
}


int	BSurface::GetDegree( const Direction Dir ) const
{
	if( Dir == DIR_U )
		return m_pu;
	else
		return m_pv;
}

