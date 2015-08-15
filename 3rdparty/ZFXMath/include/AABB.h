// von oese

#ifndef _ZFXMATH_INCLUDE_AABB_H_
#define _ZFXMATH_INCLUDE_AABB_H_

namespace ZFXMath
{
	template<typename T>struct TVector3D;
	template<typename T>struct TPlane;

	template<typename T>
	struct TAABB
	{
		TVector3D<T> min, max;

		TAABB(){}

		TAABB(const TVector3D<T> &a, const TVector3D<T> &b)
		{
			min = TVector3D<T>::Min(a,b);
			max = TVector3D<T>::Max(a,b);
		}

		TAABB& operator += ( const TVector3D<T>& p ) 
		{
			min = TVector3D<T>::Min( min, p );
			max = TVector3D<T>::Max( max, p );
			return *this; 
		}

		TAABB& operator += ( const TAABB& aabb ) 
		{
			min = TVector3D<T>::Min( min, aabb.min );
			max = TVector3D<T>::Max( max, aabb.max );
			return *this; 
		}

		TAABB& operator *= ( const TPackedMatrix<T>& trans )
		{
			return this->Transform( trans );
		}

		TAABB operator * ( const TPackedMatrix<T>& trans ) const
		{
			TAABB aabb = *this;
			return aabb.Transform( trans );
		}

		TAABB& Transform( const TPackedMatrix<T>& trans ) 
		{
			TVector3D<T> _min = min;
			TVector3D<T> _max = max;
			*this = TAABB( trans * TVector4D<T>( _min.x, _min.y, _min.z, 1.0 ),
						   trans * TVector4D<T>( _min.x, _min.y, _max.z, 1.0 ) );
			*this += trans * TVector4D<T>( _min.x, _max.y, _min.z, 1.0 );
			*this += trans * TVector4D<T>( _min.x, _max.y, _max.z, 1.0 );
			*this += trans * TVector4D<T>( _max.x, _min.y, _min.z, 1.0 );
			*this += trans * TVector4D<T>( _max.x, _min.y, _max.z, 1.0 );
			*this += trans * TVector4D<T>( _max.x, _max.y, _min.z, 1.0 );
			*this += trans * TVector4D<T>( _max.x, _max.y, _max.z, 1.0 );
			return *this;
		}

		TVector3D<T> GetCenter() const
		{
			return (max + min) * (T)0.5;
		}

		TVector3D<T> GetExtents() const
		{
			return (max - min) * (T)0.5;
		}

		T GetExtent() const
		{
			return GetExtents().Length();
		}

		//! distance squared to a point from the box (arvos algorithm)
		T GetDistanceSqr( const TVector3D<T>& p ) const
		{ 
			T dstSqR = (T)0.0;

			//for each component, find the point's relative position and the distance contribution
			for ( int n = 0; n < 3; n++ )
			{
				T localDist =
						( p(n) < min.val[n] ) ? ( p(n) - min.val[n] ) : 
						( p(n) > max.val[n] ) ? ( p(n) - max.val[n] ) : (T)0.0;

				dstSqR += localDist * localDist;
			}

			return dstSqR; 
		}

		int GetPlanes(TPlane<T> *target) const
		{
			if(target)
			{
				target[0] = TPlane<T>(min,-TVector3D<T>::XAxis());
				target[1] = TPlane<T>(min,-TVector3D<T>::YAxis());
				target[2] = TPlane<T>(min,-TVector3D<T>::ZAxis());
				target[3] = TPlane<T>(max, TVector3D<T>::XAxis());
				target[4] = TPlane<T>(max, TVector3D<T>::YAxis());
				target[5] = TPlane<T>(max, TVector3D<T>::ZAxis());
			}
			return 6;
		}

		int GetPoints(TVector3D<T>* target)const
		{
			if(target)
			{
				target[0] = min;
				target[1] = TVector3D<T>(max.x,min.y,min.z);
				target[2] = TVector3D<T>(max.x,min.y,max.z);
				target[3] = TVector3D<T>(min.x,min.y,max.z);
				target[4] = TVector3D<T>(min.x,max.y,min.z);
				target[5] = TVector3D<T>(max.x,max.y,min.z);
				target[6] = max;
				target[7] = TVector3D<T>(min.x,max.y,max.z);
			}
			return 8;
		}

