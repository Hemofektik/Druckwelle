

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
		static const u8 ElevationFlag_RLE = 0xFE;
		static const u8 ElevationFlag_RelativeBulk = 0xFF;
		static const int MaxNumElementsPerRowSegment = (1 << (sizeof(u16) * 8)) - 1;
		bool CompressElevation(Image& img, const utils::InvalidValue& invalidValue)
		{
			assert(img.rawDataType == DT_S16);

			img.processedDataSize = 0;
			img.processedData = new u8[img.rawDataSize * 2 + img.height * sizeof(u32)];

			s16* elevation = (s16*)img.rawData;
			u8* compressedElevation = img.processedData + img.height * sizeof(u32);
			u8* compressedElevationRowOffset = img.processedData;

			struct RowSegment
			{
				u8 flag;
				u16 numElements;
				s16 startValue;
			};

			struct BigEndianU16
			{
				union
				{
					u16		value;
					struct
					{
#ifdef LITTLE_ENDIAN
						u8 l : 8;
						u8 u : 8;
#else
						u8 u : 8;
						u8 l : 8;
#endif
					};
				};

				BigEndianU16(u16 value) : value(value) {}
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
			};

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
					}
				}

				elevation = elevationRow;
				for (size_t s = 0; s < segments.size(); s++)
				{
					RowSegment& segment = segments[s];

					BigEndianU16 length(segment.numElements);

					compressedElevation[0] = segment.flag;
					compressedElevation[1] = length.u;
					compressedElevation[2] = length.l;
					compressedElevation += 3;

					if (segment.flag == ElevationFlag_RLE)
					{
						BigEndianU16 value(segment.startValue);

						compressedElevation[0] = value.u;
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
							compressedElevation[1] = reference.u;
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
			return false;
		}
	}
}