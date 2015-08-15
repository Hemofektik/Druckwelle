/// \file
///
/// \if DE
/// @brief TBSplineSurface
///
/// TCubicBezier: n-dimensionale BSpline-Fläche mit m Elementen
/// \else
/// @brief TBSplineSurface
///
/// TCubicBezier: n-dimensional BSpline-Surface with m elements
/// \endif


#ifndef	_ZFXMATH_INCLUDE_BSPLINE_SURFACE_H_
#define	_ZFXMATH_INCLUDE_BSPLINE_H_

#include "ParametricSurface.h"

namespace ZFXMath
{

namespace ParametricSurface
{


/// \if DE
/// @brief n-dimensionale BSpline-Fläche mit m Elementen
///
/// \param PrecisionType Genauigkeit der Funktionsauswertung (z.B. float)
/// \param FuncValueType Typ der Funktion (z.B. TVector3D<float>)
/// \else
/// @brief n-dimensional BSpline-Surface with m elements
///
/// \param PrecisionType precision of function evaluation (e.g. float)
/// \param FuncValueType Type of function (e.g. TVector3D<float>)
/// \endif	
template<class PrecisionType, class FuncValueType>
class TBSplineSurface : public TParametricSurface< PrecisionType, FuncValueType, ZFXMath::ParametricCurve::TBSpline<PrecisionType,FuncValueType> >
{
public:

	TBSplineSurface( int numSplinePointsU, int numSplinePointsV, int degreeU, int degreeV )
	{
		this->m_splineU = new ZFXMath::ParametricCurve::TBSpline<PrecisionType,FuncValueType>*[numSplinePointsV];

		for ( int v = 0; v < numSplinePointsV; v++ )
		{
			this->m_splineU[v] = new ZFXMath::ParametricCurve::TBSpline<PrecisionType,FuncValueType>( numSplinePointsU, degreeU );
		}

		this->m_splineV = new ZFXMath::ParametricCurve::TBSpline<PrecisionType,FuncValueType>*[numSplinePointsU];

		for ( int u = 0; u < numSplinePointsU; u++ )
		{
			this->m_splineV[u] = new ZFXMath::ParametricCurve::TBSpline<PrecisionType,FuncValueType>( numSplinePointsV, degreeV );
		}
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
};

}; // namespace ParametricSurface

}; // namespace ZFXMath


#endif //_ZFXMATH_INCLUDE_BSPLINE_H_
