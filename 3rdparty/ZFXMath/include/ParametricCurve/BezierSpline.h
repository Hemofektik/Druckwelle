/// \file
///
/// \if DE
/// @brief TBezierSpline
///
/// TCubicBezier: n-dimensionale Bezier-Kurve mit m Elementen
/// \else
/// @brief TBezierSpline
///
/// TCubicBezier: n-dimensional Bezier-Curve with m elements
/// \endif


#ifndef	_ZFXMATH_INCLUDE_BEZIER_SPLINE_H_
#define	_ZFXMATH_INCLUDE_BEZIER_SPLINE_H_

#include "ParametricCurve.h"

namespace ZFXMath
{

namespace ParametricCurve
{

/// \if DE
/// @brief n-dimensionale Bezier-Kurve mit m Elementen
///
/// \param PrecisionType Genauigkeit der Funktionsauswertung (z.B. float)
/// \param FuncValueType Typ der Funktion (z.B. TVector3D<float>)
/// \else
/// @brief n-dimensional Bezier-Curve with m elements
///
/// \param PrecisionType precision of function evaluation (e.g. float)
/// \param FuncValueType Type of function (e.g. TVector3D<float>)
/// \endif	
template<class PrecisionType, class FuncValueType>
class TBezierSpline : public TParametricCurve<PrecisionType, FuncValueType>
{
	friend ZFXMath::ParametricSurface::TParametricSurface<PrecisionType, FuncValueType, ZFXMath::ParametricCurve::TBezierSpline<PrecisionType,FuncValueType> >;

public:
	TBezierSpline( int numSplinePoints ) 
		: TParametricCurve<PrecisionType, FuncValueType>( numSplinePoints )
	{
	}

protected:
	virtual PrecisionType EvalBasisFunction( int index, PrecisionType u ) const
	{
		PrecisionType invu = 1.0f - u;

		int N = this->GetNumBasisFunctions();
		int k = index;

		int invK = N - k;

		return ( (PrecisionType)Fac( N ) / (PrecisionType)( Fac( k ) * Fac( invK ) ) ) * Pow( u, (PrecisionType)k ) * Pow( invu, (PrecisionType)invK );
	}
};

}; // namespace ParametricCurve

}; // namespace ZFXMath


#endif //_ZFXMATH_INCLUDE_BEZIER_SPLINE_H_
