
#include <cstring>
#include <ZFXMath.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <shapefil.h>
#include <ogr_spatialref.h>
#include "../src/utils/Quadtree.h"

using namespace ZFXMath;
using namespace std;
using namespace std::chrono;

#define TestTag "TestSDFRasterizer - "


class QuadtreeShape : public Quadtree<double>::Object
{
public:

	SHPObject* shape;

	QuadtreeShape(SHPObject* shape)
		: Quadtree<double>::Object(shape->dfXMin, shape->dfYMin, shape->dfXMax - shape->dfXMin, shape->dfYMax - shape->dfYMin)
		, shape(shape)
	{
	}

	~QuadtreeShape()
	{
		SHPDestroyObject(shape);
	}
};

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
	Quadtree<double> qt(-180.0, -90.0, 360.0, 180.0, 9);
	vector<QuadtreeShape*> shapes(shpHandle->nRecords);

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
		auto newQTShape = new QuadtreeShape(shape);
		shapes[n] = newQTShape;

		qt.AddObject(newQTShape);

		/*for (int part = 0; part < shape->nParts; part++)
		{
			int startIndex = shape->panPartStart[part];
		}*/
	}


	vector<Quadtree<double>::Object*> intersectingObjects;
	qt.GetObjectsAt(13.2, 52.0, intersectingObjects);

	int numElements = intersectingObjects.size();


	//TODO: delete all shape objects within shapes

	OGRCoordinateTransformation::DestroyCT(transform);
	OGRSpatialReference::DestroySpatialReference(epsg3857);
	OGRSpatialReference::DestroySpatialReference(epsg4326);

	SHPClose(shpHandle);

	return true;
}
