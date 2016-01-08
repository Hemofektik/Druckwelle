
#include "../src/utils/Elevation.h"

#include <cstring>
#include <ZFXMath.h>
#include <chrono>
#include <iostream>
#include <iomanip>

using namespace dw;
using namespace dw::utils;
using namespace ZFXMath;
using namespace std;
using namespace std::chrono;

#define TestTag "TestElevationCompression - "

bool TestElevationCompression()
{
	const int ImageSize = 2048;
	Image elevationImg(ImageSize, ImageSize, DT_S16);
	Image elevationImgVisual(ImageSize, ImageSize, DT_U8);

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

	if (!ConvertRawImageToContentType(elevationImg, CT_Image_Elevation))
	{
		printf(TestTag "Unable to compress elevation data\n");
		return false;
	}

	//elevationImgVisual.SaveToPNG("C:/Dev/temp/noise.png");
	//elevationImg.SaveProcessedDataToFile("C:/Dev/temp/noise.cem");

	Image decompressedElevationImg(elevationImg.processedData, elevationImg.processedDataSize, elevationImg.processedContentType, false);

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	if (!ConvertContentTypeToRawImage(decompressedElevationImg))
	{
		printf(TestTag "Unable to decompress elevation data\n");
		return false;
	}

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1) * 1000.0;

	std::cout << "Elevation Decompression was done within " << std::setprecision(5) << time_span.count() << " ms" << endl;

	if (decompressedElevationImg.rawDataSize != elevationImg.rawDataSize ||
		decompressedElevationImg.width != elevationImg.width ||
		decompressedElevationImg.height != elevationImg.height)
	{
		printf(TestTag "Decompressed elevation buffer size does not match source buffer size.\n");
		return false;
	}

	if (memcmp(decompressedElevationImg.rawData, elevationImg.rawData, elevationImg.rawDataSize) != 0)
	{
		printf(TestTag "Decompressed elevation data does not match source data.\n");
		return false;
	}

	return true;
}
