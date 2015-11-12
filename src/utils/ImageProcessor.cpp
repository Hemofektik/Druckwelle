
#if ALLOW_AMP
#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>
using namespace Concurrency;
using namespace Concurrency::graphics;
#endif

#include "ImageProcessor.h"
#include "Elevation.h"

#include <algorithm>
#include <fstream>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <ZFXMath.h>



using namespace std;
using namespace ZFXMath;


namespace dw
{
	Image::Image(int width, int height, DataType dataType)
		: ownsRawDataPointer(true)
		, ownsProcessedDataPointer(true)
		, rawDataSize(width * height * DataTypePixelSize[dataType])
		, rawData((rawDataSize > 0) ? new u8[rawDataSize] : NULL)
		, width(width)
		, height(height)
		, rawPixelSize(DataTypePixelSize[dataType])
		, rawDataType(dataType)
		, processedData(NULL)
		, processedDataSize(0)
		, processedContentType(CT_Unknown)
	{
	}

	Image::Image(int width, int height, DataType dataType, u8* rawData, bool ownsRawDataPointer)
		: ownsRawDataPointer(ownsRawDataPointer)
		, ownsProcessedDataPointer(true)
		, rawDataSize(width * height * DataTypePixelSize[dataType])
		, rawData(rawData)
		, width(width)
		, height(height)
		, rawPixelSize(DataTypePixelSize[dataType])
		, rawDataType(dataType)
		, processedData(NULL)
		, processedDataSize(0)
		, processedContentType(CT_Unknown)
	{
	}

	Image::Image(u8* processedData, size processedDataSize, ContentType contentType, bool ownsProcessedDataPointer)
		: ownsRawDataPointer(true)
		, ownsProcessedDataPointer(ownsProcessedDataPointer)
		, rawDataSize(0)
		, rawData(NULL)
		, width(0)
		, height(0)
		, rawPixelSize(0)
		, rawDataType(DT_Unknown)
		, processedData(processedData)
		, processedDataSize(processedDataSize)
		, processedContentType(contentType)
	{
	}

	Image::~Image()
	{
		FreeProcessedData();
		FreeRawData();
	}

	void Image::FreeRawData()
	{
		if (ownsRawDataPointer)
		{
			delete[] rawData;
		}
		rawData = NULL;
	}

	void Image::FreeProcessedData()
	{
		if (ownsProcessedDataPointer)
		{
			switch (processedContentType)
			{
			case dw::CT_Image_PNG:
				STBIW_FREE(processedData);
				break;
			case CT_Image_Raw_S16:
			case CT_Image_Raw_U8:
			case CT_Image_Raw_U32:
			case CT_Unknown:
			default:
				if (rawData != processedData)
				{
					delete[] processedData;
				}
				break;
			}
		}

		processedData = NULL;
	}

	bool Image::SaveToPNG(const string& filename)
	{
		assert(rawDataType == DT_U8 || rawDataType == DT_RGBA8);
		return (utils::ConvertRawImageToContentType(*this, CT_Image_PNG) && SaveProcessedDataToFile(filename));
	}

	bool Image::SaveProcessedDataToFile(const string& filename) const
	{
		ofstream file(filename.c_str(), ios::out | ios::trunc | ios::binary);
		if (file.is_open())
		{
			try
			{
				file.write((const char*)processedData, processedDataSize);
			}
			catch (...)
			{
				return false;
			}
		}
		else if (file.fail())
		{
			return false;
		}
		file.close();
		return true;
	}

	template <typename srcType, typename dstType>
	static bool SaveToPNG(const string& filename, std::function<dstType(srcType)> convert, Image& img)
	{
		assert(sizeof(srcType) == img.rawPixelSize);
		assert(sizeof(dstType) == 4 || sizeof(dstType) == 1);
		Image tmpImg(img.width, img.height, (sizeof(dstType) == 4) ? DT_RGBA8 : DT_U8);
		dstType* dstEnd = ((dstType*)tmpImg.rawData) + tmpImg.width * tmpImg.height;
		dstType* dst = tmpImg.rawData;
		srcType* src = (srcType*)img.rawData;
		while (dst < dstEnd)
		{
			dstType newValue = convert(*src);

			*dst = newValue;

			src++;
			dst++;
		}

		return tmpImg.SaveToPNG(filename);
	}

