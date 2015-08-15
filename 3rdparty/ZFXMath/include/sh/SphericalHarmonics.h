/// \file
///
/// \if DE
/// @brief Basis-SH Klassen
/// \else
/// @brief Basic SH Classes
/// \endif

#ifndef	_ZFXMATH_INCLUDE_SH_H_
#define	_ZFXMATH_INCLUDE_SH_H_

namespace ZFXMath
{

/// \if DE
/// @brief enthält Basis SH Klassen
/// \else
/// @brief contains Basic SH Classes
///
///	SphericalHarmonics::TSphericalFunction
///	SphericalHarmonics::TCoeffs
///	SphericalHarmonics::Project
///
/// Based on Spherical Harmonic Rotation
/// Don Williamson, July 2004
/// http://www.donw.co.uk
/// \endif
namespace SphericalHarmonics
{

/// \if DE
/// @brief beinhaltet eine SH-kompimierte sphärische Funktion
///
/// T = PrecisionType/FuncValueType
/// \else
/// @brief Contains a SH comressed spherical function
///
/// T = PrecisionType/FuncValueType
/// \endif
template<class T>
class TCoeffs
{
	public:
		/// \if DE
		/// @brief Speicherzuteiler für Koeffizienten
		/// \else
		/// @brief Allocator for coefficients
		/// \endif
		template<class TAlloc>
		class TAllocator
		{
			public:
				TAllocator( int size )
				{
					m_Size = size;
					m_pMemory = new TAlloc[size];
					assert( m_pMemory );		// out of memory
					m_pNextMem = m_pMemory;
				}

				~TAllocator()
				{
					delete[] m_pMemory;
				}

				TAlloc* GetMem( int amount )
				{
					m_Size -= amount;
					assert( m_Size >= 0 );

					TAlloc* pMem = m_pNextMem;
					m_pNextMem += amount;

					return pMem;
				}

			private:
				TAlloc*				m_pMemory;
				TAlloc*				m_pNextMem;
				int					m_Size;
		};
		typedef TAllocator<T>	Allocator;

		TCoeffs() : m_coeffs(0), m_numBands(0), m_numCoeffs(0) { }

		TCoeffs(const int numBands) :
			m_coeffs(0),
			m_numBands(numBands),
			m_numCoeffs(numBands * numBands),
			m_FromAllocator(false)
		{
			assert( m_numBands <= BANDS_MAX );

			// Allocate the coeff list
			m_coeffs = new T[m_numCoeffs];
		}

		TCoeffs(const int numBands, Allocator* pAlloc) :
			m_coeffs(0),
			m_numBands(numBands),
			m_numCoeffs(numBands * numBands),
			m_FromAllocator(true)
		{
			assert( m_numBands <= BANDS_MAX );

			// Allocate the coeff list
			m_coeffs = pAlloc->GetMem( m_numCoeffs );
		}

		~TCoeffs()
		{
			// Stack allocator cleans itself up
			if (!m_FromAllocator)
				delete [] m_coeffs;
		}

		TCoeffs(const TCoeffs& other) :
			m_coeffs(0),
			m_numBands(other.m_numBands),
			m_numCoeffs(other.m_numCoeffs),
			m_FromAllocator(other.m_FromAllocator)
		{
			// Just copy the pointer in this case
			if (m_FromAllocator)
				m_coeffs = other.m_coeffs;

			else
			{
				// Make a copy when not using the stack allocator
				m_coeffs = new T[m_numCoeffs];
				for (int i = 0; i < m_numCoeffs; i++)
					m_coeffs[i] = other.m_coeffs[i];
			}
		}

		// Const/non-const accessors for band/arg
		T operator () (const int l, const int m) const
		{
			return (m_coeffs[Check(l * (l + 1) + m)]);
		}
		T& operator () (const int l, const int m)
		{
			return (m_coeffs[Check(l * (l + 1) + m)]);
		}

