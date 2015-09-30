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

#undef ZeroMemory
#undef FillMemory
#undef CopyMemory
#undef MoveMemory

#include <stdint.h>

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
    inline void FillMemory(void *dest, uint32_t count, uint8_t character)
    {
        uint32_t size32  = count >> 2;
        uint32_t fill    = (character << 24 | character << 16 | character << 8 | character);
        uint32_t *dest32 = (uint32_t*)dest;

        switch ((count - (size32 << 2)))
        {
            case 3: ((uint8_t*)dest)[count - 3] = character;
            case 2: ((uint8_t*)dest)[count - 2] = character;
            case 1: ((uint8_t*)dest)[count - 1] = character;
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
	inline void ZeroMemory(void *dest, uint32_t count)
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
    inline void CopyMemory(void *dest, const void *src, uint32_t count)
    {
        uint32_t    size32    = count >> 2;
        uint32_t    *dest32   = (uint32_t*)dest;
        uint32_t    *src32    = (uint32_t*)src;

        switch ((count - (size32 << 2)))
        {
            case 3: ((uint8_t*)dest)[count - 3] = ((uint8_t*)src)[count - 3];
            case 2: ((uint8_t*)dest)[count - 2] = ((uint8_t*)src)[count - 2];
            case 1: ((uint8_t*)dest)[count - 1] = ((uint8_t*)src)[count - 1];
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
    inline void MoveMemory(void *dest, const void *src, uint32_t count)
    {
        uint32_t    size32    = count >> 2;
        uint32_t    *dest32   = (uint32_t*)dest;
        uint32_t    *src32    = (uint32_t*)src;

        if (dest > src)
        {
            switch ((count - (size32 << 2)))
            {
                case 3:
                ((uint8_t*)dest)[count - 1] = ((uint8_t*)src)[count - 1];
                ((uint8_t*)dest)[count - 2] = ((uint8_t*)src)[count - 2];
                ((uint8_t*)dest)[count - 3] = ((uint8_t*)src)[count - 3];
                break;

                case 2:
                ((uint8_t*)dest)[count - 1] = ((uint8_t*)src)[count - 1];
                ((uint8_t*)dest)[count - 2] = ((uint8_t*)src)[count - 2];
                break;

                case 1:
                ((uint8_t*)dest)[count - 1] = ((uint8_t*)src)[count - 1];
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
