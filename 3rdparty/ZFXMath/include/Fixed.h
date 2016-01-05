/// \file
///
/// \if DE
/// @brief TFixed
///
/// TFixed: Datentyp für Festkommazahlen
/// \else
/// @brief TFixed
///
/// TFixed: Data type for fixed point numbers
/// \endif

#ifndef _ZFXMATH_INCLUDE_FIXED_H_
#define _ZFXMATH_INCLUDE_FIXED_H_

namespace ZFXMath
{
	/// \if DE
	/// @brief TFixed
	///
	/// TFixed: Datentyp für Festkommazahlen
	/// Es sollte maximal Int32 für T verwendet werden.
	/// \else
	/// @brief TFixed
	///
	/// TFixed: Data type for fixed point numbers
	/// A bigger Type as Int32 should not be used for T.
	/// \endif
	template<typename T, int Fraction>
	struct TFixed
	{
		T value;

		TFixed()
		{
		};

		TFixed(	const TFixed& val )
		{
			value = val.value;
		}

		TFixed(	const T& val )
		{ 
			value = val << Fraction;
		}

		TFixed(	const double& val )
		{
			value = (T)(val * ((T)1 << Fraction));
		}

		TFixed(	const float& val )
		{
			value = (T)(val * ((T)1 << Fraction));
		}

		operator double() const
		{
			return ((double)(value)) / ((T)1 << Fraction);
		}

		operator float() const
		{
			return (float)(((double)(value)) / ((T)1 << Fraction));
		}

		operator int() const
		{
			return (int)( value >> Fraction );
		}

		operator unsigned int() const
		{
			return (unsigned int)( value >> Fraction );
		}

		/// \if DE
		/// @brief Unary Operator
		/// \else
		/// @brief Unary operator
		/// \endif
		TFixed operator + () const { return *this; }
		TFixed operator - () const { TFixed ret; ret.value = -value; return ret; }

		TFixed operator + ( const TFixed& fx ) const { return Add( fx ); }
		TFixed operator - ( const TFixed& fx ) const { return Subtract( fx ); }
		TFixed operator * ( const TFixed& fx ) const { return Multiply( fx ); }
		TFixed operator / ( const TFixed& fx ) const { return Divide( fx ); }
		bool   operator > ( const TFixed& fx ) const { return value > fx.value; }
		bool   operator < ( const TFixed& fx ) const { return value < fx.value; }
		bool   operator >= ( const TFixed& fx ) const { return value >= fx.value; }
		bool   operator <= ( const TFixed& fx ) const { return value <= fx.value; }
		bool   operator == ( const TFixed& fx ) const { return value == fx.value; }
		bool   operator != ( const TFixed& fx ) const { return value != fx.value; }

		/// \if DE
		/// @brief Zuweisungsoperator
		/// \else
		/// @brief Assignment operator
		/// \endif
		TFixed& operator += ( const TFixed& fx ) { *this = Add( fx ); return *this; }
		TFixed& operator -= ( const TFixed& fx ) { *this = Subtract( fx ); return *this; }
		TFixed& operator *= ( const TFixed& fx ) { *this = Multiply( fx ); return *this; }
		TFixed& operator /= ( const TFixed& fx ) { *this = Divide( fx ); return *this; }

		/// \if DE
		/// @brief Addiert zwei Fixed Point Zahlen miteinander
		/// \else
		/// @brief adds two fixed point numbers
		/// \endif
		TFixed Add( const TFixed& fx ) const
		{
			TFixed result;

			result.value = value + fx.value;

			return result;
		}

		/// \if DE
		/// @brief Fixed Point Subtraktion
		/// \else
		/// @brief fixed point subtraction
		/// \endif
		TFixed Subtract( const TFixed& fx ) const
		{
			TFixed result;

			result.value = value - fx.value;

			return result;
		}

		/// \if DE
		/// @brief Multipliziert zwei Fixed Point Zahlen miteinander
		/// \else
		/// @brief Multiplies two fixed point numbers
		/// \endif
		TFixed Multiply( const TFixed& fx ) const
		{
			TFixed result;

			result.value = (T)( ( (Int64)value * (Int64)fx.value ) >> Fraction );

			return result;
		}

		/// \if DE
		/// @brief Fixed Point Division
		/// \else
		/// @brief fixed point division
		/// \endif
		TFixed Divide( const TFixed& fx ) const
		{
			TFixed result;

			result.value = (T)( ( (Int64)value << Fraction ) / (Int64)fx.value );

			return result;
		}
	};

