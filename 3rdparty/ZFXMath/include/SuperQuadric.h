/// \file
///
/// \if DE
/// @brief TSuperQuadric
///
/// TSuperQuadric: parametrisierter geschlossener 3D Körper
/// \else
/// @brief TSuperQuadric
///
/// TSuperQuadric: parameterized closed 3D shape
/// \endif


#ifndef _ZFXMATH_INCLUDE_SUPPERQUADRIC_H_
#define _ZFXMATH_INCLUDE_SUPPERQUADRIC_H_

namespace ZFXMath
{

	/// \if DE
	/// @brief TSuperQuadric
	///
	/// TSuperQuadric: parametrisierter geschlossener 3D Körper
	/// \else
	/// @brief TSuperQuadric
	///
	/// TSuperQuadric: parameterized closed 3D shape
	/// \endif
	template<typename T>
	struct TSuperQuadric
	{

		union 
		{
			struct 
			{
				T n;					//! North-South Roundness/Squareness Factor
				T e;					//! East-West Roundness/Squareness Factor
				T toroidRadius;			//! For generating toroids. This is the inner radius
			};
			struct
			{
				TVector3D<T> parameter;
			};
		};

		TSuperQuadric() {};

		TSuperQuadric( const T& n, const T& e, const T& toroidRadius = (T)0.0 ) : n(n), e(e), toroidRadius(toroidRadius) {}


		TVector3D<T> EvalSurfacePosition( const T& u, const T& v )
		{
			T uRad = ( u - (T)0.5 ) * (T)PI;
			if( toroidRadius > (T)0.0 ) uRad *= (T)2.0;
			T vRad = v * (T)2.0 * (T)PI - (T)PI;

			TVector3D<T> pos;

			pos.x = ( toroidRadius + sqC( uRad, e ) ) * sqS( vRad, n );
			pos.y = sqS( uRad, e );
			pos.z = ( toroidRadius + sqC( uRad, e ) ) * sqC( vRad, n );

			return pos;
		}

		TVector3D<T> EvalSurfaceNormal( const T& u, const T& v )
		{
			T uRad = ( u - (T)0.5 ) * (T)PI;
			if( toroidRadius > (T)0.0 ) uRad *= (T)2.0;
			T vRad = v * (T)2.0 * (T)PI - (T)PI;

			TVector3D<T> normal;

			normal.x = sqC( uRad, (T)2.0 - e) * sqS( vRad, (T)2.0 - n );
			normal.y = sqS( uRad, (T)2.0 - e );
			normal.z = sqC( uRad, (T)2.0 - e) * sqC( vRad, (T)2.0 - n );

			return normal;
		}

		/// \if DE
		/// @brief Testet ob der punkt p sich innerhalb der Superquadrik befindet
		///
		/// Gibt 1 zurück wenn p auf der Oberfläche liegt, > 1 wenn p sich außerhalb
		/// und < 1 wenn p sich innerhalb des körpers befindet
		/// \else
		/// @brief Tests to see if point p is inside the SuperQuadric
		///
		/// Returns 1 if on the surface, > 1 if outside the surface, or
		/// < 1 if inside the surface
		/// \endif
		T GetDistance( TVector3D<T> p ) 
		{
			T result;

			result =	Pow(
						Pow(
						Pow( p.x, (T)2.0 / e ) +
						Pow( p.z , (T)2.0 / e ), e / (T)2.0 ) - toroidRadius, (T)2.0 / n ) +
						Pow( p.y , (T)2.0 / n );

			return result;
		}


	private:

		T sqC ( T v, T n ) 
		{
			return Sign( cos(v) ) * Pow( Abs( Cos(v) ), n );
		}

		T sqS ( T v, T n ) 
		{
		   return Sign( Sin(v) ) * Pow( Abs( Sin(v) ), n );
		}
	};



}


#endif // _ZFXMATH_INCLUDE_SUPPERQUADRIC_H_