		// Const/non-const accessors by index
		T operator () (const int i) const
		{
			return (m_coeffs[Check(i)]);
		}
		T& operator () (const int i)
		{
			return (m_coeffs[Check(i)]);
		}

		int	GetNumBands() const
		{
			return m_numBands;
		}

		int GetCount() const
		{
			return m_numCoeffs;
		}

	private:
		inline int Check(const int index) const
		{
			// Check bounds in debug build
			assert(index >= 0 && index < m_numCoeffs);

			return (index);
		}

		// List of SH coefficients
		T*		m_coeffs;

		// Number of bands used
		int		m_numBands;

		// Number of coefficients (=bands^2)
		int		m_numCoeffs;

		// Allocated from a special-purpose allocator?
		bool	m_FromAllocator;

};


/// \if DE
/// @brief KoeffizientenIteratorInterface
/// \else
/// @brief Iterator interface for Coefficients
/// \endif
class ICoefIterator 
{
public:
	virtual bool operator++() = 0;

	virtual ~ICoefIterator() {}

	int numBands;
	int numCoefs;

	int coefIndex;
	int l;
	int m;

protected:
	ICoefIterator( int _numBands, int _coefIndex, int _l, int _m ) :
	numBands( _numBands ),
	numCoefs( numBands * numBands ),
	coefIndex( _coefIndex ),
	l( _l ),
	m( _m )
	{
	}
};

/// \if DE
/// @brief Schneller KoeffizientenIterator
/// \else
/// @brief Fast Coefficient Iterator 
/// \endif
class CFastCoefIterator : public ICoefIterator  
{
public:
	CFastCoefIterator( int numBands ) : 
	ICoefIterator( numBands, 0, 0, 0 )
	{
	}

	virtual bool operator++()	
	{
		m++;
		if( m > l )
		{
			l++;
			m = -l;
		}

		coefIndex++;

		if( coefIndex < numCoefs ) return true;
		return false;
	}
};

/// \if DE
/// @brief Preziser KoeffizientenIterator
///
/// Nützlich für SH-Rekonstruktion höherer Ordnung
/// \else
/// @brief Precise Coefficient Iterator
///
/// Useful for SHReconstruction over higher bands
/// \endif
class CPreciseCoefIterator : public ICoefIterator  
{
public:
	CPreciseCoefIterator( int numBands ) : 
	ICoefIterator( numBands, numBands * numBands - 1, numBands - 1, numBands - 1 )
	{
	}

