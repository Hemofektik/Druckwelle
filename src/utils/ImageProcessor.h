#pragma once
#include "../dwcore.h"
#include <functional>

namespace dw
{
	class Image
	{
	public:
		u8* rawData;
		size rawDataSize;
		
		int width;
		int height;
		size rawPixelSize;

		DataType rawDataType;
		
		u8* processedData;
		size processedDataSize;
		ContentType processedContentType;

		Image(int width, int height, DataType dataType);
		~Image();

		bool SaveToPNG(const astring& filename);

		// srcType must be of same size as the DataType given in the ctor of this image
		// dstType must be either one or four bytes wide (greyscale or rgba)
		template <typename srcType, typename dstType>
		bool SaveToPNG(const astring& filename, std::function<dstType(srcType)> convert);
	};

	namespace utils
	{
		bool ConvertRawImageToContentType(Image& image, ContentType contentType);
		void ExtendBoundingBoxForLanczos(BBox& asterBBox, double srcDegreesPerPixelX, double srcDegreesPerPixelY, double dstDegreesPerPixelX, double dstDegreesPerPixelY);

		struct SampleTransform
		{
			double scaleX;
			double scaleY;
			double offsetX;
			double offsetY;
		};

		struct InvalidValue
		{
			union 
			{
				u8		u8[8];
				u16		u16[4];
				s16		s16[4];
				u32		u32[2];
				float	f32[2];
				double	f64;
			} value;

			InvalidValue(u8 invalidValue)
			{
				value.u8[0] = invalidValue;
			}
			InvalidValue(u16 invalidValue)
			{
				value.u16[0] = invalidValue;
			}
			InvalidValue(s16 invalidValue)
			{
				value.s16[0] = invalidValue;
			}
			InvalidValue(u32 invalidValue)
			{
				value.u32[0] = invalidValue;
			}
			InvalidValue(f64 invalidValue)
			{
				value.f64 = invalidValue;
			}
		};

		void SampleWithLanczos(const Image& src, Image& dst, const SampleTransform& transform, const InvalidValue* invalidValue = NULL);
	}
}
