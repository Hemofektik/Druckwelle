

#include "ReadZIP.h"


/*
miniunz.c
Version 1.1, February 14h, 2010
sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

Modifications of Unzip for Zip64
Copyright (C) 2007-2008 Even Rouault

Modifications for Zip64 support on both zip and unzip
Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )
*/

#ifndef _WIN32
#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif
#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BIT
#define _FILE_OFFSET_BIT 64
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string>

#ifdef unix
# include <unistd.h>
# include <utime.h>
#else
# include <direct.h>
# include <io.h>
#endif

#include <unzip.h>

/*#ifdef _WIN32
#define USEWIN32IOAPI
#include <iowin32.h>
#endif*/


int do_extract_currentfile(unzFile uf, ZIPFileItem& fileItem, bool decompressContent)
{
	const int MaxFileNameLength = 1024;
	char filename_inzip[MaxFileNameLength];
	int err = UNZ_OK;

	unz_file_info64 file_info;
	err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
	fileItem.dataSize = file_info.uncompressed_size;

	// copy file name
	if (fileItem.name.length() == 0)
	{
		fileItem.name.append(filename_inzip);
	}

	if (err != UNZ_OK)
	{
		printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
		return err;
	}

	// read file contents
	if (decompressContent)
	{
		err = unzOpenCurrentFile(uf);
		if (err != UNZ_OK)
		{
			printf("error %d with zipfile in unzOpenCurrentFile\n", err);
		}
		else
		{
			fileItem.data = new uint8_t[fileItem.dataSize + 1]; // data might be text, so add zero termination ...
			fileItem.data[fileItem.dataSize] = 0;

			const int MaxNumReadBytes = 8192;
			int readNumBytes = 0;
			uint64_t bufferIndex = 0;
			while ((readNumBytes = unzReadCurrentFile(uf, &fileItem.data[bufferIndex], MaxNumReadBytes)) > 0)
			{
				bufferIndex += readNumBytes;
			}

			if (readNumBytes < 0)
			{
				err = readNumBytes;
				printf("error %d with zipfile %s in unzReadCurrentFile\n", err, filename_inzip);
			}

			if (err == UNZ_OK)
			{
				err = unzCloseCurrentFile(uf);
				if (err != UNZ_OK)
				{
					printf("error %d with zipfile in unzCloseCurrentFile\n", err);
				}
			}
			else
			{
				unzCloseCurrentFile(uf); /* don't lose the error */
			}
		}
	}

	return err;
}


int do_extract(unzFile uf, ZIPFile& zipFile, bool decompressContent, int itemIndex)
{
	uLong i;
	unz_global_info64 gi;
	int err;
	FILE* fout = NULL;

	err = unzGetGlobalInfo64(uf, &gi);
	if (err != UNZ_OK)
	{
		printf("error %d with zipfile in unzGetGlobalInfo \n", err);
		return 1;
	}

	if (itemIndex < 0)
	{
		zipFile.numItems = gi.number_entry;
		zipFile.items = new ZIPFileItem[zipFile.numItems];
		memset(zipFile.items, 0, sizeof(ZIPFileItem)* zipFile.numItems);
	}

	for (i = 0; i<gi.number_entry; i++)
	{
		if (do_extract_currentfile(uf, zipFile.items[i], decompressContent || (itemIndex == i)) != UNZ_OK)
		{
			return 1;
		}

		if ((i + 1)<gi.number_entry)
		{
			err = unzGoToNextFile(uf);
			if (err != UNZ_OK)
			{
				printf("error %d with zipfile in unzGoToNextFile\n", err);
				return 1;
			}
		}
	}

	return 0;
}


bool ReadZIPFile(const std::string& fileName, ZIPFile& zipFile, bool decompressContent, int itemIndex)
{
	unzFile uf = NULL;
	if (itemIndex < 0)
	{
		memset(&zipFile, 0, sizeof(ZIPFile));
	}

#ifdef USEWIN32IOAPI
	zlib_filefunc64_def ffunc;
	fill_win32_filefunc64A(&ffunc);
	uf = unzOpen2_64(fileName.c_str(), &ffunc);
#else
	uf = unzOpen64(fileName.c_str());
#endif

	if (uf == NULL)
	{
		printf("Cannot open %s\n", fileName.c_str());
		return false;
	}

	int ret_value = do_extract(uf, zipFile, decompressContent, itemIndex);

	unzClose(uf);

	return ret_value == 0;
}

void DestroyZIPFileItem(ZIPFileItem& zipFileItem)
{
	delete[] zipFileItem.data;	
	zipFileItem.name.clear();
}

bool ReadZIPFile(const std::string& fileName, ZIPFile& zipFile, bool decompressContent)
{
	return ReadZIPFile(fileName, zipFile, decompressContent, -1);
}

bool ReadZIPFileItem(const std::string& fileName, ZIPFile& zipFile, int itemIndex)
{
	return ReadZIPFile(fileName, zipFile, false, itemIndex);
}


void DestroyZIPFile(ZIPFile& zipFile)
{
	const uint64_t numItems = zipFile.numItems;
	for (uint64_t n = 0; n < numItems; n++)
	{
		DestroyZIPFileItem(zipFile.items[n]);
	}
	delete[] zipFile.items;
	memset(&zipFile, 0, sizeof(ZIPFile));
}
