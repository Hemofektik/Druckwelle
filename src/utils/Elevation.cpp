

#include <algorithm>
#include <fstream>
#include <vector>

#include <ZFXMath.h>

#include "Elevation.h"

using namespace std;
using namespace ZFXMath;



namespace dw
{
	namespace utils
	{
		struct BigEndianU16
		{
			union
			{
				u16		value;
				struct
				{
#ifdef LITTLE_ENDIAN
					u8 l : 8;
					u8 h : 8;
#else
					u8 h : 8;
					u8 l : 8;
#endif
				};
			};

			BigEndianU16(u16 value) : value(value) {}
			BigEndianU16(u8 ch1, u8 ch0)
#ifdef LITTLE_ENDIAN
				: l(ch0), h(ch1)
#else
				: h(ch1), l(ch0)
#endif
			{}
		};

		struct BigEndianU32
		{
			union
			{
				u32		value;
				struct
				{
#ifdef LITTLE_ENDIAN
					u8 b0 : 8;
					u8 b1 : 8;
					u8 b2 : 8;
					u8 b3 : 8;
#else
					u8 b3 : 8;
					u8 b2 : 8;
					u8 b1 : 8;
					u8 b0 : 8;
#endif
				};
			};

			BigEndianU32(u32 value) : value(value) {}
			BigEndianU32(u8 ch3, u8 ch2, u8 ch1, u8 ch0)
#ifdef LITTLE_ENDIAN
				: b0(ch0), b1(ch1), b2(ch2), b3(ch3)
#else
				: b3(ch3), b2(ch2), b1(ch1), b0(ch0)
#endif
			{}

			static BigEndianU32 MakeFourCC(u8 ch0, u8 ch1, u8 ch2, u8 ch3)
			{
				return BigEndianU32(ch0, ch1, ch2, ch3);
			}

			bool operator==(const BigEndianU32& other) const
			{
				return other.value == value;
			}
			bool operator!=(const BigEndianU32& other) const
			{
				return other.value != value;
			}
		};

		static const BigEndianU32 CompressedElevationModelFourCC = BigEndianU32::MakeFourCC('C', 'E', 'M', 1);  // version 1

		struct ElevationHeader
		{
			BigEndianU32 fourCC;
			BigEndianU32 width;
			BigEndianU32 height;

			ElevationHeader(BigEndianU32 fourCC, u32 width, u32 height)
				: fourCC(fourCC)
				, width(width)
				, height(height)
			{
			}
		};

		struct RowSegment
		{
			u8 flag;
			u16 numElements;
			s16 startValue;
		};

		static const u8 ElevationFlag_RLE = 0xFE;
		static const u8 ElevationFlag_RelativeBulk = 0xFF;
		static const int MaxNumElementsPerRowSegment = (1 << (sizeof(u16) * 8)) - 1;
		bool CompressElevation(Image& img, const utils::InvalidValue& invalidValue)
		{
			assert(img.rawDataType == DT_S16);

			img.processedDataSize = 0;
			img.processedData = new u8[sizeof(ElevationHeader) + img.rawDataSize * 2 + img.height * sizeof(u32)];

			s16* elevation = (s16*)img.rawData;
			u8* compressedElevation = img.processedData + sizeof(ElevationHeader) + img.height * sizeof(u32);
			u8* compressedElevationRowOffset = img.processedData + sizeof(ElevationHeader);

			ElevationHeader header(CompressedElevationModelFourCC, img.width, img.height);

			img.processedData[0] = header.fourCC.b3;
			img.processedData[1] = header.fourCC.b2;
			img.processedData[2] = header.fourCC.b1;
			img.processedData[3] = header.fourCC.b0;

			img.processedData[4] = header.width.b3;
			img.processedData[5] = header.width.b2;
			img.processedData[6] = header.width.b1;
			img.processedData[7] = header.width.b0;

			img.processedData[8] = header.height.b3;
			img.processedData[9] = header.height.b2;
			img.processedData[10] = header.height.b1;
			img.processedData[11] = header.height.b0;

			vector<RowSegment> segments(img.width / 2);
			for (int y = 0; y < img.height; y++)
			{
				BigEndianU32 offset((u32)(compressedElevation - img.processedData));
				compressedElevationRowOffset[y * 4 + 0] = offset.b3;
				compressedElevationRowOffset[y * 4 + 1] = offset.b2;
				compressedElevationRowOffset[y * 4 + 2] = offset.b1;
				compressedElevationRowOffset[y * 4 + 3] = offset.b0;

				segments.clear();

				// approximate segments to consider
				s16* elevationRow = elevation;
				const s16* elevationRowEnd = elevation + img.width;
				while (elevation < elevationRowEnd)
				{
					RowSegment segment;
					segment.flag = 0;
					segment.numElements = 1;

					bool hasLastValue = false;
					s16 lastValue = 0;
					while (elevation < elevationRowEnd)
					{
						s16 value = *elevation;

						if (!hasLastValue)
						{
							lastValue = value;
							hasLastValue = true;
							segment.startValue = value;
						}
						else
						{
							u8 flag = (value == lastValue) ? ElevationFlag_RLE : ElevationFlag_RelativeBulk;
							if (segment.flag == 0)
							{
								segment.flag = flag;
							}
							else if (segment.flag != flag)
							{
								if (segment.flag == ElevationFlag_RelativeBulk &&
									flag == ElevationFlag_RLE) // we switch to RLE which means we already consumed a value which belongs to the new RLE segment
								{
									elevation--;
									segment.numElements--;
								}
								break;
							}

							lastValue = value;
							segment.numElements++;
						}

						elevation++;

						if (segment.numElements == MaxNumElementsPerRowSegment)
						{
							break;
						}
					}

					if (segment.numElements == 1)
					{
						segment.flag = ElevationFlag_RLE;
					}

					segments.push_back(segment);
				}

				// remove small RLE segments as they better off to be merged with their relative offset neighbor segments
				for (int s = (int)segments.size() - 1; s >= 0; s--)
				{
					RowSegment& segment = segments[s];

					if (segment.flag != ElevationFlag_RLE ||
						segment.numElements > 5 ||
						(invalidValue.IsSet() && (segment.startValue == invalidValue.GetValue().sint16[0])))
					{
						continue;
					}

					// merge into next segment
					if (s < segments.size() - 2 && segments[s + 1].flag == ElevationFlag_RelativeBulk)
					{
						segments[s + 1].numElements += segment.numElements;
						segments.erase(segments.begin() + s);
					}

					// merge into previous segment
					if (s > 0 && segments[s - 1].flag == ElevationFlag_RelativeBulk)
					{
						segments[s - 1].numElements += segments[s].numElements;
						segments.erase(segments.begin() + s);

						if (s < segments.size()) // reevaluate next segment
						{
							s++;
						}
					}
				}

				elevation = elevationRow;
				for (size_t s = 0; s < segments.size(); s++)
				{
					RowSegment& segment = segments[s];

					BigEndianU16 length(segment.numElements);

					compressedElevation[0] = segment.flag;
					compressedElevation[1] = length.h;
					compressedElevation[2] = length.l;
					compressedElevation += 3;

					if (segment.flag == ElevationFlag_RLE)
					{
						BigEndianU16 value(segment.startValue);

						compressedElevation[0] = value.h;
						compressedElevation[1] = value.l;

						compressedElevation += 2;

						elevation += segment.numElements;
					}
					else if (segment.flag == ElevationFlag_RelativeBulk)
					{
						s16* elevationSegmentStart = elevation;
						s16* elevationSegmentEnd = elevation + segment.numElements;

						while (elevation < elevationSegmentEnd)
						{
							s16* elevationSubSegmentStart = elevation;

							s16 min = *elevation;
							s16 max = min;

							elevation++;
							while (elevation < elevationSegmentEnd)
							{
								s16 value = *elevation;
								s16 newMin = Min(min, value);
								s16 newMax = Max(max, value);

								if (newMax - newMin >= ElevationFlag_RelativeBulk)
								{
									break;
								}

								min = newMin;
								max = newMax;
								elevation++;
							}

							BigEndianU16 reference(min);
							compressedElevation[0] = ElevationFlag_RelativeBulk;
							compressedElevation[1] = reference.h;
							compressedElevation[2] = reference.l;
							compressedElevation += 3;

							s16* elevationSubSegmentEnd = elevation;
							elevation = elevationSubSegmentStart;
							while (elevation < elevationSubSegmentEnd)
							{
								*compressedElevation = (u8)(*elevation - min);
								compressedElevation++;
								elevation++;
							}
						}
					}
				}
			}

			img.processedDataSize = (compressedElevation - img.processedData);

			return true;
		}

