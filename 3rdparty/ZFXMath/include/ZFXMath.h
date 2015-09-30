/// \file
///
/// \if DE
///
/// @brief fügt alle Header zusammen
///
/// Erstellt von Florianx am 2004/08/04
/// Letzte Änderung: Version $Revision: $ am $Date: $ von $Author: $
///
/// \else
///
/// @brief include all headers
///
/// Created by Florianx at 2004/08/04
/// Last change: Version $Revision: $ at $Date: $ by $Author: $
///
/// \endif
///
/// CVS-Log:
/// \n $Log: $

/// \mainpage
/// \if DE
/// \htmlinclude main_de.htm
/// \else
/// \htmlinclude main_eng.htm
/// \endif

#ifndef	_ZFXMATH_INCLUDE_ZFXMATH_H_
#define	_ZFXMATH_INCLUDE_ZFXMATH_H_

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif

#include "ZFXTypes.h"

#include "BasicMath.h"
#include "Fixed.h"

#include "CubicBezier.h"

#include "ComplexN.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "VectorN.h"
#include "Vector4D.h"
#include "MultiplyStack.h"
#include "MatrixMxN.h"
#include "Matrix3x3.h"
#include "Matrix3x3Stack.h"
#include "Matrix4x4.h"
#include "Matrix4x4Stack.h"
#include "Quaternion.h"
#include "PackedMatrix.h"
#include "PackedMatrixStack.h"

#include "Geometry.h"
#include "Ray.h"
#include "Plane.h"
#include "Sphere.h"
#include "Frustum.h"
#include "AABB.h"
#include "OBB.h"
#include "Polygon.h"
#include "Triangle.h"
#include "SuperQuadric.h"
#include "Distribution.h"

#include "sh/SHDevice.h"
#include "ParametricCurve/ParametricCurve.h"
#include "ParametricSurface/ParametricSurface.h"

#endif //_ZFXMATH_INCLUDE_ZFXMATH_H_
