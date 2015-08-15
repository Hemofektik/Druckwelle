/// \file BasicMath.h
/// $Id$
///
/// \if DE
/// @brief Basis-Mathefunktionen und Konstanten.
/// \else
/// @brief Basic math functions and constants.
/// \endif

#ifndef	_ZFXMATH_INCLUDE_BASICMATH_H_
#define	_ZFXMATH_INCLUDE_BASICMATH_H_

#include <assert.h>

/// \if DE
/// @brief Hauptnamespace
/// \else
/// @brief main namespace
/// \endif
namespace ZFXMath {
	#undef DELTA
	#undef E
	#undef LOG2_E
	#undef LOG10_E
	#undef LOGE_2
	#undef LOGE_10
	#undef PI
	#undef SQRT_2

	/// \if DE
	/// @brief Genauigkeit
	/// \else
	/// @brief accuracy
	/// \endif
	const double EPSILON = 0.00001;

	/// \if DE
	/// @brief Eulersche Zahl E
	/// \else
	/// @brief mathematical constant e
	/// \endif
	const double E = 2.71828182845904523536;

	/// \if DE
	/// @brief Logarithmus von E zur Basis 2
	/// \else
	/// @brief logarithm of e to the base 2
	/// \endif
	const double LOG2_E = 1.44269504088896340736;

	/// \if DE
	/// @brief Logarithmus von E zur Basis 10
	/// \else
	/// @brief logarithm of e to the base 10
	/// \endif
	const double LOG10_E = 0.434294481903251827651;

	/// \if DE
	/// @brief Logarithmus von 2 zur Basis E
	/// \else
	/// @brief logarithm of 2 to the base e
	/// \endif
	const double LOGE_2 = 0.693147180559945309417;

	/// \if DE
	/// @brief Logarithmus von 10 zur Basis E
	/// \else
	/// @brief logarithm of 10 to the base e
	/// \endif
	const double LOGE_10 = 2.30258509299404568402;

	/// \if DE
	/// @brief Kreiszahl pi
	/// \else
	/// @brief mathematical constant pi
	/// \endif
	const double PI = 3.14159265358979323846;

	/// \if DE
	/// @brief Wurzel aus 2
	/// \else
	/// @brief quareroot of 2
	/// \endif
	const double SQRT_2 = 1.41421356237309504880;

	/// \if DE
	/// @brief Konvertierung von Radian in Grad
	/// \else
	/// @brief conversion from radian to degree
	/// \endif
	template<class T> inline T RadToDeg(const T& rad)
	{
		// rad * 180° / pi
		return rad * 57.295779513082320876798154814105;
	}

	/// \if DE
	/// @brief Konvertierung von Grad in Radian
	/// \else
	/// @brief conversion from degree to radian
	/// \endif
	template<class T> inline T DegToRad(const T& degree)
	{
		// degree * pi / 180°
		return degree * 0.017453292519943295769236907684886;
	}

	/// \if DE
	/// @brief Sinus
	/// \else
	/// @brief sine
	/// \endif
	template<class T> inline T Sin(const T& rad)
	{
		return ::sin(rad);
	}

	/// \if DE
	/// @brief Kosinus
	/// \else
	/// @brief cosine
	/// \endif
	template<class T> inline T Cos(const T& rad)
	{
		return ::cos(rad);
	}

	/// \if DE
	/// @brief Sinus und Kosinus
	/// \else
	/// @brief sine and cosine
	/// \endif
	template<class T> inline void SinCos( const T& rad, T& retSin, T& retCos )
	{
		retSin = Sin( rad );
		retCos = Cos( rad );
	}

	/// \if DE
	/// @brief Tangens
	/// \else
	/// @brief tangent
	/// \endif
	template<class T> inline T Tan(const T& rad)
	{
		return ::tan(rad);
	}

	/// \if DE
	/// @brief Kotangens
	/// \else
	/// @brief cotangent
	/// \endif
	template<class T> inline T Cot(const T& rad)
	{
		return (T)1.0 / (T)::tan(rad);
	}

	/// \if DE
	/// @brief Arcus Sinus
	/// \else
	/// @brief arcsine
	/// \endif
	template<class T> inline T ASin(const T& value)
	{
		return ::asin(value);
	}

	/// \if DE
	/// @brief Arcus Kosinus
	/// \else
	/// @brief arcsine
	/// \endif
	template<class T> inline T ACos(const T& value)
	{
		return ::acos(value);
	}

