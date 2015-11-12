
#include "../WebMapTileService.h"
#include "../utils/ImageProcessor.h"
#include "../utils/Elevation.h"

#include <Poco/Net/HTTPRequest.h>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <istream>
#include <ostream>
#include <sstream>
#include <fstream>

#include "../utils/Filesystem.h"

#include <thread>

#include <ZFXMath.h>

using namespace std;
using namespace ZFXMath;

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;

namespace dw
{
	namespace Layers
	{
		class TileCache : public WebMapTileService::Layer
		{
			virtual ~TileCache() override {};

			thread* createTileCacheThread;

		public:

			virtual const char* GetIdentifier() const override
			{
				return desc.id.c_str();
			}

			virtual const char_t* GetTitle() const override
			{
				return desc.title.c_str();
			}

			virtual const char_t* GetAbstract() const override
			{
				return desc.abstract.c_str();
			}

			virtual int GetTileWidth() const override
			{
				return desc.tileWidth;
			}

			virtual int GetTileHeight() const override
			{
				return desc.tileHeight;
			}

			virtual const vector<DataType>& GetSuppordetFormats() const override
			{
				static const vector<DataType> SuppordetFormats = { DT_S16 };
				return SuppordetFormats;
			}

			virtual bool Init(/* layerconfig */) override
			{
				ReadConfig();

				if (!EnumerateFiles()) return false;

				createTileCacheThread = new thread([this] { CreateTileCacheAsync(); });

				return true;
			};

			virtual HandleGetTileRequestResult HandleGetTileRequest(const WebMapTileService::GetTileRequest& gmr, Image& img) override
			{
				return HGTRR_OK;
			}

		private:

			struct TileCacheDescription
			{
				string id;
				string title;
				string abstract;

				string srcHost;
				u16 srcPort;
				string srcLayerName;
				string storagePath;
				string fileExtension;

				u32 tileWidth;
				u32 tileHeight;
				u32 tilePaddingLeft;
				u32 tilePaddingTop;
				u32 tilePaddingRight;
				u32 tilePaddingBottom;

				ContentType srcContentType;
				ContentType cachedContentType;
				DataType dataType;
				utils::InvalidValue invalidValue;

				u32 numTilesX;
				u32 numTilesY;

				u32 numLevels;

				u32 numXDigits;
				u32 numYDigits;
				u32 numLevelDigits;
			};

			TileCacheDescription desc;

			struct Level
			{
				unique_ptr<u8> fileStatus;
			};

			unique_ptr<Level> levels;

			const u8 FileStatus_Missing = 0;
			const u8 FileStatus_Empty = 1;
			const u8 FileStatus_Exists = 2;

			void ReadConfig(/* layerconfig */)
			{
				const int AsterPixelsPerDegree = 3600;

				const int NumSrcPixelsAlongLongitude = 360 * AsterPixelsPerDegree;
				const int NumSrcPixelsAlongLatitude = 180 * AsterPixelsPerDegree;

				const u32 NumDstPixelsAlongLongitude = NextPowerOfTwo(NumSrcPixelsAlongLongitude);
				const u32 NumDstPixelsAlongLatitude = NextPowerOfTwo(NumSrcPixelsAlongLatitude);

				desc.id = "Tile Cache";
				desc.title = "Tile Cache";
				desc.abstract = "";

				desc.srcHost = "localhost";
				desc.srcPort = 8282;
				desc.srcLayerName = "QualityElevation";
				desc.storagePath = "D:/QECache";
				//desc.storagePath = "/home/rsc/Desktop/QECache";
				desc.fileExtension = ".cem";

				//const u32 TileQuadCount = 16;
				//const u32 MaxTesselationFactor = 32;
				const u32 Padding = 0;//MaxTesselationFactor / 2;

				desc.tileWidth = 2048; // TileQuadCount * MaxTesselationFactor;
				desc.tileHeight = desc.tileWidth;
				desc.tilePaddingLeft = Padding;
				desc.tilePaddingTop = Padding;
				desc.tilePaddingRight = Padding;
				desc.tilePaddingBottom = Padding;

				desc.srcContentType = CT_Image_Raw_S16;
				desc.cachedContentType = CT_Image_Elevation;
				desc.dataType = DT_S16;
				desc.invalidValue = utils::InvalidValue(InvalidValueASTER);

				desc.numTilesX = NumDstPixelsAlongLongitude / desc.tileWidth;
				desc.numTilesY = NumDstPixelsAlongLatitude / desc.tileHeight;
				desc.numLevels = Min(Log2OfPowerOfTwo(desc.numTilesX), Log2OfPowerOfTwo(desc.numTilesY)) + 1;

				desc.numXDigits = (u32)RoundUp(Log10<double>(desc.numTilesX));
				desc.numYDigits = (u32)RoundUp(Log10<double>(desc.numTilesY));
				desc.numLevelDigits = (u32)RoundUp(Log10<double>(desc.numLevels));
			}

