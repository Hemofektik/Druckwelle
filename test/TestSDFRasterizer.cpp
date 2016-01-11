
#include <cstring>
#include <ZFXMath.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <shapefil.h>
#include <ogr_spatialref.h>
#include <LooseQuadtree.h>

#pragma warning (push)
#pragma warning (disable:4251)
#include <gdal_priv.h>
#pragma warning (pop)

#include <../src/dwcore.h>

using namespace ZFXMath;
using namespace std;
using namespace std::chrono;
using namespace loose_quadtree;
using namespace dw;

#define TestTag "TestSDFRasterizer - "


class BoundingBoxExtractor
{
public:
	static void ExtractBoundingBox(SHPObject* shape, BoundingBox<double>* bb)
	{
		bb->left = shape->dfXMin;
		bb->top = shape->dfYMin;
		bb->width = shape->dfXMax - shape->dfXMin;
		bb->height = shape->dfYMax - shape->dfYMin;
	}
};

bool TestSDFRasterizer()
{
	GDALRegister_GTiff();

	SHPHandle shpHandle = SHPOpen("../../test/coastline/processed_p.shp", "rb");
	if (!shpHandle)
	{
		printf(TestTag "Unable to open shape file\n");
		return false;
	}

	if (shpHandle->nShapeType != SHPT_POLYGON)
	{
		printf(TestTag "Invalid ShapeType: Shapefile does not contain POLYGONs\n");
		return false;
	}

	auto epsg4326 = (OGRSpatialReference*)OSRNewSpatialReference(NULL);
	auto epsg3857 = (OGRSpatialReference*)OSRNewSpatialReference(NULL);
	epsg4326->importFromEPSG(4326);
	epsg3857->importFromEPSG(3857);

	OGRCoordinateTransformation* transform = OGRCreateCoordinateTransformation(epsg3857, epsg4326);
	LooseQuadtree<double, SHPObject, BoundingBoxExtractor> qt;
	vector<SHPObject*> shapes(shpHandle->nRecords);

	for (int n = 0; n < shpHandle->nRecords; n++)
	{
		SHPObject* shape = SHPReadObject(shpHandle, n);

		bool success = (transform->Transform(shape->nVertices, shape->padfX, shape->padfY) == TRUE);
		if (!success)
		{
			printf(TestTag "Shape Coordinate Transform failed\n");
			return false;
		}

		SHPComputeExtents(shape);
		shapes[n] = shape;

		qt.Insert(shape);
	}

	const int Width = 3600;
	const int Height = 1800;
	u8* worldImg = new u8[Width * Height];

	// raster image
	{
		double cellSizeX = 360.0 / Width;
		double cellSizeY = 180.0 / Height;
		for (int y = 0; y < Height; y++)
		{
			u8* world = &worldImg[(Height - y - 1) * Width];

			for (int x = 0; x < Width; x++)
			{
				*world = 0;

				BoundingBox<double> bb(x * cellSizeX - 180.0, y * cellSizeY - 90.0, cellSizeX, cellSizeY);
				auto query = qt.QueryIntersectsRegion(bb);
				while (!query.EndOfQuery())
				{
					SHPObject* shape = query.GetCurrent();

					(*world) = 255;
				
					/*for (int part = 0; part < shape->nParts; part++)
					{
						int startIndex = shape->panPartStart[part];
					}*/

					query.Next();
				}

				world++;
			}
		}
	}


	// write file to disk
	{
		u8* world = worldImg;

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

	delete[] worldImg;

	const size_t numShapes = shapes.size();
	for (size_t n = 0; n < numShapes; n++)
	{
		SHPDestroyObject(shapes[n]);
	}
	shapes.clear();

	OGRCoordinateTransformation::DestroyCT(transform);
	OGRSpatialReference::DestroySpatialReference(epsg3857);
	OGRSpatialReference::DestroySpatialReference(epsg4326);

	SHPClose(shpHandle);

	return true;
}
