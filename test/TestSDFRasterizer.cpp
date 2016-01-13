
#include <cstring>
#include <ZFXMath.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <ogr_spatialref.h>
#include <LooseQuadtree.h>

#pragma warning (push)
#pragma warning (disable:4251)
#include <gdal_priv.h>
#include <ogr_api.h>
#include <ogr_geometry.h>
#include <ogrsf_frmts.h>
#pragma warning (pop)

#include <../src/dwcore.h>
#include <../src/utils/Filesystem.h>

using namespace ZFXMath;
using namespace std;
using namespace std::chrono;
using namespace loose_quadtree;
using namespace dw;

#define TestTag "TestSDFRasterizer - "


class BoundingBoxExtractor
{
public:
	static void ExtractBoundingBox(OGRFeature* feature, BoundingBox<double>* bb)
	{
		OGREnvelope aabb;
		feature->GetGeometryRef()->getEnvelope(&aabb);

		bb->left = aabb.MinX;
		bb->top = aabb.MinY;
		bb->width = aabb.MaxX - aabb.MinX;
		bb->height = aabb.MaxY - aabb.MinY;
	}
};


void SplitPolygons(const char* source_name, const char* dest_name);
bool TestSDFRasterizer()
{
	GDALRegister_GTiff();

	const char* coastlineShapeFileSrc = "../../test/coastline/processed_p.shp";
	const char* coastlineShapeFileDst = "../../test/coastline/processed_p_split.shp";

	if (!exists(coastlineShapeFileDst))
	{
		SplitPolygons(coastlineShapeFileSrc, coastlineShapeFileDst);
	}

	RegisterOGRShape();
	auto driver = OGRGetDriverByName("ESRI Shapefile");
	OGRDataSource* source = (OGRDataSource*)OGROpen(coastlineShapeFileDst, GA_ReadOnly, &driver);
	if (!source)
	{
		printf(TestTag "Unable to open shape file\n");
		return false;
	}

	OGRLayer* srcLayer = source->GetLayer(0);
	GIntBig numFeatures = srcLayer->GetFeatureCount();
	srcLayer->ResetReading();

	auto epsg4326 = (OGRSpatialReference*)OSRNewSpatialReference(NULL);
	auto epsg3857 = (OGRSpatialReference*)OSRNewSpatialReference(NULL);
	epsg4326->importFromEPSG(4326);
	epsg3857->importFromEPSG(3857);

	OGRCoordinateTransformation* transform = OGRCreateCoordinateTransformation(epsg3857, epsg4326);
	LooseQuadtree<double, OGRFeature, BoundingBoxExtractor> qt;
	vector<OGRFeature*> features(numFeatures);

	GIntBig featureIndex = 0;
	OGRFeature* feature;
	while ((feature = srcLayer->GetNextFeature()) != NULL) 
	{
		OGRGeometry* geometry = feature->GetGeometryRef();
		OGRErr error = geometry->transform(transform);

		if (error != OGRERR_NONE)
		{
			printf(TestTag "Shape Coordinate Transform failed\n");
			return false;
		}

		if (featureIndex % 10000 == 0)
		{
			std::cerr << "shapes transformed: " << featureIndex << " / " << numFeatures << "\r";
		}

		qt.Insert(feature);
		features[featureIndex] = feature;

		featureIndex++;
	}

	const int Width = 360;
	const int Height = 180;
	u8* worldImg = new u8[Width * Height];

	// raster image
	{
		const double DistanceDomain = 10.0;
		const double cellSize = 360.0 / Width;
		const double ScaledDistanceDomain = cellSize * DistanceDomain;

		for (int y = 0; y < Height; y++)
		{
			u8* world = &worldImg[(Height - y - 1) * Width];

			for (int x = 0; x < Width; x++)
			{
				*world = 0;

				BoundingBox<double> bb(
					x * cellSize - 180.0 - ScaledDistanceDomain,
					y * cellSize - 90.0 - ScaledDistanceDomain, cellSize + ScaledDistanceDomain, cellSize + ScaledDistanceDomain);
				double minDistance = numeric_limits<double>::max();

				auto pointGeo = (OGRPoint*)OGRGeometryFactory::createGeometry(wkbPoint);
				pointGeo->setX(bb.left + bb.width * 0.5);
				pointGeo->setY(bb.top + bb.height * 0.5);

				auto query = qt.QueryIntersectsRegion(bb);
				while (!query.EndOfQuery())
				{
					OGRFeature* feature = query.GetCurrent();

					double distance = feature->GetGeometryRef()->Distance(pointGeo);
					if (distance <= 0.0)
					{
						distance = 5.0;
						// TODO: compute signed distance
					}
					else
					{
						minDistance = min(minDistance, distance);
					}

					query.Next();
				}

				if (minDistance == numeric_limits<double>::max())
				{
					(*world) = 255;
				}
				else
				{
					(*world) = (u8)min(255.0, (255.0 * (minDistance / ScaledDistanceDomain)));
				}

				OGRGeometryFactory::destroyGeometry(pointGeo);

				world++;
			}

			std::cerr << "rasterized: " << (int)(100.0f * y / (float)Height) << "%s\r";
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

	const size_t numShapes = features.size();
	for (size_t n = 0; n < numShapes; n++)
	{
		OGRFeature::DestroyFeature(feature);
	}
	features.clear();

	OGRCoordinateTransformation::DestroyCT(transform);
	OGRSpatialReference::DestroySpatialReference(epsg3857);
	OGRSpatialReference::DestroySpatialReference(epsg4326);

	OGRDataSource::DestroyDataSource(source);

	return true;
}