		bool DecompressElevation(Image& img)
		{
			u8* header = img.processedData;
			BigEndianU32 fourCC(header[0], header[1], header[2], header[3]);

			if (fourCC != CompressedElevationModelFourCC)
			{
				return false;
			}

			BigEndianU32 width(header[4], header[5], header[6], header[7]);
			BigEndianU32 height(header[8], header[9], header[10], header[11]);

			img.AllocateRawData(width.value, height.value, DT_S16);

			u8* compressedElevationRowOffset = img.processedData + sizeof(ElevationHeader);
			s16* imgDataRaw = (s16*)img.rawData;

			for (int y = 0; y < img.height; y++)
			{
				u8* rowOffset = compressedElevationRowOffset + sizeof(u32) * y;
				BigEndianU32 offset(rowOffset[0], rowOffset[1], rowOffset[2], rowOffset[3]);

				u8* rowDataCompressed = &img.processedData[offset.value];
				s16* imgRowDataRaw = &imgDataRaw[y * img.width];
				s16* imgRowDataRawEnd = imgRowDataRaw + img.width;

				while (imgRowDataRaw < imgRowDataRawEnd)
				{
					u8 elevationFlag = rowDataCompressed[0];
					u8 numElementsH = rowDataCompressed[1];
					u8 numElementsL = rowDataCompressed[2];
					rowDataCompressed += 3;

					const BigEndianU16 numElements(numElementsH, numElementsL);

					if (imgRowDataRaw + numElements.value > imgRowDataRawEnd)
					{
						return false; // file is corrupt
					}

					if (elevationFlag == ElevationFlag_RLE)
					{
						u8 valueH = rowDataCompressed[0];
						u8 valueL = rowDataCompressed[1];
						rowDataCompressed += 2;

						const BigEndianU16 value(valueH, valueL);

						for (int e = 0; e < numElements.value; e++)
						{
							*imgRowDataRaw = value.value;
							imgRowDataRaw++;
						}
					}
					else if (elevationFlag == ElevationFlag_RelativeBulk)
					{
						s16 referenceValue = 0;
						for (int e = 0; e < numElements.value; e++)
						{
							if (rowDataCompressed[0] == ElevationFlag_RelativeBulk)
							{
								u8 referenceValueH = rowDataCompressed[1];
								u8 referenceValueL = rowDataCompressed[2];
								rowDataCompressed += 3;

								const BigEndianU16 value(referenceValueH, referenceValueL);
								referenceValue = value.value;
							}

							*imgRowDataRaw = referenceValue + rowDataCompressed[0];
							imgRowDataRaw++;
							rowDataCompressed++;
						}
					}
				}
			}

			return true;
		}
	}
}

