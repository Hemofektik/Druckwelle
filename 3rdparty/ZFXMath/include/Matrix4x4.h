// von oese:

#ifndef _ZFXMATH_INCLUDE_MATRIX4X4_H_
#define _ZFXMATH_INCLUDE_MATRIX4X4_H_

namespace ZFXMath
{

	template<typename T>
	struct TMatrix4x4
	{
	public:

		union
		{
			struct
			{
				T	m[16];
			};
			struct
			{
				T	matrix[4][4];
			};
			struct
			{
				T	_11, _12, _13, _14;
				T	_21, _22, _23, _24;
				T	_31, _32, _33, _34;
				T	_41, _42, _43, _44;
			};
		};

		// Konstruktoren

		TMatrix4x4(){}

		TMatrix4x4(const T &m11,const T &m12,const T &m13,const T &m14,
				   const T &m21,const T &m22,const T &m23,const T &m24,
				   const T &m31,const T &m32,const T &m33,const T &m34,
				   const T &m41,const T &m42,const T &m43,const T &m44)
		:	_11(m11),_12(m12),_13(m13),_14(m14),
			_21(m21),_22(m22),_23(m23),_24(m24),
			_31(m31),_32(m32),_33(m33),_34(m34),
			_41(m41),_42(m42),_43(m43),_44(m44)		{}

		TMatrix4x4(const T &val)
		{
			for(register int i = 0; i < 16; i++)
			{
				m[i] = val;
			}
		}

		TMatrix4x4(const T *p)
		{
			for(register int i = 0; i < 16; i++)
			{
				m[i] = p[i];
			}
		}

		TMatrix4x4(const TMatrix4x4 &mat)
		{
			for(register int i = 0; i < 16; i++)
			{
				m[i] = mat(i);
			}
		}

		TMatrix4x4(const TMatrixMxN<4, 4, T> &mat)
		{
			for(register int i = 0; i < 16; i++)
			{
				m[i] = mat(i);
			}
		}

		TMatrix4x4(const TMatrix3x3<T> &mat)
		{
			for(register int i = 0; i < 3; i++)
			for(register int j = 0; j < 3; j++)
			{
				matrix[i][j] = mat(i,j);
			}
			_14 = _24 = _34 = _41 = _42 = _43 = 0.0;
			_44 = 1.0;
		}

		TMatrix4x4(const TVector4D<T> &a,
				   const TVector4D<T> &b,
				   const TVector4D<T> &c,
				   const TVector4D<T> &d)
		{
			_11 = a.x, _12 = b.x, _13 = c.x, _14 = d.x;
			_21 = a.y, _22 = b.y, _23 = c.y, _24 = d.y;
			_31 = a.z, _32 = b.z, _33 = c.z, _34 = d.z;
			_41 = a.w, _42 = b.w, _43 = c.w, _44 = d.w;
		}

		// Casting Operatoren

		operator T*(){return m;}
		operator const T*() const {return m;}
		operator TMatrixMxN<4,4,T>&(){return *(TMatrixMxN<4,4,T>*)m;}
		operator const TMatrixMxN<4,4,T>&() const {return *(TMatrixMxN<4,4,T>*)m;}

		/// \if DE
		/// @brief automatische Typkonvertierung
		/// \else
		/// @brief auto coercion
		/// \endif
		template<typename T2>
		operator TMatrix4x4<T2>() const
		{
			return TMatrix4x4<T2>(	(T2)_11, (T2)_12, (T2)_13, (T2)_14,
									(T2)_21, (T2)_22, (T2)_23, (T2)_24, 
									(T2)_31, (T2)_32, (T2)_33, (T2)_34, 
									(T2)_41, (T2)_42, (T2)_43, (T2)_44 );
		}

		// Elementzugriff

		T& operator()(int i){return m[Check16(i)];}
		const T& operator()(int i) const {return m[Check16(i)];}
		
		T& operator()(int i,int j){return matrix[Check(i)][Check(j)];}
		const T& operator()(int i,int j) const {return matrix[Check(i)][Check(j)];}
		
		T* operator[](int i){return matrix[Check(i)];}
		const T* operator[](int i) const {return matrix[Check(i)];}

		// Rechenoperatoren

		TMatrix4x4 operator + () const {return *this;}

		TMatrix4x4 operator + (const TMatrix4x4 &mat) const
		{
			TMatrix4x4 ret(*this);
			ret += mat;
			return ret;
		}

		TMatrix4x4& operator += (const TMatrix4x4& mat)
		{
			for(register int i = 0; i < 16; i++)
			{
				m[i] += mat(i);
			}
			return (*this);
		}

