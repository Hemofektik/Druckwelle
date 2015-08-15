/// \file
///
/// \if DE
/// @brief TBSpline
///
/// TCubicBezier: n-dimensionale BSpline-Kurve mit m Elementen
/// \else
/// @brief TBSpline
///
/// TCubicBezier: n-dimensional BSpline-Curve with m elements
/// \endif


#ifndef	_ZFXMATH_INCLUDE_BSPLINE_H_
#define	_ZFXMATH_INCLUDE_BSPLINE_H_

#include "ParametricCurve.h"


namespace ZFXMath
{

namespace ParametricCurve
{


/// \if DE
/// @brief n-dimensionale BSpline-Kurve mit m Elementen
///
/// \param PrecisionType Genauigkeit der Funktionsauswertung (z.B. float)
/// \param FuncValueType Typ der Funktion (z.B. TVector3D<float>)
/// \else
/// @brief n-dimensional BSpline-Curve with m elements
///
/// \param PrecisionType precision of function evaluation (e.g. float)
/// \param FuncValueType Type of function (e.g. TVector3D<float>)
/// \endif	
template<class PrecisionType, class FuncValueType>
class TBSpline : public TParametricCurve<PrecisionType, FuncValueType>
{

	friend ZFXMath::ParametricSurface::TParametricSurface<PrecisionType, FuncValueType, ZFXMath::ParametricCurve::TBSpline<PrecisionType,FuncValueType> >;

public:

	TBSpline( int numSplinePoints, int degree ) 
		: TParametricCurve<PrecisionType, FuncValueType>( numSplinePoints )
		, m_degree(degree)
		, m_breakPoint(NULL)
	{
		CreateSplineKnots( numSplinePoints );
	}

	virtual ~TBSpline()
	{
		delete[] m_breakPoint;
	}

	void SetKnotValue( int knotIndex, PrecisionType knotValue )
	{
		assert( knotIndex >= 0 && knotIndex < ( this->m_numSplinePoints + m_degree + 1 ) );
		m_breakPoint[knotIndex] = knotValue;
	}

protected:

	virtual PrecisionType EvalBasisFunction( int index, PrecisionType u ) const 
	{
		return EvalBasisFunction( index, m_degree, u );
	}

private:

	PrecisionType EvalBasisFunction( int index, int degree, PrecisionType v ) const
	{
		int t = degree;

		PrecisionType weight = 1;

		if ( t == 1 )
		{
			if ( ( m_breakPoint[index] <= v ) && ( v < m_breakPoint[ index + 1 ] ) )
				weight = 1;
			else
				weight = 0;
		}
		else
		{
			if ((m_breakPoint[index+t-1] == m_breakPoint[index]) && (m_breakPoint[index+t] == m_breakPoint[index+1]))
				weight = 0;
			else if (m_breakPoint[index+t-1] == m_breakPoint[index]) 
				weight = (m_breakPoint[index+t] - v) / (m_breakPoint[index+t] - m_breakPoint[index+1]) * EvalBasisFunction( index + 1, t - 1 , v );
			else if (m_breakPoint[index+t] == m_breakPoint[index+1])
				weight = (v - m_breakPoint[index]) / (m_breakPoint[index+t-1] - m_breakPoint[index]) * EvalBasisFunction( index, t - 1, v );
			else
				weight = (v - m_breakPoint[index]) / (m_breakPoint[index+t-1] - m_breakPoint[index]) * EvalBasisFunction( index, t - 1, v ) + 
						 (m_breakPoint[index + t] - v) / (m_breakPoint[index+t] - m_breakPoint[index+1]) * EvalBasisFunction( index + 1, t - 1, v );
	   }

	   return weight;
	}

	/*
	   http://astronomy.swin.edu.au/~pbourke/curves/spline/
	   The positions of the subintervals of v and breakpoints, the position
	   on the curve are called knots. Breakpoints can be uniformly defined
	   by setting u[j] = j, a more useful series of breakpoints are defined
	   by the function below. This set of breakpoints localises changes to
	   the vicinity of the control point being modified.
	*/
	void CreateSplineKnots( int numSplinePoints )
	{
		int n = numSplinePoints;
		int t = m_degree;

		int numBreakPoints = n + 1 + t;

		delete[] m_breakPoint;
		m_breakPoint = new PrecisionType[ numBreakPoints ];

		for ( int bp = 0; bp < numBreakPoints; bp++ ) 
		{
			if ( bp < t )
			{
				m_breakPoint[bp] = (PrecisionType)0.0;
			}
			else if ( bp <= n )
			{
				m_breakPoint[bp] = (PrecisionType)( bp - t + 1 );
			}
			else if ( bp > n )
			{
				m_breakPoint[bp] = (PrecisionType)( n - t + 2 );
			}
		}
	}

	int 				m_degree;
	PrecisionType* 		m_breakPoint;
};

}; // namespace ParametricCurve

}; // namespace ZFXMath


#endif //_ZFXMATH_INCLUDE_BSPLINE_H_
