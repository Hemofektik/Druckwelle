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

	GDALDataset* Open(int zoomLevel, int x, int y);
	bool IsOk() const;
};