		TMatrix4x4 operator - ()const{return Negate();}

		TMatrix4x4 operator - (const TMatrix4x4 &mat)const
		{
			return (*this) + mat.Negate();
		}

		TMatrix4x4& operator -= (const TMatrix4x4& mat)
		{
			return (*this) += mat.Negate();
		}

		TMatrix4x4 operator * (const T& val)const
		{
			TMatrix4x4 ret(*this);
			ret *= val;
			return ret;
		}

		friend TMatrix4x4 operator * (const T& val,const TMatrix4x4& mat)
		{
			return mat * val;
		}

		TMatrix4x4& operator *= (const T& val)
		{
			for(register int i = 0; i < 16; i++)
			{
				m[i] *= val;
			}
			return (*this);
		}

		TVector4D<T> operator * (const TVector4D<T>& vec)const
		{
			return TVector4D<T>(_11 * vec.x + _12 * vec.y + _13 * vec.z + _14 * vec.w,
								_21 * vec.x + _22 * vec.y + _23 * vec.z + _24 * vec.w,
								_31 * vec.x + _32 * vec.y + _33 * vec.z + _34 * vec.w,
								_41 * vec.x + _42 * vec.y + _43 * vec.z + _44 * vec.w);
		}

		TVector3D<T> operator * (const TVector3D<T>& vec)const
		{
			return TransformVector( vec );
		}

		TMatrix4x4 operator * (const TMatrix4x4& mat) const
		{
			return Multiply(mat);
		}

		TMatrix4x4& operator *= (const TMatrix4x4& mat)
		{
			return (*this) = (*this) * mat;
		}

		TMatrix4x4 operator / (const T& val)const
		{
			return (*this) * ((T)1.0 / val);
		}

		TMatrix4x4& operator /= (const T& val)
		{
			return (*this) = (*this) / val;
		}

		TMatrix4x4 operator / (const TMatrix4x4& mat)const
		{
			return (*this) * mat.Inverse();
		}

		TMatrix4x4& operator /= (const TMatrix4x4& mat)
		{
			return (*this) *= mat.Inverse();
		}

		static TMatrix4x4 Zero()
		{
			return TMatrix4x4((T)0.0);
		}

		static TMatrix4x4 Identity()
		{
			return TMatrix4x4(1.0,0.0,0.0,0.0,
							  0.0,1.0,0.0,0.0,
							  0.0,0.0,1.0,0.0,
							  0.0,0.0,0.0,1.0);
		}

		TMatrix4x4 Negate()const
		{
			return TMatrix4x4(-_11,-_12,-_13,-_14,
							  -_21,-_22,-_23,-_24,
							  -_31,-_32,-_33,-_34,
							  -_41,-_42,-_43,-_44);
		}

		TMatrix4x4 Transpose()const
		{
			return TMatrix4x4(_11,_21,_31,_41,
							  _12,_22,_32,_42,
							  _13,_23,_33,_43,
							  _14,_24,_34,_44);
		}

		T Determinant()const
		{
			return	+ _11 * TMatrix3x3<T>(_22,_23,_24,_32,_33,_34,_42,_43,_44).Determinant()
					- _21 * TMatrix3x3<T>(_12,_13,_14,_32,_33,_34,_42,_43,_44).Determinant()
					+ _31 * TMatrix3x3<T>(_12,_13,_14,_22,_23,_24,_42,_43,_44).Determinant()
					- _41 * TMatrix3x3<T>(_12,_13,_14,_22,_23,_24,_32,_33,_34).Determinant();
		}

		// Matrix bei der die i-te zeile und die j-te spalte fehlt
		TMatrix3x3<T> SubMatrix(int i, int j) const
		{
			TMatrix3x3<T> ret;
			for(register int a = 0; a < 4; a++)
			{
				if(a == i)continue;
				for(register int b = 0; b < 4; b++)
				{
					if(b == j)continue;
					ret((a<i?a:a-1),(b<j?b:b-1)) = (*this)(a,b);
				}
			}
			return ret;
		}

		TMatrix4x4 Multiply(const TMatrix4x4& mat) const
		{
			TMatrix4x4 ret;
			for ( int x = 0; x < 4; x++ )
			{	
				for ( int y = 0; y < 4; y++ )
				{	
					ret.matrix[x][y] = (T)0.0;
					for ( int i = 0; i < 4; i++ )
					{	
						ret.matrix[x][y] += matrix[i][y] * mat.matrix[x][i];
					}
				}
			}
			return ret;
		}