	virtual bool operator++()	
	{
		m = -m;

		if( m + 1 > l )
		{
			if( l == 0 ) return false;
			m--;
			l = numBands - 1;
		}
		else if ( m >= 0 )
		{
			l--;
		}

		coefIndex = l * (l + 1) + m;

		return true;
	}
};


/// \if DE
///
/// \else
// http://www.research.scea.com/gdc2003/spherical-harmonic-lighting.html
// Project a spherical function onto the spherical harmonic bases
//
// a. Spherical Function to project
// b. List of sample locations on the sphere
// c. List of y(l,m,theta,phi) for each sample
// d. Number of samples on the sphere
// e. Destination projection
/// \endif
template<class PrecisionType, class FuncValueType>
void Project(const TSphericalFunction<PrecisionType,FuncValueType>& func,
				const TSphereSample<PrecisionType>* samples,
				const TCoeffs<PrecisionType>* coeffs,
				const int numSamples,
				TCoeffs<FuncValueType>& dest )
{
	int numCoeffs = dest.GetCount();

	// Check input
	assert( samples && coeffs );
	assert( numSamples >= 0 );
	assert( dest.GetNumBands() <= BANDS_MAX );
	assert( coeffs[0].GetCount() == numCoeffs );

	// Clear out sums
	for (int i = 0; i < numCoeffs; i++)
		dest(i) = 0;

	for (int i = 0; i < numSamples; i++)
	{
		// Take the sample at this point on the sphere
		const TSphereSample<PrecisionType>&	s = samples[i];
		const TCoeffs<PrecisionType>&		c = coeffs[i];
		FuncValueType				   sample = func.EvalFunction(s);

		// Sum the projection of this sample onto each SH basis
		CFastCoefIterator it( dest.GetNumBands() );
		do
		{
			dest( it.coefIndex ) += sample * FuncValueType( c( it.coefIndex ) );
		}
		while ( ++it );
	}

	// Divide each coefficient by the number of samples and multiply by weights
	FuncValueType weight = FuncValueType( PrecisionType( (4 * PI) / numSamples ) );
	for (int i = 0; i < numCoeffs; i++)
		dest(i) = dest(i) * weight;
}

//---------------------------------------------------------------------------------------
// Evaluate Associated Legendre Polynomial
template<class PrecisionType>
PrecisionType P(const int l, const int m, const PrecisionType x)
{
	// Start with P(0,0) at 1
	PrecisionType pmm = 1;

	// First calculate P(m,m) since that is the only rule that requires results
	// from previous bands
	// No need to check for m>0 since SH function always gives positive m

	// Precalculate (1 - x^2)^0.5
	PrecisionType somx2 = (PrecisionType)sqrt(1 - x * x);

	// This calculates P(m,m). There are three terms in rule 2 that are being iteratively multiplied:
	//
	// 0: -1^m
	// 1: (2m-1)!!
	// 2: (1-x^2)^(m/2)
	//
	// Term 2 has been partly precalculated and the iterative multiplication by itself m times
	// completes the term.
	// The result of 2m-1 is always odd so the double factorial calculation multiplies every odd
	// number below 2m-1 together. So, term 3 is calculated using the 'fact' variable.
	PrecisionType fact = 1;
	for (int i = 1; i <= m; i++)
	{
		pmm *= -1 * fact * somx2;
		fact += 2;
	}

	// No need to go any further, rule 2 is satisfied
	if (l == m)
		return (pmm);

	// Since m<l in all remaining cases, all that is left is to raise the band until the required
	// l is found

	// Rule 3, use result of P(m,m) to calculate P(m,m+1)
	PrecisionType pmmp1 = x * (2 * m + 1) * pmm;

	// Is rule 3 satisfied?
	if (l == m + 1)
		return (pmmp1);

	// Finally, use rule 1 to calculate any remaining cases
	PrecisionType pll = 0;
	for (int ll = m + 2; ll <= l; ll++)
	{
		// Use result of two previous bands
		pll = PrecisionType( (x * (2.0f * ll - 1.0f) * pmmp1 - (ll + m - 1.0f) * pmm) / (ll - m) );

		// Shift the previous two bands up
		pmm = pmmp1;
		pmmp1 = pll;
	}

	return (pll);
}

// Re-normalisation constant for SH function
template<class PrecisionType>
PrecisionType K(const int l, const int m)
{
	return (PrecisionType)k[ ( l * ( l + 1 ) ) / 2 + m ];
	//return PrecisionType(sqrt(((2 * l + 1) * factorial(l - m)) / (4 * PI * factorial(l + m))));
}

// Evaluate Real Spherical Harmonic
template<class PrecisionType>
PrecisionType y(const int l, const int m, const PrecisionType theta, const PrecisionType phi)
{
	if (0 == m)
		return (PrecisionType(K<PrecisionType>(l, 0) * P(l, 0, (PrecisionType)cos(theta))));

	if (m > 0)
		return PrecisionType(sqrt(2.0) * K<PrecisionType>(l, m) * cos(m * phi) * P(l, m, (PrecisionType)cos(theta)));

	// m < 0, m is negated in call to K
	return PrecisionType(sqrt(2.0) * K<PrecisionType>(l, -m) * sin(-m * phi) * P(l, -m, (PrecisionType)cos(theta)));
}

}; // namespace SphericalHarmonics

}; // namespace ZFXMath

#endif //_ZFXMATH_INCLUDE_SH_H_