	/// \if DE
	/// @brief Arcus Tangens
	/// \else
	/// @brief arctangent
	/// \endif
	template<class T> inline T ATan(const T& value)
	{
		return ::atan( value );
	}

	/// \if DE
	/// @brief Arcus Tangens
	/// \else
	/// @brief arctangent
	/// \endif
	template<class T> inline T ATan2(const T& y, const T& x)
	{
		return ::atan2( y, x );
	}

	/// \if DE
	/// @brief Quadrat
	/// \else
	/// @brief square
	/// \endif
	template<class T> inline T Sqr(const T& value)
	{
		return value * value;
	}

	/// \if DE
	/// @brief Quadratwurzel
	/// \else
	/// @brief squareroot
	/// \endif
	template<class T> inline T Sqrt(const T& value)
	{
		return ::sqrt(value);
	}

	/// \if DE
	/// @brief Quadrat
	/// \else
	/// @brief square
	/// \endif
	template<class T> inline T Pow2(const T& base)
	{
		return base*base;
	}

	/// \if DE
	/// @brief Potenz
	/// \else
	/// @brief exponentiation
	/// \endif
	template<class T> inline T Pow(const T& base, const T& exp)
	{
		return ::pow(base,exp);
	}

	/// \if DE
	/// @brief Berechnet value1 % value2
	/// \else
	/// @brief calculates value1 % value2
	/// \endif
	template<class T> inline T Mod(const T& value1, const T& value2)
	{
		return ::fmod(value1,value2);
		//return ( value1 - RoundDown(value1 / value2) * value2 );
	}

	/// \if DE
	/// @brief Berechnet |value|
	/// \else
	/// @brief calculates |value|
	/// \endif
	template<class T> inline T Abs(const T& value)
	{
		return ((value < 0) ? -value : value); 
	}

	/// \if DE
	/// @brief Logarithmus von 'num' zur Basis E
	/// \else
	/// @brief logarithm of 'num' to the base e
	/// \endif
	template<class T> inline T LogE( const T& num )
	{
		return ::log(num);
	}

	/// \if DE
	/// @brief Logarithmus von 'num' zur Basis 10
	/// \else
	/// @brief logarithm of 'num' to the base 10
	/// \endif
	template<class T> inline T Log10( const T& num )
	{
		return ::log10(num);
	}

	/// \if DE
	/// @brief Logarithmus von 'num' zur Basis 'base'
	/// \else
	/// @brief logarithm of 'num' to the base 'base'
	/// \endif
	template<class T> inline T Log( const T& base, const T& num)
	{
		return ( ::log(num) / ::log(base) );
	}

	/// \if DE
	/// @brief Runden
	/// \else
	/// @brief round to nearest integer
	/// \endif
	template<class T> inline int Round( const T& value )
	{
		return (int)(value < 0 ? value - 0.5 : value + 0.5);
	}

	/// \if DE
	/// @brief Runden auf bestimmte Stellen
	/// Wenn 'digit' negativ, wird auf 'digit' Stellen nach dem Komma gerundet.
	/// Wenn 'digit' positiv, wird auf 'digit' Stellen vor dem Komma gerundet.
	/// 'digit' darf maximal den Wert 20 und minimal den Wert -20 haben.
	/// \else
	/// @brief round to an number of digits
	/// <muss noch übersetzt werden , please help :)>
	/// \endif
	template<class T> inline T Round( const T& value, int digits )
	{
		assert(digits >= -20 && digits <= 20);
		T v[] = {	-1e20, -1e19, -1e18, -1e17, -1e16, -1e15, -1e14, -1e13, -1e12,
					-1e11, -1e10, -1e9, -1e8, -1e7, -1e6, -1e5, -1e4, -1e3, -1e2,
					-10, 1, 10, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11,
					1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20 };
		return RoundDown(value / v[digits+20] + 0.5) * v[digits+20];
	}

	/// \if DE
	/// @brief Aufrunden
	/// \else
	/// @brief round up
	/// \endif
	template<class T> inline T RoundUp(const T& value)
	{
		//if(value - RoundDown(value) == 0)
		//	return (int)value;
		//else return (int)(value+1);
		return (T)::ceil(value);
	}

	/// \if DE
	/// @brief Abrunden
	/// \else
	/// @brief round down
	/// \endif
	template<class T> inline T RoundDown(const T& value)
	{
		return (T)::floor(value);
	}