		T AlgebraicComplement(int i, int j) const
		{
			T ret = SubMatrix(i,j).Determinant();
			if( (i+j) % 2 )return -ret;
			return ret;
		}

		// sehr langsam - dringend optimieren
		TMatrix4x4<T> Inverse() const
		{
			TMatrix4x4<T> ret;
			T det = Determinant();
			if((T)0.0 == det)return Identity();
			for(register int i = 0; i < 4; i++)
			for(register int j = 0; j < 4; j++)
			{
				ret(i,j) = AlgebraicComplement(i,j);
			}
			ret /= det;
			return ret;
		}

		TVector4D<T> GetRow(int index)const
		{
			TVector4D<T> ret;
			for(register int i = 0; i < 4; i++)
			{
				ret(i) = (*this)(index,i);
			}
			return ret;
		}
		
		TVector4D<T> GetCol(int index)const
		{
			TVector4D<T> ret;
			for(register int i = 0; i < 4; i++)
			{
				ret(i) = (*this)(i,index);
			}
			return ret;
		}

		void SetRow(int index, const TVector4D<T>& vec)
		{
			for(register int i = 0; i < 4; i++)
			{
				(*this)(index,i) = vec(i);
			}
		}
		
		void SetCol(int index, const TVector4D<T>& vec)
		{
			for(register int i = 0; i < 4; i++)
			{
				(*this)(i,index) = vec(i);
			}
		}

		/// \if DE
		/// @brief skaliert die Matrix
		/// \else
		/// @brief scales the matrix
		/// \endif
		TMatrix4x4& Scale( const T& s )
		{
			_11 *= s; _12 *= s; _13 *= s; 
			_21 *= s; _22 *= s; _23 *= s; 
			_31 *= s; _32 *= s; _33 *= s;

			return (*this);
		}

		static TMatrix4x4 Translation(const T& x, const T& y, const T& z)
		{
			TMatrix4x4 ret = Identity();
			ret._14 = x;
			ret._24 = y;
			ret._34 = z;
			return ret;
		}

		TMatrix4x4& Translate(const T& x, const T& y, const T& z)
		{
			return (*this) *= Translation(x,y,z);
		}

		static TMatrix4x4 Translation(const TVector3D<T>& v)
		{
			return Translation(v.x,v.y,v.z);
		}

		TMatrix4x4& Translate(const TVector3D<T>& v)
		{
			return (*this) *= Translation(v);
		}

		static TMatrix4x4 RotationX(const T& a)
		{
			return TMatrix4x4(TMatrix3x3<T>::RotationX(a));
		}

		TMatrix4x4& RotateX(const T& a)
		{
			return (*this) *= RotationX(a);
		}
		
		static TMatrix4x4 RotationY(const T& a)
		{
			return TMatrix4x4(TMatrix3x3<T>::RotationY(a));
		}

		TMatrix4x4& RotateY(const T& a)
		{
			return (*this) *= RotationY(a);
		}

		static TMatrix4x4 RotationZ(const T& a)
		{
			return TMatrix4x4(TMatrix3x3<T>::RotationZ(a));
		}

		TMatrix4x4& RotateZ(const T& a)
		{
			return (*this) *= RotationZ(a);
		}

		static TMatrix4x4 Rotation(const TVector3D<T>& v)
		{
			return TMatrix4x4( TMatrix3x3<T>::Rotation(v) );
		}

		TMatrix4x4& Rotate(const TVector3D<T>& v)
		{
			return (*this) *= Rotation(v);
		}

		static TMatrix4x4 LookAtLH( const TVector3D<T>& eyePos, const TVector3D<T>& eyeDir, const TVector3D<T>& up )
		{
			TVector3D<T> zaxis = eyeDir.GetNormalized();
			TVector3D<T> xaxis = up.CrossProduct( zaxis ).Normalize();
			TVector3D<T> yaxis = zaxis.CrossProduct( xaxis );

			return TMatrix4x4(	xaxis.x, xaxis.y, xaxis.z, -xaxis.DotProduct( eyePos ),
								yaxis.x, yaxis.y, yaxis.z, -yaxis.DotProduct( eyePos ),
								zaxis.x, zaxis.y, zaxis.z, -zaxis.DotProduct( eyePos ),
								0	   , 0		, 0		 ,  1						 );
		}