	// specialize any combinations you want to support here
	template <>	bool Image::SaveToPNG(const string& filename, std::function<u8(s16)> convert) { return dw::SaveToPNG(filename, convert, *this); }
	template <>	bool Image::SaveToPNG(const string& filename, std::function<u8(f32)> convert) { return dw::SaveToPNG(filename, convert, *this); }
	template <>	bool Image::SaveToPNG(const string& filename, std::function<u8(f64)> convert) { return dw::SaveToPNG(filename, convert, *this); }


	namespace utils
	{
		bool ConvertRawImageToContentType(Image& image, ContentType contentType)
		{
			image.processedContentType = contentType;
			image.FreeProcessedData();

			switch (contentType)
			{
			case dw::CT_Image_PNG:
			{
				int dataSize = 0; // TODO: replace stb library by faster lib (e.g. turbo version of libpng)
				if (image.rawDataType == DT_RGBA8 || image.rawDataType == DT_U32)
				{
					image.processedData = stbi_write_png_to_mem((u8*)image.rawData, image.width * sizeof(u32), image.width, image.height, 4, &dataSize);
				}
				else if (image.rawDataType == DT_U8)
				{
					image.processedData = stbi_write_png_to_mem((u8*)image.rawData, image.width * sizeof(u8), image.width, image.height, 1, &dataSize);
				}
				image.processedDataSize = dataSize;
				return image.processedData != NULL;
			}

			// handle all raw formats in the same way (processed data ptr points to raw data, which avoids data duplication)
			case CT_Image_Raw_S16:
				if (contentType == CT_Image_Raw_S16 && image.rawDataType != DT_S16) return false;
			case CT_Image_Raw_U8:
				if (contentType == CT_Image_Raw_U8 && image.rawDataType != DT_U8) return false;
			case CT_Image_Raw_U32:
				if (contentType == CT_Image_Raw_U32 && image.rawDataType != DT_RGBA8 && image.rawDataType != DT_U32) return false;
			case CT_Image_Raw_F32:
				if (contentType == CT_Image_Raw_F32 && image.rawDataType != DT_F32) return false;
			case CT_Image_Raw_F64:
				if (contentType == CT_Image_Raw_F64 && image.rawDataType != DT_F64) return false;

				image.processedData = image.rawData;
				image.processedDataSize = image.rawDataSize;
				return true;
			case CT_Image_Elevation:
			{
				utils::InvalidValue invalidValue(InvalidValueASTER);
				return CompressElevation(image, invalidValue);
			}
			case CT_Unknown:
			default:
				return false;
			}
		}

		bool ConvertContentTypeToRawImage(Image& image)
		{
			image.FreeRawData();

			switch (image.processedContentType)
			{
			case dw::CT_Image_PNG:
			{
				// TODO: implement
				return false;
			}

			// handle all raw formats in the same way (raw data ptr points to processed data, which avoids data duplication)
			case CT_Image_Raw_S16:
				if (image.rawDataType == DT_Unknown) image.rawDataType = DT_S16;
			case CT_Image_Raw_U8:
				if (image.rawDataType == DT_Unknown) image.rawDataType = DT_U8;
			case CT_Image_Raw_U32:
				if (image.rawDataType == DT_Unknown) image.rawDataType = DT_U32;
			case CT_Image_Raw_F32:
				if (image.rawDataType == DT_Unknown) image.rawDataType = DT_F32;
			case CT_Image_Raw_F64:
				if (image.rawDataType == DT_Unknown) image.rawDataType = DT_F64;

				image.rawData = image.processedData;
				image.rawDataSize = image.processedDataSize;
				return true;
			case CT_Image_Elevation:
			{
				return DecompressElevation(image);
			}
			case CT_Unknown:
			default:
				return false;
			}
		}

		static const int LanczosWindowSize = 3;

		void ExtendBoundingBoxForLanczos(
			BBox& asterBBox,
			double srcDegreesPerPixelX, double srcDegreesPerPixelY,
			double dstDegreesPerPixelX, double dstDegreesPerPixelY)
		{
			double paddingX = max(srcDegreesPerPixelX, dstDegreesPerPixelX) * LanczosWindowSize;
			double paddingY = max(srcDegreesPerPixelY, dstDegreesPerPixelY) * LanczosWindowSize;

			asterBBox.minX -= paddingX;
			asterBBox.minY -= paddingY;
			asterBBox.maxX += paddingX;
			asterBBox.maxY += paddingY;
		}

