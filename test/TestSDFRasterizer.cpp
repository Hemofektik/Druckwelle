
#include <cstring>
#include <ZFXMath.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <ogr_spatialref.h>
#include <omp.h>
#include <atomic>

#pragma warning (push)
#pragma warning (disable:4251)
#include <gdal_priv.h>
#include <ogr_api.h>
#include <ogr_geometry.h>
#include <ogrsf_frmts.h>
#pragma warning (pop)

#include <../src/dwcore.h>
#include <../src/WebMapService.h>
#include <../src/utils/Filesystem.h>
#include <../src/utils/ImageProcessor.h>

using namespace ZFXMath;
using namespace std;
using namespace std::chrono;
using namespace dw;

#define TestTag "TestSDFRasterizer - "

//#pragma optimize( "", off )
bool TestSDFRasterizer()
{
	GDALRegister_GTiff();

	const int Width = 3600;
	const int Height = 1800;
	Image img(Width, Height, DT_U8);

	map<std::string, WebMapService::Layer*> layers;
	layers["OSM_SDF"] = NULL;
	WebMapService::LayerFactory::CreateLayers(layers);

	auto osmSDFLayer = layers["OSM_SDF"];

	// write file to disk
	{
		u8* world = img.rawData;

		WebMapService::GetMapRequest gmr;
		gmr.styles = "";
		gmr.crs = "EPSG:4326";
		gmr.bbox.minX = -180.0;
		gmr.bbox.maxX =  180.0;
		gmr.bbox.minY = -90.0;
		gmr.bbox.maxY =  90.0;
		gmr.width = Width;
		gmr.height = Height;
		gmr.dataType = DT_U8;
		osmSDFLayer->HandleGetMapRequest(gmr, img);

		auto gtiffDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
		auto dataSet = gtiffDriver->Create("C:/dev/landWaterMask.tiff", Width, Height, 1, GDT_Byte, NULL);
		auto band = dataSet->GetRasterBand(1);
		int bX, bY;
		band->GetBlockSize(&bX, &bY);

		for (int y = 0; y < Height / bY; y++)
		{
			band->WriteBlock(0, y, world);
			world += Width * bY;
		}
		GDALClose(dataSet);
	}

	return true;
}
