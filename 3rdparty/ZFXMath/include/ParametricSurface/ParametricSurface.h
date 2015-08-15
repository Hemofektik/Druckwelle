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

#ifndef	_ZFXMATH_INCLUDE_PARAMETRIC_SURFACE_H_
#define	_ZFXMATH_INCLUDE_PARAMETRIC_SURFACE_H_

#include <assert.h>

namespace ZFXMath
{

namespace ParametricSurface
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
template<class PrecisionType, class FuncValueType, class ParametricCurveType>
class TParametricSurface
{
public:

	virtual ~TParametricSurface()
	{
		int numSplinesU = GetNumSplinesU();
		int numSplinesV = GetNumSplinesV();

		for ( int u = 0; u < numSplinesU; u++ )
		{
			delete m_splineU[u];
		}
		delete[] m_splineU;

		for ( int v = 0; v < numSplinesV; v++ )
		{
			delete m_splineV[v];
		}
		delete[] m_splineV;
	}

	void SetSplinePoint( int indexU, int indexV, FuncValueType sp )
	{
		assert( indexU >= 0 && indexU < GetNumBasisFunctionsU() );
		assert( indexV >= 0 && indexV < GetNumBasisFunctionsV() );

		m_splineU[indexV]->SetSplinePoint( indexU, sp );
		m_splineV[indexU]->SetSplinePoint( indexV, sp );
	}

	FuncValueType EvalFunction( PrecisionType u, PrecisionType v ) const
	{
		FuncValueType result = (FuncValueType)0.0;

		for ( int m = 0; m < GetNumSplinesV(); m++ )
		{
			for ( int n = 0; n < GetNumSplinesU(); n++ )
			{
				PrecisionType basisFunctionResultU = m_splineU[n]->EvalBasisFunction( m, u );
				PrecisionType basisFunctionResultV = m_splineV[m]->EvalBasisFunction( n, v );

				result += (*m_splineU[n])[m] * basisFunctionResultU * basisFunctionResultV;
			}
		}

		return result;
	}

	int GetNumBasisFunctionsU() const
	{
		return m_splineU[0]->GetNumBasisFunctions();
	}

	int GetNumBasisFunctionsV() const
	{
		return m_splineV[0]->GetNumBasisFunctions();
	}

	int GetNumSplinesU() const
	{
		return m_splineV[0]->GetNumBasisFunctions();
	}

	int GetNumSplinesV() const
	{
		return m_splineU[0]->GetNumBasisFunctions();
	}

protected:

	TParametricSurface() :
	m_splineU(NULL),
	m_splineV(NULL)
	{
	}

	ParametricCurveType**		m_splineU;
	ParametricCurveType**		m_splineV;
};

}; // namespace ParametricSurface

}; // namespace ZFXMath


#include "BezierSplineSurface.h"
#include "BSplineSurface.h"
#include "NURBSSurface.h"


#endif //_ZFXMATH_INCLUDE_PARAMETRIC_SURFACE_H_
