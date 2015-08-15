#ifndef _ZFXMATH_INCLUDE_QUATERNION_H_
#define _ZFXMATH_INCLUDE_QUATERNION_H_

namespace ZFXMath
{
	template<typename T>
	struct TQuaternion
	{
		union
		{
			struct
			{
				T	val[4];
			};
			struct
			{
				T	x, y, z, w;
			};
			struct
			{
				TVector3D<T>	v;
				T				r;
			};
		};

		TQuaternion(){}
		TQuaternion(const T &_w):w(_w){}
		TQuaternion(const TVector3D<T> &v,const T &w):v(v),w(w){}
		TQuaternion(const T &x,const T &y, const T &z, const T &w):x(x),y(y),z(z),w(w){}
		TQuaternion(const T *p):x(p[0]),y(p[1]),z(p[2]),w(p[3]){}
		TQuaternion(const TComplexN<T> &cplx):x(cplx.im),w(cplx.re){}
		TQuaternion(const TQuaternion &q):x(q.x),y(q.y),z(q.z),w(q.w){}

		TQuaternion( const TMatrix3x3<T>& rotMat )
		{
			T trace = rotMat[0][0] + rotMat[1][1] + rotMat[2][2];
			T root;

			if ( trace > T(0.0) )
			{
				// |w| > 1/2, may as well choose w > 1/2
				root = Sqrt(trace + (T)1.0);				// 2w
				w = (T)0.5 * root;
				root = (T)0.5 / root;						// 1 / (4w)
				x = (rotMat[1][2] - rotMat[2][1]) * root;
				y = (rotMat[2][0] - rotMat[0][2]) * root;
				z = (rotMat[0][1] - rotMat[1][0]) * root;
			}
			else
			{
				// |w| <= 1/2
				static int Next[3] = { 1, 2, 0 };
				int i = 0;
				if (rotMat[1][1] > rotMat[0][0])
				{
					i = 1;
				}
				if (rotMat[2][2] > rotMat[i][i])
				{
					i = 2;
				}
				int j = Next[i];
				int k = Next[j];

				root = Sqrt(rotMat[i][i] - rotMat[j][j] - rotMat[k][k] + (T)1.0);
				T* apkQuat[3] = { &x, &y, &z };
				*apkQuat[i] = (T)0.5 * root;
				root = (T)0.5 / root;
				w = (rotMat[j][k] - rotMat[k][j]) * root;
				*apkQuat[j] = (rotMat[i][j] + rotMat[j][i]) * root;
				*apkQuat[k] = (rotMat[i][k] + rotMat[k][i]) * root;
			}
		}

		/// \if DE
		/// @brief automatische Typkonvertierung
		/// \else
		/// @brief auto coercion
		/// \endif
		template<typename T2>
		operator TQuaternion<T2>() const
		{
			return TQuaternion<T2>( (T2)x, (T2)y, (T2)z, (T2)w );
		}

		T& operator()(int i){return val[Check(i)];}
		const T& operator()(int i)const{return val[Check(i)];}

		TQuaternion operator + ()const {return (*this);}
		TQuaternion operator + (const TQuaternion &q) const { return TQuaternion( v + q.v, w + q.w ); }
		TQuaternion& operator += (const TQuaternion &q) {v += q.v; w += q.w; return (*this);}

		TQuaternion operator - () const { return Negate(); }
		TQuaternion operator - (const TQuaternion &q)const {return TQuaternion(v - q.v, w - v.w);}
		TQuaternion& operator -= (const TQuaternion &q) {(*this) += -q;return (*this);}

		TQuaternion operator * (const T &a)const {return TQuaternion(v * a, w * a);}
		TQuaternion& operator *= (const T &a) {v *= a;w *= a; return (*this);}
		friend TQuaternion<T> operator * (const T &a, const TQuaternion<T> &q){return q * a;}
		TQuaternion operator * (const TQuaternion &q) const { return Multiply(q);}
		TQuaternion& operator *= (const TQuaternion &q) {(*this) = Multiply(q); return (*this);}

		TQuaternion operator / (const T &a)const {return TQuaternion(v / a, w / a);}
		TQuaternion& operator /= (const T &a)const {return (*this) = (*this) / a;}
		friend TQuaternion<T> operator / (const T &a, const TQuaternion<T> &q){return a * q.Inverse();}
		TQuaternion operator / (const TQuaternion &q)const {return (*this) * q.Inverse();}
		TQuaternion& operator /= (const TQuaternion &q) {return (*this) = (*this) / q;}

		TQuaternion operator ~ () const { return Conjugate(); }

		TQuaternion Negate() const
		{
			return TQuaternion(-v,-w);
		}

		T Magnitude() const
		{
			return Sqrt(x*x+y*y+z*z+w*w);
		}

		T SqrMagnitude() const
		{
			return x*x+y*y+z*z+w*w;
		}

		TQuaternion Multiply(const TQuaternion<T> &q) const
		{
			TQuaternion ret;

			ret.w = q.w * w - q.v.DotProduct(v);
			ret.v = q.v * w + q.w * v + q.v.CrossProduct(v);

			return ret;
		}

