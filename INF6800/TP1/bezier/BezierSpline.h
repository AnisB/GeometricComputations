#ifndef	_BEZIERSPLINE_H_
#define	_BEZIERSPLINE_H_

#include "Vector4.h"
#include <vector>
#include "Vector3.h"


/// Definition d'une courbe de bezier.
class	BezierSpline
{
private:
	/// Definition d'un point de controle d'une courbe de bezier.
	class	ControlPoint
	{
	public:
		/// Initialise le point de controle.
		ControlPoint( const CVector4& _p );

	public:
		/// Position.
		/**
		La 4e composante du vecteur est le poid.  Les points sont
		premultiplies par le poid, c'est a dire qu'il ont la forme:
		( w*x, w*y, w*z, w )
		*/
		CVector4		p;
		/// Identificateur unique du point de controle.
		unsigned int	Id;
		/// Indique si le point est selectionne.
		bool			Selected;

	private:
		/// Compteur pour generer des identificateurs uniques.
		static unsigned int	IdCounter;
	};
protected:
	/// Vecteur de points de controle.
	typedef std::vector<ControlPoint>			CPVector;

	/// Iterateur dans un vecteur de points de controle.
	typedef std::vector<ControlPoint>::iterator	CPIterator;

public:
	/// Construit une courbe d'bezier.
	BezierSpline();

	/// Destructeur.
	virtual ~BezierSpline();

	// Calcule la valeur de la courbe de bezier avec l'algorithme de castelnau
	CVector3 ComputePoint(CPVector parPoints, float parT) const;
	
	/// Ajoute un point de controle a la courbe.
	/**
	@param p la position.
	@param t le vecteur tangent.
	*/
	unsigned int AddControlPoint( const CVector4& p );

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
	void SetSelectedPosition( const CVector4& p );

	/// Retourne la position du point de controle actuellement selectionne.
	CVector4 GetSelectedPosition() const;

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

protected:
	/// Le vecteur des points de controle.
	CPVector	m_ControlPoints;

	/// Le point de controle actuellement selectionne. NULL si aucun.
	CPIterator	m_SelectedControlPoint;
};


#endif
