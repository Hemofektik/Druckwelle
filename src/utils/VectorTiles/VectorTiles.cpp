
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

void AddFielDefn(OGRFeatureDefn* featureDefn, const vector_tile::Tile_Layer& layer, uint32_t keyIndex, uint32_t valueIndex)
{
	const auto& key = layer.keys(keyIndex);
	const auto& value = layer.values(valueIndex);

	if (value.has_string_value())
	{
		featureDefn->AddFieldDefn(new OGRFieldDefn(key.c_str(), OFTString));
	}
	else if (value.has_float_value())
	{
		auto field = new OGRFieldDefn(key.c_str(), OFTReal);
		field->SetSubType(OFSTFloat32);
		featureDefn->AddFieldDefn(field);
	}
	else if (value.has_double_value())
	{
		auto field = new OGRFieldDefn(key.c_str(), OFTReal);
		featureDefn->AddFieldDefn(field);
	}
	else if (value.has_int_value())
	{
		auto field = new OGRFieldDefn(key.c_str(), OFTInteger64);
		featureDefn->AddFieldDefn(field);
	}
	else if (value.has_uint_value())
	{
		auto field = new OGRFieldDefn(key.c_str(), OFTInteger64);
		featureDefn->AddFieldDefn(field);
	}
	else if (value.has_sint_value())
	{
		auto field = new OGRFieldDefn(key.c_str(), OFTInteger64);
		featureDefn->AddFieldDefn(field);
	}
	else if (value.has_bool_value())
	{
		auto field = new OGRFieldDefn(key.c_str(), OFTInteger);
		field->SetSubType(OFSTBoolean);
		featureDefn->AddFieldDefn(field);
	}
}

void SetFeatureField(OGRFeature* feature, const vector_tile::Tile_Layer& layer, uint32_t keyIndex, uint32_t valueIndex)
{
	const auto& key = layer.keys(keyIndex);
	const auto& value = layer.values(valueIndex);
	
	if (value.has_string_value())
	{
		feature->SetField(key.c_str(), value.string_value().c_str());
	}
	else if (value.has_float_value())
	{
		feature->SetField(key.c_str(), (double)value.float_value());
	}
	else if (value.has_double_value())
	{
		feature->SetField(key.c_str(), (double)value.double_value());
	}
	else if (value.has_int_value())
	{
		feature->SetField(key.c_str(), (GIntBig)value.int_value());
	}
	else if (value.has_uint_value())
	{
		feature->SetField(key.c_str(), (GIntBig)value.uint_value());
	}
	else if (value.has_sint_value())
	{
		feature->SetField(key.c_str(), (GIntBig)value.sint_value());
	}
	else if (value.has_bool_value())
	{
		feature->SetField(key.c_str(), (int)(value.bool_value() ? 1 : 0));
	}
}

