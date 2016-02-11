
#include <ogr_api.h>
#include <ogr_spatialref.h>

#pragma warning (push)
#pragma warning (disable:4251)
#include <gdal_priv.h>
#include <ogr_api.h>
#include <ogr_geometry.h>
#include <ogrsf_frmts.h>
#pragma warning (pop)

#include <zlib.h>

#include <vector>
#include <ZFXMath.h>

#include "VectorTiles.h"
#include "vector_tile.pb.h"

using namespace std;
using namespace ZFXMath;

const int SEG_MOVETO = 1;
const int SEG_LINETO = 2;
const int SEG_CLOSE = 7;

int32_t ZigZagUint32ToInt32(uint32_t zigzag)
{
	return (int32_t)((zigzag & 1) ? -(int64_t)(zigzag >> 1) - 1 : (int64_t)(zigzag >> 1));
}

std::string decompress_gzip(const uint8_t* data, uint32_t dataSize)
{
	z_stream zs;                        // z_stream is zlib's control structure
	memset(&zs, 0, sizeof(zs));

	if (inflateInit2(&zs, MAX_WBITS + 16) != Z_OK)
	{
		return "";
	}

	zs.next_in = (Bytef*)data;
	zs.avail_in = dataSize;

	int ret;
	char outbuffer[32768];
	std::string outstring;

	// get the decompressed bytes blockwise using repeated calls to inflate
	do 
	{
		zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
		zs.avail_out = sizeof(outbuffer);

		ret = inflate(&zs, 0);

		if (outstring.size() < zs.total_out) 
		{
			outstring.append(outbuffer, zs.total_out - outstring.size());
		}
	} 
	while (ret == Z_OK);

	inflateEnd(&zs);

	if (ret != Z_STREAM_END) 
	{ 
		return "";// an error occurred that was not EOF
	}

	return outstring;
}

bool ParseTile(vector_tile::Tile& tile, GDALDataset* dataset)
{
	for (int l = 0; l < tile.layers_size(); l++)
	{
		vector_tile::Tile_Layer const& layer = tile.layers(l);
		const double tileScale = 1.0 / layer.extent();

		std::cout << layer.name() << ":\n";
		std::cout << "  version: " << layer.version() << "\n";
		std::cout << "  extent: " << layer.extent() << "\n";
		std::cout << "  features: " << static_cast<std::size_t>(layer.features_size()) << "\n";
		std::cout << "  keys: " << static_cast<std::size_t>(layer.keys_size()) << "\n";
		std::cout << "  values: " << static_cast<std::size_t>(layer.values_size()) << "\n";
		unsigned total_repeated = 0;
		unsigned num_commands = 0;
		unsigned num_move_to = 0;
		unsigned num_line_to = 0;
		unsigned num_close = 0;
		unsigned num_Points = 0;
		unsigned num_empty = 0;
		unsigned degenerate = 0;

		vector<TPolygon2D<double>> lineStrings;
		uint64_t lastFeatureId = 0;
		for (int featureIndex = 0; featureIndex < layer.features_size(); featureIndex++)
		{
			int32_t cursorX = 0;
			int32_t cursorY = 0;
			vector_tile::Tile_Feature const & f = layer.features(featureIndex);
			total_repeated += f.geometry_size();
			int cmd = -1;
			const int cmd_bits = 3;
			unsigned length = 0;
			unsigned g_length = 0;
			vector<TPolygon2D<int32_t>> polys;
			TPolygon2D<int32_t> poly;
			for (int k = 0; k < f.geometry_size();)
			{
				if (!length) {
					unsigned cmd_length = f.geometry(k++);
					cmd = cmd_length & ((1 << cmd_bits) - 1);
					length = cmd_length >> cmd_bits;
					if (length <= 0) num_empty++;
					num_commands++;
				}
				if (length > 0) {
					length--;
					if (cmd == SEG_MOVETO || cmd == SEG_LINETO)
					{
						uint32_t xZigZag = f.geometry(k++);
						uint32_t yZigZag = f.geometry(k++);
						int32_t xRel = ZigZagUint32ToInt32(xZigZag);
						int32_t yRel = ZigZagUint32ToInt32(yZigZag);
						cursorX += xRel;
						cursorY += yRel;

						g_length++;
						if (cmd == SEG_MOVETO)
						{
							if (poly.GetNumVertices() > 0)
							{
								polys.push_back(move(poly));
								poly = TPolygon2D<int32_t>();
								poly.ReserveNumVertices(10);
							}

							num_move_to++;
						}
						else if (cmd == SEG_LINETO)
						{
							num_line_to++;
						}

						poly.AddVertex(TVector2D<int32_t>(cursorX, cursorY));
					}
					else if (cmd == (SEG_CLOSE & ((1 << cmd_bits) - 1)))
					{
						if (g_length <= 2) degenerate++;
						g_length = 0;
						num_close++;

						polys.push_back(move(poly));
						poly = TPolygon2D<int32_t>();
						poly.ReserveNumVertices(10);
					}
					else
					{
						return false;
					}
				}
			}

			if (f.type() == vector_tile::Tile_GeomType_POLYGON)
			{
				size_t polyStartIndex = 0;
				for (size_t p = 0; p < polys.size(); p++)
				{
					int64_t polyArea = polys[p].ComputeArea<int64_t>();
					if (p > polyStartIndex && polyArea < 0) // test for multipolygons including interior polys
					{
						auto& poly = polys[polyStartIndex];
						const auto* nextPoly = &polys[polyStartIndex + 1];
						/*Polygon polygon(f.id(), tileScale, poly,
							(polyStartIndex + 1 < polys.size()) ? nextPoly : NULL,
							(uint32_t)(p - polyStartIndex - 1));
						polygons.push_back(move(polygon));*/

						polyStartIndex = p;
					}
				}
				if (polyStartIndex < polys.size())
				{
					/*Polygon polygon(f.id(), tileScale, polys[polyStartIndex],
						(polyStartIndex + 1 < polys.size()) ? &polys[polyStartIndex + 1] : NULL,
						(uint32_t)(polys.size() - 1 - polyStartIndex));
					polygons.push_back(move(polygon));*/
				}
			}
			else if (f.type() == vector_tile::Tile_GeomType_LINESTRING)
			{
				polys.push_back(move(poly));

				for (const auto& p : polys)
				{
					lineStrings.push_back(move(p.Clone<double>(tileScale)));
				}
			}
			else if (f.type() == vector_tile::Tile_GeomType_POINT)
			{
				num_Points += poly.GetNumVertices();
				//polys.push_back(move(poly));
			}
		}
		std::cout << "  geometry summary:\n";
		std::cout << "    total: " << total_repeated << "\n";
		std::cout << "    commands: " << num_commands << "\n";
		std::cout << "    move_to: " << num_move_to << "\n";
		std::cout << "    line_to: " << num_line_to << "\n";
		std::cout << "    close: " << num_close << "\n";
		std::cout << "    degenerate polygons: " << degenerate << "\n";
		std::cout << "    empty geoms: " << num_empty << "\n";
		std::cout << "    NUM LINE STRINGS: " << lineStrings.size() << "\n";
		std::cout << "    NUM POINTS: " << num_Points << "\n";
	}
	return true;
}


