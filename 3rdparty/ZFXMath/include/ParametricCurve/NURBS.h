/// \file
///
/// \if DE
/// @brief TNURBS
///
/// TCubicBezier: n-dimensionale NURBS mit m Elementen
/// \else
/// @brief TNURBS
///
/// TCubicBezier: n-dimensional NURBS with m elements
/// \endif


#ifndef	_ZFXMATH_INCLUDE_NURBS_H_
#define	_ZFXMATH_INCLUDE_NURBS_H_

#include "BSpline.h"


namespace ZFXMath
{
namespace ParametricSurface
{

template<class PrecisionType, class FuncValueType>
class TNURBSSurface;

}

}

namespace ZFXMath
{

namespace ParametricCurve
{


/// \if DE
/// @brief n-dimensionale NURBS mit m Elementen
/// 
///	NonUniform Rational BSpline
/// \param PrecisionType Genauigkeit der Funktionsauswertung (z.B. float)
/// \param FuncValueType Typ der Funktion (z.B. TVector3D<float>)
/// \else
/// @brief n-dimensional NURBS with m elements
///
///	NonUniform Rational BSpline
/// \param PrecisionType precision of function evaluation (e.g. float)
/// \param FuncValueType Type of function (e.g. TVector3D<float>)
/// \endif	
template<class PrecisionType, class FuncValueType>
class TNURBS : public TBSpline<PrecisionType, FuncValueType>
{
	
	friend ZFXMath::ParametricSurface::TNURBSSurface<PrecisionType, FuncValueType>;

public:

	TNURBS( int numSplinePoints, int degree ) 
		: TBSpline<PrecisionType, FuncValueType>( numSplinePoints, degree )
		, m_weight( new PrecisionType[numSplinePoints] )
	{
		for ( int n = 0; n < numSplinePoints; n++ )
		{
			m_weight[n] = (PrecisionType)1.0;
		}
	}

	virtual ~TNURBS()
	{
		delete[] m_weight;
	}

	void SetWeight( int index, PrecisionType w )
	{
		assert( index >= 0 && index < this->m_numSplinePoints );
		m_weight[index] = w;
	}

	FuncValueType EvalFunction( PrecisionType u ) const
	{
		FuncValueType nominator = (FuncValueType)0.0;
		PrecisionType denominator = (PrecisionType)0.0;

		for ( int n = 0; n < this->GetNumBasisFunctions(); n++ )
		{
			PrecisionType basisFunctionResult = EvalBasisFunction( n, u );
			nominator += this->m_spline[n] * basisFunctionResult;
			denominator += basisFunctionResult;
		}

		return nominator / denominator;
	}

protected:

	virtual PrecisionType EvalBasisFunction( int index, PrecisionType u ) const
	{
		return TBSpline<PrecisionType, FuncValueType>::EvalBasisFunction( index, u ) * m_weight[index];
	}

private:

	PrecisionType*		m_weight;
};

}; // namespace ParametricCurve

}; // namespace ZFXMath


#endif //_ZFXMATH_INCLUDE_NURBS_H_
