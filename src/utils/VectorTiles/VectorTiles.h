#pragma once

class GDALDataset;

class VectorTiles
{
private:
	GDALDataset* mbtiles;

public:

	VectorTiles(const char* path2mbtiles);
	~VectorTiles();

	GDALDataset* Open(int zoomLevel, int x, int y);
	bool IsOk() const;
};