		MathResult Check(const TVector3D<T>& v)const
		{
			return ( ( min <= v ) && ( v <= max ) ) ? INSIDE : OUTSIDE;
		}

		MathResult Check(const TPlane<T>& plane)const
		{
			if(INTERSECTION == plane.Check(this))return INTERSECTION;
			return NONE;
		}

		MathResult Check(const TAABB& aabb, bool solid = true) const
		{
			if(min.x > aabb.max.x)return NONE;
			if(min.y > aabb.max.y)return NONE;
			if(min.z > aabb.max.z)return NONE;
			if(max.x < aabb.min.x)return NONE;
			if(max.y < aabb.min.y)return NONE;
			if(max.z < aabb.min.z)return NONE;
			if(!solid)
			{
				MathResult rmin = Check(aabb.min), rmax = Check(aabb.max);
				if(OUTSIDE == rmin && OUTSIDE == rmax)return SURROUNDING;
				if(INSIDE == rmin && INSIDE == rmax)return INSIDE;
			}
			return INTERSECTION;
		}

		MathResult Check( const TSphere<T>& sphere ) const
		{
			return ( GetDistanceSqr(sphere.position) <= sphere.radius * sphere.radius ) ? INTERSECTION : NONE;
		}


		MathResult Check( const TRay<T>& ray ) const
		{
			TVector3D<T> boxCenter = GetCenter();
			TVector3D<T> boxExtents = GetExtents();
			TVector3D<T> rayDir = ray.dir * ray.len * (T)0.5;
			TVector3D<T> diff = ( ray.pos + rayDir ) - boxCenter;

			T fAWdU[3];
			fAWdU[0] = Abs(rayDir.x);
			fAWdU[1] = Abs(rayDir.y);
			fAWdU[2] = Abs(rayDir.z);

			if( Abs(diff.x) > boxExtents.x + fAWdU[0] )	return OUTSIDE;
			if( Abs(diff.y) > boxExtents.y + fAWdU[1] )	return OUTSIDE;
			if( Abs(diff.z) > boxExtents.z + fAWdU[2] )	return OUTSIDE;

			T f;
			f = rayDir.y * diff.z - rayDir.z * diff.y;	if( Abs(f) > boxExtents.y * fAWdU[2] + boxExtents.z * fAWdU[1] )	return OUTSIDE;
			f = rayDir.z * diff.x - rayDir.x * diff.z;	if( Abs(f) > boxExtents.x * fAWdU[2] + boxExtents.z * fAWdU[0] )	return OUTSIDE;
			f = rayDir.x * diff.y - rayDir.y * diff.x;	if( Abs(f) > boxExtents.x * fAWdU[1] + boxExtents.y * fAWdU[0] )	return OUTSIDE;

			return INTERSECTION;
		}

		MathResult Check( const TFrustum<T>& frustum ) const
		{
			TVector3D<T>	iMin, iMax;
			MathResult		result = NONE;

			for( int n = 0; n < 6 ; n++ )
			{
				if ( frustum.plane[n].a >= (T)0.0 )		// X-Coordinate
				{
					iMin.x = min.x;
					iMax.x = max.x;
				}
				else
				{
					iMin.x = max.x;
					iMax.x = min.x;
				}
				if ( frustum.plane[n].b >= (T)0.0 )		// Y-Coordinate
				{
					iMin.y = min.y;
					iMax.y = max.y;
				}
				else
				{
					iMin.y = max.y;
					iMax.y = min.y;
				}
				if ( frustum.plane[n].c >= (T)0.0 )		// Z-Coordinate
				{
					iMin.z = min.z;
					iMax.z = max.z;
				}
				else
				{
					iMin.z = max.z;
					iMax.z = min.z;
				}

				if ( frustum.plane[n].GetDistance( iMin ) > (T)0.0) return NONE; // outside
				if ( frustum.plane[n].GetDistance( iMax ) >= (T)0.0) result = INTERSECTION;	// partial within
			}

			if ( result == INTERSECTION ) return INTERSECTION;
			return SURROUNDING;
		}

		
	};
}

#endif