			bool EnumerateFiles()
			{
				path root = desc.storagePath;

				error_code err;
				create_directories(root, err);

				if (err)
				{
					cout << "Tile Cache Error: Creating Directory Failed: " << root << " (" << err.message() << ")"<< endl;
					return false;
				}

				levels.reset(new Level[desc.numLevels]);

				for (u32 l = 0; l < desc.numLevels; l++)
				{
					EnumerateFilesForLevel(l, root);
				}

				return true;
			}

			void EnumerateFilesForLevel(int level, const path& storagePath)
			{
				path levelPath = desc.storagePath;
				levelPath /= CreateZeroPaddedString(level, desc.numLevelDigits);

				int numTilesX = desc.numTilesX >> (desc.numLevels - level - 1);
				int numTilesY = desc.numTilesY >> (desc.numLevels - level - 1);

				auto& fileStatus = levels.get()[level].fileStatus;
				fileStatus.reset(new u8[numTilesX * numTilesY]);
				memset(fileStatus.get(), FileStatus_Missing, numTilesX * numTilesY);
				if (exists(levelPath))
				{
					for (directory_iterator di(levelPath); di != fsend(di); di++)
					{
						const auto& entity = *di;
						if (!is_directory(entity.status())) continue;

						int y = atoi(entity.path().filename().generic_string().c_str());

						for (directory_iterator fi(entity.path()); fi != fsend(fi); fi++)
						{
							const auto& fileEntity = *fi;
							const auto extension = fileEntity.path().extension();
							if (is_regular_file(fileEntity.status()) && extension == desc.fileExtension)
							{
								int x = atoi(fileEntity.path().filename().generic_string().c_str());

								auto fileSize = file_size(fileEntity.path());

								fileStatus.get()[y * numTilesX + x] = (fileSize > 0) ? FileStatus_Exists : FileStatus_Empty;
							}
						}
					}
				}
			}

			string CreateZeroPaddedString(int number, u32 numberOfDigits)
			{
				string str = "";
				string strEnd = to_string(number);
				for (int d = 0; d < numberOfDigits - (int)strEnd.length(); d++)
				{
					str.push_back('0');
				}
				str += strEnd;
				return str;
			}

			bool StoreTileToDisk(Image& tileImg, int x, int y, int level)
			{
				path path = desc.storagePath;

				string levelString = CreateZeroPaddedString(level, desc.numLevelDigits);
				string yString = CreateZeroPaddedString(y, desc.numYDigits);

				path /= levelString;
				path /= yString;

				error_code err;
				create_directories(path, err);

				if (err)
				{
					cout << "Tile Cache Error: Creating Directory Failed: " << path << " (" << err.message() << ")" << endl;
					return false;
				}

				string xString = CreateZeroPaddedString(x, desc.numXDigits);

				string filename = xString + desc.fileExtension;
				path /= filename;

				if (!utils::ConvertRawImageToContentType(tileImg, desc.cachedContentType))
				{
					std::cout << "Tile Cache Error: compressing elevation failed" << std::endl;
				}
				if (!tileImg.SaveProcessedDataToFile(path.string()))
				{
					std::cout << "Tile Cache Error: writing to file failed" << std::endl;
					return false;
				}

				return true;
			}

			void CreateTileCacheAsync()
			{
				CreateTileCacheLevel0();
				CreateTileCacheMipLevels();
			}

