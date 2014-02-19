#ifndef _BSPLINE_H_
#define _BSPLINE_H_


#include <vector>
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Exception.h"


/// Definition d'une surface b-splinaire.
class BSurface
{
public:
	/// Indicateur de direction parametrique.
	typedef enum { DIR_U, DIR_V } Direction;

public:
	/// Constructeur.
	BSurface();

	/// Constructeur.
	/**
	Cette fonction alloue les point de controle et les noeuds necessaires
	pour satisfaires les parametres, mais ces derniers ne sont pas initialises.

	@param pu le degree de la surface en u.
	@param pv le degree de la surface en v.
	@param mu le nombre de noeuds en u - 1.
	@param mv le nombre de noeuds en v - 1.
	*/
	BSurface( const int pu, const int pv, const int mu, const int mv );

	/// Destructeur.
	virtual ~BSurface();

	/// Initialise la courbe pour qu'elle soit de degre p et ayant m noeuds.
	/**
	Cette fonction alloue les point de controle et les noeuds necessaires
	pour satisfaires les parametres, mais ces derniers ne sont pas initialises.

	@param pu le degree de la surface en u.
	@param pv le degree de la surface en v.
	@param mu le nombre de noeuds en u - 1.
	@param mv le nombre de noeuds en v - 1.
	*/
	void		Create( const int pu, const int pv, const int mu, const int mv );

	/// Desaloue la memoire utilisee par la spline.
	void		Destroy();

	/// Ajuste un point de controle.
	/**
	@param i le numero du point de controle en u.
	@param j le numero du point de controle en v.
	@param ControlPoint la nouvelle valeur du point de controle.
	*/
	void		SetControlPoint( const int i, const int j, const CVector3& ControlPoint );

	/// Retourne un point de controle.
	/**
	@param i le numero du point de controle en u.
	@param j le numero du point de controle en v.
	*/
	CVector3	GetControlPoint( const int i, const int j ) const;

	/// Retourne le nombre de points de controle.
	/**
	@param Dir la direction pour laquelle on veut le nombre de points de controle.
	*/
	int			GetControlPointNb( const Direction Dir ) const;

	/// Retourne le nombre de noeuds.
	/**
	@param Dir la direction pour laquelle on veut le nombre de noeuds.
	*/
	int			GetKnotsNb( const Direction Dir ) const;

	/// Retourne le degre.
	/**
	@param Dir la direction pour laquelle on veut le degree.
	*/
	int			GetDegree( const Direction Dir ) const;

	/// Retourne un noeud du vecteur nodal.
	/**
	@param Dir la direction pour laquelle on veut obtenir le noeud.
	@param Index le numero du noeud.
	*/
	float		GetKnot( const Direction Dir, const int Index ) const;

	/// Ajuste la valeur d'un noeud du vecteur nodal.
	/**
	@param Dir la direction pour laquelle on veut ajuster le noeud.
	@param Index le numero du point de controle.
	@param Knot la nouvelle valeur du noeud.
	*/
	void		SetKnot( const Direction Dir, const int Index, const float Knot );

	/// Dessine la surface.
	/**
	@param USegments le nombre de segments utilises pour la discretisation en u.
	@param VSegments le nombre de segments utilises pour la discretisation en v.
	*/
	void		Draw( const int USegments, const int VSegments ) const;

	/// Trouve l'intervalle dans lequel est compris le parametre.
	/**
	@param Dir la direction pour laquelle on veut l'intervalle.
	@param uv le parametre.
	*/
	int			FindSpan( const Direction Dir, const float uv ) const;

	/// Calcule les fonctions de base.
	/**
	@param Dir la direction pour laquelle on veut l'intervalle.
	@param ij l'intervalle dans lequel est compris le parametre.
	@param uv la valeur du parametre pour laquelle on calcule les fonctions de base.
	@param N un tableau de pu + 1 ou pv + 1 valeurs, dependemment de la direction.
	*/
	void		BasisFunctions( const Direction Dir, const int ij, const float uv, float *N ) const;

	/// Calcule les derivees des fonctions de base.
	/**
	@param uv la valeur du parametre pour laquelle on calcule les derivees des fonctions de base.
	@param ij  l'intervalle dans lequel est compris le parametre.
	@param n l'ordre de la derniere derivee a calculer.
	@param dN le tableau contenant les derivees. dN[k][j] contient la derivee k de la fonction j.
	@param Dir the direction.
	*/
	void		BasisFunctionDerivatives( const Direction Dir, const int ij, const float uv, const int n, float **dN ) const;

	/// Evalue un point de la surface.
	/**
	@param u le parametre u ou la surface sera evaluee.
	@param v le parametre v ou la surface sera evaluee.
	*/
	CVector3	Evaluate( const float u, const float v ) const;

	/// Evalue les derivees de la surface.
	/**
	Cette fonction calcule les derivees 0-n dans les directions u et v.

	@param u la valeur du parametre u pour laquelle on calcule la derivee.
	@param v la valeur du parametre v pour laquelle on calcule la derivee.
	@param d l'ordre de la derniere derivee a calculer.
	@param Derivatives le tableau contenant les derivees.  Derivatives[k][l] contient la derivee k en u en l en v de la surface.
	*/
	void		EvaluateDerivatives( const float u, const float v, const int d, CVector3 **Derivatives ) const;


protected:
	/// Vecteur de points de controle.
	typedef std::vector<CVector3>			CPVector;

	/// Grille de points de controle.
	typedef std::vector<CPVector>			CPGrid;

	/// Vecteur de noeuds.
	typedef std::vector<float>				KnotVector;

	/// La grille des points de controle.
	CPGrid		m_ControlPoints;

	/// Le vecteur nodal en u.
	KnotVector	m_UKnots;

	/// Le vecteur nodal en v.
	KnotVector	m_VKnots;
	
	/// Le degree de la courbe en u.
	int			m_pu;

	/// Le degree de la courbe en v.
	int			m_pv;
};


#endif
