/// \file
///
/// @brief TMultiplyStack
///
/// \if DE
/// TMultiplyStack: Stack, bei dem die Elemtente Multipliziert werden können.
/// \else
/// TMultiplyStack: Stack, where you can multiply the top elements.
/// \endif

#ifndef	ZFXMATH_INCLUDE_MULTIPLYSTACK_H
#define	ZFXMATH_INCLUDE_MULTIPLYSTACK_H

#include <stack>

namespace ZFXMath {
	/// \if DE
	/// @brief Stack, bei dem die Elemtente Multipliziert werden können.
	///
	/// Erweiterung von std::stack um Funktionen,
	/// mit denen das erste Elemten multipliziert werden kann, etc.
	/// z.B. TMatrix4x4Stack
	/// \else
	/// @brief Stack, where you can multiply the top elements.
	///
	/// Extension of std::stack with functions,
	/// which alloc multiplication of the top elements.
	/// for example: TMatrix4x4Stack
	/// \endif
	template<class T>
	class TMultiplyStack : public std::stack<T>
	{
	public:
		/// \if DE
		/// @brief Kopiert das erste Element.
		/// \else
		/// @brief Copy the top element.
		/// \endif
		inline void PushTop()
		{
			if( !std::stack<T>::empty() )
			{
				std::stack<T>::push( std::stack<T>::top() );
			}
		}

		/// \if DE
		/// @brief Kopiert das erste Element und multipliziert es mit 'val'.
		/// \else
		/// @brief Copy the top element and multiply it with 'val'.
		/// \endif
		inline void PushMultiply(const T& val)
		{
			if( !std::stack<T>::empty() )
			{
				std::stack<T>::push( std::stack<T>::top() * val );
			}
			else 
			{
				std::stack<T>::push( val );
			}
		}

		/// \if DE
		/// @brief Multipliziert das erste Element mit val.
		/// \else
		/// @brief Multiply the top element with 'val'.
		/// \endif
		inline void Multiply(const T& val)
		{
			if( !std::stack<T>::empty() )
			{
				std::stack<T>::top() *= val;
			}
			else 
			{
				std::stack<T>::push( val );
			}
		}
	};
}

#endif // ZFXMATH_INCLUDE_MULTIPLYSTACK_H