		TVector3D<T> operator * ( const TVector3D<T>& v ) const
		{
			return Transform( v );
		}

		friend TVector3D<T> operator *= ( TVector3D<T>& v , const TQuaternion<T> &q )
		{
			v = q * v;
			return v;
		}

		TVector3D<T> Transform( const TVector3D<T>& vec ) const
		{
			return 2.0 * ( w * v.CrossProduct(vec) + vec.DotProduct(v) * v ) + ( w * w - v.DotProduct(v) ) * vec;
		}

		TQuaternion Conjugate() const
		{
			return TQuaternion( -x, -y, -z, w );
		}

		TQuaternion Inverse() const
		{
			return Conjugate();
		}

		static TQuaternion Rotation(const TVector3D<T> &rv, const T &factor = 1.0)
		{
			TQuaternion q;
			T angle = rv.GetLength();
			q.v = rv / angle;
			angle *= factor;
			q.w = Cos(angle);
			q.v *= Sin(angle);
			return q;
		}

		static TQuaternion RotationAxis(const TVector3D<T> &rv, const T &angle)
		{
			T halfAngle = angle * (T)0.5;
			return TQuaternion( rv * Sin( halfAngle ), Cos( halfAngle ) );
		}

		static TQuaternion Identity()
		{
			return TQuaternion( (T)0.0, (T)0.0, (T)0.0, (T)1.0 );
		}

		static TQuaternion LookAtLH( const TVector3D<T>& dir, const TVector3D<T>& up )
		{
			return TQuaternion<T>( TMatrix3x3<T>::LookAtLH( dir, up ) );
		}

		static TQuaternion LookAtX()
		{
			return TQuaternion<T>( TMatrix3x3<T>::LookAtLH( TVector3D<T>( (T)1.0, (T)0.0, (T)0.0 ), TVector3D<T>( (T)0.0, (T)1.0, (T)0.0 ) ) );
		}

		static TQuaternion LookAtY()
		{
			return TQuaternion<T>( TMatrix3x3<T>::LookAtLH( TVector3D<T>( (T)0.0, (T)1.0, (T)0.0 ), TVector3D<T>( (T)0.0, (T)0.0, (T)-1.0 ) ) );
		}

		static TQuaternion LookAtZ()
		{
			return TQuaternion<T>( TMatrix3x3<T>::LookAtLH( TVector3D<T>( (T)0.0, (T)0.0, (T)1.0 ), TVector3D<T>( (T)0.0, (T)1.0, (T)0.0 ) ) );
		}

		TVector3D<T> GetXVector() const
		{
			T Y2 = y + y;
			T Z2 = z + z;

			T yy = y * Y2;
			T zz = z * Z2;

			T xy = x * Y2;
			T wz = w * Z2;

			T xz = x * Z2;
			T wy = w * Y2;

			return TVector3D<T>( (T)1.0 - (yy + zz), xy + wz, xz - wy );
		}

		TVector3D<T> GetYVector() const
		{
			T X2 = x + x;
			T Y2 = y + y;
			T Z2 = z + z;

			T xy = x * Y2;
			T wz = w * Z2;

			T xx = x * X2;
			T zz = z * Z2;

			T wx = w * X2;
			T yz = y * Z2;

			return TVector3D<T>( xy - wz, (T)1.0 - (xx + zz), yz + wx );
		}

		TVector3D<T> GetZVector() const
		{
			T X2 = x + x;
			T Y2 = y + y;
			T Z2 = z + z;

			T wy = w * Y2;
			T xz = x * Z2;

			T yz = y * Z2;
			T wx = w * X2;

			T xx = x * X2;
			T yy = y * Y2;

			return TVector3D<T>( xz + wy, yz - wx, (T)1.0 - (xx + yy) );
		}

		static TQuaternion PitchYawRoll( const TVector3D<T>& pyr )
		{
			T cosPitch = Cos(pyr.x * (T)0.5);
			T sinPitch = Sin(pyr.x * (T)0.5);
			T cosYaw = Cos(pyr.y * (T)0.5);
			T sinYaw = Sin(pyr.y * (T)0.5);
			T cosRoll = Cos(pyr.z * (T)0.5);
			T sinRoll = Sin(pyr.z * (T)0.5);
			return TQuaternion(	cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
								cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw,
								sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw,
								cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw);

		}

		T GetPitch() const
		{
			return ATan2( (T)2.0 * ( y * z + w * x ), (T)1.0 - (T)2.0 * ( x * x + y * y ) );
		}

		T GetYaw() const
		{
			return ASin( (T)2.0 * ( w * y - x * z ) );
		}

		T GetRoll() const
		{
            return ATan2( (T)2.0 * ( x * y + w * z ), (T)1.0 - (T)2.0 * ( y * y + z * z ) );
		}

	private:

		int Check(int i) const
		{
			assert(i >= 0 && i < 4);
			return i;
		}
	};
}

#endif	//_ZFXMATH_INCLUDE_QUATERNION_H_
