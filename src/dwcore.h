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
		"unknown",
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

	ContentType GetContentType(const string& contentTypeId);
	DataType FindCompatibleDataType(ContentType contentType, const std::vector<DataType>& availableDataTypes);
}