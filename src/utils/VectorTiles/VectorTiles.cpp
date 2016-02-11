
#include <ogr_api.h>
#include <ogr_spatialref.h>

#pragma warning (push)
#pragma warning (disable:4251)
#include <gdal_priv.h>
#include <ogr_api.h>
#include <ogr_geometry.h>
#include <ogrsf_frmts.h>
#pragma warning (pop)

#include "VectorTiles.h"
#include "vector_tile.pb.h"





VectorTiles::VectorTiles(const char* path2mbtiles)
	: mbtiles(NULL)
{
	RegisterOGRSQLite();
	auto driver = OGRGetDriverByName("SQLite");
	mbtiles = (GDALDataset*)OGROpen(path2mbtiles, GA_ReadOnly, &driver);
}

VectorTiles::~VectorTiles()
{
	if (mbtiles)
	{
		mbtiles->Release();
	}
}

GDALDataset* VectorTiles::OpenVectorTile(int zoomLevel, int x, int y)
{
	if (!mbtiles) return NULL;

	OGRLayer* layer = mbtiles->ExecuteSQL("SELECT tile_data FROM tiles WHERE zoom_level=0 AND tile_column=0 AND tile_row=0", NULL, "SQLITE");

	if (layer->GetFeatureCount() > 0)
	{
		const auto feature = layer->GetNextFeature();
		int dataSize = 0;
		GByte* data = feature->GetFieldAsBinary(0, &dataSize);

		// read vector tile (mvt/pbf)
	}

	mbtiles->ReleaseResultSet(layer);
	return NULL;
}

bool VectorTiles::IsOk() const
{
	return mbtiles != NULL;
}