			void CreateTileCacheLevel0()
			{
				Image emptyTile(0, 0, desc.dataType);

				const double TileWidthInDegree = (360.0 / desc.numTilesX);
				const double TileHeightInDegree = (180.0 / desc.numTilesY);
				const double TilePaddingLeftInDegree = TileWidthInDegree * (desc.tilePaddingLeft / (double)desc.tileWidth);
				const double TilePaddingRightInDegree = TileWidthInDegree * (desc.tilePaddingRight / (double)desc.tileWidth);
				const double TilePaddingTopInDegree = TileHeightInDegree * (desc.tilePaddingTop / (double)desc.tileHeight);
				const double TilePaddingBottomInDegree = TileHeightInDegree * (desc.tilePaddingBottom / (double)desc.tileHeight);

				const auto& fileStatus = levels.get()[desc.numLevels - 1].fileStatus;

				for (int y = 0; y < desc.numTilesY; y++)
				{
					#pragma omp parallel for
					for (int x = 0; x < desc.numTilesX; x++)
					{
						if (fileStatus.get()[y * desc.numTilesX + x] != FileStatus_Missing)
						{
							continue;
						}

						Image tileImg(desc.tileWidth, desc.tileHeight, desc.dataType);
						const size expectedTileSize = tileImg.rawDataSize;

						string tileRequestUri = "/?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&CRS=EPSG:4326&LAYERS=" + desc.srcLayerName + "&STYLES=";
						tileRequestUri += "&WIDTH=" + to_string(desc.tileWidth);
						tileRequestUri += "&HEIGHT=" + to_string(desc.tileHeight);
						tileRequestUri += "&FORMAT=" + ContentTypeId[desc.srcContentType];

						const double left = (x / (double)desc.numTilesX) * 360.0 - 180.0 - TilePaddingLeftInDegree;
						const double top = -(y / (double)desc.numTilesY) * 180.0 + 90.0 + TilePaddingTopInDegree;
						const double right = left + TileWidthInDegree + TilePaddingRightInDegree;
						const double bottom = top - TileHeightInDegree - TilePaddingBottomInDegree;

						tileRequestUri += "&BBOX=" + to_string(left) + "," + to_string(bottom) + "," + to_string(right) + "," + to_string(top);

						bool retry = false;
						do
						{
							try 
							{
								retry = false;

								HTTPClientSession s(desc.srcHost, desc.srcPort);
								HTTPRequest request(HTTPRequest::HTTP_GET, tileRequestUri);

								s.sendRequest(request);
								HTTPResponse response;

								if (response.getStatus() != HTTPResponse::HTTP_OK)
								{
									cout << "Tile Cache Error: failed to receive tile (" << x << "," << y << ")! http return code: "  << response.getStatus() << endl;
									retry = true;
									continue;
								}

								istream& rs = s.receiveResponse(response);

								std::streamsize len = 0;
								rs.read((char*)tileImg.rawData, expectedTileSize);
								std::streamsize readSize = rs.gcount();
								if (readSize < expectedTileSize)
								{
									cout << "Tile Cache Error: Cache Creation failed! Failed to receive valid tile (" << x << "," << y << ")!" << endl;
									retry = true;
									continue;
								}

								s.reset();


								if (desc.invalidValue.IsSet() && utils::IsImageCompletelyInvalid(tileImg, desc.invalidValue))
								{
									if (!StoreTileToDisk(emptyTile, x, y, desc.numLevels - 1))
									{
										retry = true;
										continue;
									}
								}
								else if (!StoreTileToDisk(tileImg, x, y, desc.numLevels - 1))
								{
									retry = true; 
									continue;
								}
							}
							catch (Poco::Exception& e) 
							{
								std::cout << "Tile Cache Error: " << e.displayText() << " ... retrying" << std::endl;
								retry = true;
							}
							catch (...)
							{
								std::cout << "Tile Cache Error: " << "unknown error during tile retrieval ... retrying" << std::endl;
								retry = true;
							}
						} 
						while (retry);

					}
				}
			}

			void CreateTileCacheMipLevels()
			{
				int level = desc.numLevels - 1;
				int numTilesX = desc.numTilesX;
				int numTilesY = desc.numTilesY;
				while (level > 0)
				{
					level--;
					numTilesX /= 2;
					numTilesY /= 2;

					const auto& fileStatus = levels.get()[level].fileStatus;

					for (int y = 0; y < desc.numTilesY; y++)
					{
						//#pragma omp parallel for
						for (int x = 0; x < desc.numTilesX; x++)
						{
							if (fileStatus.get()[y * numTilesX + x] != FileStatus_Missing)
							{
								continue;
							}

							// TODO: create mip level image from four high level images
						}
					}
				}
			}
		};

		IMPLEMENT_WEBMAPTILESERVICE_LAYER(TileCache, "TileCache", "Can become a tile cache for any WMS layer");
	}
}