		// source: http://src.chromium.org/svn/trunk/src/skia/ext/image_operations.cc
		//
		// Evaluates the Lanczos filter of the given filter size window for the given
		// position.
		//
		// |filter_size| is the width of the filter (the "window"), outside of which
		// the value of the function is 0. Inside of the window, the value is the
		// normalized sinc function:
		//   lanczos(x) = sinc(x) * sinc(x / filter_size);
		// where
		//   sinc(x) = sin(pi*x) / (pi*x);
		static double EvalLanczos(int filter_size, double x)
		{
			double floatFilterSize = filter_size;
			if (x <= -floatFilterSize || x >= floatFilterSize)
			{
				return 0.0f;  // Outside of the window.
			}

			if (x > -numeric_limits<double>::epsilon() &&
				x < numeric_limits<double>::epsilon())
			{
				return 1.0f;  // Special case the discontinuity at the origin.
			}

			double xpi = x * ZFXMath::PI;
			double xUnit = xpi / floatFilterSize;

			double sincx = sin(xpi) / xpi;
			double sincxUnit = sin(xUnit) / (xUnit);

			return sincx * sincxUnit;
		}

		template<typename T>
		static void SampleWithLanczosInternal(const Image& src, Image& dst, const SampleTransform& transform, const T invalidValue)
		{
			const int width = dst.width;
			const int height = dst.height;
			const int srcWidth = src.width;

			const int imgLanczosWindowX = max(LanczosWindowSize, (int)ceil(LanczosWindowSize * transform.scaleX));
			const int imgLanczosWindowY = max(LanczosWindowSize, (int)ceil(LanczosWindowSize * transform.scaleY));
			const int tmpImageHeight = (int)ceil(dst.height * transform.scaleY) + imgLanczosWindowY * 2;

			const int tmpImageOffsetY = (int)(transform.offsetY - imgLanczosWindowY);
			const double invImgLanczosWindowX = LanczosWindowSize / (double)imgLanczosWindowX;
			const double invImgLanczosWindowY = LanczosWindowSize / (double)imgLanczosWindowY;

			const float inf = numeric_limits<float>::infinity();
			Image horizontalLanczos(dst.width, tmpImageHeight, DT_F32);

			// horizontal pass
			{
				float* dstPixels = (float*)horizontalLanczos.rawData;
				const T* srcPixels = (const T*)src.rawData;
				for (int y = 0; y < tmpImageHeight; y++)
				{
					for (int x = 0; x < width; x++)
					{
						double srcX = x * transform.scaleX + transform.offsetX;
						
						const int iSrcX = (int)floor(srcX);
						const int iSrcY = y + tmpImageOffsetY;

						const double xOffset = iSrcX - srcX;

						const T* srcPixel = &srcPixels[iSrcY * srcWidth + iSrcX];

						double lanczosAcc = 0.0;
						double srcAcc = 0.0;
						for (int lx = -imgLanczosWindowX + 1; lx < imgLanczosWindowX; lx++)
						{
							T pixel = srcPixel[lx];
							if (pixel == invalidValue) continue;

							double l = EvalLanczos(LanczosWindowSize, (lx + xOffset) * invImgLanczosWindowX );

							lanczosAcc += l;
							srcAcc += pixel * l;
						}

						dstPixels[y * width + x] = (lanczosAcc == 0.0) ? inf : (float)(srcAcc / lanczosAcc);
					}
				}
			}

			// vertical pass
			{
				T* dstPixels = (T*)dst.rawData;
				const float* srcPixels = (const float*)horizontalLanczos.rawData;
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						double srcY = y * transform.scaleY + imgLanczosWindowY;

						const int iSrcX = x;
						const int iSrcY = (int)floor(srcY);

						const double yOffset = iSrcY - srcY;

						const float* srcPixel = &srcPixels[iSrcY * width + iSrcX];

						double lanczosAcc = 0.0;
						double srcAcc = 0.0;
						for (int ly = -imgLanczosWindowY + 1; ly < imgLanczosWindowY; ly++)
						{
							float pixel = srcPixel[ly * width];
							if (pixel == inf) continue;

							double l = EvalLanczos(LanczosWindowSize, (ly + yOffset) * invImgLanczosWindowY);

							lanczosAcc += l;
							srcAcc += pixel * l;
						}

						dstPixels[y * width + x] = (lanczosAcc == 0.0) ? invalidValue : (T)(srcAcc / lanczosAcc);
					}
				}
			}
		}