	/// \if DE
	/// @brief Quadratwurzel für Fixed Point Zahlen
	/// \else
	/// @brief squareroot for fixed point numbers
	/// \endif
	template<typename T, int Fraction>
	inline TFixed<T,Fraction> Sqrt(const TFixed<T,Fraction>& value)
	{	// will not work if value is near to 1.0
		/*TFixed<T,Fraction> root;
		TFixed<T,Fraction> next;

		if ( value.value <= (T)0 )
		{
			return TFixed<T,Fraction>( (T)0 );
		}

		next.value = value.value >> 2;

		do
		{
			root = next;
			next.value = (next + value / next).value >> 1;
		} 
		while ( root != next );

		return root;*/

		double floating = value;
		return Sqrt( floating );
	}

	/// \if DE
	/// @brief Wenn value<0, dann -1, wenn value>0, dann 1, ansonsten 0
	/// \else
	/// @brief if value<0, than -1, if value>0, than 1, else 0
	/// \endif
	template<typename T, int Fraction>
	inline TFixed<T,Fraction> Sign(const TFixed<T,Fraction>& value)
	{
		return (TFixed<T,Fraction>)(( value.value < 0 ) ? (-1.0) : (value.value > 0 ? 1.0 : 0.0));
	}

	/// \if DE
	/// @brief Berechnet |value|
	/// \else
	/// @brief calculates |value|
	/// \endif
	template<typename T, int Fraction>
	inline TFixed<T,Fraction> Abs(const TFixed<T,Fraction>& value)
	{
		return ( value.value < 0 ) ? -value : value; 
	}

	/// \if DE
	/// @brief berechnet Nachkommastellen
	/// \else
	/// @brief computes fraction
	/// \endif
	template<typename T, int Fraction>
	inline TFixed<T,Fraction> Frac( const TFixed<T,Fraction>& value )
	{
		TFixed<T,Fraction> ret;

		const T fractionMask = RightBitMask<T, Fraction>::GetMask();

		ret.value = fractionMask & value.value;
		return ret;
	}

	/// \if DE
	/// @brief Sinus
	/// \else
	/// @brief sine
	/// \endif
	template<typename T, int Fraction>
	inline TFixed<T,Fraction> Sin(const TFixed<T,Fraction>& rad)
	{
		return (TFixed<T,Fraction>)::sin((double)rad);
	}

	/// \if DE
	/// @brief Kosinus
	/// \else
	/// @brief cosine
	/// \endif
	template<typename T, int Fraction>
	inline TFixed<T,Fraction> Cos(const TFixed<T,Fraction>& rad)
	{
		return (TFixed<T,Fraction>)::cos((double)rad);
	}

	/// \if DE
	/// @brief Tangens
	/// \else
	/// @brief tangent
	/// \endif
	template<typename T, int Fraction>
	inline TFixed<T,Fraction> Tan(const TFixed<T,Fraction>& rad)
	{
		return (TFixed<T,Fraction>)::tan((double)rad);
	}

	/// \if DE
	/// @brief Kotangens
	/// \else
	/// @brief cotangent
	/// \endif
	template<typename T, int Fraction>
	inline TFixed<T,Fraction> Cot(const TFixed<T,Fraction>& rad)
	{
		return (TFixed<T,Fraction>)( 1.0 / ::tan( (double)rad ) );
	}

	/// \if DE
	/// @brief Potenz
	/// \else
	/// @brief exponentiation
	/// \endif
	template<typename T, int Fraction>
	inline TFixed<T,Fraction> Pow(const TFixed<T,Fraction>& base, const TFixed<T,Fraction>& exp)
	{
		return (TFixed<T,Fraction>)::pow( (double)base, (double)exp );
	}

	/// \if DE
	/// @brief Aufrunden
	/// \else
	/// @brief round up
	/// \endif
	template<typename T, int Fraction>
	inline TFixed<T,Fraction> RoundUp(const TFixed<T,Fraction>& value)
	{
		return (TFixed<T,Fraction>)::ceil((double)value);
	}

	/// \if DE
	/// @brief Abrunden
	/// \else
	/// @brief round down
	/// \endif
	template<typename T, int Fraction>
	inline TFixed<T,Fraction> RoundDown(const TFixed<T,Fraction>& value)
	{
		return (TFixed<T,Fraction>)::floor((double)value);
	}
}

#endif //_ZFXMATH_INCLUDE_FIXED_H_


