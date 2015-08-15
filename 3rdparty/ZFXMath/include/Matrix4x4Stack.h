/// \file
///
/// @brief TMatrix4x4Stack
///
/// TMatrix4x4Stack: TMultiplyStack<TMatrix4x4>

#ifndef	ZFXMATH_INCLUDE_MATRIX4X4STACK_H
#define	ZFXMATH_INCLUDE_MATRIX4X4STACK_H

namespace ZFXMath
{
	/// \if DE
	/// @brief TMultiplyStack für TMatrix4x4
	/// \else
	/// @brief TMultiplyStack for TMatrix4x4
	/// \endif
	template<class T>
	class TMatrix4x4Stack : public TMultiplyStack< TMatrix4x4<T> >
	{
	};
}

#endif // ZFXMATH_INCLUDE_MATRIX4X4STACK_H