#if ALLOW_AMP
		static float EvalLanczosAMP(int filter_size, float x) restrict(amp)
		{
			float floatFilterSize = (float)filter_size;
			if (x <= -floatFilterSize || x >= floatFilterSize)
			{
				return 0.0f;  // Outside of the window.
			}

			if (x > -0.0001 &&
				x <  0.0001)
			{
				return 1.0f;  // Special case the discontinuity at the origin.
			}

			float xpi = x * 3.1415927f;
			float xUnit = xpi / floatFilterSize;

			float sincx = fast_math::sinf(xpi) / xpi;
			float sincxUnit = fast_math::sinf(xUnit) / (xUnit);

			return sincx * sincxUnit;
		}

		static void SampleWithLanczosInternalAMP(const Image& src, Image& dst, const SampleTransform& transform, const s16 invalidValue)
		{
			const int width = dst.width;
			const int height = dst.height;
			const int srcWidth = src.width;

			const int imgLanczosWindowX = max(LanczosWindowSize, (int)ceil(LanczosWindowSize * transform.scaleX));
			const int imgLanczosWindowY = max(LanczosWindowSize, (int)ceil(LanczosWindowSize * transform.scaleY));
			const int tmpImageHeight = (int)ceil(dst.height * transform.scaleY) + imgLanczosWindowY * 2;

			const int tmpImageOffsetY = (int)(transform.offsetY - imgLanczosWindowY);
			const float invImgLanczosWindowX = LanczosWindowSize / (float)imgLanczosWindowX;
			const float invImgLanczosWindowY = LanczosWindowSize / (float)imgLanczosWindowY;

			const float inf = numeric_limits<float>::infinity();
			const int InvalidValueAMP = invalidValue;

			float scaleX = (float)transform.scaleX;
			float scaleY = (float)transform.scaleY;
			float offsetX = (float)transform.offsetX;

			texture<float, 2> horizontalLanczosTex(tmpImageHeight, dst.width);
			texture_view<const float, 2> horizontalLanczosTexReadOnly(horizontalLanczosTex);
			texture_view<float, 2> horizontalLanczosTexWriteOnly(horizontalLanczosTex);

			// horizontal pass
			{
				texture<int, 2> srcPixelsTexture(src.height, src.width, (void*)src.rawData, (uint)src.rawDataSize, 16U);
				texture_view<const int, 2> srcPixelsTextureViewReadOnly(srcPixelsTexture);

				parallel_for_each(
					horizontalLanczosTexWriteOnly.extent,
					[=, &srcPixelsTexture](index<2> idx) restrict(amp)
					{
						int x = idx[1];
						int y = idx[0];

						float srcX = x * scaleX + offsetX;

						const int iSrcX = (int)fast_math::floorf(srcX);
						const int iSrcY = y + tmpImageOffsetY;

						const float xOffset = iSrcX - srcX;

						float lanczosAcc = 0.0;
						float srcAcc = 0.0;
						for (int lx = -imgLanczosWindowX + 1; lx < imgLanczosWindowX; lx++)
						{
							index<2> srcIdx(iSrcY, iSrcX + lx);
							int pixel = srcPixelsTextureViewReadOnly[srcIdx];
							if (pixel == InvalidValueAMP) continue;

							float l = EvalLanczosAMP(LanczosWindowSize, (lx + xOffset) * invImgLanczosWindowX);

							lanczosAcc += l;
							srcAcc += pixel * l;
						}

						float result = (lanczosAcc == 0.0) ? inf : (srcAcc / lanczosAcc);
						horizontalLanczosTexWriteOnly.set(idx, result);
					}
				);
			}

			// vertical pass
			{
				// we use int32 array view here because s16 UAVs are usually not supported by GPUs, thus we do a copy manually afterwards (see below)
				array_view<int, 2> dstPixelsArray(src.height, src.width);
				dstPixelsArray.discard_data();

				parallel_for_each(
					dstPixelsArray.extent,
					[=](index<2> idx) restrict(amp)
					{
						int x = idx[1];
						int y = idx[0];

						float srcY = y * scaleY + imgLanczosWindowY;

						const int iSrcX = x;
						const int iSrcY = (int)fast_math::floor(srcY);

						const float yOffset = iSrcY - srcY;

						float lanczosAcc = 0.0;
						float srcAcc = 0.0;
						for (int ly = -imgLanczosWindowY + 1; ly < imgLanczosWindowY; ly++)
						{
							index<2> srcIdx(iSrcY + ly, iSrcX);
							float pixel = horizontalLanczosTexReadOnly[srcIdx];

							if (pixel == inf) continue;

							float l = EvalLanczosAMP(LanczosWindowSize, (ly + yOffset) * invImgLanczosWindowY);

							lanczosAcc += l;
							srcAcc += pixel * l;
						}

						int result = (lanczosAcc == 0.0) ? InvalidValueAMP : (int)(srcAcc / lanczosAcc);
						dstPixelsArray[idx] = result;
					}
				);

				dstPixelsArray.synchronize();

				// copy s32 to s16 data
				s16* dstImg = (s16*)dst.rawData;
				for (int y = 0; y < dst.height; y++)
				{
					for (int x = 0; x < dst.width; x++)
					{
						index<2> srcIdx(y, x);

						*dstImg = (s16)dstPixelsArray[srcIdx];
						dstImg++;
					}
				}
			}
		}
