#ifndef _BSPLINE_H_
#define _BSPLINE_H_


#include <vector>
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Exception.h"


/// Definition d'une coube b-spline.
class BSpline
{
public:
	/// Constructeur.
	BSpline();

	/// Constructeur.
	/**
	Cette fonction alloue les point de controle et les noeuds necessaires
	pour satisfaires les parametres, mais ces derniers ne sont pas initialises.

	@param p le degree de la courbe.
	@param m le nombre de noeuds - 1.
	*/
	BSpline( const int p, const int m );

	/// Destructeur.
	virtual ~BSpline();

	/// Initialise la courbe pour qu'elle soit de degre p et ayant m noeuds.
	/**
	Cette fonction alloue les point de controle et les noeuds necessaires
	pour satisfaires les parametres, mais ces derniers ne sont pas initialises.

	@param p le degree de la courbe.
	@param m le nombre de noeuds - 1.
	*/
	void		Create( const int p, const int m );

	/// Desaloue la memoire utilisee par la spline.
	void		Destroy();

	/// Ajoute un point de controle et un noeud.
	/**
	L'ajout d'un point de controle implique l'ajout d'un noeud.  Idealment il aurait
	fallu utiliser l'algorithme d'insertion de noeud, mais pour simplifier les choses,
	j'ai utilise quelque chose de plus simple.

	@param Index la position ou le point de controle sera insere.
	@param ControlPoint le point de controle.
	*/
	void		AddControlPoint( const int Index, const CVector3& ControlPoint );

	/// Ajuste un point de controle.
	/**
	@param Index le numero du point de controle.
	@param ControlPoint la nouvelle valeur du point de controle.
	*/
	void		SetControlPoint( const int Index, const CVector3& ControlPoint );

	/// Retourne un point de controle.
	/**
	@param Index le numero du point de controle.
	*/
	CVector3	GetControlPoint( const int Index ) const;

	/// Efface un point de controle et un noeud.
	/**
	Lorsqu'on enleve un point de controle, on doit aussi enlever un
	noeud.  Pour garder les choses simples, on enleve toujours le
	premier point de controle.

	@param Index le numero du point de controle.
	*/
	void		DeleteControlPoint( const int Index );

	/// Retourne le nombre de points de controle.
	int			GetControlPointNb() const	{ return (int)m_ControlPoints.size();	}

	/// Retourne le nombre de noeuds.
	int			GetKnotsNb() const			{ return (int)m_Knots.size();			}

	/// Retourne le degre.
	int			GetDegree() const			{ return m_p;							}

	/// Retourne un noeud du vecteur nodal.
	/**
	@param Index le numero du noeud.
	*/
	float		GetKnot( const int Index ) const;

	/// Ajuste la valeur d'un noeud du vecteur nodal.
	/**
	@param Index le numero du point de controle.
	@param Knot la nouvelle valeur du noeud.
	*/
	void		SetKnot( const int Index, const float Knot );

	/// Change le degre de la courbe.
	/**
	Le changement de degree implique l'ajout ou la suppression de noeuds
	dans le vecteur nodal.  Idealement, il faudrait utiliser les algorithmes
	d'elevation ou diminution de degree, mais pour simplifier les choses,
	j'ai utilise quelque chose de plus simple.

	@param p le nouveau degre.
	*/
	void		SetDegree( const int p );

	/// Dessine la courbe.
	/**
	@param Segments le nombre de segments utilises pour la discretisation.
	*/
	void		DrawSpline( const int Segments ) const;

	/// Dessine les fonctions de base.
	void		DrawBasisFunctions( const int Segments ) const;

	/// Trouve l'intervalle dans lequel est compris u.
	/**
	@param u le parametre.
	*/
	int			FindSpan( const float u ) const;

	/// Calcule les fonctions de base.
	/**
	@param i l'intervalle dans lequel est compris u.
	@param u la valeur du parametre pour laquelle on calcule les fonctions de base.
	@param N un tableau de p + 1 valeurs 
	*/
	void		BasisFunctions( const int i, const float u, float *N ) const;

	/// Evalue un point de la courbe.
	/**
	@param u le parametre ou la courbe sera evaluee.
	*/
	CVector3	Evaluate( const float u ) const;


protected:
	/// Vecteur de points de controle.
	typedef std::vector<CVector3>			CPVector;

	/// Iterateur des point de controle.
	typedef std::vector<CVector3>::iterator	CPIterator;

	/// Vecteur de noeuds.
	typedef std::vector<float>				KnotVector;

	/// Iterateur des noeuds.
	typedef std::vector<float>::iterator	KnotIterator;

	/// Le vecteur des points de controle.
	CPVector	m_ControlPoints;

	/// Le vecteur nodal.
	KnotVector	m_Knots;
	
	/// Le degree de la courbe.
	int			m_p;
};


#endif
