/// \file
///
/// \if DE
/// @brief Kugelfunktionalität
/// \else
/// @brief Sphere functionality
/// \endif

#ifndef	_ZFXMATH_INCLUDE_SPHERE_H_
#define	_ZFXMATH_INCLUDE_SPHERE_H_

namespace ZFXMath
{


/// \if DE
/// @brief Sphäre (Kugel)
/// \else
/// @brief Sphere
/// \endif
template<class PrecisionType>
struct TSphere
{
	TVector3D<PrecisionType>		position;
	PrecisionType					radius;

	TSphere(){}

	TSphere( const TVector3D<PrecisionType>& position, PrecisionType radius ) :
	position(position),
	radius(radius)
	{
	}

	TSphere( const TAABB<PrecisionType>& aabb ) :
	position( aabb.GetCenter() ),
	radius( aabb.GetExtent() * (PrecisionType)0.5 )
	{
	}

	bool Intersects( const TVector3D<PrecisionType>& point ) const
	{
		return ( ( position - point ).LengthSqr() <= Sqr( radius ) );
	}

	bool Intersects( const TSphere<PrecisionType>& sphere ) const
	{
		return ( ( sphere.position - position ).LengthSqr() <= Sqr( sphere.radius + radius ) );
	}
};


/// \if DE
/// @brief Punkt auf einer Einheitssphäre
/// \else
/// @brief Point on a unit Sphere
/// \endif
template<class PrecisionType>
struct TSphereSample
{
	/// Spherical co-ordinates
	PrecisionType	theta, phi;

	/// Cartesian co-ordinates
	PrecisionType	x, y, z;

	// Default constructor
	TSphereSample() { };

	// Construct from spherical co-ordinates
	TSphereSample( const PrecisionType u, const PrecisionType v ) :
		theta(u),
		phi(v)
	{
		x = PrecisionType( sin(theta) * cos(phi) );
		y = PrecisionType( sin(theta) * sin(phi) );
		z = PrecisionType( cos(theta) );
	}

	// Construct from cartesian co-ordinates
	TSphereSample( const PrecisionType x, const PrecisionType y, const PrecisionType z ) :
		x(x),
		y(y),
		z(z)
	{
		theta	= ACos( z );
		phi		= ATan2( y, x );
	}
};


/// \if DE
/// @brief Funktionswert eines Punktes auf einer Einheitssphäre
/// \else
/// @brief Value of a Point on a unit Sphere
/// \endif
template<class PrecisionType, class FuncValueType>
struct TSphereSampleValue : public TSphereSample<PrecisionType>
{
	FuncValueType	value;

	// Default constructor
	TSphereSampleValue() : TSphereSample<PrecisionType>() { };

	// Construct from spherical co-ordinates
	TSphereSampleValue( const PrecisionType u, const PrecisionType v, FuncValueType value ) : 
		TSphereSample<PrecisionType>( u, v ),
		value(value)
	{
	}