	/// \if DE
	/// @brief Testet ob die Zahl gerade oder Ungerade ist
	/// \else
	/// @brief test if number is even or not
	/// \endif
    template <class T> inline const bool IsEven(const T& value)
    {
        return (!(value&1));
    }

	/// \if DE
	/// @brief kleinere Zahl von <t1> und <t2>
	/// \else
	/// @brief less number of <t1> and <t2>
	/// \endif
	template<class T> inline const T& Min(const T& value1, const T& value2)
	{
		return ( (value1 < value2) ? value1 : value2 );
	}

	/// \if DE
	/// @brief größere Zahl von <t1> und <t2>
	/// \else
	/// @brief bigger number of <t1> and <t2>
	/// \endif
	template<class T> inline const T& Max(const T& value1, const T& value2)
	{
		return ( (value1 > value2) ? value1 : value2 );
	}

	/// \if DE
	/// @brief <value> auf <min> und <max> begrenzen
	/// \else
	/// @brief clamp <value> to <min> and <max>
	/// \endif
	template<class T> inline const T& Clamp(const T& value, const T& min, const T& max)
	{
		return ( (value < min) ? min : ( (value > max) ? max : value) );
	}

	/// \if DE
	/// @brief <value> auf 0 und 1 begrenzen
	/// \else
	/// @brief clamp <value> to 0 and 1
	/// \endif
	template<class T> inline const T& Saturate( const T& value )
	{
		return Clamp( value, (T)0.0, (T)1.0 );
	}

	/// \if DE
	/// @brief Wert ist nahe einem Anderen
	/// \else
	/// @brief value is near to another
	/// \endif
	template<class T> inline bool NearTo( const T& value, const T& nearto )
	{
		return Abs( nearto - value ) <= (T)EPSILON;
	}

	/// \if DE
	/// @brief Wenn value<0, dann -1, wenn value>0, dann 1, ansonsten 0
	/// \else
	/// @brief if value<0, than -1, if value>0, than 1, else 0
	/// \endif
	template<class T> inline T Sign(const T& value)
	{
		return (T)((value < 0) ? (-1) : (value > 0 ? 1 : 0));
	}


	/// \if DE
	/// @brief berechnet Fakultät (Helferklasse)
	/// \else
	/// @brief computes factorial (Helperclass)
	/// \endif
	template<class T, int value> 
	struct FactorialHelperClass
	{
		static T GetResult()
		{
			return (T)value * FactorialHelperClass<T, value-1>::GetResult();
		}
	};

	template<class T>
	struct FactorialHelperClass<T,1>
	{
		static T GetResult()
		{
			return (T)1;
		}
	};

	/// \if DE
	/// @brief berechnet Fakultät zur Compilezeit
	/// \else
	/// @brief computes factorial at compile time
	/// \endif
	template<class T, int value> inline T Fac()
	{
		return FactorialHelperClass<T,value>::GetResult();
	}


	/// @brief berechnet Fakultät
	/// \else
	/// @brief computes factorial
	/// \endif
	template<class T> inline T Fac( T value )
	{
		assert( value >= 0 );
		T res = 1;
		while( value > 1 )
		{
			res *= value;
			value--;
		}
		return res;
	}


	/// \if DE
	/// @brief berechnet Nachkommastellen
	/// \else
	/// @brief computes fraction
	/// \endif
	template<class T> inline T Frac( const T& value )
	{
		return value - RoundDown( value );
	}

	/// \if DE
	/// @brief Zufallszahl
	/// \else
	/// @brief random number 
	/// \endif
	inline UInt32 Rand()
	{
		static UInt32 noiseIndex = 0;

		UInt32 x = noiseIndex;
		noiseIndex++;

		x = ( x << 13 ) ^ x;
		return ( x * ( x * x * 15731 + 789221 ) + 1376312589); 
	}

	/// \if DE
	/// @brief normalisierte Zufallszahl
	/// \else
	/// @brief normalized random number 
	/// \endif
	template<class T> inline T RandN()
	{
		return (T)Rand() / (T)0xFFFFFFFF;
	}

	/// \if DE
	/// @brief gibt die Anzahl der 1 Bits zurück
	/// \else
	/// @brief returns the number of 1 bits
	/// \endif
	template<typename T>
	unsigned char Population( T v )
	{
		T c = 0;
		for ( ; v; v >>= 1 )
		{
			c += v & 1;
			}
		return c;
	}

