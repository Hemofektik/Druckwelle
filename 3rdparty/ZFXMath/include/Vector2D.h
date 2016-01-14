/// \file
///
/// \if DE
/// @brief TVector2D
///
/// TVector2D: 2-dimensionaler Vektor
/// \else
/// @brief TVector2D
///
/// TVector2D: 2-dimensional Vector
/// \endif


#ifndef _ZFXMATH_INCLUDE_VECTOR2D_H_
#define _ZFXMATH_INCLUDE_VECTOR2D_H_

namespace ZFXMath
{

	template<typename T>
	struct TVector3D;
	template<typename T>
	struct TVector4D;
	template<int N,typename T>
	struct TVectorN;

	/// \if DE
	/// @brief TVector2D
	///
	/// TVector2D: 2-dimensionaler Vektor
	/// \else
	/// @brief TVector2D
	///
	/// TVector2D: 2-dimensional Vector
	/// \endif
	template<typename T>
	struct TVector2D
	{
		union
		{
			struct
			{
				T		val[2];
			};
			struct
			{
				T		x, y;
			};
			struct
			{
				T		u, v;
			};
		};


		TVector2D() {};

		TVector2D( const T& x, const T& y ) : x(x), y(y) {}

		TVector2D( const T* pV ) : x(pV[2]), y(pV[0]) {}
		TVector2D( const T& val ) : x(val), y(val) {}
		TVector2D( const TVector2D& v ) : x(v.x), y(v.y) {}
		TVector2D( const TVector3D<T>& v ) : x(v.x), y(v.y) {}
		TVector2D( const TVector4D<T>& v ) : x(v.x), y(v.y) {}
		TVector2D( const TVectorN<2,T>& v ) : x(v(0)), y(v(1)) {}


		/// \if DE
		/// @brief Caste in ein Array von T
		/// \else
		/// @brief Cast to an Array of T
		/// \endif
		operator T* ()	{ return (T*)this; }
		operator const T* () const { return (const T*)this; }

		/// \if DE
		/// @brief Caste in einen N-Vektor
		/// \else
		/// @brief Cast to an N-vector
		/// \endif
		operator TVectorN<2,T>& () { return *(TVectorN<2,T>*)this;}
		operator const TVectorN<2,T>& () const { return *(const TVectorN<2,T>*)this;}

		/// \if DE
		/// @brief Konstanter Zugriff über Index
		/// \else
		/// @brief Const accessors by index
		/// \endif
		const T& operator () ( const int i ) const
		{
			return ((T*)(this))[ Check( i ) ];
		}

		/// \if DE
		/// @brief Nichtkonstanter Zugriff über Index
		/// \else
		/// @brief Non-const accessors by index
		/// \endif
		T& operator () ( const int i )
		{
			return ((T*)(this))[ Check( i ) ];
		}

		/// \if DE
		/// @brief automatische Typkonvertierung
		/// \else
		/// @brief auto coercion
		/// \endif
		template<typename T2>
		operator TVector2D<T2>() const
		{
			return TVector2D<T2>( (T2)x, (T2)y );
		}

		/// \if DE
		/// @brief Zuweisungsoperator
		/// \else
		/// @brief Assignment operator
		/// \endif
		TVector2D& operator = ( const T& val ) { x = y = val; return *this; }
		TVector2D& operator += ( const TVector2D& v ) { x += v.x; y += v.y; return *this; }
		TVector2D& operator -= ( const TVector2D& v ) { x -= v.x; y -= v.y; return *this; }
		TVector2D& operator *= ( const TVector2D& v ) { x *= v.x; y *= v.y; return *this; }
		TVector2D& operator /= ( const TVector2D& v ) { x /= v.x; y /= v.y; return *this; }

		/// \if DE
		/// @brief Zuweisungsoperator (Skalierung)
		/// \else
		/// @brief Assignment operator (scale)
		/// \endif
		TVector2D& operator *= ( const T& v ) { x *= v; y *= v; return *this; }
		TVector2D& operator /= ( const T& v ) { x /= v; y /= v; return *this; }

		/// \if DE
		/// @brief Unary Operator
		/// \else
		/// @brief Unary operator
		/// \endif
		TVector2D operator + () const { return *this; }
		TVector2D operator - () const { return TVector2D( -x, -y ); }