bool ConvertVectorTileToOGRDataset(
	vector_tile::Tile& tile,
	GDALDataset* dataset,
	OGRSpatialReference* webmercator,
	int zoomLevel, int x, int y)
{
	// transform integer vector tile coordinates into web mercator
	int numTiles = 1 << zoomLevel;
	double tileSize = 20037508.34 * 2.0 / (double)numTiles;
	double left = -20037508.34 + x * tileSize;
	double top = 20037508.34 - y * tileSize;

	for (int l = 0; l < tile.layers_size(); l++)
	{
		vector_tile::Tile_Layer const& srcLayer = tile.layers(l);
		const double tileScale = tileSize / srcLayer.extent();

		auto dstLayer = dataset->GetLayerByName(srcLayer.name().c_str());
		if (!dstLayer)
		{
			dstLayer = dataset->CreateLayer(srcLayer.name().c_str(), webmercator);
		}
		//dstLayer->SetMetadataItem("version", to_string(srcLayer.version()).c_str());

		vector<TPolygon2D<double>> lineStrings;
		for (int featureIndex = 0; featureIndex < srcLayer.features_size(); featureIndex++)
		{
			int32_t cursorX = 0;
			int32_t cursorY = 0;
			const vector_tile::Tile_Feature& srcFeature = srcLayer.features(featureIndex);

			int cmd = -1;
			const int cmd_bits = 3;
			unsigned int length = 0;
			vector<TPolygon2D<int32_t>> vtPolys;
			TPolygon2D<int32_t> vtPoly;
			for (int k = 0; k < srcFeature.geometry_size();)
			{
				if (!length) 
				{
					unsigned cmd_length = srcFeature.geometry(k++);
					cmd = cmd_length & ((1 << cmd_bits) - 1);
					length = cmd_length >> cmd_bits;
				}
				if (length > 0) 
				{
					length--;
					if (cmd == SEG_MOVETO || cmd == SEG_LINETO)
					{
						uint32_t xZigZag = srcFeature.geometry(k++);
						uint32_t yZigZag = srcFeature.geometry(k++);
						int32_t xRel = ZigZagUint32ToInt32(xZigZag);
						int32_t yRel = ZigZagUint32ToInt32(yZigZag);
						cursorX += xRel;
						cursorY += yRel;

						if (cmd == SEG_MOVETO)
						{
							if (vtPoly.GetNumVertices() > 0)
							{
								vtPolys.push_back(move(vtPoly));
								vtPoly = TPolygon2D<int32_t>();
								vtPoly.ReserveNumVertices(10);
							}
						}
						else if (cmd == SEG_LINETO)
						{
						}

						vtPoly.AddVertex(TVector2D<int32_t>(cursorX, cursorY));
					}
					else if (cmd == (SEG_CLOSE & ((1 << cmd_bits) - 1)))
					{					
						vtPolys.push_back(move(vtPoly));
						vtPoly = TPolygon2D<int32_t>();
						vtPoly.ReserveNumVertices(10);
					}
					else
					{
						return false; // unknown command
					}
				}
			}

			auto featureDefn = new OGRFeatureDefn();
			for (int t = 0; t < srcFeature.tags_size(); t += 2)
			{
				uint32_t keyIndex = srcFeature.tags(t + 0);
				uint32_t valueIndex = srcFeature.tags(t + 1);

				AddFielDefn(featureDefn, srcLayer, keyIndex, valueIndex);
			}

			auto dstFeature = new OGRFeature(featureDefn);
			dstFeature->SetFID(srcFeature.id());

			for (int t = 0; t < srcFeature.tags_size(); t += 2)
			{
				uint32_t keyIndex = srcFeature.tags(t + 0);
				uint32_t valueIndex = srcFeature.tags(t + 1);

				SetFeatureField(dstFeature, srcLayer, keyIndex, valueIndex);
			}

			if (srcFeature.type() == vector_tile::Tile_GeomType_POLYGON)
			{
				size_t polyStartIndex = 0;
				for (size_t p = 0; p < vtPolys.size(); p++)
				{
					int64_t polyArea = vtPolys[p].ComputeArea<int64_t>();
					if (p > polyStartIndex && polyArea < 0) // test for multipolygons including interior polys
					{
						auto& poly = vtPolys[polyStartIndex];
						const auto* nextPoly = &vtPolys[polyStartIndex + 1];
						/*Polygon polygon(f.id(), tileScale, poly,
							(polyStartIndex + 1 < polys.size()) ? nextPoly : NULL,
							(uint32_t)(p - polyStartIndex - 1));
						polygons.push_back(move(polygon));*/

						polyStartIndex = p;
					}
				}
				if (polyStartIndex < vtPolys.size())
				{
					/*Polygon polygon(f.id(), tileScale, polys[polyStartIndex],
						(polyStartIndex + 1 < polys.size()) ? &polys[polyStartIndex + 1] : NULL,
						(uint32_t)(polys.size() - 1 - polyStartIndex));
					polygons.push_back(move(polygon));*/
				}
			}
			else if (srcFeature.type() == vector_tile::Tile_GeomType_LINESTRING)
			{
				vtPolys.push_back(move(vtPoly));

				for (const auto& vtp : vtPolys)
				{
					lineStrings.push_back(move(vtp.Clone<double>(tileScale)));
				}
			}
			else if (srcFeature.type() == vector_tile::Tile_GeomType_POINT)
			{
				//polys.push_back(move(poly));
			}

			//dstFeature->SetGeometryDirectly(  )

			if (dstLayer->CreateFeature(dstFeature))
			{
				return false;
			}
		}
	}
	return true;
}


VectorTiles::VectorTiles(const char* path2mbtiles)
	: mbtiles(NULL)
	, webMercator(new OGRSpatialReference("EPSG:3857"))
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

	webMercator->Release();

	//google::protobuf::ShutdownProtobufLibrary();
}

struct IntermediateQueryData
{
	GDALDataset* dataset;
	OGRLayer* layer;
	OGRFeature* feature;

	IntermediateQueryData(GDALDataset* dataset)
		: dataset(dataset)
		, layer(NULL)
		, feature(NULL)
	{
	}

	~IntermediateQueryData()
	{
		if (feature) OGRFeature::DestroyFeature(feature);
		if (layer) dataset->ReleaseResultSet(layer);
	}
};

GDALDataset* VectorTiles::Open(int zoomLevel, int x, int y)
{
	if (!mbtiles) return NULL;

	string pbf;
	{
		IntermediateQueryData qd(mbtiles);

		string query =	"SELECT tile_data FROM tiles WHERE zoom_level=" + to_string(zoomLevel) + 
						" AND tile_column=" + to_string(x) + 
						" AND tile_row=" + to_string(y);
		qd.layer = mbtiles->ExecuteSQL(query.c_str(), NULL, "SQLITE");

		if (!qd.layer || qd.layer->GetFeatureCount() == 0)
		{
			return NULL;
		}

		qd.feature = qd.layer->GetNextFeature();
		int zippedPBFSize = 0;
		GByte* zippedPBF = qd.feature->GetFieldAsBinary(0, &zippedPBFSize);
		if (!zippedPBF || !zippedPBFSize)
		{
			return NULL;
		}

		pbf = decompress_gzip(zippedPBF, zippedPBFSize);
		if (pbf.size() == 0)
		{
			return NULL;
		}
	}

	vector_tile::Tile tile;
	if (!tile.ParseFromArray(pbf.data(), (int)pbf.size()))
	{
		return NULL;
	}

	auto driver = (GDALDriver*)OGRGetDriverByName("ESRI Shapefile");
	auto dataset = driver->Create("VectorTile", 0, 0, 0, GDT_Unknown, NULL);
	if (!ConvertVectorTileToOGRDataset(tile, dataset, webMercator, zoomLevel, x, y))
	{
		return NULL;
	}

	return dataset;
}

bool VectorTiles::IsOk() const
{
	return mbtiles != NULL;
}