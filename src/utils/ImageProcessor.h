#pragma once
#include "../dwcore.h"
#include <functional>

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

		void FreeRawData();
		void FreeProcessedData();
		bool SaveToPNG(const string& filename);

		// srcType must be of same size as the DataType given in the ctor of this image
		// dstType must be either one or four bytes wide (greyscale or rgba)
		template <typename srcType, typename dstType>
		bool SaveToPNG(const string& filename, std::function<dstType(srcType)> convert);

		bool SaveProcessedDataToFile(const string& filename) const;
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

		struct InvalidValue
		{
		public:
			union Value
			{
				u8		uint8[8];
				u16		uint16[4];
				s16		sint16[4];
				u32		uint32[2];
				f32		float32[2];
				f64		float64;
			};


		private:
			Value value;
			bool isSet;

		public:

			InvalidValue()
			{
				value.float64 = 0.0;
				isSet = false;
			}
			InvalidValue(u8 invalidValue)
			{
				value.uint8[0] = invalidValue;
				isSet = true;
			}
			InvalidValue(u16 invalidValue)
			{
				value.uint16[0] = invalidValue;
				isSet = true;
			}
			InvalidValue(s16 invalidValue)
			{
				value.sint16[0] = invalidValue;
				isSet = true;
			}
			InvalidValue(u32 invalidValue)
			{
				value.uint32[0] = invalidValue;
				isSet = true;
			}
			InvalidValue(f32 invalidValue)
			{
				value.float32[0] = invalidValue;
				isSet = true;
			}
			InvalidValue(f64 invalidValue)
			{
				value.float64 = invalidValue;
				isSet = true;
			}

			const Value GetValue() const
			{
				return value;
			}

			const bool IsSet() const
			{
				return isSet;
			}
		};

		void SampleWithLanczos(const Image& src, Image& dst, const SampleTransform& transform, const InvalidValue& invalidValue = InvalidValue());
		bool IsImageCompletelyInvalid(const Image& img, const InvalidValue& invalidValue);
	}
}
