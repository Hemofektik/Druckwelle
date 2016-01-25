
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

	Polygon(OGRFeature* src)
		: id(src->GetFID())
		, polygons(NULL)
		, numPolygons(0)
		, aabb(0, 0, 0, 0)
	{
		auto poly = (OGRPolygon*)src->GetGeometryRef();

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

		return pointIsRightOfEdge ? -sqrDistance : sqrDistance;
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
void SplitPolygons(const char* source_name, const char* dest_name);

namespace dw
{
	namespace Layers
	{
		const string LayerName = "OSM_SDF";
		const string LayerTitle = "Open Streetmap data as Signed Distance Field";
		const string LayerAbstract = "Rasterizes Open Streetmap data to signed distance fields.";

		class OSMSDF : public WebMapService::Layer
		{
			OGRSpatialReference* OSM_SpatRef;

			LooseGeoQuadtree qt;
			vector<Polygon*> landPolygons;
		public:

			virtual bool Init(/* layerconfig */) override
			{
				OSM_SpatRef = supportedCRS["EPSG:4326"];

				if (!OSM_SpatRef)
				{
					cout << "OSMSDF: " << "CRS support for EPSG:4326 is mandatory" << endl;
					return false;
				}

				const char* coastlineShapeFileSrc = "../../test/coastline/land_polygons.shp";
				const char* coastlineShapeFileDst = "../../test/coastline/land_polygons_split.shp";

				if (!exists(coastlineShapeFileDst))
				{
					SplitPolygons(coastlineShapeFileSrc, coastlineShapeFileDst);
				}

				if (!LoadLandShapes(coastlineShapeFileDst)) return false;
				
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

		private:

			virtual ~OSMSDF() override
			{
				const size_t numShapes = landPolygons.size();
				for (size_t n = 0; n < numShapes; n++)
				{
					delete landPolygons[n];
				}
				landPolygons.clear();
			}

			HandleGetMapRequestResult HandleGetMapRequest(const WebMapService::GetMapRequest& gmr, const OGRSpatialReference* requestSRS, Image& img)
			{
				assert(img.rawDataType == DT_U8);

				BBox asterBBox;
				if (!TransformBBox(gmr.bbox, asterBBox, requestSRS, OSM_SpatRef))
				{
					return HGMRR_InvalidBBox; // TODO: according to WMS specs bbox may lay outside of valid areas (e.g. latitudes greater than 90 degrees in CRS:84)
				}

				high_resolution_clock::time_point t1 = high_resolution_clock::now();

				memset(img.rawData, 0x7F, img.rawDataSize);
				HandleGetMapRequestResult result = RasterImg(gmr, requestSRS, img);

				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				duration<double> time_span = duration_cast<duration<double>>(t2 - t1) * 1000.0;
				std::cout << "OSM SDF computation took " << std::setprecision(5) << time_span.count() << " ms" << endl;

				return result;
			}

			HandleGetMapRequestResult RasterImg(const WebMapService::GetMapRequest& gmr, const OGRSpatialReference* requestSRS, Image& img)
			{
				const int Width = img.width;
				const int Height = img.height;

				const double DistanceDomain = 10.0;
				const double cellSize = 360.0 / Width;
				const double ScaledDistanceDomain = cellSize * DistanceDomain;

				//omp_lock_t myLock;
				//omp_init_lock(&myLock);
				atomic_int yProgress = 0;

				//#pragma omp parallel for
				for (int y = 0; y < Height; y++)
				{
					u8* world = &img.rawData[(Height - y - 1) * Width];

					for (int x = 0; x < Width; x++)
					{
						*world = 0;

						BoundingBox<double> bb(
							x * cellSize - 180.0 - ScaledDistanceDomain,
							y * cellSize - 90.0 - ScaledDistanceDomain, cellSize + ScaledDistanceDomain * 2.0, cellSize + ScaledDistanceDomain * 2.0);
						double minSqrDistance = numeric_limits<double>::max();

						double px = bb.left + bb.width * 0.5;
						double py = bb.top + bb.height * 0.5;

						//omp_set_lock(&myLock);
						LooseGeoQuadtree::Query query = qt.QueryIntersectsRegion(bb);
						//omp_unset_lock(&myLock);

						while (!query.EndOfQuery())
						{
							Polygon* poly = query.GetCurrent();

							const double aabbDistanceSqr = poly->ComputeSqrDistanceToBoundingBox(px, py);
							if (aabbDistanceSqr < minSqrDistance) // test distance to AABB first before doing the expensive polygon distance test
							{
								double sqrDistance = poly->ComputeSignedSquareDistance(px, py);
								minSqrDistance = min(minSqrDistance, sqrDistance);
							}

							if (minSqrDistance < 0.0)
							{
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
							(*world) = 255;
						}
						else
						{
							double minDistance = Sign(minSqrDistance) * Sqrt(Abs(minSqrDistance));
							u8 worldValue = (u8)Clamp(255.0 * (minDistance * 0.5 / ScaledDistanceDomain + 0.5), 0.0, 255.0);
							(*world) = worldValue;
						}

						world++;
					}

					yProgress++;
					cout << "rasterized: " << (int)(100.0f * yProgress / (float)Height) << "%s\r";
				}

				return HGMRR_OK;
			}

			bool LoadLandShapes(const char* landShapeFile)
			{
				RegisterOGRShape();
				auto driver = OGRGetDriverByName("ESRI Shapefile");
				OGRDataSource* source = (OGRDataSource*)OGROpen(landShapeFile, GA_ReadOnly, &driver);
				if (!source)
				{
					cout << "OSMSDF: " << "Unable to open shape file " << landShapeFile << endl;
					return false;
				}

				OGRLayer* srcLayer = source->GetLayer(0);
				GIntBig numFeatures = srcLayer->GetFeatureCount();
				srcLayer->ResetReading();

				landPolygons.resize(numFeatures);

				GIntBig featureIndex = 0;
				OGRFeature* feature;
				while ((feature = srcLayer->GetNextFeature()) != NULL)
				{
					OGRGeometry* geometry = feature->GetGeometryRef();

					if (featureIndex % 10000 == 0)
					{
						cout << "OSMSDF: " << "shapes loaded: " << featureIndex << " / " << numFeatures << "\r";
					}

					auto poly = new Polygon(feature);

					qt.Insert(poly);
					landPolygons[featureIndex] = poly;

					OGRFeature::DestroyFeature(feature);

					featureIndex++;
				}

				OGRDataSource::DestroyDataSource(source);

				return true;
			}
		};

		IMPLEMENT_WEBMAPSERVICE_LAYER(OSMSDF, LayerName, LayerTitle);
	}
}
