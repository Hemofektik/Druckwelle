/// \file
///
/// @brief TPackedMatrix
///
/// TPackedMatrix: TMultiplyStack<TMatrix4x4>

#ifndef	ZFXMATH_INCLUDE_PACKEDMATRIXSTACK_H
#define	ZFXMATH_INCLUDE_PACKEDMATRIXSTACK_H

namespace ZFXMath
{
	/// \if DE
	/// @brief TMultiplyStack für TPackedMatrix
	/// \else
	/// @brief TMultiplyStack for TPackedMatrix
	/// \endif
	template<class T>
	class TPackedMatrixStack : public TMultiplyStack< TPackedMatrix<T> >
	{
	};
}

#endif // ZFXMATH_INCLUDE_PACKEDMATRIXSTACK_H
