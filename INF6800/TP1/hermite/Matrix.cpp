//-----------------------------------------------------------------------------
// This file is part of the b-reality project:
// http://sourceforge.net/projects/b-reality
//
// (C) Francis Page  2003
//-----------------------------------------------------------------------------

#include "Matrix.h"
#include <string.h>



CMatrix::CMatrix( const int m, const int n ) :
	m_pData( NULL ),
	m_m( m ),
	m_n( n )
{
	m_pData	= new float[m_m * m_n];

	memset( m_pData, 0, sizeof( float ) * m_m * m_n );
}


void	CMatrix::Recreate( const int m, const int n )
{
	delete[]	m_pData;

	m_m		= m;
	m_n		= n;
	m_pData	= new float[m_m * m_n];

	memset( m_pData, 0, sizeof( float ) * m_m * m_n );
}


CMatrix::CMatrix( const CMatrix& m ) :
	m_pData( NULL ),
	m_m( m.m_m ),
	m_n( m.m_n )
{
	m_pData	= new float[m_m * m_n];

	memcpy( m_pData, m.m_pData, m_m * m_n * sizeof( float ) );
}


CMatrix::~CMatrix()
{
	delete[]	m_pData;
}


CMatrix&	CMatrix::operator = ( const CMatrix& m )
{
	if( this == &m )
		return *this;

	delete[]	m_pData;

	m_m	= m.m_m;
	m_n	= m.m_n;

	m_pData	= new float[m_m * m_n];

	memcpy( m_pData, m.m_pData, m_m * m_n * sizeof( float ) );

	return *this;
}


CMatrix CMatrix::operator * ( const CMatrix& m ) const
{
	if( m_n != m.m_m )
		return CMatrix();

	CMatrix	Ret( m_m, m.m_n );
	int		i, j, k;

	for( i = 0; i < m_m; i++ )
	{
		for( j = 0; j < m.m_n; j++ )
		{
			float	x = 0.0f;

			for( k = 0; k < m_n; k++ )
				x += m_pData[i * m_n + k] * m.m_pData[k * m.m_n + j];

			Ret.m_pData[i * m.m_n + j] = x;
		}
	}

	return Ret;
}


CMatrix	CMatrix::MultTranspose( const CMatrix& m ) const
{
	if( m_n != m.m_n )
		return CMatrix();

	CMatrix	Ret( m_m, m.m_m );
	int		i, j, k;

	for( i = 0; i < m_m; i++ )
	{
		for( j = 0; j < m.m_m; j++ )
		{
			float	x = 0.0f;

			for( k = 0; k < m_n; k++ )
				x += m_pData[i * m_n + k] * m.m_pData[j * m.m_n + k];

			Ret.m_pData[i * m.m_m + j] = x;
		}
	}

	return Ret;
}


CMatrix	CMatrix::TransposeMult( const CMatrix& m ) const
{
	if( m_m != m.m_m )
		return CMatrix();

	CMatrix	Ret( m_n, m.m_n );
	int		i, j, k;

	for( i = 0; i < m_n; i++ )
	{
		for( j = 0; j < m.m_n; j++ )
		{
			float	x = 0.0f;

			for( k = 0; k < m_m; k++ )
				x += m_pData[k * m_n + i] * m.m_pData[k * m.m_n + j];

			Ret.m_pData[i * m.m_n + j] = x;
		}
	}

	return Ret;
}


CMatrix	CMatrix::operator + ( const CMatrix& m ) const
{
	if( m_m != m.m_m || m_n != m.m_n )
		return CMatrix();

	CMatrix	Ret( m_m, m_n );
	int		i, j;

	for( i = 0; i < m_m; i++ )
		for( j = 0; j < m_n; j++ )
			Ret.m_pData[i * m_n + j] = m_pData[i * m_n + j] + m.m_pData[i * m_n + j];

	return Ret;
}


CMatrix	CMatrix::operator - ( const CMatrix& m ) const
{
	if( m_m != m.m_m || m_n != m.m_n )
		return CMatrix();

	CMatrix	Ret( m_m, m_n );
	int		i, j;

	for( i = 0; i < m_m; i++ )
		for( j = 0; j < m_n; j++ )
			Ret.m_pData[i * m_n + j] = m_pData[i * m_n + j] - m.m_pData[i * m_n + j];

	return Ret;
}


ostream&	operator << ( ostream& os, const CMatrix& m )
{
	int		i, j;

	for( i = 0; i < m.m_m; i++ )
	{
		os << "| ";

		for( j = 0; j < m.m_n; j++ )
		{
			//os.precision( 5 );
			os << m.m_pData[i * m.m_n + j] << " ";
		}

		os << "|\n";
	}

	return os;
}
