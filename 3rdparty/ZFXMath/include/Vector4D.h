/// \file
///
/// \if DE
/// @brief TVector4D
///
/// TVector4D: 4-dimensionaler Vektor
/// \else
/// @brief TVector4D
///
/// TVector4D: 4-dimensional Vector
/// \endif

#ifndef ZFXMATH_INCLUDE_VECTOR4D_H
#define ZFXMATH_INCLUDE_VECTOR4D_H

namespace ZFXMath 
{
	template<typename T>struct TVector2D;
	template<typename T>struct TVector3D;
	template<int N,typename T>struct TVectorN;

	/// \if DE
	/// @brief TVector4D
	///
	/// TVector4D: 4-dimensionaler Vektor
	/// \else
	/// @brief TVector4D
	///
	/// TVector4D: 4-dimensional Vector
	/// \endif
	template<typename T>
	struct TVector4D
	{
	public:

		union
		{
			struct
			{
				T	val[4];
			};

			struct
			{
				T	x, y, z, w;
			};

			struct
			{
				T	r, g, b, a;
			};
		};

		TVector4D(){}
		TVector4D(const T &v):x(v),y(v),z(v),w(v){}
		TVector4D(const T &x,const T &y,const T &z,const T &w):x(x),y(y),z(z),w(w){}
		TVector4D(const T *p):x(p[0]),y(p[1]),z(p[2]),w(p[3]){}
		TVector4D(const TVector4D &v):x(v.x),y(v.y),z(v.z),w(v.w){}
		TVector4D(const TVector2D<T> &v, const TVector2D<T> &u = TVector2D<T>( T(0.0), T(1.0) ) ):x(v.x),y(v.y),z(u.x),w(u.y){}
		TVector4D(const TVector3D<T> &v,const T &w = T(1.0)):x(v.x),y(v.y),z(v.z),w(w){}
		TVector4D(const TVectorN<4,T> &v):x(v(0)),y(v(1)),z(v(2)),w(v(3)){}

		operator T*(){return val;}
		operator const T*()const{return val;}
		operator TVectorN<4,T>&(){return *(TVectorN<4,T>*)this;}
		operator const TVectorN<4,T>&()const{return *(const TVectorN<4,T>*)this;}

		T& operator()(int i){return val[Check(i)];}
		const T& operator()(int i)const{return val[Check(i)];}


		/// \if DE
		/// @brief automatische Typkonvertierung
		/// \else
		/// @brief auto coercion
		/// \endif
		template<typename T2>
		operator TVector4D<T2>() const
		{
			return TVector4D<T2>( (T2)x, (T2)y, (T2)z, (T2)w );
		}

		/// \if DE
		/// @brief automatische Typkonvertierung
		/// \else
		/// @brief auto coercion
		/// \endif
		operator ARGB() const
		{
			return ARGB( x, y, z, w );
		}

		TVector4D& operator=(const TVector4D &v){x=v.x;y=v.y;z=v.z;w=v.w; return *this; }
		TVector4D& operator+=(const TVector4D &v){x+=v.x;y+=v.y;z+=v.z;w+=v.w; return *this; }
		TVector4D& operator-=(const TVector4D &v){x-=v.x;y-=v.y;z-=v.z;w-=v.w; return *this; }
		TVector4D& operator*=(const TVector4D &v){x*=v.x;y*=v.y;z*=v.z;w*=v.w; return *this; }
		TVector4D& operator/=(const TVector4D &v){x/=v.x;y/=v.y;z/=v.z;w/=v.w; return *this; }
		TVector4D& operator/=(const T& s){x/=s;y/=s;z/=s;w/=s; return *this; }

		TVector4D operator+()const{return *this;}
		TVector4D operator-()const{return TVector4D(-x,-y,-z,-w);}
		TVector4D operator+(const TVector4D &v)const{return TVector4D(x+v.x,y+v.y,z+v.z,w+v.w);}
		TVector4D operator-(const TVector4D &v)const{return TVector4D(x-v.x,y-v.y,z-v.z,w-v.w);}
		TVector4D operator*(const TVector4D &v)const{return TVector4D(x*v.x,y*v.y,z*v.z,w*v.w);}
		TVector4D operator/(const TVector4D &v)const{return TVector4D(x/v.x,y/v.y,z/v.z,w/v.w);}
		TVector4D operator*(const T &t)const{return TVector4D(x*t,y*t,z*t,w*t);}
		TVector4D operator/(const T &t)const{return TVector4D(x/t,y/t,z/t,w/t);}
		friend TVector4D<T> operator*(const T &t,const TVector4D &v){return v*t;}

		unsigned long ColorToDWORD()const
		{
			ARGB argb(r,g,b,a);
			return argb;
		}
		
		unsigned long ToDWORD()const
		{
			ARGB argb(r * 0.5 + 0.5,g * 0.5 + 0.5,b * 0.5 + 0.5,a * 0.5 + 0.5);
			return argb;
		}

		T DotProduct(const TVector4D &v) const
		{
			return x*v.x+y*v.y+z*v.z+w*v.w;
		}

		T LengthSqr() const
		{
			return DotProduct(*this);
		}

		T Length() const
		{
			return Sqrt(LengthSqr());
		}

		TVector4D& Normalize()
		{
			return (*this) /= Length();
		};

		TVector4D GetNormalized() const
		{
			return (*this) / Length();
		};

		T Luminance()const
		{
			return DotProduct(TVector4D<T>(T(0.39),T(0.5),T(0.11),T(1.0))); 
		}

		static TVector4D Zero()
		{
			return TVector4D( (T)0.0, (T)0.0, (T)0.0, (T)0.0 );
		}

		static TVector4D<T> Min(const TVector4D<T> &a,const TVector4D<T> &b)
		{
			return TVector4D<T>(ZFXMath::Min(a.x,b.x),ZFXMath::Min(a.y,b.y),ZFXMath::Min(a.z,b.z),ZFXMath::Min(a.w,b.w));
		}

		static TVector4D Max(const TVector4D &a, const TVector4D &b)
		{
			return TVector4D(ZFXMath::Max(a.x,b.x),ZFXMath::Max(a.y,b.y),ZFXMath::Max(a.z,b.z),ZFXMath::Max(a.w,b.w));
		}

		TVector4D Interpolate( const TVector4D& v, T lerp ) const
		{
			return ZFXMath::Interpolate( lerp, *this, v );
		}

		TVector4D& Max( const TVector4D& v )
		{
			x = ZFXMath::Max( x, v.x );
			y = ZFXMath::Max( y, v.y );
			z = ZFXMath::Max( z, v.z );
			w = ZFXMath::Max( w, v.w );
			return *this;
		};

		TVector4D& Min( const TVector4D& v )
		{
			x = ZFXMath::Min( x, v.x );
			y = ZFXMath::Min( y, v.y );
			z = ZFXMath::Min( z, v.z );
			w = ZFXMath::Min( w, v.w );
			return *this;
		};

	private:

		inline int Check(int i)
		{
			assert(i >= 0 && i < 4);
			return i;
		}
	};
}

#endif // ZFXMATH_INCLUDE_VECTOR4D_H
