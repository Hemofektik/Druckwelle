/// \file
///
/// \if DE
/// @brief Diverse Speicherfunktionen
/// Funktionen:	ZeroMemory, FillMemory, CopyMemory, MoveMemory
/// \else
/// @brief Some memory functions
/// Functions:	ZeroMemory, FillMemory, CopyMemory, MoveMemory
/// \endif

#ifndef _ZFXMATH_INCLUDE_MEMORY_H_
#define _ZFXMATH_INCLUDE_MEMORY_H_

#include "types.h"

#undef ZeroMemory
#undef FillMemory
#undef CopyMemory
#undef MoveMemory

namespace ZFXMath
{
	/// \if DE
	/// @brief Füllt einen Speicherbereich
	/// Füllt einen Speicherbereich
	/// \param dest [in] Zielbereich
	/// \param count [in] Größe in Byte
	/// \param character [in] Wert
	/// \else
	/// @brief Sets a buffer to a character
	/// Sets a buffer to a character
	/// \param dest [in] destination
	/// \param count [in] size in bytes
	/// \param character [in] value
	/// \endif
    inline void FillMemory(void *dest, unsigned __int32 count, unsigned __int8 character)
    {
        unsigned __int32 size32  = count >> 2;
        unsigned __int32 fill    = (character << 24 | character << 16 | character << 8 | character);
        unsigned __int32 *dest32 = (unsigned __int32*)dest;

        switch ((count - (size32 << 2)))
        {
            case 3: ((unsigned __int8*)dest)[count - 3] = character;
            case 2: ((unsigned __int8*)dest)[count - 2] = character;
            case 1: ((unsigned __int8*)dest)[count - 1] = character;
        }

        while (size32-- > 0)
            *(dest32++) = fill;
    }

	/// \if DE
	/// @brief Füllt einen Speicherbereich mit 0
	/// Füllt einen Speicherbereich mit 0
	/// \param dest [in] Zielbereich
	/// \param count [in] Größe in Byte
	/// \else
	/// @brief Sets a buffer to 0
	/// Sets a buffer to 0
	/// \param dest [in] destination
	/// \param count [in] size in bytes
	/// \endif
	inline void ZeroMemory(void *dest, unsigned __int32 count)
	{
		FillMemory(dest,count,0);
	}

	/// \if DE
	/// @brief Kopiert einen Speicherbereich
	/// Kopiert einen Speicherbereich
	/// \param dest [in] Zielbereich
	/// \param src [in] Quellbereich
	/// \param count [in] Größe in Byte
	/// \else
	/// @brief Copy a buffer to another
	/// Copy a buffer to another
	/// \param dest [in] destination
	/// \param src [in] source
	/// \param count [in] size in bytes
	/// \endif
    inline void CopyMemory(void *dest, const void *src, unsigned __int32 count)
    {
        unsigned __int32    size32    = count >> 2;
        unsigned __int32    *dest32   = (unsigned __int32*)dest;
        unsigned __int32    *src32    = (unsigned __int32*)src;

        switch ((count - (size32 << 2)))
        {
            case 3: ((unsigned __int8*)dest)[count - 3] = ((unsigned __int8*)src)[count - 3];
            case 2: ((unsigned __int8*)dest)[count - 2] = ((unsigned __int8*)src)[count - 2];
            case 1: ((unsigned __int8*)dest)[count - 1] = ((unsigned __int8*)src)[count - 1];
        }

        while (size32-- > 0)
            *(dest32++) = *(src32++);
    }

	/// \if DE
	/// @brief Bewegt einen Speicherbereich
	/// Bewegt einen Speicherbereich
	/// \param dest [in] Zielbereich
	/// \param src [in] Quellbereich
	/// \param count [in] Größe in Byte
	/// \else
	/// @brief Move a buffer
	/// move a buffer
	/// \param dest [in] destination
	/// \param src [in] source
	/// \param count [in] size in bytes
	/// \endif
    inline void MoveMemory(void *dest, const void *src, unsigned __int32 count)
    {
        unsigned __int32    size32    = count >> 2;
        unsigned __int32    *dest32   = (unsigned __int32*)dest;
        unsigned __int32    *src32    = (unsigned __int32*)src;

        if (dest > src)
        {
            switch ((count - (size32 << 2)))
            {
                case 3:
                ((unsigned __int8*)dest)[count - 1] = ((unsigned __int8*)src)[count - 1];
                ((unsigned __int8*)dest)[count - 2] = ((unsigned __int8*)src)[count - 2];
                ((unsigned __int8*)dest)[count - 3] = ((unsigned __int8*)src)[count - 3];
                break;

                case 2:
                ((unsigned __int8*)dest)[count - 1] = ((unsigned __int8*)src)[count - 1];
                ((unsigned __int8*)dest)[count - 2] = ((unsigned __int8*)src)[count - 2];
                break;

                case 1:
                ((unsigned __int8*)dest)[count - 1] = ((unsigned __int8*)src)[count - 1];
                break;
            }

            while (size32-- > 0)
                dest32[size32] = src32[size32];
        }
        else
        {
            for(int i=0; i<size32; ++i)
                *(dest32++) = *(src32++);

            switch ((count - (size32 << 2)))
            {
                case 3: *(dest32++) = *(src32++);
                case 2: *(dest32++) = *(src32++);
                case 1: *(dest32++) = *(src32++);
            }
        }
    }
} // Namespace

#endif //_ZFXMATH_INCLUDE_MEMORY_H_