	// Construct from cartesian co-ordinates
	TSphereSampleValue( const PrecisionType x, const PrecisionType y, const PrecisionType z, FuncValueType value )  : 
		TSphereSample<PrecisionType>( x, y, z ),
		value(value)
	{

	}
};

template<class PrecisionIntegerType>
void MultiStageNRooks(const PrecisionIntegerType size, PrecisionIntegerType* cells)
{
	if (size == 1)
		return;

	PrecisionIntegerType size1 = size >> 1;
	PrecisionIntegerType size2 = size >> 1;

	if (size & 1)
	{
		if ( RandN<float>() > 0.5f )
			size1++;
		else
			size2++;
	}

	PrecisionIntegerType* upperCells = new PrecisionIntegerType[size1];
	PrecisionIntegerType* lowerCells = new PrecisionIntegerType[size2];

	PrecisionIntegerType i = 0;
	PrecisionIntegerType j = 0;
	for ( ; i < size - 1; i += 2, j++ )
	{
		if ( Rand() & 1 )
		{
			upperCells[j] = cells[i];
			lowerCells[j] = cells[i + 1];
		}
		else
		{
			upperCells[j] = cells[i + 1];
			lowerCells[j] = cells[i];
		}
	}

	if (size1 != size2)
	{
		if (size1 > size2)
		{
			upperCells[j] = cells[i];
		}
		else
		{
			lowerCells[j] = cells[i];
		}
	}

	MultiStageNRooks<PrecisionIntegerType>(size1, upperCells);
	memcpy(cells, upperCells, size1 * sizeof(PrecisionIntegerType));
	delete [] upperCells;

	MultiStageNRooks<PrecisionIntegerType>(size2, lowerCells);
	memcpy(cells + size1, lowerCells, size2 * sizeof(PrecisionIntegerType));
	delete [] lowerCells;
}

/// \if DE
/// @brief GenerateSphereSamples
/// \else
/// @brief GenerateSphereSamples
///
/// Method of uniformly distributing samples over a sphere
/// From ACM Journal of Graphics Tools (http://www.acm.org/jgt)
/// Thanks for Robin Green for suggestion
/// \endif
template<class PrecisionType>
TSphereSample<PrecisionType>* GenerateSphereSamples( const int numSamples )
{
	// Generate nrook cells
	int* cells = new int[numSamples];
	for (int i = 0; i < numSamples; i++)
	{
		cells[i] = i;
	}
	MultiStageNRooks<int>(numSamples, cells);

	// Allocate the samples list
	TSphereSample<PrecisionType>* samples = new TSphereSample<PrecisionType>[numSamples];

	for (int i = 0; i < numSamples; i++)
	{
		// Generate uniform random sample
		PrecisionType x = ( i + RandN<PrecisionType>() ) / numSamples;
		PrecisionType y = ( cells[i] + RandN<PrecisionType>() ) / numSamples;

		// Generate spherical/cartesian co-ordinates
		samples[i] = TSphereSample<PrecisionType>( PrecisionType( 2 * acos(sqrt(1 - x)) ), PrecisionType( 2 * PI * y ) );
	}

	delete [] cells;

	return samples;
}





/// \if DE
/// @brief Spherical Function Interface
///
/// Dessen Implementation ist der Weg eine beliebige Beleuchtungsumgebung in SH-Koeffizienten zu komprimieren.
/// \else
/// @brief Spherical Function Interface
///
/// Its implementation is the way to compress an arbitrary lighting situation into SH Coefficients.
/// \endif
template<class PrecisionType, class FuncValueType>
struct TSphericalFunction
{
	/// \if DE
	/// Ermittelt den Funktionswert am gegebenen Punkt
	/// \else
	/// Evaluates the function at the given sample point
	/// \endif
	virtual FuncValueType	EvalFunction(const TSphereSample<PrecisionType>& s) const = 0;
};


/// \if DE
/// @brief Spherical Function basierend auf Sphere Samples
///
/// Mit dieser Klasse ist es möglich eine sphärische 
/// Funktion aus einer Menge von Funktionswerten zu definieren.
/// Die Funktionsevaluation basiert auf ...-Interpolation 
/// und wird mit Hilfe eines Quadtrees beschleunigt.
/// \else
/// @brief Spherical Function based on Sphere Samples
///
/// This class provides spherical function
/// properties by given function values.
/// Function evaluation is based on ... interpolation
/// and is accelerated by a Quadtree.
/// \endif
template<class PrecisionType, class FuncValueType>
struct TSampleBasedSphericalFunction : public TSphericalFunction<PrecisionType, FuncValueType>
{
	/// \if DE
	/// @brief Erzeugt eine Instanz von TSampleBasedSphericalFunction
	///
	/// \param interpolationRadius Umkreis in dem interpoliert werden soll (Wertebereich: 0.0 - 1.0 (kein Einfluss - gesamte Hemisphäre))
	/// \param numSamples Anzahl der hinzuzufügenden Funktionswerte, wenn diese bekannt ist
	/// \param sampleValues Funktionswertarray, kann NULL sein selbst wenn numSamples > 0
	/// \else
	/// @brief Creates an instance of TSampleBasedSphericalFunction
	///
	/// \param interpolationRadius Interpolation boundary (Domain: 0.0 - 1.0 (nothing - entire hemisphere))
	/// \param numSamples number of Samples to add, if known at this time
	/// \param sampleValues sample value array, may be NULL even if numSamples > 0
	/// \endif
	TSampleBasedSphericalFunction( PrecisionType interpolationRadius = 0.1, int numSamples = 10000, const TSphereSampleValue<PrecisionType, FuncValueType>* sampleValues = NULL )
	{
		SetInterpolationRadius( interpolationRadius );

		// numSamples gives a good estimate how many samples a quad should contain
		// divide sphere segment area by entire sphere area
		PrecisionType h = (PrecisionType)1.0 - Sin( ACos( interpolationRadius ) );
		int numSamplesPerQuad = Max( 10, (int)( ( ( ( (PrecisionType)2.0 * interpolationRadius * interpolationRadius + h * h ) * PI ) * (PrecisionType)numSamples ) / (PrecisionType)( 4 * PI ) ) );

		samples = new TSphereSampleValueQuad<PrecisionType, FuncValueType>( PrecisionType( 0.0 ), PrecisionType( 0.0 ), 
																			PrecisionType( PI ),  PrecisionType( PI * 2.0 ), numSamplesPerQuad );

		if( sampleValues )
		{
			for ( int n = 0; n < numSamples; n++ )
			{
				AddSample( sampleValues[n] );
			}
		}
	}