	/// \if DE
	/// @brief gibt die Anzahl der 1 Bits zurück
	/// \else
	/// @brief returns the number of 1 bits
	/// \endif
	template<typename T>
	unsigned char Population( UInt8 v )
	{
		v = ( ( v & 0xAA ) >> 1 ) + ( v & 0x55 );
		v = ( ( v & 0xCC ) >> 2 ) + ( v & 0x33 );
		v = ( ( v & 0xF0 ) >> 4 ) + ( v & 0x0F );
		return v;
	}

	/// \if DE
	/// @brief gibt die Anzahl der 1 Bits zurück
	/// \else
	/// @brief returns the number of 1 bits
	/// \endif
	template<typename T>
	unsigned char Population( UInt16 v )
	{
		v = ( ( v & 0xAAAA ) >> 1 ) + ( v & 0x5555 );
		v = ( ( v & 0xCCCC ) >> 2 ) + ( v & 0x3333 );
		v = ( ( v & 0xF0F0 ) >> 4 ) + ( v & 0x0F0F );
		v = ( ( v & 0xFF00 ) >> 8 ) + ( v & 0x00FF );
		return v;
	}


	/// \if DE
	/// @brief gibt die Anzahl der 1 Bits zurück
	/// \else
	/// @brief returns the number of 1 bits
	/// \endif
	template<typename T>
	unsigned char Population( UInt32 v )
	{
		int const w = v - ( ( v >> 1 ) & 0x55555555 );
		int const x = ( w & 0x33333333 ) + ( ( w >> 2 ) & 0x33333333 );
		return ( ( x + ( x >> 4 ) & 0xF0F0F0F ) * 0x1010101 ) >> 24;
	}

	/// \if DE
	/// @brief gibt die Anzahl der 1 Bits zurück
	/// \else
	/// @brief returns the number of 1 bits
	/// \endif
	template<typename T>
	unsigned char Population( UInt64 v )
	{
		const int v0 = (const int)( v & 0xFFFFFFFF );
		const int v1 = (const int)( ( v >> 32 ) & 0xFFFFFFFF );
		const int w0 = v0 - ( ( v0 >> 1 ) & 0x55555555 );
		const int w1 = v1 - ( ( v1 >> 1 ) & 0x55555555 );
		const int x0 = ( w0 & 0x33333333 ) + ( ( w0 >> 2 ) & 0x33333333 );
		const int x1 = ( w1 & 0x33333333 ) + ( ( w1 >> 2 ) & 0x33333333 );
		const int p0 = ( ( x0 + ( x0 >> 4 ) & 0xF0F0F0F ) * 0x1010101 ) >> 24;
		const int p1 = ( ( x1 + ( x1 >> 4 ) & 0xF0F0F0F ) * 0x1010101 ) >> 24;

		return p0 + p1;
	}


	/// \if DE
	/// @brief gibt zurück ob value zwischen v0 und v1 ist
	/// \else
	/// @brief returns true if value is between v0 and v1
	/// \endif
	template<class T> inline bool IsBetween( const T& value, const T& v0, const T& v1 )
	{
		T min = Min( v0, v1 );
		T max = Max( v0, v1 );
		return ( min <= value ) && ( value <= max );
	}

	/// \if DE
	/// @brief interpoliert linear zwischen zwei werten
	/// \else
	/// @brief interpolates linearly between two values
	/// \endif
	template<class T1, class T2> inline T1 Interpolate( const T2& lerp, const T1& v0, const T1& v1 )
	{
		return (T1)( v0 + ( v1 - v0 ) * lerp );
	}

	/// \if DE
	/// @brief interpoliert bilinear zwischen vier werten
	/// \else
	/// @brief interpolates bilinearly between four values
	/// \endif
	template<class T1, class T2> inline	T1 BilinearInterpolate( const T2& u, const T2& v, const T1& v00, const T1& v10, const T1& v01, const T1& v11 )
	{
		return Interpolate( v, Interpolate( u, v00, v10 ), Interpolate( u, v01, v11 ) );
	}

	/// \if DE
	/// @brief interpoliert trilinear zwischen acht werten
	/// \else
	/// @brief interpolates trilinearly between eight values
	/// \endif
	template<class T1, class T2> inline T1 TrilinearInterpolate( const T2& u, const T2& v, const T2& w, const T1& v000, const T1& v100, const T1& v010, const T1& v110,
																										const T1& v001, const T1& v101, const T1& v011, const T1& v111 )
	{
		return Interpolate( w, BilinearInterpolate( u, v, v000, v100, v010, v110 ), BilinearInterpolate( u, v, v001, v101, v011, v111 ) );
	}

