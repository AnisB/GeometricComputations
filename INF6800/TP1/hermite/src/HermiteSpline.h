#ifndef	_HERMITESPLINE_H_
#define	_HERMITESPLINE_H_

#include "Vector3.h"
#include <vector>


/// Definition d'une courbe d'hermite.
class	HermiteSpline
{
private:
	/// Definition d'un point de controle d'une courbe d'hermite.
	class	ControlPoint
	{
	public:
		/// Initialise le point de controle.
		ControlPoint( const CVector3& _p, const CVector3& _t );

	public:
		/// Position.
		CVector3		p;
		/// Tangente.
		CVector3		t;
		/// Identificateur unique du point de controle.
		unsigned int	Id;
		/// Indique si le point est selectionne.
		bool			Selected;

	private:
		/// Compteur pour generer des identificateurs uniques.
		static unsigned int	IdCounter;
	};


public:
	/// Construit une courbe d'hermite.
	HermiteSpline();

	/// Destructeur.
	virtual ~HermiteSpline();

	/// Ajoute un point de controle a la courbe.
	/**
	@param p la position.
	@param t le vecteur tangent.
	*/
	unsigned int AddControlPoint( const CVector3& p, const CVector3& t = CVector3::ZERO );

	/// Selectionne un point de controle.
	/**
	@param Id l'identificateur unique du point de controle.
	*/
	void SelectControlPoint( const unsigned int& Id );

	/// Selectionne un point de controle par sa position.
	/**
	@param	Pos la position dans le vecteur des points de controle.
	*/
	void SelectControlPointByPosition( const int Pos );

	/// Deselectionne tous les points de controle.
	void UnselectAll();

	/// Efface les points de controle selectionnes.
	void DeleteSelected();

	/// Ajuste la position du point de controle actuellement selectionne.
	/**
	@param p la nouvelle position du point de controle.
	*/
	void SetSelectedPosition( const CVector3& p );

	/// Ajuste la tangente du point de controle actuellement selectionne.
	/**
	@param t la nouvelle tangente du point de controle.
	*/
	void SetSelectedTangent( const CVector3& t );

	/// Retourne la position du point de controle actuellement selectionne.
	CVector3 GetSelectedPosition() const;

	/// Retourne la tangente du point de controle actuellement selectionne.
	CVector3 GetSelectedTangent() const;

	/// Retourne vrai s'il y a au moins un point de controle selectionne.
	bool IsSelection();

	/// Retourne le nombre de points de controle.
	int	GetNbControlPoints() const	{ return (int)m_ControlPoints.size();	}

	/// Dessine la courbe a l'aide d'OpenGL.
	/**
	@param SelectMode	indique si l'on dessine la courbe en mode de selection.
						Si c'est le cas, alors seulement les points de controle
						sont dessines et pas la courbe.
	*/
	void Draw( const bool SelectMode = false ) const;

	/**
	@brief	Calcule un point en fonction de son abssysse curviligne

	@param	parP1 le premier point.
	@param	parP2 le deuxième point.
	@param	t l'abssysse curviligne.
	*/
	CVector3 ComputePoint(ControlPoint parP1, ControlPoint parP2, float parT) const;
	/**
	@brief	Calcule les tangentes a chaque point de controle pour obtenir une
			une continuite C2 en fonction des tangentes aux extremites.

	@param	T1 la tangente du premier point de controle.
	@param	Tn la tangente du dernier point de controle.
	*/
	void C2WithImposedTangents( const CVector3& T1, const CVector3& Tn );

	/**
	@brief	Calcule les tangentes a chaque point de controle pour obtenir une
			une continuite C2 en fonction des courbures aux extremites.

	@param	C1 la courbure du premier point de controle.
	@param	Cn la courbure du dernier point de controle.
	*/
	void C2WithImposedCurvature( const CVector3& C1, const CVector3& Cn );

	/**
	@brief		Calcule les tangentes a chaque point de controle pour obtenir une
				une continuite C2 si la courbe est fermee.
	*/
	void C2WithClosedCurve();

protected:
	/// Vecteur de points de controle.
	typedef std::vector<ControlPoint>			CPVector;

	/// Iterateur dans un vecteur de points de controle.
	typedef std::vector<ControlPoint>::iterator	CPIterator;

	/// Le vecteur des points de controle.
	CPVector	m_ControlPoints;

	/// Le point de controle actuellement selectionne. NULL si aucun.
	CPIterator	m_SelectedControlPoint;
};


void Tridiag ( float *, float *, float *, float *, float *, int);
void Cyclic( float*, float*, float*, float, float, float*, float*, int );


#endif
