#pragma once
#include "../dwcore.h"
#include <functional>
#include <memory>

namespace dw
{
	class Image
	{
		bool ownsRawDataPointer;
		bool ownsProcessedDataPointer;

	public:
		size rawDataSize;
		u8* rawData;
		
		int width;
		int height;
		size rawPixelSize;

		DataType rawDataType;

		u8* processedData;
		size processedDataSize;
		ContentType processedContentType;

		Image(int width, int height, DataType dataType);
		Image(int width, int height, DataType dataType, u8* rawData, bool ownsRawDataPointer);
		Image(u8* processedData, size processedDataSize, ContentType contentType, bool ownsProcessedDataPointer);
		~Image();

		void AllocateRawData(int width, int height, DataType dataType);
		void FreeRawData();
		void FreeProcessedData();
		bool SaveToPNG(const string& filename);

		void CopyFromSubImage(const Image& src, int targetX, int targetY);

		// srcType must be of same size as the DataType given in the ctor of this image
		// dstType must be either one or four bytes wide (greyscale or rgba)
		template <typename srcType, typename dstType>
		bool SaveToPNG(const string& filename, std::function<dstType(srcType)> convert);

		bool SaveProcessedDataToFile(const string& filename) const;
		
		static bool LoadContentFromFile(const string& filename, ContentType contentType, std::shared_ptr<Image>& imageOut);
	};

	namespace utils
	{
		bool ConvertRawImageToContentType(Image& image, ContentType contentType);
		bool ConvertContentTypeToRawImage(Image& image);
		void ExtendBoundingBoxForLanczos(BBox& asterBBox, double srcDegreesPerPixelX, double srcDegreesPerPixelY, double dstDegreesPerPixelX, double dstDegreesPerPixelY);

		struct SampleTransform
		{
			double scaleX;
			double scaleY;
			double offsetX;
			double offsetY;
		};

		void SampleWithLanczos(const Image& src, Image& dst, const SampleTransform& transform, const Variant& invalidValue = Variant());
		void SampleWithBoxFilter(const Image& src, Image& dst, const Variant& invalidValue = Variant());

		bool IsImageCompletelyInvalid(const Image& img, const Variant& invalidValue);
	}
}