	/// \if DE
	/// @brief Hermite Lerp Wert zwischen 0 und 1
	///
	/// Gibt 0 zurück wenn <lerp> < 0. Gibt 1 zurück wenn <lerp> > 1. Gibt einen weiche Hermite-Wert zwischen 0 and 1 zurück.
	/// \else
	/// @brief Hermite lerp value between 0 and 1
	///
	/// Returns 0 if <lerp> < 0. Returns 1 if <lerp> > 1. Returns a smooth Hermite value between 0 and 1.
	/// \endif
	template<class T> inline T Hermite( const T& lerp )
	{
		T sqrL = lerp * lerp;
		return ( (T)3.0 * sqrL ) - ( (T)2.0 * sqrL * lerp );
	}

	/// \if DE
	/// @brief Hermite Lerp Wert zwischen min und max
	///
	///	Returns 0 if <lerp> < <min>. Returns 1 if <lerp> > <max>. Gibt eine weiche Hermite-Wert zwischen 0 and 1 zurück, wenn <lerp> innerhalb von min und max ist.
	/// \else
	/// @brief Hermite lerp value between min and max
	///	
	///	Returns 0 if <lerp> < <min>. Returns 1 if <lerp> > <max>. Returns a smooth Hermite value between 0 and 1, if <lerp> is in the range [min, max].
	/// \endif
	template<class T> inline T Hermite( const T& lerp, const T& min, const T& max )
	{
		assert( min < max );
		return Hermite( ( max - min ) * ( lerp - min ) );
	}

	/// \if DE
	/// @brief 1D-Perlin Noise Funktion (-1.0 bis 1.0)
	/// \else
	/// @brief 1D-Perlin Noise function (-1.0 to 1.0)
	/// \endif
	__inline double Noise( unsigned int x )
	{
		x = ( x << 13 ) ^ x;
		return ( 1.0 - ( ( x * ( x * x * 15731 + 789221 ) + 1376312589) & 0x7FFFFFFF ) / 1073741824.0 );    
	}

	/// \if DE
	/// @brief 1D-Perlin Noise Funktion (0.0 bis 1.0)
	/// \else
	/// @brief 1D-Perlin Noise function (0.0 to 1.0)
	/// \endif
	__inline double PositiveNoise( unsigned int x )
	{
		x = ( x << 13 ) ^ x;
		return ( 1.0 - ( ( x * ( x * x * 15731 + 789221 ) + 1376312589) & 0x7FFFFFFF ) / 2147483648.0 );    
	}

	/// \if DE
	/// @brief 2D-Perlin Noise Funktion
	/// \else
	/// @brief 2D-Perlin Noise function
	/// \endif
	__inline double Noise( unsigned int x, unsigned int y )
	{
		unsigned int n = x + y * 57;
		n = ( n << 13 ) ^ n;
		unsigned int result = ( ( n * ( n * n * 15731 + 789221 ) + 1376312589 ) & 0x7FFFFFFF);

		return ( 1.0 - double(result) / 1073741824.0 );
	}

	/// \if DE
	/// @brief Weiches 1D-Noise (Gausssche Verteilung)
	/// \else
	/// @brief Smooth 1D-Noise (Gaussian Distribution)
	/// \endif
	__inline double SmoothNoise( unsigned int x )
	{
		double sides   = ( Noise( x - 1 ) + Noise( x + 1 ) ) /  4.0;
		double center  =   Noise( x ) / 2.0;
		return sides + center;
	}

	/// \if DE
	/// @brief Weiches 2D-Noise (Gausssche Verteilung)
	/// \else
	/// @brief Smooth 2D-Noise (Gaussian Distribution)
	/// \endif
	__inline double SmoothNoise( unsigned int x, unsigned int y )
	{
		double corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16.0;
		double sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8.0;
		double center  =   Noise(x, y) / 4.0;
		return corners + sides + center;
	}

