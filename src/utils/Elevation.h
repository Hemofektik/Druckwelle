
#pragma once

#include "ImageProcessor.h"

namespace dw
{
	static const s16 InvalidValueASTER = -9999;

	namespace utils
	{
		struct BigEndianU16
		{
			union
			{
				u16		value;
				struct
				{
#if LITTLE_ENDIAN
					u8 l : 8;
					u8 h : 8;
#else
					u8 h : 8;
					u8 l : 8;
#endif
				};
			};

			BigEndianU16(u16 value) : value(value) {}
			BigEndianU16(u8 ch1, u8 ch0)
#if LITTLE_ENDIAN
				: l(ch0), h(ch1)
#else
				: h(ch1), l(ch0)
#endif
			{}

			static u16 Swap(u16 value)
			{
				u8 l = (u8)(value & 0xF);
				u8 h = (u8)(value >> 8);
				return (((u16)l) << 8) | h;
			}
		};

		struct BigEndianU32
		{
			union
			{
				u32		value;
				struct
				{
#if	LITTLE_ENDIAN
					u8 b0 : 8;
					u8 b1 : 8;
					u8 b2 : 8;
					u8 b3 : 8;
#else
					u8 b3 : 8;
					u8 b2 : 8;
					u8 b1 : 8;
					u8 b0 : 8;
#endif
				};
			};

			BigEndianU32(u32 value) : value(value) {}
			BigEndianU32(u8 ch3, u8 ch2, u8 ch1, u8 ch0)
#ifdef LITTLE_ENDIAN
				: b0(ch0), b1(ch1), b2(ch2), b3(ch3)
#else
				: b3(ch3), b2(ch2), b1(ch1), b0(ch0)
#endif
			{}

			static BigEndianU32 MakeFourCC(u8 ch0, u8 ch1, u8 ch2, u8 ch3)
			{
				return BigEndianU32(ch0, ch1, ch2, ch3);
			}

			bool operator==(const BigEndianU32& other) const
			{
				return other.value == value;
			}
			bool operator!=(const BigEndianU32& other) const
			{
				return other.value != value;
			}
		};

		bool CompressElevation(Image& img, const Variant& invalidValue);
		bool DecompressElevation(Image& img);

		void ConvertBigEndianToLocalEndianness(Image& img);
	}
}