
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

#include "../src/dwcore.h"
#include "../src/WebMapService.h"
#include "../src/utils/Filesystem.h"
#include "../src/utils/ImageProcessor.h"

using namespace ZFXMath;
using namespace std;
using namespace std::chrono;
using namespace dw;
using namespace libconfig;

#define TestTag "TestSDFRasterizer - "

//#pragma optimize( "", off )
bool TestSDFRasterizer()
{
	GDALRegister_GTiff();

	const int Width = 3600;
	const int Height = 1600;
	Image img(Width, Height, DT_U8);

	Config cfg;
	cfg.readString("OSM_SDF = { CRS = [\"EPSG:3857\", \"EPSG:4326\"]; VectorTileFilePath = \"../../test/world_z0-z5.mbtiles\"; };");
	ChainedSetting config(cfg.getRoot());

	map<std::string, WebMapService::Layer*> layers;
	WebMapService::LayerFactory::CreateLayers(layers, config);

	auto osmSDFLayer = layers["OSM_SDF"];

	// write file to disk
	{
		u8* world = img.rawData;

		WebMapService::GetMapRequest gmr;
		gmr.styles = "";
		gmr.crs = "EPSG:4326";
		gmr.bbox.minX = -180.0;
		gmr.bbox.maxX =  180.0;
		gmr.bbox.minY = -80.0;
		gmr.bbox.maxY =  80.0;
		gmr.width = Width;
		gmr.height = Height;
		gmr.dataType = DT_U8;
		osmSDFLayer->HandleGetMapRequest(gmr, img);

		auto gtiffDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
		auto dataSet = gtiffDriver->Create("C:/dev/landWaterMask.tiff", Width, Height, 1, GDT_Byte, NULL);
		auto band = dataSet->GetRasterBand(1);
		int bX, bY;
		band->GetBlockSize(&bX, &bY);

		int numBlocks = (int)ceil(Height / (double)bY);
		for (int y = 0; y < numBlocks; y++)
		{
			band->WriteBlock(0, y, world);
			world += Width * bY;
		}
		GDALClose(dataSet);
	}

	delete osmSDFLayer;

	return true;
}
