#ifndef _ZFXMATH_INCLUDE_FRUSTUM_H_
#define _ZFXMATH_INCLUDE_FRUSTUM_H_

namespace ZFXMath
{
	template<typename T>
	struct TFrustum
	{
		union
		{
			struct
			{
				TPlane<T>		plane[6];
			};

			struct
			{
				TPlane<T>		leftPlane;
				TPlane<T>		rightPlane;
				TPlane<T>		topPlane;
				TPlane<T>		bottomPlane;
				TPlane<T>		nearPlane;
				TPlane<T>		farPlane;
			};
		};

		TFrustum(){}

		TFrustum( const TMatrix4x4<T>& viewProjMatrix )
		{
			// left Clipping Plane
			leftPlane.a		= -(viewProjMatrix._41 + viewProjMatrix._11);
			leftPlane.b		= -(viewProjMatrix._42 + viewProjMatrix._12);
			leftPlane.c		= -(viewProjMatrix._43 + viewProjMatrix._13);
			leftPlane.d		= -(viewProjMatrix._44 + viewProjMatrix._14);

			// right Clipping Plane
			rightPlane.a	= -(viewProjMatrix._41 - viewProjMatrix._11);
			rightPlane.b	= -(viewProjMatrix._42 - viewProjMatrix._12);
			rightPlane.c	= -(viewProjMatrix._43 - viewProjMatrix._13);
			rightPlane.d	= -(viewProjMatrix._44 - viewProjMatrix._14);

			// top Clipping Plane
			topPlane.a		= -(viewProjMatrix._41 - viewProjMatrix._21);
			topPlane.b		= -(viewProjMatrix._42 - viewProjMatrix._22);
			topPlane.c		= -(viewProjMatrix._43 - viewProjMatrix._23);
			topPlane.d		= -(viewProjMatrix._44 - viewProjMatrix._24);

			// bottom Clipping Plane
			bottomPlane.a	= -(viewProjMatrix._41 + viewProjMatrix._21);
			bottomPlane.b	= -(viewProjMatrix._42 + viewProjMatrix._22);
			bottomPlane.c	= -(viewProjMatrix._43 + viewProjMatrix._23);
			bottomPlane.d	= -(viewProjMatrix._44 + viewProjMatrix._24);

			// near Clipping Plane
			nearPlane.a		= -(viewProjMatrix._41 + viewProjMatrix._31);
			nearPlane.b		= -(viewProjMatrix._42 + viewProjMatrix._32);
			nearPlane.c		= -(viewProjMatrix._43 + viewProjMatrix._33);
			nearPlane.d		= -(viewProjMatrix._44 + viewProjMatrix._34);

			// far Clipping Plane
			farPlane.a		= -(viewProjMatrix._41 - viewProjMatrix._31);
			farPlane.b		= -(viewProjMatrix._42 - viewProjMatrix._32);
			farPlane.c		= -(viewProjMatrix._43 - viewProjMatrix._33);
			farPlane.d		= -(viewProjMatrix._44 - viewProjMatrix._34);

			// normalize normal and origin distance
			for( int n = 0; n < 6 ; n++ )
			{
				plane[n].Normalize();
			}
		}

	};


}

#endif // _ZFXMATH_INCLUDE_FRUSTUM_H_
