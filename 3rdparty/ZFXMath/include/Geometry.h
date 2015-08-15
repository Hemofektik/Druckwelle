#ifndef _ZFXMATH_INCLUDE_GEOMETRY_H_
#define _ZFXMATH_INCLUDE_GEOMETRY_H_

namespace ZFXMath
{
	enum MathResult
	{
		NONE				= 0,
		INTERSECTION,
		FRONTFACE,
		BACKFACE,
		INSIDE,
		OUTSIDE,
		SURROUNDING
	};
}

#endif
