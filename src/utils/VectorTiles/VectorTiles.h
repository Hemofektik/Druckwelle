#pragma once

class OGRDataSource;



class VectorTiles
{
private:
	GDALDataset* mbtiles;

public:

	VectorTiles(const char* path2mbtiles);
	~VectorTiles();

	GDALDataset* OpenVectorTile(int zoomLevel, int x, int y);
	bool IsOk() const;
};
