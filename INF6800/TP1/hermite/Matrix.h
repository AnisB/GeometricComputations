//-----------------------------------------------------------------------------
// This file is part of the b-reality project:
// http://sourceforge.net/projects/b-reality
//
// (C) Francis Page  2003
//-----------------------------------------------------------------------------

#ifndef	_MATRIX_H_
#define _MATRIX_H_


#if _MSC_VER == 1200
#pragma warning(disable:4786)
#endif

#include	<iostream>

using namespace std;



/// Definition of an any size matrix class.
/**
This class can be used when operations on matrices of any size are needed.
*/
class	CMatrix
{
public:
	/// Construct a matrix object.
	/**
	@param m number of rows.
	@param n number of columns.
	*/
	CMatrix( const int m = 0, const int n = 0 );

	/// Copy constructor.
	CMatrix( const CMatrix& m );
	
	/// Destructor.
	virtual	~CMatrix();

	/// Recreate a matrix.
	/**
	This function destroy the current matrix data and resize the matrix
	to the specified size.
	@param m number of rows.
	@param n number of columns.
	*/
	void		Recreate( const int m = 0, const int n = 0 );

	/// Returns a copy of the matrix.
	CMatrix&	operator = ( const CMatrix& m );

	/// Multiplies two matrices.
	CMatrix		operator * ( const CMatrix& m ) const;

	/// Adds two matrices.
	CMatrix		operator + ( const CMatrix& m ) const;

	/// Subtracts two matrices.
	CMatrix		operator - ( const CMatrix& m ) const;

	/// Multiplies by the transpose of a matrix.
	CMatrix		MultTranspose( const CMatrix& m ) const;

	/// Multiplies the transpose of the matrix with another matrix.
	CMatrix		TransposeMult( const CMatrix& m ) const;

	/// Returns a matrix element.
	/**
	@param i the row.
	@param j the column.
	*/
	float	Get( const int i, const int j ) const
	{
		return m_pData[i * m_n + j];
	}

	/// Sets a matrix element.
	/**
	@param i the row.
	@param j the column.
	@param x the value.
	*/
	void	Set( const int i, const int j, const float x )
	{
		m_pData[i * m_n + j] = x;
	}

	/// Returns the number of rows.
	int		GetM() const	{ return m_m;	}

	/// Returns the number of columns.
	int		GetN() const	{ return m_n;	}

	/// Outputs a matrix to a stream.
	friend	ostream&	operator << ( ostream& os, const CMatrix& m );

protected:
	/// The number of rows.
	int		m_m;

	/// The number of columns.
	int		m_n;

	/// The matrix elements pointer.
	float	*m_pData;
};



#endif