#endif // ALLOW_AMP

		void SampleWithLanczos(const Image& src, Image& dst, const SampleTransform& transform, const InvalidValue& invalidValue)
		{
			assert(src.rawDataType == dst.rawDataType);
			assert(transform.offsetX - LanczosWindowSize * max(1.0, transform.scaleX) >= 0);
			assert(transform.offsetY - LanczosWindowSize * max(1.0, transform.scaleY) >= 0);
			assert((dst.width - 1) * transform.scaleX + LanczosWindowSize * max(1.0, transform.scaleX) + transform.offsetX < src.width);
			assert((dst.height - 1) * transform.scaleY + LanczosWindowSize * max(1.0, transform.scaleY) + transform.offsetY < src.height);

			switch (src.rawDataType)
			{
			case DT_U8:
				return SampleWithLanczosInternal<u8>(src, dst, transform, invalidValue.IsSet() ? invalidValue.GetValue().uint8[0] : 0);
			case DT_S16:
#if ALLOW_AMP
				return SampleWithLanczosInternalAMP(src, dst, transform, invalidValue.IsSet() ? invalidValue.GetValue().sint16[0] : 0);
#else
				return SampleWithLanczosInternal<s16>(src, dst, transform, invalidValue.IsSet() ? invalidValue.GetValue().sint16[0] : 0);
#endif
			case DT_U32:
				return SampleWithLanczosInternal<u32>(src, dst, transform, invalidValue.IsSet() ? invalidValue.GetValue().uint32[0] : 0);
			case DT_F32:
				return SampleWithLanczosInternal<f32>(src, dst, transform, invalidValue.IsSet() ? invalidValue.GetValue().float32[0] : 0);
			case DT_F64:
				return SampleWithLanczosInternal<f64>(src, dst, transform, invalidValue.IsSet() ? invalidValue.GetValue().float64 : 0);
			default:
				assert(false); // requested datatype not implemented yet, sorry
				break;
			}
		}

		template<typename T>
		bool IsImageCompletelyInvalid(const Image& img, const T invalidValue)
		{
			const T* imgPixels = (const T*)img.rawData;
			const T* imgPixelsEnd = (const T*)(img.rawData + img.rawDataSize);

			while (imgPixels < imgPixelsEnd)
			{
				const T imgPixel = *imgPixels;

				if (imgPixel != invalidValue)
				{
					return false;
				}

				imgPixels++;
			}

			return true;
		}

		bool IsImageCompletelyInvalid(const Image& img, const InvalidValue& invalidValue)
		{
			assert(invalidValue.IsSet());

			switch (img.rawDataType)
			{
			case DT_U8:
				return IsImageCompletelyInvalid<u8>(img, invalidValue.GetValue().uint8[0]);
			case DT_S16:
				return IsImageCompletelyInvalid<s16>(img, invalidValue.GetValue().sint16[0]);
			case DT_U32:
				return IsImageCompletelyInvalid<u32>(img, invalidValue.GetValue().uint32[0]);
			case DT_F32:
				return IsImageCompletelyInvalid<f32>(img, invalidValue.GetValue().float32[0]);
			case DT_F64:
				return IsImageCompletelyInvalid<f64>(img, invalidValue.GetValue().float64);
			default:
				assert(false); // requested datatype not implemented yet, sorry
				break;
			}

			return true;
		}
	}
}