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
	}
}
