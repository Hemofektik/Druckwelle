#pragma once

class GDALDataset;
class OGRSpatialReference;

class VectorTiles
{
private:
	GDALDataset* mbtiles;
	OGRSpatialReference* webMercator;

public:

	VectorTiles(const char* path2mbtiles);
	~VectorTiles();

	// Returns a dataset containing the requested vector tiles 
	// as OGR Dataset in Web Mercator Projection 3857.
	// Caller owns the dataset.
	GDALDataset* Open(int zoomLevel, int x, int y);

	// Indicates whether constructor did run through without any issues
	bool IsOk() const;
};
