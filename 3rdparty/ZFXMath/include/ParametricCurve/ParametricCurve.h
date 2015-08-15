/// \file
///
/// \if DE
///
/// @brief fügt alle Header für die Parametrischen Kurven zusammen
///
/// \else
///
/// @brief include all headers for parametric curves
///
/// \endif

#ifndef	_ZFXMATH_INCLUDE_PARAMETRIC_CURVE_H_
#define	_ZFXMATH_INCLUDE_PARAMETRIC_CURVE_H_

#include <assert.h>


namespace ZFXMath
{
namespace ParametricSurface
{

template<class PrecisionType, class FuncValueType, class ParametricCurveType>
class TParametricSurface;

}

}

namespace ZFXMath
{

namespace ParametricCurve
{

/// \if DE
/// @brief n-dimensionale Paramtrische Kurve
///
/// \param PrecisionType Genauigkeit der Funktionsauswertung (z.B. float)
/// \param FuncValueType Typ der Funktion (z.B. TVector3D<float>)
/// \else
/// @brief n-dimensional Parametric Curve
///
/// \param PrecisionType precision of function evaluation (e.g. float)
/// \param FuncValueType Type of function (e.g. TVector3D<float>)
/// \endif
template<class PrecisionType, class FuncValueType>
class TParametricCurve
{
public:

	TParametricCurve( int numSplinePoints ) 
		: m_numSplinePoints(numSplinePoints)
		, m_spline( new FuncValueType[numSplinePoints] )
	{
	}

	virtual ~TParametricCurve()
	{
		delete[] m_spline;
	}

	void SetSplinePoint( int index, FuncValueType sp )
	{
		assert( index >= 0 && index < m_numSplinePoints );
		m_spline[index] = sp;
	}

	FuncValueType EvalFunction( PrecisionType u ) const
	{
		FuncValueType result = (FuncValueType)0;

		for ( int n = 0; n < GetNumBasisFunctions(); n++ )
		{
			result += m_spline[n] * EvalBasisFunction( n, u ); 	
		}

		return result;
	}

	int GetNumBasisFunctions() const
	{
		return m_numSplinePoints;
	}

	FuncValueType& operator[](int index)
	{
		assert( index >= 0 && index < m_numSplinePoints );
		return m_spline[index];
	}

	const FuncValueType& operator[](int index) const
	{
		assert( index >= 0 && index < m_numSplinePoints );
		return m_spline[index];
	}

protected:
	virtual PrecisionType EvalBasisFunction( int index, PrecisionType u ) const = 0;

	int					m_numSplinePoints;
	FuncValueType*		m_spline;
};

}; // namespace ParametricCurve

}; // namespace ZFXMath


#include "BezierSpline.h"
#include "BSpline.h"
#include "NURBS.h"


#endif //_ZFXMATH_INCLUDE_PARAMETRIC_CURVE_H_
