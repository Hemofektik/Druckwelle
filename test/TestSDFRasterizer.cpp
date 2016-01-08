
#include <cstring>
#include <ZFXMath.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <shapefil.h>
#include <ogr_spatialref.h>
#include <quadtree.hpp>

using namespace ZFXMath;
using namespace std;
using namespace std::chrono;

#define TestTag "TestSDFRasterizer - "

bool TestSDFRasterizer()
{
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

	Point<2> center;
	center[0] = 0.0;
	center[1] = 0.0;
	double halfRectWidth[2] = { 180.0, 90.0 };
	Rectangle<2> rect(center, halfRectWidth);

	vector<Point<2>> points;
	QuadTree<2> qt(rect, points);
	SHPObject** shapes = new SHPObject*[shpHandle->nRecords];

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



		/*for (int part = 0; part < shape->nParts; part++)
		{
			int startIndex = shape->panPartStart[part];
		}*/
	}

	//TODO: delete all shape objects ( SHPDestroyObject(shape) )
	delete[] shapes;

	OGRCoordinateTransformation::DestroyCT(transform);
	OGRSpatialReference::DestroySpatialReference(epsg3857);
	OGRSpatialReference::DestroySpatialReference(epsg4326);

	SHPClose(shpHandle);

	return true;
}
