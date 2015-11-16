
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


	void SetTypedMemory(void* dest, const Variant& value, size count)
	{
		assert(value.IsSet());

		switch (value.GetDataType())
		{
		case DT_U8:
			SetTypedMemory((u8*)dest, value.GetValue().uint8[0], count);
			break;
		case DT_S16:
			SetTypedMemory((s16*)dest, value.GetValue().sint16[0], count);
			break;
		case DT_U32:
			SetTypedMemory((u32*)dest, value.GetValue().uint32[0], count); 
			break;
		case DT_F32:
			SetTypedMemory((f32*)dest, value.GetValue().float32[0], count);
			break;
		case DT_F64:
			SetTypedMemory((f64*)dest, value.GetValue().float64, count);
			break;
		default:
			assert(false); // requested datatype not implemented yet, sorry
			break;
		}
	}

}