		static TMatrix4x4 PerspectiveFovLH( const T& fovY, const T& aspectRatio, const T& nearPlane, const T& farPlane )
		{
			T yScale = Cot( fovY / (T)2.0 );
			T xScale = yScale / aspectRatio;

			return TMatrix4x4(	xScale,	(T)0.0,	(T)0.0,								 (T)0.0,
								(T)0.0,	yScale, (T)0.0,								 (T)0.0,
								(T)0.0,	(T)0.0,	farPlane / ( farPlane - nearPlane ), -nearPlane * farPlane / ( farPlane - nearPlane ),
								(T)0.0,	(T)0.0,	(T)1.0,								 (T)0.0 );
		}

		static TVector3D<T> ProjectVector( const TVector3D<T>& vec, const TMatrix4x4<T>& viewProjection )
		{
			TVector4D<T> projected = vec;

			projected = viewProjection * projected;

			if( projected.w != (T)0.0 )
			{
				projected /= projected.w;
			}

			return projected;
		}

		static TVector3D<T> ProjectVector( const TVector3D<T>& vec, const TMatrix4x4<T>& viewProjection, const TVector2D<T>& screenDim )
		{
			TVector3D<T> projected;

			projected = ProjectVector( vec, viewProjection );

			projected.x = ( ( projected.x + (T)1.0 ) * screenDim.x ) * (T)0.5;
			projected.y = ( ( (T)1.0 - projected.y ) * screenDim.y ) * (T)0.5;

			return projected;
		}

		static TVector3D<T> ProjectVector( const TVector3D<T>& vec, const TMatrix4x4<T>& view, const TMatrix4x4<T>& proj )
		{
			TVector4D<T> projected = vec;

			projected = proj * ( view * projected );

			if( projected.w != (T)0.0 )
			{
				projected /= projected.w;
			}

			return projected;
		}

		static TVector3D<T> ProjectVector( const TVector3D<T>& vec, const TMatrix4x4<T>& view, const TMatrix4x4<T>& proj, const TVector2D<T>& screenDim )
		{
			TVector3D<T> projected;

			projected = ProjectVector( vec, view, proj );

			projected.x = ( ( projected.x + (T)1.0 ) * screenDim.x ) * (T)0.5;
			projected.y = ( ( (T)1.0 - projected.y ) * screenDim.y ) * (T)0.5;

			return projected;
		}

		static TVector3D<T> UnprojectVector( const TVector3D<T>& vec, const TMatrix4x4<T>& invViewProj )
		{
			TVector3D<T> unProjected(	vec.x * invViewProj.matrix[0][0] + vec.y * invViewProj.matrix[1][0] + vec.z * invViewProj.matrix[2][0] + invViewProj.matrix[3][0],
										vec.x * invViewProj.matrix[0][1] + vec.y * invViewProj.matrix[1][1] + vec.z * invViewProj.matrix[2][1] + invViewProj.matrix[3][1],
										vec.x * invViewProj.matrix[0][2] + vec.y * invViewProj.matrix[1][2] + vec.z * invViewProj.matrix[2][2] + invViewProj.matrix[3][2] );
			T rhw =			(T)1.0 / (	vec.x * invViewProj.matrix[0][3] + vec.y * invViewProj.matrix[1][3] + vec.z * invViewProj.matrix[2][3] + invViewProj.matrix[3][3] );

			unProjected *= rhw;

			return unProjected;
		}

		TVector3D<T> TransformVector( const TVector3D<T>& vec ) const
		{
			return TVector3D<T>(_11 * vec.x + _12 * vec.y + _13 * vec.z,
								_21 * vec.x + _22 * vec.y + _23 * vec.z,
								_31 * vec.x + _32 * vec.y + _33 * vec.z);
		}

		TVector3D<T> TransformPosition( const TVector3D<T>& vec ) const
		{
			return TVector3D<T>(_11 * vec.x + _12 * vec.y + _13 * vec.z + _14,
								_21 * vec.x + _22 * vec.y + _23 * vec.z + _24,
								_31 * vec.x + _32 * vec.y + _33 * vec.z + _34);
		}

		friend std::ostream& operator<< ( std::ostream& os, const TMatrix4x4<T>& m )
		{
			for ( int x = 0; x < 4; x++ )
			{
				os << "[";
				for ( int y = 0; y < 4; y++ )
				{
					os << m[y][x];
					if (y < 3)
					{
						os<<", "; 
					}
				}
				os << "]" << std::endl;
			}
			return os;
		}

	private:

		int Check(int i) const
		{
			assert(i >= 0 && i < 4);
			return i;
		}

		int Check16(int i) const
		{
			assert(i >= 0 && i < 16);
			return i;
		}

	};

}

#endif	//_ZFXMATH_INCLUDE_MATRIX4X4_H_
