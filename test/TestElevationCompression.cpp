
#include "../src/utils/Elevation.h"

#include <cstring>
#include <ZFXMath.h>

using namespace dw;
using namespace dw::utils;
using namespace ZFXMath;

#define TestTag "TestElevationCompression - "

bool TestElevationCompression()
{
	Image elevationImg(128, 128, DT_S16);
	Image elevationImgVisual(128, 128, DT_U8);

	s16* data = (s16*)elevationImg.rawData;
	u8* dataVisual = elevationImgVisual.rawData;
	for (int y = 0; y < elevationImgVisual.height; y++)
	{
		for (int x = 0; x < elevationImgVisual.width; x++)
		{
			double noise = InterpolatedNoise(x * 0.2, y * 0.2);
			*data = (s16)(noise * 500.0 + 500.0);
			*dataVisual = (u8)(noise * 127.5 + 127.5);
			data++;
			dataVisual++;
		}
	}

	elevationImgVisual.SaveToPNG("C:/Dev/temp/noise.png");

	if (!ConvertRawImageToContentType(elevationImg, CT_Image_Elevation))
	{
		printf(TestTag "Unable to compress elevation data\n");
		return false;
	}

	Image compressedElevationImg(elevationImg.processedData, elevationImg.processedDataSize, elevationImg.processedContentType, false);

	if (!ConvertContentTypeToRawImage(compressedElevationImg))
	{
		printf(TestTag "Unable to decompress elevation data\n");
		return false;
	}

	if (compressedElevationImg.rawDataSize != elevationImg.rawDataSize ||
		compressedElevationImg.width != elevationImg.width ||
		compressedElevationImg.height != elevationImg.height)
	{
		printf(TestTag "Decompressed elevation buffer size does not match source buffer size.\n");
		return false;
	}

	if (memcmp(compressedElevationImg.rawData, elevationImg.rawData, elevationImg.rawDataSize) != 0)
	{
		printf(TestTag "Decompressed elevation data does not match source data.\n");
		return false;
	}

	return false;
}
