// von oese

#ifndef _ZFXMATH_INCLUDE_TRIANGLE_H_
#define _ZFXMATH_INCLUDE_TRIANGLE_H_

namespace ZFXMath
{
	template<typename T>
	struct TTriangle
	{
	private:
		TVector3D<T>	A,B,C;
		TPlane<T>		plane;
		TAABB<T>		aabb;

	public:

		TTriangle(){}

		TTriangle(const TVector3D<T> &a,const TVector3D<T> &b,const TVector3D<T> &c)
		{
			A = a, B = b, C = c;
			plane = TPlane<T>(a,b,c);
			aabb.min = TVector3D<T>::Min(A,TVector3D<T>::Min(B,C));
			aabb.max = TVector3D<T>::Max(A,TVector3D<T>::Max(B,C));
		}
	};
}

#endif
