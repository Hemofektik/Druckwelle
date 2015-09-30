
#include "dwcore.h"

#include <cpplinq.hpp>
#include <map>

using namespace std;
using namespace cpplinq;

namespace dw
{
	map<string, ContentType> ContentTypeIdMapping;

	struct InitStaticCore
	{
		InitStaticCore()
		{
			for (int ctIndex = 0; ctIndex < CT_NumContentTypes; ctIndex++)
			{
				ContentTypeIdMapping[ContentTypeId[ctIndex]] = (ContentType)ctIndex;
			}
		}

	} staticRunCtor;

	ContentType GetContentType(const string& contentTypeId)
	{
		auto availableContentType = ContentTypeIdMapping.find(contentTypeId);
		if (availableContentType == ContentTypeIdMapping.end())
		{
			return CT_Unknown;
		}
		return availableContentType->second;
	}

	DataType FindCompatibleDataType(ContentType contentType, const vector<DataType>& availableDataTypes)
	{
		switch (contentType)
		{
		case CT_Image_PNG:
			return from(availableDataTypes) >> first_or_default([](DataType dt) { return dt == DT_RGBA8 || dt == DT_U8 || dt == DT_U32; });
		case CT_Image_Raw_S16:
		case CT_Image_Elevation:
			return from(availableDataTypes) >> first_or_default([](DataType dt) { return dt == DT_S16; });
		case CT_Image_Raw_U8:
			return from(availableDataTypes) >> first_or_default([](DataType dt) { return dt == DT_U8; });
		case CT_Image_Raw_U32:
			return from(availableDataTypes) >> first_or_default([](DataType dt) { return dt == DT_U32; });
		default:
			return DT_Unknown;
		}
	}

}