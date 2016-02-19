
#include <vector>
#include <iostream>
#include <iomanip>
#include <memory>
#include <chrono>
#include <atomic>

#include <ogr_api.h>
#include <ogr_spatialref.h>

#pragma warning (push)
#pragma warning (disable:4251)
#include <gdal_priv.h>
#include <ogr_api.h>
#include <ogr_geometry.h>
#include <ogrsf_frmts.h>
#pragma warning (pop)
#include <cpl_vsi.h>

#include <omp.h>

#include <ZFXMath.h>

#include "../WebMapService.h"
#include "../utils/ImageProcessor.h"
#include "../utils/Filesystem.h"
#include "../utils/VectorTiles/VectorTiles.h"

#include <ogr_spatialref.h>
#include <LooseQuadtree.h>

using namespace std;
using namespace std::chrono;

using namespace ZFXMath;

using namespace loose_quadtree;
using namespace dw;
using namespace dw::utils;


class Polygon
{

public:
	GIntBig id;
	TPolygon2D<double>* polygons;
	int numPolygons;
	BoundingBox<double> aabb;

	Polygon(OGRPolygon* poly, GIntBig id)
		: id(id)
		, polygons(NULL)
		, numPolygons(0)
		, aabb(0, 0, 0, 0)
	{
		// get AABB
		{
			OGREnvelope env;
			poly->getEnvelope(&env);

			aabb.left = env.MinX;
			aabb.top = env.MinY;
			aabb.width = env.MaxX - env.MinX;
			aabb.height = env.MaxY - env.MinY;
		}

		// create outer perimeter
		{
			numPolygons = poly->getNumInteriorRings() + 1;
			polygons = new TPolygon2D<double>[numPolygons];

			auto ring = poly->getExteriorRing();
			polygons[0].SetNumVertices(ring->getNumPoints());
			auto vertices = polygons[0].GetVertices();
			ring->getPoints(&vertices[0].x, sizeof(TVector2D<double>), &vertices[0].y, sizeof(TVector2D<double>));
			polygons[0].CloseRing();

			double area = polygons[0].ComputeArea();
			assert(area >= 0.0);
		}

		// create inner holes
		{
			for (int p = 0; p < poly->getNumInteriorRings(); p++)
			{
				auto ring = poly->getInteriorRing(p);
				polygons[p + 1].SetNumVertices(ring->getNumPoints());
				auto vertices = polygons[p + 1].GetVertices();
				ring->getPoints(&vertices[0].x, sizeof(TVector2D<double>), &vertices[0].y, sizeof(TVector2D<double>));
				polygons[p + 1].CloseRing();

				double area = polygons[p + 1].ComputeArea();
				assert(area <= 0.0);
			}
		}
	}

	double ComputeSqrDistanceToBoundingBox(double x, double y) const
	{
		const double clampedX = min(max(x, aabb.left), aabb.left + aabb.width);
		const double clampedY = min(max(y, aabb.top), aabb.top + aabb.height);

		const double deltaX = clampedX - x;
		const double deltaY = clampedY - y;

		return deltaX * deltaX + deltaY * deltaY;
	}

	double ComputeSignedSquareDistance(double x, double y) const
	{
		bool pointIsRightOfEdge = false;
		TVector2D<double> point(x, y);
		double sqrDistance = polygons[0].ComputeSqrDistance(point, pointIsRightOfEdge);

		for (int p = 1; p < numPolygons; p++)
		{
			bool pointIsRightOfInnerEdge = false;
			double innerSqrDistance = polygons[p].ComputeSqrDistance(point, pointIsRightOfInnerEdge);
			if (innerSqrDistance < sqrDistance)
			{
				sqrDistance = innerSqrDistance;
				pointIsRightOfEdge = pointIsRightOfInnerEdge;
			}
		}

		return pointIsRightOfEdge ? sqrDistance : -sqrDistance;
	}
};

class BoundingBoxExtractor
{
public:
	static void ExtractBoundingBox(Polygon* poly, BoundingBox<double>* bb)
	{
		*bb = poly->aabb;
	}
};

typedef LooseQuadtree<double, Polygon, BoundingBoxExtractor> LooseGeoQuadtree;

namespace dw
{
	namespace Layers
	{
		const string LayerName = "OSM_SDF";
		const string LayerTitle = "Open Streetmap data as Signed Distance Field";
		const string LayerAbstract = "Rasterizes Open Streetmap data to signed distance fields.";

		class OSMSDF : public WebMapService::Layer
		{
			DECLARE_WEBMAPSERVICE_LAYER(OSMSDF, LayerName, LayerTitle);

			OGRSpatialReference* OSM_SpatRef;

			VectorTiles* vectorTiles;
		public:

			virtual bool Init(/* layerconfig */) override
			{
				OSM_SpatRef = supportedCRS["EPSG:3857"];

				if (!OSM_SpatRef)
				{
					cout << "OSMSDF: " << "CRS support for EPSG:3857 is mandatory" << endl;
					return false;
				}

				const char* worldVectorTiles = "../../test/world_z0-z5.mbtiles";
				vectorTiles = new VectorTiles(worldVectorTiles);
				if (!vectorTiles->IsOk()) return false;
				
				return true;
			}

			virtual const char* GetName() const override
			{
				return LayerName.c_str();
			}

			virtual const char_t* GetTitle() const override
			{
				return LayerTitle.c_str();
			}

			virtual const char_t* GetAbstract()  const override
			{
				return LayerAbstract.c_str();
			};

			virtual const vector<DataType>& GetSuppordetFormats() const override
			{
				static const vector<DataType> SuppordetFormats = { DT_U8 };
				return SuppordetFormats;
			}

			virtual HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, Image& img) override
			{
				auto crs = supportedCRS.find(gmr.crs);
				if (crs == supportedCRS.end())
				{
					return HGMRR_InvalidSRS;
				}

				switch (gmr.dataType)
				{
				case DT_U8:
				{
					return HandleGetMapRequest(gmr, crs->second, img);
				}
				default:
					return HGMRR_InvalidFormat;
				}
			}

		protected:

			OSMSDF()
				: OSM_SpatRef(NULL)
				, vectorTiles(NULL)
			{
			}

		private:

			virtual ~OSMSDF() override
			{
				delete vectorTiles;
			}

			HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, const OGRSpatialReference* requestSRS, Image& img)
			{
				assert(img.rawDataType == DT_U8);

				BBox osmBBox;
				if (!TransformBBox(gmr.bbox, osmBBox, requestSRS, OSM_SpatRef))
				{
					return HGMRR_InvalidBBox; // TODO: according to WMS specs bbox may lay outside of valid areas (e.g. latitudes greater than 90 degrees in CRS:84)
				}

				high_resolution_clock::time_point t1 = high_resolution_clock::now();

				HandleGetMapRequestResult result = RasterImg(gmr, osmBBox, requestSRS, img);

				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				duration<double> time_span = duration_cast<duration<double>>(t2 - t1) * 1000.0;
				std::cout << "OSM SDF computation took " << std::setprecision(5) << time_span.count() << " ms" << endl;

				return result;
			}

			struct QueryIntermediateData
			{
				LooseGeoQuadtree waterPolyQuadtree;
				vector<Polygon*> waterPolygons;
				OGRCoordinateTransformation* transform;

				QueryIntermediateData(OGRCoordinateTransformation* transform)
					: transform(transform)
				{
				}

				~QueryIntermediateData()
				{
					const size_t numShapes = waterPolygons.size();
					for (size_t n = 0; n < numShapes; n++)
					{
						delete waterPolygons[n];
					}
					waterPolygons.clear();
				}
			};

			bool LoadDataSets(
				const BBox& osmBBox,
				const int width,
				const int height,
				QueryIntermediateData& qid)
			{
				const double DesiredDataDensity = (256 / 1) / VectorTiles::MapWidth; // unit: (pixel / tile) / meter
				const double densityX = width / (osmBBox.maxX - osmBBox.minX);
				const double densityY = height / (osmBBox.maxY - osmBBox.minY);
				const double maxDensity = Max(densityX, densityY);

				const double numTilesDesired = maxDensity / DesiredDataDensity;
				const int zoomLevel = (int)ceil(log2(numTilesDesired));
				const int numTiles = 1 << zoomLevel;

				// TODO: Take distance DistanceDomain into account ensure all geometries 
				// outside of requested area but within DistanceDomain are loaded as well
				const int left = Max(0, (int)floor(numTiles * ((osmBBox.minX - VectorTiles::MapLeft) / VectorTiles::MapWidth)));
				const int right = Min(numTiles, 1 + (int)ceil(numTiles * ((osmBBox.maxX - VectorTiles::MapLeft) / VectorTiles::MapWidth)));
				const int bottom = Max(0, (int)floor(numTiles * ((osmBBox.minY - VectorTiles::MapBottom) / VectorTiles::MapHeight)));
				const int top = Min(numTiles, 1 + (int)ceil(numTiles * ((osmBBox.maxY - VectorTiles::MapBottom) / VectorTiles::MapHeight)));

				for (int y = bottom; y < top; y++)
				{
					for (int x = left; x < right; x++)
					{
						if (!LoadDataSet(zoomLevel, x, y, qid))
						{
							return false;
						}
					}
				}

				return true;
			}

