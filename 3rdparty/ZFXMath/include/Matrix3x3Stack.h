/// \file
///
/// @brief TMatrix3x3Stack
///
/// TMatrix3x3Stack: TMultiplyStack<TMatrix3x3>

#ifndef	ZFXMATH_INCLUDE_MATRIX3X3STACK_H
#define	ZFXMATH_INCLUDE_MATRIX3X3STACK_H

namespace ZFXMath 
{
	/// \if DE
	/// @brief TMultiplyStack für TMatrix3x3
	/// \else
	/// @brief TMultiplyStack for TMatrix3x3
	/// \endif
	template<class T>
	class TMatrix3x3Stack : public TMultiplyStack< TMatrix3x3<T> >
	{
	};
}

#endif // ZFXMATH_INCLUDE_MATRIX3X3STACK_H