	~TSampleBasedSphericalFunction()
	{
		delete samples;
	}

	void AddSample( const TSphereSampleValue<PrecisionType, FuncValueType>& sampleValue )
	{
		samples->AddSample( sampleValue );
	}

	PrecisionType GetInterpolationRadius()
	{
		return interpolationRadius;
	}

	/// \if DE
	/// Ermittelt den Funktionswert am gegebenen Punkt
	/// \else
	/// Evaluates the function at the given sample point
	/// \endif
	FuncValueType EvalFunction(const TSphereSample<PrecisionType>& s) const
	{
		return samples->EvalFunction( s, interpolationRadius );
	}


protected:

	template<class PrecType, class FuncValType>
	struct TSphereSampleValueQuad
	{
		TSphereSampleValueQuad( PrecType uMin, PrecType vMin, PrecType uMax, PrecType vMax, int maxNumSamples ) :
			sample(NULL),
			subQuad(NULL),
			numSamples(0),
			uMin(uMin),
			vMin(vMin),
			uMax(uMax),
			vMax(vMax),
			maxNumSamples(maxNumSamples)
		{
			sample = new TSphereSampleValue<PrecType, FuncValType>[maxNumSamples];

			TSphereSample<PrecType> s00( uMin, vMin );
			TSphereSample<PrecType> s11( uMax, vMax );

			radius = ( (PrecType)1.0 - ( s00.x * s11.x + s00.y * s11.y + s00.z * s11.z ) ) * (PrecType)2.0;

			TSphereSample<PrecType> s( ( uMin + uMax ) * (PrecType)0.5, ( vMin + vMax ) * (PrecType)0.5 );
			center = TVector3D<PrecType>( s.x, s.y, s.z );
		}

		~TSphereSampleValueQuad()
		{
			delete[] sample;
			for ( int n = 0; n < 4; n++ )
			{
				delete subQuad[n];
			}
			delete[] subQuad;
		}

		void AddSample( const TSphereSampleValue<PrecType, FuncValType>& sampleValue )
		{
			if( sample )
			{
				sample[numSamples] = sampleValue;
				numSamples++;

				if( numSamples == maxNumSamples )
				{	// divide this quad into four new quads
					subQuad = new TSphereSampleValueQuad<PrecType, FuncValType>*[4];

					PrecType uMid = ( uMax + uMin ) * (PrecType)0.5;
					PrecType vMid = ( vMax + vMin ) * (PrecType)0.5;
					subQuad[0] = new TSphereSampleValueQuad<PrecType, FuncValType>( uMin, vMin, uMid, vMid, maxNumSamples );	// lt
					subQuad[1] = new TSphereSampleValueQuad<PrecType, FuncValType>( uMid, vMin, uMax, vMid, maxNumSamples );	// rt
					subQuad[2] = new TSphereSampleValueQuad<PrecType, FuncValType>( uMin, vMid, uMid, vMax, maxNumSamples );	// lb
					subQuad[3] = new TSphereSampleValueQuad<PrecType, FuncValType>( uMid, vMid, uMax, vMax, maxNumSamples );	// rb

					TSphereSampleValue<PrecType, FuncValType>*	tmpSample = sample;

					sample = NULL;
					numSamples = 0;

					for ( int n = 0; n < maxNumSamples; n++ )
					{	// add all samples to the new subQuads
						AddSample( tmpSample[n] );
					}

					delete[] tmpSample;
				}
			}
			else
			{
				if( sampleValue.theta < subQuad[0]->uMax )
				{
					if( sampleValue.phi < subQuad[0]->vMax )
					{
						subQuad[0]->AddSample( sampleValue );
					}
					else
					{
						subQuad[2]->AddSample( sampleValue );
					}
				}
				else
				{
					if( sampleValue.phi < subQuad[1]->vMax )
					{
						subQuad[1]->AddSample( sampleValue );
					}
					else
					{
						subQuad[3]->AddSample( sampleValue );
					}
				}
			}
		}

