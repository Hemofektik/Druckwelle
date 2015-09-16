
#include "../WebMapTileService.h"
#include "../utils/ImageProcessor.h"

#include <Poco/Net/HTTPRequest.h>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <istream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <filesystem>

#include <thread>

#include <ZFXMath.h>

using namespace std;
using namespace std::experimental::filesystem::v1;
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

				EnumerateFiles();

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
				astring id;
				string title;
				string abstract;

				astring srcHost;
				u16 srcPort;
				astring srcLayerName;
				astring storagePath;
				astring fileExtension;

				u32 tileWidth;
				u32 tileHeight;
				ContentType srcContentType;
				ContentType cachedContentType;
				DataType dataType;
				utils::InvalidValue invalidValue;

				u32 numTilesX;
				u32 numTilesY;

				u32 numLevels;
			};

			TileCacheDescription desc;
			unique_ptr<u8> fileStatus;

			const u8 FileStatus_Missing = 0;
			const u8 FileStatus_Empty = 1;
			const u8 FileStatus_Exists = 2;

			void ReadConfig(/* layerconfig */)
			{
				const int AsterPixelsPerDegree = 3600;

				const int NumSrcPixelsAlongLongitude = 360 * AsterPixelsPerDegree;
				const int NumSrcPixelsAlongLatitude = 180 * AsterPixelsPerDegree;

				const u32 NumDstPixelsAlongLongitude = NextPoweOfTwo(NumSrcPixelsAlongLongitude);
				const u32 NumDstPixelsAlongLatitude = NextPoweOfTwo(NumSrcPixelsAlongLatitude);

				const s16 InvalidValueASTER = -9999;

				desc.id = "Tile Cache";
				desc.title = dwTEXT("Tile Cache");
				desc.abstract = dwTEXT("");

				desc.srcHost = "localhost";
				desc.srcPort = 8282; 
				desc.srcLayerName = "QualityElevation";
				desc.storagePath = "D:/QECache";
				desc.fileExtension = ".elv";

				desc.tileWidth = 2048;
				desc.tileHeight = 2048;
				desc.srcContentType = CT_Image_Raw_S16;
				desc.cachedContentType = CT_Image_Elevation;
				desc.dataType = DT_S16;
				desc.invalidValue = utils::InvalidValue(InvalidValueASTER);

				desc.numTilesX = NumDstPixelsAlongLongitude / desc.tileWidth;
				desc.numTilesY = NumDstPixelsAlongLatitude / desc.tileHeight;
				desc.numLevels = Min(PowerOfTwoLog2(desc.numTilesX), PowerOfTwoLog2(desc.numTilesY)) + 1;
			}

			void EnumerateFiles()
			{
				create_directories(desc.storagePath);

				fileStatus.reset(new u8[desc.numTilesX * desc.numTilesY]);
				memset(fileStatus.get(), FileStatus_Missing, desc.numTilesX * desc.numTilesY);
				for (directory_iterator di(desc.storagePath); di != end(di); di++)
				{
					const auto& entity = *di;
					if (!is_directory(entity.status())) continue;

					int y = atoi(entity.path().filename().generic_string().c_str());

					for (directory_iterator fi(entity.path()); fi != end(fi); fi++)
					{
						const auto& fileEntity = *fi;
						const auto extension = fileEntity.path().extension();
						if (is_regular_file(fileEntity.status()) && extension == desc.fileExtension)
						{
							int x = atoi(fileEntity.path().filename().generic_string().c_str());

							auto fileSize = file_size(fileEntity.path());

							fileStatus.get()[y * desc.numTilesX + x] = (fileSize > 0) ? FileStatus_Exists : FileStatus_Empty;
						}
					}
				}
			}

			void StoreTileToDisk(Image& tileImg, int x, int y)
			{
				path path = desc.storagePath;

				const int numXDigits = (int)RoundUp(Log10<double>(desc.numTilesX));
				const int numYDigits = (int)RoundUp(Log10<double>(desc.numTilesY));

				astring yString = "";
				astring yStrEnd = to_string(y);
				for (int d = 0; d < numYDigits - (int)yStrEnd.length(); d++)
				{
					yString.push_back('0');
				}
				yString += yStrEnd;

				path = path.append(yString);
				create_directory(path);

				astring xString = "";
				astring xStrEnd = to_string(x);
				for (int d = 0; d < numXDigits - (int)xStrEnd.length(); d++)
				{
					xString.push_back('0');
				}

				astring filename = xString + xStrEnd + desc.fileExtension;
				path = path.append(filename);

				utils::ConvertRawImageToContentType(tileImg, desc.cachedContentType);
				ofstream file(path.c_str(), ios::out | ios::trunc | ios::binary);
				{
					file.write((const char*)tileImg.processedData, tileImg.processedDataSize);
				}
				file.close();
			}

			void CreateTileCacheAsync()
			{
				Image emptyTile(0, 0, desc.dataType);
				Image tileImg(desc.tileWidth, desc.tileHeight, desc.dataType);
				const size expectedTileSize = tileImg.rawDataSize;

				bool runCacheCreation = true;
				for (int y = 0; y < desc.numTilesY && runCacheCreation; y++)
				{
					for (int x = 0; x < desc.numTilesX; x++)
					{
						if (fileStatus.get()[y * desc.numTilesX + x] != FileStatus_Missing)
						{
							continue;
						}

						astring tileRequestUri = "/?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&CRS=EPSG:4326&LAYERS=" + desc.srcLayerName + "&STYLES=";
						tileRequestUri += "&WIDTH=" + to_string(desc.tileWidth);
						tileRequestUri += "&HEIGHT=" + to_string(desc.tileHeight);
						tileRequestUri += "&FORMAT=" + ContentTypeId[desc.srcContentType];
						tileRequestUri += "&BBOX=" + to_string(x) + "," + to_string(y) + "," + to_string(x + 0.5) + "," + to_string(y + 0.5);
						// TODO: fix BBOX

						HTTPClientSession s(desc.srcHost, desc.srcPort);
						HTTPRequest request(HTTPRequest::HTTP_GET, tileRequestUri);

						s.sendRequest(request);
						HTTPResponse response;
						istream& rs = s.receiveResponse(response);

						std::streamsize len = 0;
						rs.read((char*)tileImg.rawData, expectedTileSize);
						std::streamsize readSize = rs.gcount();
						if (readSize < expectedTileSize)
						{
							runCacheCreation = false;
							cout << "Error: Cache Creation failed! Failed to receive valid tile (" << x << "," << y << ")!";
							break;
						}

						if (desc.invalidValue.IsSet())
						{
							if (utils::IsImageCompletelyInvalid(tileImg, desc.invalidValue))
							{
								StoreTileToDisk(emptyTile, x, y);
								continue;
							}
						}

						StoreTileToDisk(tileImg, x, y);
					}
				}
			}
		};

		IMPLEMENT_WEBMAPTILESERVICE_LAYER(TileCache, "TileCache", dwTEXT("Can become a tile cache for any WMS layer"));
	}
}