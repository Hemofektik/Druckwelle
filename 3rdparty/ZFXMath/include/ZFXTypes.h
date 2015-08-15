/// \file
///
/// @brief Diverse Typdefinitionen
///
/// Funktionen:

#ifndef _ZFXMATH_INCLUDE_TYPES_H_
#define _ZFXMATH_INCLUDE_TYPES_H_

#if _MSC_VER >= 1300		// VC7

typedef __int8				Int8;
typedef unsigned __int8		UInt8;
typedef __int16				Int16;
typedef unsigned __int16	UInt16;
typedef __int32				Int32;
typedef unsigned __int32	UInt32;
typedef __int64				Int64;
typedef unsigned __int64	UInt64;

#else	// GCC or something else

typedef char				Int8;
typedef unsigned char		UInt8;
typedef short				Int16;
typedef unsigned short		UInt16;
typedef int					Int32;
typedef unsigned int		UInt32;
typedef long long			Int64;
typedef unsigned long long	UInt64;

#endif


#endif