VectorTiles::VectorTiles(const char* path2mbtiles)
	: mbtiles(NULL)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	RegisterOGRShape();
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

	//google::protobuf::ShutdownProtobufLibrary();
}

GDALDataset* VectorTiles::Open(int zoomLevel, int x, int y)
{
	if (!mbtiles) return NULL;

	OGRLayer* layer = mbtiles->ExecuteSQL("SELECT tile_data FROM tiles WHERE zoom_level=1 AND tile_column=1 AND tile_row=1", NULL, "SQLITE");

	if (!layer || layer->GetFeatureCount() == 0)
	{
		mbtiles->ReleaseResultSet(layer);
		return NULL;
	}

	const auto feature = layer->GetNextFeature();
	int zippedPBFSize = 0;
	GByte* zippedPBF = feature->GetFieldAsBinary(0, &zippedPBFSize);
	if (!zippedPBF || !zippedPBFSize)
	{
		OGRFeature::DestroyFeature(feature);
		mbtiles->ReleaseResultSet(layer);
		return NULL;
	}

	string pbf = decompress_gzip(zippedPBF, zippedPBFSize);
	if (pbf.size() == 0)
	{
		OGRFeature::DestroyFeature(feature);
		mbtiles->ReleaseResultSet(layer);
		return NULL;
	}

	vector_tile::Tile tile;
	if (!tile.ParseFromArray(pbf.data(), (int)pbf.size()))
	{
		OGRFeature::DestroyFeature(feature);
		mbtiles->ReleaseResultSet(layer);
		return NULL;
	}

	auto driver = (GDALDriver*)OGRGetDriverByName("ESRI Shapefile");
	auto dataset = driver->Create("VectorTile", 0, 0, 0, GDT_Unknown, NULL);
	ParseTile(tile, dataset);

	OGRFeature::DestroyFeature(feature);
	mbtiles->ReleaseResultSet(layer);
	return dataset;
}

bool VectorTiles::IsOk() const
{
	return mbtiles != NULL;
}