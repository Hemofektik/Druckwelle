
#include <cstring>
#include <ZFXMath.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <ogr_spatialref.h>
#include <LooseQuadtree.h>
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
#include <../src/utils/Filesystem.h>

using namespace ZFXMath;
using namespace std;
using namespace std::chrono;
using namespace loose_quadtree;
using namespace dw;

#define TestTag "TestSDFRasterizer - "


class Polygon
{

public:
	GIntBig id;
	TPolygon2D<double>* polygons;
	int numPolygons;
	BoundingBox<double> aabb;

	Polygon(OGRFeature* src)
		: id(src->GetFID())
		, polygons(NULL)
		, numPolygons(0)
		, aabb(0, 0, 0, 0)
	{
		auto poly = (OGRPolygon*)src->GetGeometryRef();

		// get AABB
		{
			OGREnvelope env;
			poly->getEnvelope(&env);

			aabb.left = env.MinX;
			aabb.top = env.MinY;
			aabb.width = env.MaxX - env.MinX;
			aabb.height = env.MaxY - env.MinY;
		}

		// create outer perimeter
		{
			numPolygons = poly->getNumInteriorRings() + 1;
			polygons = new TPolygon2D<double>[numPolygons];

			auto ring = poly->getExteriorRing();
			polygons[0].SetNumVertices(ring->getNumPoints());
			auto vertices = polygons[0].GetVertices();
			ring->getPoints(&vertices[0].x, sizeof(TVector2D<double>), &vertices[0].y, sizeof(TVector2D<double>));
			polygons[0].CloseRing();
		}

		// create inner holes
		{
			for (int p = 0; p < poly->getNumInteriorRings(); p++)
			{
				auto ring = poly->getInteriorRing(p);
				polygons[p + 1].SetNumVertices(ring->getNumPoints());
				auto vertices = polygons[p + 1].GetVertices();
				ring->getPoints(&vertices[0].x, sizeof(TVector2D<double>), &vertices[0].y, sizeof(TVector2D<double>));
				polygons[p + 1].CloseRing();
			}
		}
	}

	double ComputeSqrDistanceToBoundingBox(double x, double y) const 
	{
		const double clampedX = min(max(x, aabb.left), aabb.left + aabb.width);
		const double clampedY = min(max(y, aabb.top), aabb.top + aabb.height);

		const double deltaX = clampedX - x;
		const double deltaY = clampedY - y;

		return deltaX * deltaX + deltaY * deltaY;
	}

	double ComputeSignedSquareDistance(double x, double y) const
	{
		bool pointIsRightOfEdge = false;
		double sqrDistance = polygons[0].ComputeSqrDistance(TVector2D<double>(x, y), pointIsRightOfEdge);
		return pointIsRightOfEdge ? -sqrDistance : sqrDistance;

		// TODO: test against interior polygons as well
	}

};

class BoundingBoxExtractor
{
public:
	static void ExtractBoundingBox(Polygon* poly, BoundingBox<double>* bb)
	{
		*bb = poly->aabb;
	}
};



//#pragma optimize( "", off )
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
	typedef LooseQuadtree<double, Polygon, BoundingBoxExtractor> LooseGeoQuadtree;
	LooseGeoQuadtree qt;
	vector<Polygon*> polygons(numFeatures);

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

		auto poly = new Polygon(feature);

		qt.Insert(poly);
		polygons[featureIndex] = poly;

		OGRFeature::DestroyFeature(feature);

		featureIndex++;
	}

	const int Width = 3600;
	const int Height = 1800;
	u8* worldImg = new u8[Width * Height];

	// raster image
	{
		const double DistanceDomain = 10.0;
		const double cellSize = 360.0 / Width;
		const double ScaledDistanceDomain = cellSize * DistanceDomain;

		omp_lock_t myLock;
		omp_init_lock(&myLock);
		atomic_int yProgress = 0;

		#pragma omp parallel for
		for (int y = 0; y < Height; y++)
		{
			u8* world = &worldImg[(Height - y - 1) * Width];

			for (int x = 0; x < Width; x++)
			{
				*world = 0;

				BoundingBox<double> bb(
					x * cellSize - 180.0 - ScaledDistanceDomain,
					y * cellSize - 90.0 - ScaledDistanceDomain, cellSize + ScaledDistanceDomain * 2.0, cellSize + ScaledDistanceDomain * 2.0);
				double minSqrDistance = numeric_limits<double>::max();

				auto pointGeo = (OGRPoint*)OGRGeometryFactory::createGeometry(wkbPoint);
				double px = bb.left + bb.width * 0.5;
				double py = bb.top + bb.height * 0.5;

				omp_set_lock(&myLock);
				LooseGeoQuadtree::Query query = qt.QueryIntersectsRegion(bb);
				omp_unset_lock(&myLock);

				while (!query.EndOfQuery())
				{
					Polygon* poly = query.GetCurrent();

					const double aabbDistanceSqr = poly->ComputeSqrDistanceToBoundingBox(px, py);
					if (aabbDistanceSqr < minSqrDistance) // test distance to AABB first before doing the expensive polygon distance test
					{
						double sqrDistance = poly->ComputeSignedSquareDistance(px, py);
						minSqrDistance = min(minSqrDistance, sqrDistance);
					}

					if (minSqrDistance < 0.0)
					{
						break;
					}

					query.Next();
				}

				if (minSqrDistance == numeric_limits<double>::max())
				{
					(*world) = 255;
				}
				else
				{
					double minDistance = Sign(minSqrDistance) * Sqrt(Abs(minSqrDistance));
					u8 worldValue = (u8)min(255.0, (255.0 * (minDistance * 0.5 / ScaledDistanceDomain + 0.5)));
					(*world) = worldValue;
				}

				OGRGeometryFactory::destroyGeometry(pointGeo);

				world++;
			}

			yProgress++;
			std::cerr << "rasterized: " << (int)(100.0f * yProgress / (float)Height) << "%s\r";
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

	const size_t numShapes = polygons.size();
	for (size_t n = 0; n < numShapes; n++)
	{
		delete polygons[n];
	}
	polygons.clear();

	OGRCoordinateTransformation::DestroyCT(transform);
	OGRSpatialReference::DestroySpatialReference(epsg3857);
	OGRSpatialReference::DestroySpatialReference(epsg4326);

	OGRDataSource::DestroyDataSource(source);

	return true;
}