			bool LoadDataSet(int zoomLevel, int x, int y, QueryIntermediateData& qid)
			{
				auto dataset = vectorTiles->Open(zoomLevel, x, y);
				if (!dataset) return true; // there may be missing tile datasets

				bool success = LoadShapes(dataset, qid);
				dataset->Release();

				return success;
			}

			HandleGetMapRequestResult RasterImg(
				const WebMapService::GetMapRequest& gmr,
				const BBox& osmBBox,
				const OGRSpatialReference* requestSRS, 
				Image& img)
			{
				const int Width = img.width;
				const int Height = img.height;

				const double DistanceDomain = 10.0;
				const double cellSize = (gmr.bbox.maxX - gmr.bbox.minX) / Width;
				const double ScaledDistanceDomain = cellSize * DistanceDomain;

				QueryIntermediateData qid(GetTransform(OSM_SpatRef, requestSRS));
				if (!LoadDataSets(osmBBox, Width, Height, qid))
				{
					return HGMRR_InternalError;
				}

				atomic_int yProgress(0);

				for (int y = 0; y < Height; y++)
				{
					u8* sdf = &img.rawData[(Height - y - 1) * Width];

					for (int x = 0; x < Width; x++)
					{
						*sdf = 0;

						BoundingBox<double> bb( 
							gmr.bbox.minX + x * cellSize - ScaledDistanceDomain,
							gmr.bbox.minY + y * cellSize - ScaledDistanceDomain, 
							cellSize + ScaledDistanceDomain * 2.0, 
							cellSize + ScaledDistanceDomain * 2.0);
						double minSqrDistance = numeric_limits<double>::max();

						double px = bb.left + bb.width * 0.5;
						double py = bb.top + bb.height * 0.5;

						LooseGeoQuadtree::Query query = qid.waterPolyQuadtree.QueryIntersectsRegion(bb);
						
						while (!query.EndOfQuery())
						{
							Polygon* poly = query.GetCurrent();

							// TODO: support perfect precision mode:
							// reproject polygon into azimuthal equidistant projection 
							// around requested pixel location before computing the distance

							const double aabbDistanceSqr = poly->ComputeSqrDistanceToBoundingBox(px, py);
							if (aabbDistanceSqr < minSqrDistance) // test distance to AABB first before doing the expensive polygon distance test
							{
								double sqrDistance = poly->ComputeSignedSquareDistance(px, py);
								minSqrDistance = Min(minSqrDistance, sqrDistance);
							}

							if (minSqrDistance < 0.0)
							{
								// TODO: merge polygon with surrounding polygons if their id matches
								// do distance check again to get correct distance

								break;
							}

							query.Next();
						}
						while (!query.EndOfQuery()) // we have to finish a query to release it from the quadtree
						{
							query.Next();
						}

						if (minSqrDistance == numeric_limits<double>::max())
						{
							(*sdf) = 255;
						}
						else
						{
							double minDistance = Sign(minSqrDistance) * Sqrt(Abs(minSqrDistance));
							u8 sdfValue = (u8)Clamp(255.0 * (minDistance * 0.5 / ScaledDistanceDomain + 0.5), 0.0, 255.0);
							(*sdf) = sdfValue;
						}

						sdf++;
					}

					yProgress++;
					cout << "rasterized: " << (int)(100.0f * yProgress / (float)Height) << "%s\r";
				}

				return HGMRR_OK;
			}

			bool LoadShapes(GDALDataset* dataset, QueryIntermediateData& qid)
			{
				OGRLayer* waterLayer = dataset->GetLayerByName("water");
				if (!waterLayer) return true; // requested layer may not be present in requested dataset

				waterLayer->ResetReading();

				OGRFeature* feature;
				while ((feature = waterLayer->GetNextFeature()) != NULL)
				{
					auto geo = feature->GetGeometryRef();
					auto featureId = feature->GetFID();

					if (qid.transform)
					{
						if (geo->transform(qid.transform) != OGRERR_NONE)
						{
							continue;
						}
					}

					if (geo->getGeometryType() == wkbMultiPolygon)
					{
						auto mp = (OGRMultiPolygon*)geo;
						const int numPolys = mp->getNumGeometries();
						for (int p = 0; p < numPolys; p++)
						{
							auto poly = new Polygon((OGRPolygon*)mp->getGeometryRef(p), featureId);

							qid.waterPolyQuadtree.Insert(poly);
							qid.waterPolygons.push_back(poly);
						}
					}
					else if (geo->getGeometryType() == wkbPolygon)
					{
						auto poly = new Polygon((OGRPolygon*)geo, featureId);

						qid.waterPolyQuadtree.Insert(poly);
						qid.waterPolygons.push_back(poly);
					}

					OGRFeature::DestroyFeature(feature);
				}

				return true;
			}
		};

		IMPLEMENT_WEBMAPSERVICE_LAYER(OSMSDF, LayerName, LayerTitle);
	}
}
