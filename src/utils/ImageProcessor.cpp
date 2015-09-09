
#include "ImageProcessor.h"

#include <algorithm>
#include <fstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <ZFXMath.h>

using namespace std;

namespace dw
{
	Image::Image(int width, int height, DataType dataType)
		: rawData(new u8[width * height * DataTypePixelSize[dataType]])
		, rawDataSize(width * height * DataTypePixelSize[dataType])
		, width(width)
		, height(height)
		, rawPixelSize(DataTypePixelSize[dataType])
		, rawDataType(dataType)
		, processedData(NULL)
		, processedDataSize(0)
		, processedContentType(CT_Unknown)
	{
	}

	Image::~Image()
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

		delete[] rawData;
	}

	bool Image::SaveToPNG(const astring& filename)
	{
		assert(rawDataType == DT_U8 || rawDataType == DT_RGBA8);
		if (utils::ConvertRawImageToContentType(*this, CT_Image_PNG))
		{
			ofstream file(filename, ios::out | ios::binary);
			if (file.is_open())
			{
				file.write((char*)processedData, processedDataSize);
			}
			file.close();

			return true;
		}
		return false;
	}

	template <typename srcType, typename dstType>
	static bool SaveToPNG(const astring& filename, std::function<dstType(srcType)> convert, Image& img)
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
	template <>	bool Image::SaveToPNG(const astring& filename, std::function<u8(s16)> convert) { return dw::SaveToPNG(filename, convert, *this); }
	template <>	bool Image::SaveToPNG(const astring& filename, std::function<u8(f32)> convert) { return dw::SaveToPNG(filename, convert, *this); }
	template <>	bool Image::SaveToPNG(const astring& filename, std::function<u8(f64)> convert) { return dw::SaveToPNG(filename, convert, *this); }

	namespace utils
	{
		bool ConvertRawImageToContentType(Image& image, ContentType contentType)
		{
			image.processedContentType = contentType;

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

			const int imgLanczosWindow = (int)ceil(LanczosWindowSize * transform.scaleY);
			const int tmpImageHeight = (int)ceil(dst.height * transform.scaleY) + imgLanczosWindow * 2;

			const int tmpImageOffsetY = (int)(transform.offsetY - imgLanczosWindow);
			const double invImgLanczosWindow = LanczosWindowSize / (double)imgLanczosWindow;

			Image horizontalLanczos(dst.width, tmpImageHeight, src.rawDataType);

			// horizontal pass
			{
				T* dstPixels = (T*)horizontalLanczos.rawData;
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
						for (int lx = -imgLanczosWindow + 1; lx < imgLanczosWindow; lx++)
						{
							T pixel = srcPixel[lx];
							if (pixel == invalidValue) continue;

							double l = EvalLanczos(LanczosWindowSize, (lx + xOffset) * invImgLanczosWindow );

							lanczosAcc += l;
							srcAcc += pixel * l;
						}

						dstPixels[y * width + x] = (lanczosAcc == 0.0) ? invalidValue : (T)(srcAcc / lanczosAcc);
					}
				}
			}

			// vertical pass
			{
				T* dstPixels = (T*)dst.rawData;
				const T* srcPixels = (const T*)horizontalLanczos.rawData;
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						double srcY = y * transform.scaleY + imgLanczosWindow;

						const int iSrcX = x;
						const int iSrcY = (int)floor(srcY);

						const double yOffset = iSrcY - srcY;

						const T* srcPixel = &srcPixels[iSrcY * width + iSrcX];

						double lanczosAcc = 0.0;
						double srcAcc = 0.0;
						for (int ly = -imgLanczosWindow; ly <= imgLanczosWindow; ly++)
						{
							T pixel = srcPixel[ly * width];
							if (pixel == invalidValue) continue;

							double l = EvalLanczos(LanczosWindowSize, (ly + yOffset) * invImgLanczosWindow);

							lanczosAcc += l;
							srcAcc += pixel * l;
						}

						dstPixels[y * width + x] = (lanczosAcc == 0.0) ? invalidValue : (T)(srcAcc / lanczosAcc);
					}
				}
			}
		}

		void SampleWithLanczos(const Image& src, Image& dst, const SampleTransform& transform, const InvalidValue* invalidValue)
		{
			assert(src.rawDataType == dst.rawDataType);
			assert(transform.offsetX - LanczosWindowSize * transform.scaleX >= 0);
			assert(transform.offsetY - LanczosWindowSize * transform.scaleY >= 0);
			assert((dst.width - 1 + LanczosWindowSize) * transform.scaleX + transform.offsetX < src.width);
			assert((dst.height - 1 + LanczosWindowSize) * transform.scaleY + transform.offsetY < src.height);

			switch (src.rawDataType)
			{
			case DT_U8:
				return SampleWithLanczosInternal<u8>(src, dst, transform, invalidValue ? invalidValue->value.u8[0] : 0);
			case DT_S16:
				return SampleWithLanczosInternal<s16>(src, dst, transform, invalidValue ? invalidValue->value.s16[0] : 0);
			case DT_U32:
				return SampleWithLanczosInternal<u32>(src, dst, transform, invalidValue ? invalidValue->value.u32[0] : 0);
			case DT_F32:
				return SampleWithLanczosInternal<f32>(src, dst, transform, invalidValue ? invalidValue->value.f32[0] : 0);
			case DT_F64:
				return SampleWithLanczosInternal<f64>(src, dst, transform, invalidValue ? invalidValue->value.f64 : 0);
			default:
				assert(false); // requested datatype not implemented yet, sorry
				break;
			}
		}
	}
}