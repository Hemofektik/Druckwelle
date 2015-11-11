
#include "../src/utils/Elevation.h"

using namespace dw;
using namespace dw::utils;

bool TestElevationCompression()
{
	Image elevationImg(128, 128, DT_S16);

	ConvertRawImageToContentType(elevationImg, CT_Image_Elevation);

	// TODO: create img with processed elevationImg data and decompress it
	// TODO: compare decompressed data with raw data

	return false;
}
