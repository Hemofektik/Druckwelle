/// \file
///
/// \if DE
/// @brief TVector3D
///
/// TVector3D: 3-dimensionaler Vektor
/// \else
/// @brief TVector3D
///
/// TVector3D: 3-dimensional Vector
/// \endif


#ifndef _ZFXMATH_INCLUDE_VECTOR3D_H_
#define _ZFXMATH_INCLUDE_VECTOR3D_H_

namespace ZFXMath
{

	template<typename T>
	struct TVector4D;

	template<typename T>
	struct TQuaternion;

	/// \if DE
	/// @brief TVector3D
	///
	/// TVector3D: 3-dimensionaler Vektor
	/// \else
	/// @brief TVector3D
	///
	/// TVector3D: 3-dimensional Vector
	/// \endif
	template<typename T>
	struct TVector3D
	{
		union
		{
			struct
			{
				T			val[3];
			};
			struct
			{
				T		x, y, z;
			};
			struct
			{
				T		r, g, b;
			};
		};

		TVector3D() {}

		TVector3D( const T& x, const T& y, const T& z ) : x(x), y(y), z(z) {}

		TVector3D( const T* pV ) : x(pV[2]), y(pV[0]), z(pV[2]) {}
		TVector3D( const T& val ) : x(val), y(val), z(val) {}
		TVector3D( const TVector3D& v ) : x(v.x), y(v.y), z(v.z) {}
		TVector3D( const TVector2D<T>& v, const T& z = T( 1.0 ) ) : x(v.x), y(v.y), z(z) {}
		TVector3D( const TVector4D<T>& v ) : x(v.x), y(v.y), z(v.z) {}
		TVector3D( const TVectorN<3,T>& v) : x(v(0)), y(v(1)), z(v(2)) {}


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
		operator TVectorN<3,T>& () { return *(TVectorN<3,T>*)this; }
		operator const TVectorN<3,T>& () const { return *(const TVectorN<3,T>*)this; }

		/// \if DE
		/// @brief automatische Typkonvertierung
		/// \else
		/// @brief auto coercion
		/// \endif
		template<typename T2>
		operator TVector3D<T2>() const
		{
			return TVector3D<T2>( (T2)x, (T2)y, (T2)z );
		}

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
		/// @brief Zuweisungsoperator
		/// \else
		/// @brief Assignment operator
		/// \endif
		TVector3D& operator = ( const T& val ) { x = y = z = val; return *this; }
		TVector3D& operator += ( const TVector3D& v ) { x += v.x; y += v.y; z += v.z; return *this; }
		TVector3D& operator -= ( const TVector3D& v ) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		TVector3D& operator *= ( const TVector3D& v ) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		TVector3D& operator /= ( const TVector3D& v ) { x /= v.x; y /= v.y; z /= v.z; return *this; }

		/// \if DE
		/// @brief Zuweisungsoperator (Skalierung)
		/// \else
		/// @brief Assignment operator (scale)
		/// \endif
		TVector3D& operator *= ( const T& v ) { x *= v; y *= v; z *= v; return *this; }
		TVector3D& operator /= ( const T& v ) { x /= v; y /= v; z /= v; return *this; }

		/// \if DE
		/// @brief Unary Operator
		/// \else
		/// @brief Unary operator
		/// \endif
		TVector3D operator + () const { return *this; }
		TVector3D operator - () const { return TVector3D(-x,-y,-z); }

		/// \if DE
		/// @brief Binärer Operator
		/// \else
		/// @brief Binary operator
		/// \endif
		TVector3D operator + ( const TVector3D& v ) const { return TVector3D( x + v.x, y + v.y, z + v.z ); }
		TVector3D operator - ( const TVector3D& v ) const { return TVector3D( x - v.x, y - v.y, z - v.z ); }
		TVector3D operator * ( const TVector3D& v ) const { return TVector3D( x * v.x, y * v.y, z * v.z ); }
		TVector3D operator / ( const TVector3D& v ) const { return TVector3D( x / v.x, y / v.y, z / v.z ); }
		TVector3D operator * ( const T& v ) const { return TVector3D( x * v, y * v, z * v ); }
		TVector3D operator / ( const T& v ) const { return TVector3D( x / v, y / v, z / v ); }
		friend TVector3D<T> operator * ( const T& val, const TVector3D<T>& vec ) { return TVector3D( vec.x * val, vec.y * val, vec.z * val ); }

		bool operator < ( const TVector3D& v ) const { return ( x < v.x ) && ( y < v.y ) && ( z < v.z ); }
		bool operator <= ( const TVector3D& v ) const { return ( x <= v.x ) && ( y <= v.y ) && ( z <= v.z ); }
		bool operator > ( const TVector3D& v ) const { return ( x > v.x ) && ( y > v.y ) && ( z > v.z ); }
		bool operator >= ( const TVector3D& v ) const { return ( x >= v.x ) && ( y >= v.y ) && ( z >= v.z ); }

		/// \if DE
		/// @brief packt die Color in einen unsigned long
		/// \else
		/// @brief packs the color into unsigned long
		/// \endif
		unsigned long ColorToDWORD( T alpha = 1.0 ) const
		{
			ARGB color( r, g, b, alpha );

			return color;
		};

		/// \if DE
		/// @brief packt den Vektor in einen unsigned long
		/// \else
		/// @brief packs the vector into unsigned long
		/// \endif
		unsigned long ToDWORD( T w = 0.0 ) const
		{
			ARGB packedVector(  x * 0.5 + 0.5,
								y * 0.5 + 0.5,
								z * 0.5 + 0.5,
								w * 0.5 + 0.5 );

			return packedVector;
		};

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
		/// \else
		/// @brief Normalizes the vector
		///
		/// Vector gets normalized
		/// i.e. it gets scaled to length of 1
		/// \endif
		TVector3D& Normalize()
		{
			return (*this) /= Length();
		};

