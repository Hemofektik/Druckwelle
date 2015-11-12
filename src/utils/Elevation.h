
#pragma once

#include "ImageProcessor.h"

namespace dw
{
	static const s16 InvalidValueASTER = -9999;

	namespace utils
	{
		bool CompressElevation(Image& img, const utils::InvalidValue& invalidValue);
		bool DecompressElevation(Image& img);
	}
}