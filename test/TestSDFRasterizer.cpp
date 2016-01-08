
#include <cstring>
#include <ZFXMath.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <shapefil.h>
#include <ogr_spatialref.h>

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

	const int GridX = 360;
	const int GridY = 180;
	vector<SHPObject*>* shape = new vector<SHPObject*>[GridX * GridY];

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
	delete[] shape;

	OGRCoordinateTransformation::DestroyCT(transform);
	OGRSpatialReference::DestroySpatialReference(epsg3857);
	OGRSpatialReference::DestroySpatialReference(epsg4326);

	SHPClose(shpHandle);

	return true;
}