		TVector3D GetNormalized() const
		{
			return (*this) / Length();
		};

		/// \if DE
		/// @brief Skalarprodukt
		/// \else
		/// @brief Dotproduct
		/// \endif
		T DotProduct( const TVector3D& v ) const { return v.x * x + v.y * y + v.z * z; }

		/// \if DE
		/// @brief Vektorprodukt
		/// \else
		/// @brief vector product
		/// \endif
		TVector3D CrossProduct( const TVector3D& v ) const { return TVector3D( y * v.z - v.y * z, z * v.x - v.z * x, x * v.y - v.x * y ); }

		/// \if DE
		/// @brief Helligkeit (Vektor als Farbe)
		/// \else
		/// @brief Luminance (vector as color)
		/// \endif
		T Luminance() const { return DotProduct( TVector3D( T(0.39), T(0.5), T(0.11) ) ); }


		/// \if DE
		/// @brief Gibt das Volumen des aufgespannten Quaders zurück
		/// \else
		/// @brief Gets the box volume
		/// \endif
		T GetVolume() const { return x * y * z; }


		/// \if DE
		/// @brief Gibt den Halfway-vektor zurück
		/// \else
		/// @brief Returns the halfway vector vector
		/// \endif
		TVector3D HalfWay( const TVector3D& r ) const
		{
			return ( ( (*this) + r ) * (T)0.5 ).GetNormalized();
		}

		/// \if DE
		/// @brief Gibt den reflektierten Vektor zurück
		///
		/// \param n Normale der reflektierenden Oberfläche
		/// \else
		/// @brief Returns the reflected vector
		///
		/// \param n normal of the reflecting surface
		/// \endif
		TVector3D Reflect( const TVector3D& n ) const
		{
			return TVector3D( (*this) - 2 * DotProduct( n ) * n );
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
		TVector3D Refract( const TVector3D& n, const T& ri ) const
		{
			T cos = DotProduct( n ) * ri;

			return Interpolate( (*this),  n, cos ).Normalize();
		}

		/// \if DE
		/// @brief Gibt einen beliebigen auf diesem Vektor senkrechten Vektor zurück
		/// \else
		/// @brief Returns an arbitrary orthogonal vector to this vector
		/// \endif
		TVector3D GetArbitraryOrthogonal() const
		{
			if( Abs( DotProduct( XAxis() ) ) < (T)0.998 )
			{
				return CrossProduct( XAxis() );
			}
			else
			{
				return CrossProduct( YAxis() );
			}
		}

		/// \if DE
		/// @brief Gibt die Rotationsmatrix zurück die den Vektor in den Zielvektor transformiert
		/// \else
		/// @brief Returns the rotation matrix which transforms the vector to the target vector
		/// \endif
		TQuaternion<T> GetRotationDelta( const TVector3D& target ) const
		{
			T cos = Clamp( DotProduct( target ), (T)-1.0, (T)1.0 );
			T angle = ACos( cos );
			return TQuaternion<T>::RotationAxis( CrossProduct( target ).Normalize(), angle );
		}


		/// \if DE
		/// @brief Gibt eine Rotationsmatrix zurück die den Vektor als Z-Achse interprtiert
		/// \else
		/// @brief Returns a rotation matrix which uses the vector as z-axis
		/// \endif
		TQuaternion<T> GetRotation() const
		{
			TVector3D<T> upVector = GetArbitraryOrthogonal();
			return TQuaternion<T>::LookAtLH( *this, upVector );
		}

		static TVector3D XAxis()
		{
			return TVector3D( (T)1.0, (T)0.0, (T)0.0 );
		}

		static TVector3D YAxis()
		{
			return TVector3D( (T)0.0, (T)1.0, (T)0.0 );
		}

		static TVector3D ZAxis()
		{
			return TVector3D( (T)0.0, (T)0.0, (T)1.0 );
		}

		static TVector3D Zero()
		{
			return TVector3D( (T)0.0, (T)0.0, (T)0.0 );
		}

		static TVector3D<T> Min(const TVector3D<T> &a,const TVector3D<T> &b)
		{
			return TVector3D<T>(ZFXMath::Min(a.x,b.x),ZFXMath::Min(a.y,b.y),ZFXMath::Min(a.z,b.z));
		}

		static TVector3D Max(const TVector3D &a, const TVector3D &b)
		{
			return TVector3D(ZFXMath::Max(a.x,b.x),ZFXMath::Max(a.y,b.y),ZFXMath::Max(a.z,b.z));
		}

		TVector3D Interpolate( const TVector3D& v, T lerp ) const
		{
			return ZFXMath::Interpolate( lerp, *this, v );
		}

		TVector3D& Max( const TVector3D& v )
		{
			x = ZFXMath::Max( x, v.x );
			y = ZFXMath::Max( y, v.y );
			z = ZFXMath::Max( z, v.z );
			return *this;
		};

		TVector3D& Min( const TVector3D& v )
		{
			x = ZFXMath::Min( x, v.x );
			y = ZFXMath::Min( y, v.y );
			z = ZFXMath::Min( z, v.z );
			return *this;
		};


	private:
		inline int Check( const int index ) const
		{
			// Check bounds in debug build
			assert( index >= 0 && index < 3 );

			return index;
		}
	};
}

#endif //_ZFXMATH_INCLUDE_VECTOR3D_H_


