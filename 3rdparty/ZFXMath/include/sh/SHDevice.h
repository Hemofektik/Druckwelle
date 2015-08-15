/// \file
///
/// \if DE
/// @brief SphericalHarmonics::TDevice
///
/// TDevice: Werkzeug für effiziente Implementation von SH Lighting
/// \else
/// @brief SphericalHarmonics::TDevice
///
/// TDevice: Tool for efficient implementation of SH Lighting
/// \endif


#ifndef	_ZFXMATH_INCLUDE_SH_DEVICE_H_
#define	_ZFXMATH_INCLUDE_SH_DEVICE_H_

#include <assert.h>
#include "SHConstants.h"
#include "SphericalHarmonics.h"
#include "SHRotateMatrix.h"
#include "SHRotate.h"

namespace ZFXMath
{

namespace SphericalHarmonics
{

/// \if DE
/// @brief Spherical Harmonic Lighting Device
///
/// Ziemlich nützliches Werkzeug
/// \else
/// @brief Spherical Harmonic Lighting Device
///
/// Pretty handy Tool
/// \endif
template< class PrecisionType, class FuncValueType >
struct TDevice
{
	/// \if DE
	/// @brief Device Konstruktor
	///
	/// \param numBands SH Ordnung + 1 (Genauigkeit der Funktionsrekonstruktion)
	/// \param numSamples Anzahl der Samples um die Originalfunktion zu komprimieren (Genauigkeit der Funktionskompression)
	/// \else
	/// @brief Device Constructor
	///
	/// \param numBands SH Order + 1 (accuracy of function reconstruction)
	/// \param numSamples Number of samples to compress the original function (accuracy of function compression)
	/// \endif
	TDevice( int numBands, int numSamples ) :
		m_NumBands( numBands ),
		m_NumSamples( numSamples )
	{
		assert( numBands > 0 );
		assert( numBands <= BANDS_MAX );
		assert( numSamples > 0 );

		// Generate the sphere samples
		m_Samples = GenerateSphereSamples<PrecisionType>( m_NumSamples );

		// Create an allocator with enough space to store y(l,m) over each band for each sample
		m_CoeffsAllocator = new typename TCoeffs<PrecisionType>::Allocator( m_NumSamples * m_NumBands * m_NumBands );

		// Allocate a set of empty coeff lists
		m_SHValues = new TCoeffs<PrecisionType>[m_NumSamples];

		for (int i = 0; i < m_NumSamples; i++)
		{
			// De-ref current sample
			TSphereSample<PrecisionType>& s = m_Samples[i];

			// Create an SH coefficients to store the required number of bands
			TCoeffs<PrecisionType>& sh = m_SHValues[i] = TCoeffs<PrecisionType>( m_NumBands, m_CoeffsAllocator );

			// Record all y(l,m) for this sample
			CFastCoefIterator it( m_NumBands );
			do
			{
				sh( it.coefIndex ) = y<PrecisionType>( it.l, it.m, s.theta, s.phi );
			}
			while ( ++it );
		}
	}

	~TDevice()
	{
		delete m_CoeffsAllocator;
		delete[] m_SHValues;	
		delete[] m_Samples;
	}

	/// \if DE
	/// @brief Komprimiert eine sphärische Funktion in SH-Koeffizienten
	///
	/// \param func sphärische Funktion die projiziert werden soll (in)
	/// \param dest Zielprojektion (out)
	/// \else
	/// @brief Compress a Spherical Function into SH Coefficients
	///
	/// \param func Spherical Function to project (in)
	/// \param dest Destination projection (out)
	/// \endif
	void Project(const TSphericalFunction< PrecisionType, FuncValueType >& func,
				 TCoeffs< FuncValueType >& dest )
	{
		SphericalHarmonics::Project( func, m_Samples, m_SHValues, m_NumSamples, dest );
	}

	private:

		// Number of samples on the sphere
		int											m_NumSamples;

		// Number of SH bands
		int											m_NumBands;

		// Uniformly distributed samples on the sphere
		TSphereSample<PrecisionType>*				m_Samples;

		// Allocator for y(l,m) values for each sample
		typename TCoeffs<PrecisionType>::Allocator*	m_CoeffsAllocator;

		// y(l,m) stored for each sample over m_NumBands
		TCoeffs<PrecisionType>*						m_SHValues;
	
};

}; // namespace SphericalHarmonics

}; // namespace ZFXMath

#endif //_ZFXMATH_INCLUDE_SH_DEVICE_H_
