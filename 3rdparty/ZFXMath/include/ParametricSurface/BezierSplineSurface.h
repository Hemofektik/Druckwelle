/// \file
///
/// \if DE
/// @brief TBezierSplineSurface
///
/// TCubicBezier: n-dimensionale Bezier-Fläche mit m Elementen
/// \else
/// @brief TBezierSplineSurface
///
/// TCubicBezier: n-dimensional Bezier-Surface with m elements
/// \endif


#ifndef	_ZFXMATH_INCLUDE_BEZIER_SPLINE_SURFACE_H_
#define	_ZFXMATH_INCLUDE_BEZIER_SPLINE_SURFACE_H_

#include "ParametricSurface.h"

namespace ZFXMath
{

namespace ParametricSurface
{

/// \if DE
/// @brief n-dimensionale Bezier-Fläche mit m Elementen
///
/// \param PrecisionType Genauigkeit der Funktionsauswertung (z.B. float)
/// \param FuncValueType Typ der Funktion (z.B. TVector3D<float>)
/// \else
/// @brief n-dimensional Bezier-Surface with m elements
///
/// \param PrecisionType precision of function evaluation (e.g. float)
/// \param FuncValueType Type of function (e.g. TVector3D<float>)
/// \endif	
template<class PrecisionType, class FuncValueType>
class TBezierSplineSurface : public TParametricSurface< PrecisionType, FuncValueType, ZFXMath::ParametricCurve::TBezierSpline<PrecisionType,FuncValueType> >
{

public:
	TBezierSplineSurface( int numSplinePointsU, int numSplinePointsV )
	{
		this->m_splineU = new ZFXMath::ParametricCurve::TBezierSpline<PrecisionType,FuncValueType>*[numSplinePointsV];

		for ( int v = 0; v < numSplinePointsV; v++ )
		{
			this->m_splineU[v] = new ZFXMath::ParametricCurve::TBezierSpline<PrecisionType,FuncValueType>( numSplinePointsU );
		}

		this->m_splineV = new ZFXMath::ParametricCurve::TBezierSpline<PrecisionType,FuncValueType>*[numSplinePointsU];

		for ( int u = 0; u < numSplinePointsU; u++ )
		{
			this->m_splineV[u] = new ZFXMath::ParametricCurve::TBezierSpline<PrecisionType,FuncValueType>( numSplinePointsV );
		}
	}
};

}; // namespace ParametricSurface

}; // namespace ZFXMath


#endif //_ZFXMATH_INCLUDE_BEZIER_SPLINE_SURFACE_H_
