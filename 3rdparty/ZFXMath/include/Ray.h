#ifndef _ZFXMATH_INCLUDE_RAY_H_
#define _ZFXMATH_INCLUDE_RAY_H_

namespace ZFXMath
{
	template<typename T>
	struct TRay
	{
		TVector3D<T> pos;		//! Position
		TVector3D<T> dir;		//! Direction
		T			 len;		//! Length

		TRay(){}

		TRay( const TVector3D<T>& pos, const TVector3D<T>& dir, const T& len ) :
		pos(pos),
		dir(dir),
		len(len)
		{
		}

		TRay( const TVector3D<T>& pos, const TVector3D<T>& dir ) :
		pos(pos)
		{
			len = dir.Length();
			this->dir = dir / len;
		}

		TVector3D<T> GetDestination() const
		{
			return pos + dir * len;
		}

	};


}

#endif // _ZFXMATH_INCLUDE_RAY_H_
