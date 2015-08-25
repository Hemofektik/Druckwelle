#pragma once
#include "../dwcore.h"

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
	};

	namespace utils
	{
		bool ConvertRawImageToContentType(Image& image, ContentType contentType);
		void ExtendBoundingBoxForLanczos(BBox& asterBBox, double srcDegreesPerPixelX, double srcDegreesPerPixelY, double dstDegreesPerPixelX, double dstDegreesPerPixelY);
	}
}
