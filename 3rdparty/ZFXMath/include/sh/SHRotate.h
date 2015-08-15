/// \file
///
/// \if DE
/// @brief SphericalHarmonics::SHRotate
/// 
/// SHRotate: Funktion um eine SH-Matrix zu erstellen auf Basis einer gewöhnlichen 4x4-Matrix
/// \else
/// @brief SphericalHarmonics::SHRotate
///
/// Rotation Matrices for Real Spherical Harmonics. Direct Determination by Recursion
/// Joseph Ivanic and Klaus Ruedenberg
/// J. Phys. Chem. 1996, 100, 6342-5347
/// 
/// Additions and Corrections (to previous paper)
/// Joseph Ivanic and Klaus Ruedenberg
/// J. Phys. Chem. A, 1998, Vol. 102, No. 45, 9099
///
/// SHRotate: Function to create an SH-Matrix based on an ordinary 4x4-Matrix
/// \endif

#ifndef	ZFXMATH_INCLUDE_SH_ROTATE_H
#define	ZFXMATH_INCLUDE_SH_ROTATE_H

namespace ZFXMath
{

namespace SphericalHarmonics
{

	inline int delta(const int m, const int n)
	{
		// Kronecker Delta
		return ( m == n ? 1 : 0 );
	}

	template<class PrecisionType>
	void uvw(const int l, const int m, const int n, PrecisionType& u, PrecisionType& v, PrecisionType& w)
	{
		// Pre-calculate simple reusable terms
		PrecisionType d = PrecisionType( delta(m, 0) );
		int abs_m = Abs(m);

		// Only calculate the required denominator once
		PrecisionType denom;
		if (Abs(n) == l)
			denom = PrecisionType( (2 * l) * (2 * l - 1) );

		else
			denom = PrecisionType( (l + n) * (l - n) );

		// Now just calculate the scalars
		u = PrecisionType( sqrt((l + m) * (l - m) / denom) );
		v = PrecisionType( 0.5f * sqrt((1 + d) * (l + abs_m - 1) * (l + abs_m) / denom) * (1 - 2 * d) );
		w = PrecisionType( -0.5f * sqrt((l - abs_m - 1) * (l - abs_m) / denom) * (1 - d) );
	}

	template<class PrecisionType, class FuncValueType>
	PrecisionType M(const int l, const int m, const int n, const TRotateMatrix<PrecisionType,FuncValueType>& M)
	{
		// First get the scalars
		PrecisionType u, v, w;
		uvw(l, m, n, u, v, w);

		// Scale by their functions
		if (u)
			u *= GetU(l, m, n, M);
		if (v)
			v *= GetV(l, m, n, M);
		if (w)
			w *= GetW(l, m, n, M);

		return (u + v + w);
	}

	template<class PrecisionType, class FuncValueType>
	PrecisionType P(const int i, const int l, const int a, const int b, const TRotateMatrix<PrecisionType,FuncValueType>& M)
	{
		// Rather than passing the permuted rotation matrix around grab it directly from the first
		// rotation band which is never modified
		PrecisionType ri1 = M(1, i, 1);
		PrecisionType rim1 = M(1, i, -1);
		PrecisionType ri0 = M(1, i, 0);

		if (b == -l)
			return (ri1 * M(l - 1, a, -l + 1) + rim1 * M(l - 1, a, l - 1));

		else if (b == l)
			return (ri1 * M(l - 1, a, l - 1) - rim1 * M(l - 1, a, -l + 1));

		else // |b|<l
			return (ri0 * M(l - 1, a, b));
	}


	template<class PrecisionType, class FuncValueType>
	PrecisionType GetU(const int l, const int m, const int n, const TRotateMatrix<PrecisionType,FuncValueType>& M)
	{
		// All cases fall correctly through here
		return (P(0, l, m, n, M));
	}


	template<class PrecisionType, class FuncValueType>
	PrecisionType GetV(const int l, const int m, const int n, const TRotateMatrix<PrecisionType,FuncValueType>& M)
	{
		if (m == 0)
		{
			PrecisionType p0 = P(1, l, 1, n, M);
			PrecisionType p1 = P(-1, l, -1, n, M);
			return (p0 + p1);
		}

		else if (m > 0)
		{
			PrecisionType d = PrecisionType( delta(m, 1) );
			PrecisionType p0 = P(1, l, m - 1, n, M);
			PrecisionType p1 = P(-1, l, -m + 1, n, M);
			return PrecisionType(p0 * sqrt(1 + d) - p1 * (1 - d));
		}

		else // m < 0
		{
			PrecisionType d = PrecisionType( delta(m, -1) );
			PrecisionType p0 = P(1, l, m + 1, n, M);
			PrecisionType p1 = P(-1, l, -m - 1, n, M);
			return PrecisionType(p0 * (1 - d) + p1 * sqrt(1 + d));
		}
	}


	template<class PrecisionType, class FuncValueType>
	PrecisionType GetW(const int l, const int m, const int n, const TRotateMatrix<PrecisionType,FuncValueType>& M)
	{
		if (m == 0)
		{
			// Never gets called as kd=0
			assert(false);
			return (0);
		}

		else if (m > 0)
		{
			PrecisionType p0 = P(1, l, m + 1, n, M);
			PrecisionType p1 = P(-1, l, -m - 1, n, M);
			return (p0 + p1);
		}

		else // m < 0
		{
			PrecisionType p0 = P(1, l, m - 1, n, M);
			PrecisionType p1 = P(-1, l, -m + 1, n, M);
			return (p0 - p1);
		}
	}

	/// \if DE
	/// @brief setze eine SHRotateMatrix durch eine einfache 4x4-Matrix
	/// \else
	/// @brief fill an SHRotateMatrix by an ordinary 4x4-Matrix
	/// \endif
	template<class PrecisionType, class FuncValueType>
	void SHRotate( TRotateMatrix<PrecisionType,FuncValueType>& shrm, const TMatrix3x3<PrecisionType>& rotation )
	{
		// The first band is rotated by a permutation of the original matrix
		shrm(1, -1, -1) = (PrecisionType)  rotation._22;
		shrm(1, -1,  0) = (PrecisionType) -rotation._32;
		shrm(1, -1, +1) = (PrecisionType)  rotation._12;
		shrm(1,  0, -1) = (PrecisionType) -rotation._23;
		shrm(1,  0,  0) = (PrecisionType)  rotation._33;
		shrm(1,  0, +1) = (PrecisionType) -rotation._13;
		shrm(1, +1, -1) = (PrecisionType)  rotation._21;
		shrm(1, +1,  0) = (PrecisionType) -rotation._31;
		shrm(1, +1, +1) = (PrecisionType)  rotation._11;

		// Calculate each block of the rotation matrix for each subsequent band
		for (int band = 2; band < shrm.GetNumBands(); band++)
		{
			for (int m = -band; m <= band; m++)
				for (int n = -band; n <= band; n++)
					shrm(band, m, n) = M(band, m, n, shrm);
		}
	}

} // Namespace SphericalHarmonics
} // Namespace ZFXMath

#endif	// ZFXMATH_INCLUDE_SH_ROTATE_H
