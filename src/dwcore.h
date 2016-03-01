#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <cstdlib>


namespace dw
{
	typedef char			char_t;

	typedef std::string		string; // used to store UTF-8 strings

	typedef int8_t			s8;
	typedef int16_t			s16;
	typedef int32_t			s32;
	typedef int64_t			s64;
	typedef uint8_t			u8;
	typedef uint16_t		u16;
	typedef uint32_t		u32;
	typedef uint64_t		u64;

	typedef std::size_t		size;

	typedef float			f32;
	typedef double			f64;

	enum ContentType
	{
		CT_Unknown,
		CT_Image_PNG,
		CT_Image_Raw_S16,
		CT_Image_Raw_U8,
		CT_Image_Raw_U32,
		CT_Image_Raw_F32,
		CT_Image_Raw_F64,
		CT_Image_Elevation,

		CT_NumContentTypes // must be last entry
	};

	const string ContentTypeId[] =
	{
		"application/octet-stream",
		"image/png",
		"application/raw-s16",
		"application/raw-u8",
		"application/raw-u32",
		"application/raw-f32",
		"application/raw-f64",
		"application/elevation",
	};

	enum DataType
	{
		DT_Unknown,
		DT_RGBA8,
		DT_U8,
		DT_S16,
		DT_U32,
		DT_F32,
		DT_F64,

		DT_NumDataTypes // must be last entry
	};

	const size_t DataTypePixelSize[] =
	{
		0,
		sizeof(u32),
		sizeof(u8),
		sizeof(s16),
		sizeof(u32),
		sizeof(f32),
		sizeof(f64),
	};

	enum ServiceType
	{
		WMS,
		WMTS
	};

	struct BBox
	{
		double minX;
		double minY;
		double maxX;
		double maxY;

		double GetWidth() const { return maxX - minX; }
		double GetHeight() const { return maxY - minY; }
	};

	struct Variant
	{
	public:
		union Value
		{
			u8		uint8[8];
			s16		sint16[4];
			u32		uint32[2];
			f32		float32[2];
			f64		float64;
		};

	private:
		Value value;
		bool isSet;
		DataType dataType;

	public:

		Variant()
		{
			value.float64 = 0.0;
			isSet = false;
			dataType = DT_Unknown;
		}
		Variant(u8 invalidValue)
		{
			value.uint8[0] = invalidValue;
			isSet = true;
			dataType = DT_U8;
		}
		Variant(s16 invalidValue)
		{
			value.sint16[0] = invalidValue;
			isSet = true;
			dataType = DT_S16;
		}
		Variant(u32 invalidValue)
		{
			value.uint32[0] = invalidValue;
			isSet = true;
			dataType = DT_U32;
		}
		Variant(f32 invalidValue)
		{
			value.float32[0] = invalidValue;
			isSet = true;
			dataType = DT_F32;
		}
		Variant(f64 invalidValue)
		{
			value.float64 = invalidValue;
			isSet = true;
			dataType = DT_F64;
		}

		const Value GetValue() const
		{
			return value;
		}

		const bool IsSet() const
		{
			return isSet;
		}

		const DataType GetDataType() const
		{
			return dataType;
		}
	};

	ContentType GetContentType(const string& contentTypeId);
	DataType FindCompatibleDataType(ContentType contentType, const std::vector<DataType>& availableDataTypes);

	template<typename T>
	void SetTypedMemory(T* dest, T value, size count)
    {
    	const T* end = dest + count;
    	while(dest < end)
    	{
    		*dest = value;
    		dest++;
    	}
    }

	void SetTypedMemory(void* dest, const Variant& value, size count);
}
