
#include "ImageProcessor.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace dw
{
	Image::Image(int width, int height, DataType dataType)
		: rawData(new u8[width * height * DataTypePixelSize[dataType]])
		, rawDataSize(width * height * DataTypePixelSize[dataType])
		, width(width)
		, height(height)
		, rawPixelSize(DataTypePixelSize[dataType])
		, rawDataType(dataType)
		, processedData(NULL)
		, processedDataSize(0)
		, processedContentType(CT_Unknown)
	{		
	}

	Image::~Image()
	{
		switch (processedContentType)
		{
		case dw::CT_Image_PNG:
			STBIW_FREE(processedData);
			break;
		case CT_Image_Raw_S16:
		case CT_Image_Raw_U8:
		case CT_Image_Raw_U32:
		case CT_Unknown:
		default:
			if (rawData != processedData)
			{
				delete[] processedData;
			}
			break;
		}

		delete[] rawData;
	}

	bool ConvertRawImageToContentType(Image& image, ContentType contentType)
	{
		image.processedContentType = contentType;

		switch (contentType)
		{
			case dw::CT_Image_PNG:
			{
				int dataSize = 0; // TODO: replace stb library by faster lib (e.g. turbo version of libpng)
				if (image.rawDataType == DT_RGBA8 || image.rawDataType == DT_U32)
				{
					image.processedData = stbi_write_png_to_mem((u8*)image.rawData, image.width * sizeof(u32), image.width, image.height, 4, &dataSize);
				}
				else if (image.rawDataType == DT_U8)
				{
					image.processedData = stbi_write_png_to_mem((u8*)image.rawData, image.width * sizeof(u8), image.width, image.height, 1, &dataSize);
				}
				image.processedDataSize = dataSize;
				return image.processedData != NULL;
			}

			// handle all raw formats in the same way (processed data points to raw data)
			case CT_Image_Raw_S16:
				if (contentType == CT_Image_Raw_S16 && image.rawDataType != DT_S16) return false;
			case CT_Image_Raw_U8:
				if (contentType == CT_Image_Raw_U8 && image.rawDataType != DT_U8) return false;
			case CT_Image_Raw_U32:
				if (contentType == CT_Image_Raw_U32 && image.rawDataType != DT_RGBA8 && image.rawDataType != DT_U32) return false;

				image.processedData = image.rawData;
				image.processedDataSize = image.rawDataSize;
				return true;
			case CT_Unknown:
			default:
				return false;
		}
	}
}