		PrecType GetWeight(	const TSphereSample<PrecType>& s0,
							const TSphereSample<PrecType>& s1,
							PrecType interpolationRadius ) const
		{
			PrecType xDelta = s0.x - s1.x;
			PrecType yDelta = s0.y - s1.y;
			PrecType zDelta = s0.z - s1.z;
			PrecType distance = Sqrt( xDelta * xDelta + yDelta * yDelta + zDelta * zDelta );
			return PrecType( 1.0 / ( distance + Sqrt( 1.0 - ( s0.x * s1.x + s0.y * s1.y + s0.z * s1.z ) ) ) );
		}

		FuncValueType EvalFunction( const TSphereSample<PrecType>& s, PrecType interpolationRadius, PrecType& weightSum ) const
		{
			FuncValueType result = 0.0;

			if( (PrecType)1.0 - center.DotProduct( TVector3D<PrecType>( s.x, s.y, s.z ) ) < interpolationRadius + radius ) 
			{
				if( sample )
				{
					for ( int n = 0; n < numSamples; n++ )
					{
						PrecType weight = GetWeight( sample[n], s, interpolationRadius );
						weightSum += weight;
						result += sample[n].value * FuncValType( weight );
					}
				}
				else
				{
					result += subQuad[0]->EvalFunction( s, interpolationRadius, weightSum );
					result += subQuad[1]->EvalFunction( s, interpolationRadius, weightSum );
					result += subQuad[2]->EvalFunction( s, interpolationRadius, weightSum );
					result += subQuad[3]->EvalFunction( s, interpolationRadius, weightSum );
				}
			}

			return result;
		}

		FuncValType EvalFunction( const TSphereSample<PrecType>& s, PrecType interpolationRadius ) const
		{
			PrecType weightSum = PrecType( 0.0 );

            FuncValType result = FuncValType ( 0.0 );

			result += EvalFunction( s, interpolationRadius, weightSum );

			// normalize the result
			return result / FuncValType( weightSum );
		}


		TSphereSampleValue<PrecType, FuncValueType>*			sample;
		TSphereSampleValueQuad<PrecType, FuncValueType>**		subQuad;
		int														numSamples;
		int														maxNumSamples;

		PrecType												uMin;
		PrecType												vMin;
		PrecType												uMax;
		PrecType												vMax;
		PrecType												radius;
		TVector3D<PrecType>										center;
	};


	void SetInterpolationRadius( PrecisionType interpolationRadius )
	{
		if( interpolationRadius > PrecisionType( 1.0 ) ) 
		{
			interpolationRadius = PrecisionType( 1.0 );
		}

		this->interpolationRadius = interpolationRadius;
	}

	TSphereSampleValueQuad<PrecisionType, FuncValueType>*		samples;

	PrecisionType												interpolationRadius;
};



/// \if DE
/// @brief Spherical Function basierend auf einem regulären Gitternetz
/// \else
/// @brief Spherical Function based on a regular grid
/// \endif
template<class PrecisionType, class FuncValueType>
struct TGridBasedSphericalFunction : public TSphericalFunction<PrecisionType, FuncValueType>
{
	TGridBasedSphericalFunction( int width, int height ) :
	width(width),
	height(height)
	{
		gridSample = new FuncValueType[ width * height];

		phiToWidth = (PrecisionType)width / (PrecisionType)(PI * 2.0);
		thetaToHeight = (PrecisionType)height / (PrecisionType)PI;
	}

	~TGridBasedSphericalFunction()
	{
		delete[] gridSample;
	}

	void SetValue( int x, int y, FuncValueType value )
	{
		gridSample[ y * width + x ] = value;
	}

	/// \if DE
	/// Ermittelt den Funktionswert am gegebenen Punkt
	/// \else
	/// Evaluates the function at the given sample point
	/// \endif
	FuncValueType EvalFunction(const TSphereSample<PrecisionType>& s) const
	{
		PrecisionType x = s.phi * phiToWidth;
		PrecisionType y = s.theta * thetaToHeight;

		int x0 = Min( (int)RoundDown(x), width - 1 );
		int y0 = Min( (int)RoundDown(y), height - 1 );

		int x1 = Min( x0 + 1, width - 1 );
		int y1 = Min( y0 + 1, height - 1 );

		PrecisionType xfrc = x - (PrecisionType)x0;
		PrecisionType yfrc = y - (PrecisionType)y0;

		return BilinearInterpolate( xfrc, yfrc, gridSample[ y0 * width + x0 ], gridSample[ y0 * width + x1 ], gridSample[ y1 * width + x0 ], gridSample[ y1 * width + x1 ] );
	}

protected:

	FuncValueType*	gridSample;

	int				width;
	int				height;

	PrecisionType	phiToWidth;
	PrecisionType	thetaToHeight;
};


}

#endif //_ZFXMATH_INCLUDE_SPHERE_H_