	/// \if DE
	/// @brief 2D-Noise basierend auf Fließkommazahlparameter
	/// \else
	/// @brief 2D-Noise based on floatingpoint parameters
	/// \endif
	__inline double InterpolatedNoise( double x, double y )
	{
		int		integer_X    = int(x);
		double	fractional_X = x - integer_X;

		int		integer_Y    = int(y);
		double	fractional_Y = y - integer_Y;

		double v1 = Noise( integer_X,     integer_Y );
		double v2 = Noise( integer_X + 1, integer_Y);
		double v3 = Noise( integer_X,     integer_Y + 1 );
		double v4 = Noise( integer_X + 1, integer_Y + 1 );

		double i1 = Interpolate( fractional_X, v1, v2 );
		double i2 = Interpolate( fractional_X, v3, v4 );

		return Interpolate( fractional_Y, i1, i2 );
	}

	/// \if DE
	/// @brief Weiches 2D-Noise basierend auf Fließkommazahlparameter
	/// \else
	/// @brief Smooth 2D-Noise based on floatingpoint parameters
	/// \endif
	__inline double SmoothInterpolatedNoise( double x, double y )
	{
		int		integer_X    = int(x);
		double	fractional_X = x - integer_X;

		int		integer_Y    = int(y);
		double	fractional_Y = y - integer_Y;

		double v1 = SmoothNoise( integer_X,     integer_Y );
		double v2 = SmoothNoise( integer_X + 1, integer_Y);
		double v3 = SmoothNoise( integer_X,     integer_Y + 1 );
		double v4 = SmoothNoise( integer_X + 1, integer_Y + 1 );

		double i1 = Interpolate( fractional_X, v1, v2 );
		double i2 = Interpolate( fractional_X, v3, v4 );

		return Interpolate( fractional_Y, i1, i2 );
	}


	/// \if DE
	/// @brief Rechte Bitmaske
	/// Erzeugt eine Konstante zur Compilezeit mit der angegebenen Anzahl an Bits die auf 1 gesetzt werden
	/// int mask = RightBitMask<int,5>::GetMask() = 0x0000001F
	/// \else
	/// @brief right bit mask
	/// creates a constant at compile time with given number of bits set to 1
	/// int mask = RightBitMask<int,5>::GetMask() = 0x0000001F
	/// \endif
	template<typename T, int NumBits>
	struct RightBitMask
	{
		static T GetMask()
		{
			return ( 0x1 << (NumBits - 1) ) | RightBitMask<T, NumBits-1>::GetMask();
		}
	};

	template<typename T>
	struct RightBitMask<T,0>
	{
		static T GetMask()
		{
			return 0;
		}
	};

	/// \if DE
	/// @brief Linke Bitmaske
	/// Erzeugt eine Konstante zur Compilezeit mit der angegebenen Anzahl an Bits die auf 1 gesetzt werden
	/// int mask = LeftBitMask<int,5>::GetMask() = 0xF8000000
	/// \else
	/// @brief left bit mask
	/// creates a constant at compile time with given number of bits set to 1
	/// int mask = LeftBitMask<int,5>::GetMask() = 0xF8000000
	/// \endif
	template<typename T, int NumBits>
	struct LeftBitMask
	{
		static T GetMask()
		{
			return ( 0x1 << ( ( sizeof(T) * 8 ) - NumBits ) ) | LeftBitMask<T, NumBits-1>::GetMask();
		}
	};

	template<typename T>
	struct LeftBitMask<T,0>
	{
		static T GetMask()
		{
			return 0;
		}
	};


	/// \if DE
	/// @brief LDR Color encoding
	/// \else
	/// @brief LDR Color encoding
	/// \endif
	struct ARGB
	{
		union
		{
			unsigned long	argb;
			struct
			{
#ifdef LITTLE_ENDIAN
				unsigned char B : 8;
				unsigned char G : 8;
				unsigned char R : 8;
				unsigned char A : 8;
#else
				unsigned char A : 8;
				unsigned char R : 8;
				unsigned char G : 8;
				unsigned char B : 8;
#endif
			};
		};

		ARGB() {}

		template<class T>
		ARGB( const T& r, const T& g, const T& b, const T& a )
		{
			R = (unsigned char) ( r * 255.0 );
			G = (unsigned char) ( g * 255.0 );
			B = (unsigned char) ( b * 255.0 );
			A = (unsigned char) ( a * 255.0 );
		}

		ARGB( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) : B(b), G(g), R(r), A(a)	{}

		ARGB( unsigned long argb ) : argb(argb) {}

		operator unsigned long ()	{ return argb; }
		operator const unsigned long () const { return argb; }

	};


}

#endif //_ZFXMATH_INCLUDE_BASICMATH_H_
