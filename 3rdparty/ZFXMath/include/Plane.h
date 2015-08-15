// vonn oese

#ifndef _ZFXMATH_INCLUDE_PLANE_H_
#define _ZFXMATH_INCLUDE_PLANE_H_

namespace ZFXMath
{
	template<typename T> struct TVector3D;
	template<typename T> struct TVector4D;
	template<typename T> struct TMatrix3x3;
	template<typename T> struct TMatrix4x4;
	template<typename T> struct TAABB;

	// Ebene gegeben durch ax + by + cz + d = 0
	template<typename T>
	struct TPlane
	{
		union
		{
			struct
			{
				T val[4];
			};

			struct
			{
				T a, b, c, d;
			};

			struct
			{
				TVector4D<T> vec;
			};
		};

		TPlane(){}

		TPlane(const TPlane &pl):vec(pl.vec){}

		TPlane(const TVector3D<T> &v, const T &dst) : vec(v, dst){}
		TPlane(const TVector4D<T> &v) : vec(v) {}

		TPlane(const T &a, const T &b, const T &c, const T &d):a(a),b(b),c(c),d(d){}

		TPlane(const TVector3D<T> &A, const TVector3D<T> &B, const TVector3D<T> &C)
		{
			TVector3D<T> n = (B-A).CrossProduct(C-A);
			n.Normalize();
			vec = TVector4D<T>( n, n.DotProduct(A) );
		}

		TPlane(const TVector3D<T> &P,const TVector3D<T> &N)
		{
			TVector3D<T> n = N;
			vec = TVector4D<T>( n, n.DotProduct(P) );
		}

		TPlane(const T *p)
		{
			val[0] = p[0];
			val[1] = p[1];
			val[2] = p[2];
			val[3] = p[3];
		}

		TPlane &SwapFace()
		{
			vec = -vec;
			return (*this);
		}

		TPlane &Normalize()
		{
			T factor = T(1.0) /TVector3D<T>(vec).Length();
			vec *= factor;
			vec *= factor;
			return (*this);
		}

		T GetDistance(const TVector3D<T> &v)const
		{
			return vec.DotProduct( v );
		}

		// Transformationen solten besser vermieden werden, besser mit transformierten punkten neue ebene erstellen
		TPlane Transform(const TMatrix3x3<T> &m, const TMatrix3x3<T> &ti) const
		{
			TVector3D<T> pt, tpt, tnm;
			pt = TVector3D<T>( vec ) * d;
			tpt = m * pt;
			tnm = ti * TVector3D<T>( vec );
			tnm.Normalize();
			return TPlane(tpt, tnm);
		}

		TPlane Transform(const TMatrix3x3<T> &m)const
		{
			TMatrix3x3<T> ti = m.Inverse().Transpose();
			return Transform(m,ti);
		}

		TPlane Transform(const TMatrix4x4<T> &m, const TMatrix4x4<T> &ti)const
		{
			TVector3D<T> pt, tpt, tnm;
			pt = TVector3D<T>( vec ) * d;
			tpt = m * pt;
			tnm = ti * TVector3D<T>( vec );
			tnm.Normalize();
			return TPlane(tpt, tnm);
		}

		TPlane Transform(const TMatrix4x4<T> &m)const
		{
			TMatrix4x4<T> ti = m.Inverse().Transpose();
			return Transform(m,ti);
		}

		MathResult Check(const TVector3D<T> &v)
		{
			switch((int)Sign(GetDistance(v)))
			{
			case -1: return BACKFACE;
			case  0: return INTERSECTION;
			case  1: return FRONTFACE;
			default: return NONE;
			}
		}

		MathResult Check(const TAABB<T> &aabb)
		{
			TVector3D<T> points[8];
			MathResult res = NONE;
			aabb.GetPoints(points);
			for(int i = 0; i < 8; i++)
			{
				if(!res) res = Check(points[i]);
				else if(MathResult newres = Check(points[i]) != res)return INTERSECTION;
			}
			return res;
		}
	};

}

#endif
