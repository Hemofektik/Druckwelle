/// \file
///
/// \if DE
/// @brief TNURBS
///
/// TCubicBezier: n-dimensionale NURBS Fläche mit m Elementen
/// \else
/// @brief TNURBS
///
/// TCubicBezier: n-dimensional NURBS Surface with m elements
/// \endif


#ifndef	_ZFXMATH_INCLUDE_NURBS_SURFACE_H_
#define	_ZFXMATH_INCLUDE_NURBS_SURFACE_H_

#include "ParametricSurface.h"

namespace ZFXMath
{

namespace ParametricSurface
{


/// \if DE
/// @brief n-dimensionale NURBS Fläche mit m Elementen
/// 
///	NonUniform Rational BSpline Surface
/// \param PrecisionType Genauigkeit der Funktionsauswertung (z.B. float)
/// \param FuncValueType Typ der Funktion (z.B. TVector3D<float>)
/// \else
/// @brief n-dimensional NURBS Surface with m elements
///
///	NonUniform Rational BSpline Surface
/// \param PrecisionType precision of function evaluation (e.g. float)
/// \param FuncValueType Type of function (e.g. TVector3D<float>)
/// \endif	
template<class PrecisionType, class FuncValueType>
class TNURBSSurface : public TParametricSurface<PrecisionType, FuncValueType, ZFXMath::ParametricCurve::TNURBS<PrecisionType, FuncValueType> >
{
public:

	TNURBSSurface( int numSplinePointsU, int numSplinePointsV, int degreeU, int degreeV )
	{
		this->m_splineU = new ZFXMath::ParametricCurve::TNURBS<PrecisionType,FuncValueType>*[numSplinePointsV];

		for ( int v = 0; v < numSplinePointsV; v++ )
		{
			this->m_splineU[v] = new ZFXMath::ParametricCurve::TNURBS<PrecisionType,FuncValueType>( numSplinePointsU, degreeU );
		}

		this->m_splineV = new ZFXMath::ParametricCurve::TNURBS<PrecisionType,FuncValueType>*[numSplinePointsU];

		for ( int u = 0; u < numSplinePointsU; u++ )
		{
			this->m_splineV[u] = new ZFXMath::ParametricCurve::TNURBS<PrecisionType,FuncValueType>( numSplinePointsV, degreeV );
		}
	}

	void SetWeight( int indexU, int indexV, PrecisionType w )
	{
		assert( indexU >= 0 && indexU < this->GetNumBasisFunctionsU() );
		assert( indexV >= 0 && indexV < this->GetNumBasisFunctionsV() );

		this->m_splineU[indexV]->SetWeight( indexU, w );
		this->m_splineV[indexU]->SetWeight( indexV, w );
	}

	FuncValueType EvalFunction( PrecisionType u, PrecisionType v ) const
	{
		FuncValueType nominator = (FuncValueType)0.0;
		PrecisionType denominator = (PrecisionType)0.0;

		for ( int m = 0; m < this->GetNumSplinesV(); m++ )
		{
			for ( int n = 0; n < this->GetNumSplinesU(); n++ )
			{
				PrecisionType basisFunctionResultU = this->m_splineU[n]->EvalBasisFunction( m, u );
				PrecisionType basisFunctionResultV = this->m_splineV[m]->EvalBasisFunction( n, v );

				nominator += (*this->m_splineU[n])[m] * basisFunctionResultU * basisFunctionResultV;
				denominator += basisFunctionResultU * basisFunctionResultV;
			}
		}

		return nominator / ( denominator + 0.001);
	}

	void SetKnotValueU( int knotIndex, PrecisionType knotValue )
	{
		for ( int u = 0; u < this->GetNumSplinesU(); u++ )
		{
			this->m_splineU[u]->SetKnotValue( knotIndex, knotValue );
		}
	}

	void SetKnotValueV( int knotIndex, PrecisionType knotValue )
	{
		for ( int v = 0; v < this->GetNumSplinesV(); v++ )
		{
			this->m_splineV[v]->SetKnotValue( knotIndex, knotValue );
		}
	}

	void SetKnotDomain( const TVector2D<PrecisionType>& uvMin, TVector2D<PrecisionType>& uvMax )
	{
		m_uvMin = uvMin;
		m_uvMax = uvMax;
	}

	const TVector2D<PrecisionType>& GetKnotMin() const
	{
		return m_uvMin;
	}

	const TVector2D<PrecisionType>& GetKnotMax() const
	{
		return m_uvMax;
	}

private:

	TVector2D<PrecisionType>  m_uvMin;
	TVector2D<PrecisionType>  m_uvMax;

};

}; // namespace ParametricSurface

}; // namespace ZFXMath


#endif //_ZFXMATH_INCLUDE_NURBS_SURFACE_H_
