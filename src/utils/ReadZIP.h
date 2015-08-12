
#include <string>
#include <stdint.h>

struct ZIPFileItem
{
	std::string name;
	uint8_t* data;
	uint64_t dataSize;
};

struct ZIPFile
{
	ZIPFileItem* items;
	uint64_t numItems;
};

bool ReadZIPFile(const std::string& fileName, ZIPFile& zipFile, bool decompressContent = true);
bool ReadZIPFileItem(const std::string& fileName, ZIPFile& zipFile, int itemIndex);
void DestroyZIPFile(ZIPFile& zipFile);
