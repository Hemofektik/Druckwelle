/// \file
///
/// \if DE
/// @brief TVectorN
///
/// TVectorN: n-dimensionaler Vektor
///
/// Erstellt von oese
/// 
/// \else
/// @brief TVectorN
///
/// Created by oese
///
/// TVectorN: n-dimensional Vector
/// \endif

#ifndef ZFXMATH_INCLUDE_TVECTORN_H
#define ZFXMATH_INCLUDE_TVECTORN_H

#include "Vector2D.h"
#include "Vector3D.h"

namespace ZFXMath {
	template<typename T>struct TVector2D;
	template<typename T>struct TVector3D;
	template<typename T>struct TVector4D;

	/// \if DE
	/// @brief TVectorN
	///
	/// TVectorN: n-dimensionaler Vektor
	/// \else
	/// @brief TVectorN
	///
	/// TVectorN: n-dimensional Vector
	/// \endif
	template< int N,typename T > 
	struct TVectorN
	{
		T	val[N];

		TVectorN(){}

		TVectorN(const T *p)
		{
			for(register int i = 0; i < N; i++) val[i] = p[i];
		}

		TVectorN(const T &value)
		{
			for(register int i = 0; i < N; i++)	val[i] = value;
		}

		TVectorN(const TVectorN &other)
		{
			for(register int i = 0; i < N; i++)	val[i] = other[i];
		}

		TVectorN(TVector2D<T> &other)
		{
			for (register int i = 0; i < N; i++) {
				val[i] = (i<2) ? other(i) : 0;
			}
		}

		TVectorN(const TVector3D<T> &other)
		{
			for(register int i = 0; i < N; i++) {
				val[i] = (i<3) ? other(i) : 0;
			}
		}

		TVectorN(const TVector4D<T> &other)
		{
			for(register int i = 0; i < N; i++) {
				val[i] = (i<4) ? other(i) : 0;
			}
		}

		template<int M>
		TVectorN( const TVectorN<M,T> &other )
		{
			for(register int i = 0; i < N; i++)	
			{
				val[i] = (i<M) ? other(i) : 0;
			}
		}
		
		operator T*()
		{
			return val;
		}

		operator const T*()const
		{
			return val;
		}

		const T& operator()(int i)const
		{
			return val[Check(i)];
		}

		T& operator()(int i)
		{
			return val[Check(i)];
		}

		TVectorN& operator=(const TVectorN &other)
		{
			for(register int i = 0; i < N; ++i)	
				val[i] = other[i];
			return (*this);
		}

		
		TVectorN& operator+=(const TVectorN &other)
		{
			for (register int i = 0; i < N; ++i)
				val[i] += other[i];
			return (*this);
		}

		
		TVectorN& operator-=(const TVectorN &other)
		{
			for(register int i = 0; i < N; i++)	val[i] -= other[i];
			return (*this);
		}

		
		TVectorN& operator*=(const TVectorN &other)
		{
			for(register int i = 0; i < N; i++)	val[i] *= other[i];
			return (*this);
		}

		
		TVectorN& operator/=(const TVectorN &other)
		{
			for(register int i = 0; i < N; i++)	val[i] /= other[i];
			return (*this);
		}

		TVectorN& operator*=(const T &scaling)
		{
			for(register int i = 0; i < N; i++) val[i] *= scaling;
			return (*this);
		}

		TVectorN& operator/=(const T &scaling)
		{
			for(register int i = 0; i < N; i++) val[i] /= scaling;
			return (*this);
		}
		
		TVectorN operator+()const
		{
			return (*this);
		}

		TVectorN operator-()const
		{
			TVectorN out;
			for(register int i = 0; i < N; i++) out.val[i] = -val[i];
			return out;
		}

		TVectorN operator+(const TVectorN &other)const
		{
			TVectorN out(*this);
			out += other;
			return out;
		}

		TVectorN operator-(const TVectorN &other)const
		{
			TVectorN out(*this);
			out -= other;
			return out;
		}

		TVectorN operator*(const TVectorN &other)const
		{
			TVectorN out(*this);
			out *= other;
			return out;
		}

		TVectorN operator/(const TVectorN &other)const
		{
			TVectorN out(*this);
			out /= other;
			return out;
		}

		TVectorN operator*(const T &scaling)const
		{
			TVectorN out(*this);
			out *= scaling;
			return out;
		}
		
		TVectorN operator/(const T &scaling)const
		{
			TVectorN out(*this);
			out /= scaling;
			return out;
		}

		friend TVectorN<N,T> operator*(const T &scaling,const TVectorN<N,T> &vec)
		{
			TVectorN<N,T> out(vec);
			out *= scaling;
			return out;
		}

		T Length()
		{
			return Sqrt(LengthSqr());
		}

		T LengthSqr()
		{
			return DotProduct(*this);
		}

		void Normalize()
		{
			(*this) /= Length();
		}

		T DotProduct(const TVectorN &other)
		{
			T out = 0;
			for(register int i = 0; i < N; i++)
				out += val[i] * other.val[i];
			return out;
		}

		friend std::ostream& operator<< ( std::ostream& os, const TVectorN<N, T>& Vector)
		{
			os << "[";
			for(register int i=0;i<N;i++)
			{
				os << Vector.val[i];
				if(i+1<N)
					os << ",";
			}
			os << "]" << std::endl;
			return os;
		}


	private:
		inline int Check(int value) const
		{
			assert(value >= 0 && value < N);
			return value;
		}
	};
}

#endif // _ZFXMATH_INCLUDE_TVECTORN_H_