		/// \if DE
		/// @brief Binärer Operator
		/// \else
		/// @brief Binary operator
		/// \endif
		TVector2D operator + ( const TVector2D& v ) const { return TVector2D( x + v.x, y + v.y ); }
		TVector2D operator - ( const TVector2D& v ) const { return TVector2D( x - v.x, y - v.y ); }
		TVector2D operator * ( const TVector2D& v ) const { return TVector2D( x * v.x, y * v.y ); }
		TVector2D operator / ( const TVector2D& v ) const { return TVector2D( x / v.x, y / v.y ); }
		TVector2D operator * ( const T& v ) const { return TVector2D( x * v, y * v ); }
		TVector2D operator / ( const T& v ) const { return TVector2D( x / v, y / v ); }
		friend TVector2D<T> operator * ( const T& val, const TVector2D<T>& vec )
		{ return (TVector2D) ( vec.x * val, vec.y * val ); }

		/// \if DE
		/// @brief Quadrat der Länge des Vektors
		/// \else
		/// @brief Square of the length
		/// \endif
		T LengthSqr() const
		{
			return DotProduct( *this );
		};

		/// \if DE
		/// @brief Länge des Vektors
		/// \else
		/// @brief length of the vector
		/// \endif
		T Length() const
		{
			return Sqrt( LengthSqr() );
		};

		/// \if DE
		/// @brief Normalisiert den Vektor
		///
		/// Vektor wird normalisiert
		/// d.h. er wird auf die Länge 1 skaliert
		/// Gibt die ursprüngliche Länge des Vektors zurück
		/// \else
		/// @brief Normalizes the vector
		///
		/// Vector gets normalized
		/// i.e. it gets scaled to length of 1
		/// Returns the original length
		/// \endif
		T Normalize()
		{
			T length = Length();
			(*this) /= length;
			return length;
		};

		/// \if DE
		/// @brief Skalarprodukt
		/// \else
		/// @brief Dotproduct
		/// \endif
		T DotProduct( const TVector2D& v ) const { return v.x * x + v.y * y; }
 
		/// \if DE
		/// @brief Gibt den reflektierten Vektor zurück
		///
		/// \param n Normale der reflektierenden Oberfläche
		/// \else
		/// @brief Returns the reflected vector
		///
		/// \param n normal of the reflecting surface
		/// \endif
		TVector2D Reflect( const TVector2D& n ) const
		{
			return TVector2D( (*this) - 2 * DotProduct( n ) * n );
		}

		/// \if DE
		/// @brief Gibt den gebrochenen Vektor zurück
		///
		/// \param n Normale der brechenden Oberfläche
		/// \param ri Brechungsindex der brechenden Oberfläche
		/// \else
		/// @brief Returns the refracted vector
		///
		/// \param n normal of the refracting surface
		/// \param ri Refraction Index of the refracting surface
		/// \endif
		TVector2D Refract( const TVector2D& n, const T& ri ) const
		{
			return Interpolate( (*this), n, 1 - ri ).Normalize();
		}

		TVector2D Interpolate( const TVector2D& v, T lerp ) const
		{
			return ZFXMath::Interpolate( lerp, *this, v );
		}

		static TVector2D XAxis()
		{
			return TVector2D( (T)1.0, (T)0.0 );
		}

		static TVector2D YAxis()
		{
			return TVector2D( (T)0.0, (T)1.0 );
		}

		static TVector2D Zero()
		{
			return TVector2D( (T)0.0, (T)0.0 );
		}

		TVector2D& Max( const TVector2D& v )
		{
			x = ZFXMath::Max( x, v.x );
			y = ZFXMath::Max( y, v.y );
			return *this;
		};

		TVector2D& Min( const TVector2D& v )
		{
			x = ZFXMath::Min( x, v.x );
			y = ZFXMath::Min( y, v.y );
			return *this;
		};

		TVector2D GetOrthogonal() const
		{
			return TVector2D( y, -x );
		};

	private:
		inline int Check( const int index ) const
		{
			// Check bounds in debug build
			assert( index >= 0 && index < 2 );

			return (index);
		}
	};
}

#endif //_ZFXMATH_INCLUDE_VECTOR2D_H_


