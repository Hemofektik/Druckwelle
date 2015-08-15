/// \file
///
/// \if DE
/// @brief TMatrix3x3
///
/// TMatrix3x3: Matrix für 2D oder Rotation für 3D
/// \else
/// @brief TMatrix3x3
///
/// TMatrix3x3: Matrix for 2D or rotation for 3D
/// \endif

#ifndef _ZFXMATH_INCLUDE_MATRIX3X3_H_
#define _ZFXMATH_INCLUDE_MATRIX3X3_H_

namespace ZFXMath
{

	template<typename T>
	struct TMatrix4x4;

	template<typename T>
	struct TQuaternion;

	/// \if DE
	/// @brief TMatrix3x3
	///
	/// TMatrix3x3: Matrix für 2D oder Rotation für 3D
	/// \else
	/// @brief TMatrix3x3
	///
	/// TMatrix3x3: Matrix for 2D or rotation for 3D
	/// \endif
	template<typename T>
	struct TMatrix3x3
	{
		union
		{
			struct
			{
				T	m[9];
			};
			struct
			{
				T	matrix[3][3];
			};
			struct
			{
				T _11; T _12; T _13;
				T _21; T _22; T _23;
				T _31; T _32; T _33;
			};
		};

		TMatrix3x3() 
		{
		// von oese: identitätsmatrix lieber direkt mit identity initialisieren, leerer standardkonstruktor ist schneller
		//	(*this) = Identity();
		};

		TMatrix3x3(	const T& val ) 
		{ 
			_11 = _12 = _13 =
			_21 = _22 = _23 =
			_31 = _32 = _33 = val;
		}

		TMatrix3x3(	const T& _11, const T& _12, const T& _13,
					const T& _21, const T& _22, const T& _23,
					const T& _31, const T& _32, const T& _33 ) 
		{ 
			this->_11 = _11; this->_12 = _12; this->_13 = _13;
			this->_21 = _21; this->_22 = _22; this->_23 = _23;
			this->_31 = _31; this->_32 = _32; this->_33 = _33;
		}

		TMatrix3x3( const T& sx, const T& sy, const T& sz = (T)1.0 )
		{
			(*this) = TMatrix3x3( sx, 0.0, 0.0,
								  0.0, sy, 0.0,
								  0.0, 0.0, sz );
		}

		TMatrix3x3(	const T* _m )
		{
			CopyMemory( this, _m, sizeof( TMatrix3x3 ) );
		}

		TMatrix3x3(	const TMatrix3x3& _m )
		{
			CopyMemory( this, _m, sizeof( TMatrix3x3 ) );
		}

		TMatrix3x3( const T& rad, const T& scale, const TVector2D<T>& p ) 
		{ 
			(*this) = Rotation2D( rad );
			Scale( scale );
			Translate( p );
		}

		TMatrix3x3(	const TVector3D<T>& v1, const TVector3D<T>& v2, const TVector3D<T>& v3 ) 
		{
			_11 = v1.x; _21 = v1.y; _31 = v1.z;
			_12 = v2.x; _22 = v2.y; _32 = v2.z;
			_13 = v3.x; _23 = v3.y; _33 = v3.z;
		}

		TMatrix3x3( const TMatrixMxN<3, 3, T> &mat)
		{
			CopyMemory( this, &mat, sizeof(TMatrixMxN<3, 3, T>) );
		}

		TMatrix3x3(	const TMatrix4x4<T>& _m ) 
		{
			_11 = _m._11; _12 = _m._12; _13 = _m._13;
			_21 = _m._21; _22 = _m._22; _23 = _m._23;
			_31 = _m._31; _32 = _m._32; _33 = _m._33;
		}

		TMatrix3x3(	const TQuaternion<T>& q ) 
		{
			T xx  = q.x + q.x;
			T yy  = q.y + q.y;
			T zz  = q.z + q.z;

			T wxx = q.w * xx;
			T wyy = q.w * yy;
			T wzz = q.w * zz;
			T xxx = q.x * xx;
			T xyy = q.x * yy;
			T xzz = q.x * zz;
			T yyy = q.y * yy;
			T yzz = q.y * zz;
			T zzz = q.z * zz;

			_11 = (T)1.0 - (yyy + zzz);
			_21 = xyy + wzz;
			_31 = xzz - wyy;
			  
			_12 = xyy - wzz;
			_22 = (T)1.0 - (xxx + zzz);
			_32 = yzz + wxx;
			  
			_13 = xzz + wyy;
			_23 = yzz - wxx;
			_33 = (T)1.0 - (xxx + yyy);
		}

		/// \if DE
		/// @brief automatische Typkonvertierung
		/// \else
		/// @brief auto coercion
		/// \endif
		template<typename T2>
		operator TMatrix3x3<T2>() const
		{
			return TMatrix3x3<T2>( (T2)_11, (T2)_12, (T2)_13, (T2)_21, (T2)_22, (T2)_23, (T2)_31, (T2)_32, (T2)_33 );
		}

		/// \if DE
		/// @brief Caste in ein Array von T
		/// \else
		/// @brief Cast to an Array of T
		/// \endif
		operator T* ()	{ return (T*)this; }
		operator const T* () const { return (const T*)this; }

		/// \if DE
		/// @brief Konstanter Zugriff über Index (Reihe, Spalte)
		/// \else
		/// @brief Const accessor by index (row, column)
		/// \endif
		const T& operator () ( const int i, const int j ) const
		{
			return ((T*)this)[ Check( i ) * 3 +  Check( j ) ];
		}

		/// \if DE
		/// @brief Nichtkonstanter Zugriff über Index (Reihe, Spalte)
		/// \else
		/// @brief Non-const accessor by index (row, column)
		/// \endif
		T& operator () ( const int i, const int j )
		{
			return ((T*)this)[ Check( i ) * 3 + Check( j ) ];
		}


		/// \if DE
		/// @brief Konstanter Zugriff über Index
		/// \else
		/// @brief Const accessor by index
		/// \endif
		const T& operator () ( const int i ) const
		{
			return ((T*)(this))[ Check9( i ) ];
		}

		/// \if DE
		/// @brief Nichtkonstanter Zugriff über Index
		/// \else
		/// @brief Non-const accessor by index
		/// \endif
		T& operator () ( const int i )
		{
			return ((T*)(this))[ Check9( i ) ];
		}

		/// \if DE
		/// @brief Zeilen Zugriff über Index
		/// \else
		/// @brief row accessor by index
		/// \endif
		T* operator [] ( const int i )
		{
			return &( ((T*)(this))[ Check( i ) * 3 ] );
		}

		/// \if DE
		/// @brief Konstanter Zeilen Zugriff über Index
		/// \else
		/// @brief const row accessor by index
		/// \endif
		T* operator [] ( const int i ) const 
		{
			return &( ((T*)(this))[ Check( i ) * 3 ] );
		}

		TMatrix3x3 operator + ( const TMatrix3x3& _m ) const { return TMatrix3x3(   _11 + _m._11, _12 + _m._12, _13 + _m._13,
																					_21 + _m._21, _22 + _m._22, _23 + _m._23,
																					_31 + _m._31, _32 + _m._32, _33 + _m._33 ); }
		TMatrix3x3 operator - ( const TMatrix3x3& _m ) const { return (*this) + _m.Negate(); }
		TMatrix3x3 operator * ( const TMatrix3x3& _m ) const { return Multiply( _m ); }
		TMatrix3x3 operator * ( const T& s ) const			 { TMatrix3x3 mcpy(*this); return mcpy.Scale( s ); }

		TMatrix3x3& operator += ( const TMatrix3x3& _m ) { *this = (*this) + _m; return (*this); }
		TMatrix3x3& operator -= ( const TMatrix3x3& _m ) { *this = (*this) - _m; return (*this); }
		TMatrix3x3& operator *= ( const TMatrix3x3& _m ) { *this = (*this) * _m; return (*this); }
		TMatrix3x3& operator *= ( const T& s )			 { return Scale( s ); }

		TVector3D<T> operator * ( const TVector3D<T>& v) const
		{
			return TVector3D<T>( _11 * v.x + _12 * v.y + _13 * v.z,
								 _21 * v.x + _22 * v.y + _23 * v.z,
								 _31 * v.x + _32 * v.y + _33 * v.z); 
		}

		friend TVector3D<T> operator *= ( TVector3D<T>& v, const TMatrix3x3<T> &m )
		{
			v = m * v;
			return v;
		}

		friend TQuaternion<T> operator * ( const TQuaternion<T>& q, const TMatrix3x3<T>& m )
		{
			return q * TQuaternion<T>( m );
		}

		friend TMatrix3x3 operator * ( const TMatrix3x3<T>& m, const TQuaternion<T>& q )
		{
			return m * TMatrix3x3( q );
		}

		static TMatrix3x3 Zero()
		{
			return TMatrix3x3( 0.0 );
		}

		static TMatrix3x3 Identity()
		{
			return TMatrix3x3(  1.0, 0.0, 0.0,
								0.0, 1.0, 0.0,
								0.0, 0.0, 1.0 );
		}

		/// \if DE
		/// @brief skaliert die Matrix
		/// \else
		/// @brief scales the matrix
		/// \endif
		TMatrix3x3& Scale( const T& s )
		{
			_11 *= s; _12 *= s; _13 *= s; 
			_21 *= s; _22 *= s; _23 *= s; 
			_31 *= s; _32 *= s; _33 *= s;

			return (*this);
		}

		/// \if DE
		/// @brief skaliert die Matrix
		/// \else
		/// @brief scales the matrix
		/// \endif
		TMatrix3x3& Scale( const T& sx, const T& sy, const T& sz = (T)1.0 )
		{
			_11 *= sx; _12 *= sy; _13 *= sz; 
			_21 *= sx; _22 *= sy; _23 *= sz; 
			_31 *= sx; _32 *= sy; _33 *= sz;

			return (*this);
		}

		/// \if DE
		/// @brief skaliert die Matrix
		/// \else
		/// @brief scales the matrix
		/// \endif
		TMatrix3x3& Scale( const TVector3D<T>& sv )
		{
			return Scale( sv.x, sv.y, sv.z );
		}

/*
		TMatrix3x3& SkewX( const T& rad )
		{
			return (*this);
		}
		TMatrix3x3& SkewY( const T& rad )
		{
			return (*this);
		}
*/

		/// \if DE
		/// @brief Erstellt eine Matrix die um p transliert ist
		/// \else
		/// @brief Creates a matrix which translates by p
		/// \endif
		static TMatrix3x3 Translation( const TVector2D<T>& p )
		{
			return Translation( p.x, p.y );
		}

		/// \if DE
		/// @brief Erstellt eine Matrix die um P(x,y) transliert ist
		/// \else
		/// @brief Creates a matrix which translates by P(x,y)
		/// \endif
		static TMatrix3x3 Translation( const T& x, const T& y )
		{
			TMatrix3x3 newM = TMatrix3x3::Identity();
			newM._13 = x;
			newM._23 = y;

			return newM;
		}

		/// \if DE
		/// @brief Transliert die Matrix um p
		/// \else
		/// @brief Translates the matrix by p
		/// \endif
		TMatrix3x3& Translate( const TVector2D<T>& p )
		{
			return Translate( p.x, p.y );
		}

		/// \if DE
		/// @brief Transliert die Matrix um P(x,y)
		/// \else
		/// @brief Translates the matrix by P(x,y)
		/// \endif
		TMatrix3x3& Translate( const T& x, const T& y )
		{
			_13 += x;
			_23 += y;
			return (*this);
		}

		/// \if DE
		/// @brief Multipliziert zwei matrizen miteineander
		/// \else
		/// @brief Multiplies two matrices
		/// \endif
		TMatrix3x3 Multiply( const TMatrix3x3& _m ) const
		{
			TMatrix3x3 newM = TMatrix3x3::Zero();

			for ( int i = 0; i < 3; i++ )
			{
				for ( int j = 0; j < 3; j++ )
				{
					for ( int k = 0; k < 3; k++ )
					{
						newM( i, j ) += (*this)( i, k ) * _m( k, j );
					}
				}
			}

			return newM;
		}

		/// \if DE
		/// @brief Erstellt eine Matrix die um X rotiert
		/// \else
		/// @brief Creates a matrix which rotates around X
		/// \endif
		static TMatrix3x3 RotationX( const T& rad )
		{
			T sin, cos;

			SinCos( rad, sin, cos );  // Determine sin and cos of angle

			return TMatrix3x3( 1.0, 0.0, 0.0,
							   0.0, cos,-sin,
							   0.0, sin, cos );
		}

		/// \if DE
		/// @brief Rotiert die Matrix um X
		/// \else
		/// @brief Rotates the matrix around X
		/// \endif
		TMatrix3x3& RotateX( const T& rad )
		{
			return (*this) *= RotationX( rad );
		}

		/// \if DE
		/// @brief Erstellt eine Matrix die um Y rotiert
		/// \else
		/// @brief Creates a matrix which rotates around Y
		/// \endif
		static TMatrix3x3 RotationY( const T& rad )
		{
			T sin, cos;

			SinCos( rad, sin, cos );  // Determine sin and cos of angle

			return TMatrix3x3( cos, 0.0, sin,
							   0.0, 1.0, 0.0,
							  -sin, 0.0, cos );
		}

		/// \if DE
		/// @brief Rotiert die Matrix um Y
		/// \else
		/// @brief Rotates the matrix around Y
		/// \endif
		TMatrix3x3& RotateY( const T& rad )
		{
			return (*this) *= RotationY( rad );
		}

		/// \if DE
		/// @brief Erstellt eine Matrix die um Z rotiert
		/// \else
		/// @brief Creates a matrix which rotates around Z
		/// \endif
		static TMatrix3x3 RotationZ( const T& rad )
		{
			T sin, cos;

			SinCos( rad, sin, cos );  // Determine sin and cos of angle

			return TMatrix3x3( cos,-sin, 0.0,
							   sin, cos, 0.0,
							   0.0, 0.0, 1.0 );
		}

		/// \if DE
		/// @brief Rotiert die Matrix um Z 
		/// \else
		/// @brief Rotates the matrix around Z
		/// \endif
		TMatrix3x3& RotateZ( const T& rad )
		{
			return (*this) *= RotationZ( rad );
		}

		static TMatrix3x3 Rotation( const TVector3D<T> &rad )
		{
			T				angle = rad.Length();
			T				sin = Sin(-angle), cos = Cos(-angle);
			TVector3D<T>	axis = rad / angle;

			T				invcos = 1.0 - cos,
							xsin = axis.x * sin,
							ysin = axis.y * sin,
							zsin = axis.z * sin;

			return TMatrix3x3(axis.x * axis.x * invcos + cos,
							  axis.y * axis.x * invcos + zsin,
							  axis.z * axis.x * invcos - ysin,
							  axis.x * axis.y * invcos - zsin,
							  axis.y * axis.y * invcos + cos,
							  axis.z * axis.y * invcos + xsin,
							  axis.x * axis.z * invcos + ysin,
							  axis.y * axis.z * invcos - xsin,
							  axis.z * axis.z * invcos + cos);
		}

		TMatrix3x3& Rotate( const TVector3D<T> &rad)
		{
			return (*this) *= Rotation(rad);
		}

		/// \if DE
		/// @brief Erstellt eine 2D-Rotationsmatrix
		/// \else
		/// @brief Creates a 2D rotation matrix
		/// \endif
		static TMatrix3x3 Rotation2D( const T& rad )
		{
			return RotationZ( rad );
		}

		/// \if DE
		/// @brief Rotiert die Matrix im 2D-Raum
		/// \else
		/// @brief Rotates the matrix in 2D-Space
		/// \endif
		TMatrix3x3& Rotate2D( const T& rad )
		{
			return (*this) *= Rotation2D( rad );
		}


		/// \if DE
		/// @brief Gibt die transponierte Matrix zurück
		/// \else
		/// @brief returns the transposed matrix
		/// \endif
		TMatrix3x3 Transpose() const
		{
			return TMatrix3x3( _11, _21, _31,
							   _12, _22, _32,
							   _13, _23, _33 );
		}

		/// \if DE
		/// @brief Gibt die negierte Matrix zurück
		/// \else
		/// @brief returns the negated matrix
		/// \endif
		TMatrix3x3 Negate() const
		{
			return TMatrix3x3( -_11, -_12, -_13,
							   -_21, -_22, -_23,
							   -_31, -_32, -_33 );
		}

		/// \if DE
		/// @brief Gibt die Determinante der Matrix zurück
		/// \else
		/// @brief returns the determinant of the matrix
		/// \endif
		T Determinant() const
		{
			return _11 * _22 * _33
				 + _12 * _23 * _31
				 + _13 * _21 * _32
				 - _31 * _22 * _13
				 - _32 * _23 * _11
				 - _33 * _21 * _12;
		}

		/// \if DE
		/// @brief Gibt die Inverse der Matrix zurück
		/// \else
		/// @brief returns the inverse of the matrix
		/// \endif
		TMatrix3x3 Inverse() const
		{
			TMatrix3x3 m;

		   // calculate the determinat of matrix
			m.matrix[0][0] = matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1];
			m.matrix[1][0] = matrix[2][0] * matrix[1][2] - matrix[1][0] * matrix[2][2];
			m.matrix[2][0] = matrix[1][0] * matrix[2][1] - matrix[2][0] * matrix[1][1];
			m.matrix[0][1] = matrix[2][1] * matrix[0][2] - matrix[0][1] * matrix[2][2];
			m.matrix[1][1] = matrix[0][0] * matrix[2][2] - matrix[2][0] * matrix[0][2];
			m.matrix[2][1] = matrix[2][0] * matrix[0][1] - matrix[0][0] * matrix[2][1];
			m.matrix[0][2] = matrix[0][1] * matrix[1][2] - matrix[1][1] * matrix[0][2];
			m.matrix[1][2] = matrix[1][0] * matrix[0][2] - matrix[0][0] * matrix[1][2];
			m.matrix[2][2] = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];

			T det = matrix[0][0] * m.matrix[0][0] +
					matrix[0][1] * m.matrix[1][0] +
					matrix[0][2] * m.matrix[2][0];

			// check if a inverse matrix exists
			// and return identity if not
			if ( Abs( det ) <= (T)EPSILON )
			{
				return TMatrix3x3::Identity();
			}

			T invdet = (T)1.0 / det;

			m *= invdet;

			return m;
		}


		/// \if DE
		/// @brief wechselt die Matrix von LeftHanded auf RightHanded und umgekehrt
		/// \else
		/// @brief switches between left handed and right handed and vice versa
		/// \endif
		TMatrix3x3 GetSwitchedHand() const
		{
			TMatrix3x3 newM( Transpose() );
			newM._32 = -newM._32;
			newM._23 = -newM._23;
			newM._13 = -newM._13;
			newM._31 = -newM._31;

			return newM;
		}

		TVector3D<T> GetRow(int index)
		{
			TVector3D<T> result;
			result(0) = (*this)(index,0);
			result(1) = (*this)(index,1);
			result(2) = (*this)(index,2);
			return result;
		}

		TVector3D<T> GetCol(int index)
		{
			TVector3D<T> result;
			result(0) = (*this)(0,index);
			result(1) = (*this)(1,index);
			result(2) = (*this)(2,index);
			return result;
		}

		void SetRow(int index, const TVector3D<T> &row)
		{
			(*this)(index,0) = row(0);
			(*this)(index,1) = row(1);
			(*this)(index,2) = row(2);
		}

		void SetCol(int index, const TVector3D<T> &col)
		{
			(*this)(0,index) = col(0);
			(*this)(1,index) = col(1);
			(*this)(2,index) = col(2);
		}

		static TMatrix3x3 LookAtLH( const TVector3D<T>& dir, const TVector3D<T>& up )
		{
			TVector3D<T> zaxis = dir.GetNormalized();
			TVector3D<T> xaxis = up.CrossProduct( zaxis ).Normalize();
			TVector3D<T> yaxis = zaxis.CrossProduct( xaxis );

			return TMatrix3x3(	xaxis.x, xaxis.y, xaxis.z, 
								yaxis.x, yaxis.y, yaxis.z,
								zaxis.x, zaxis.y, zaxis.z );
		}

		static TMatrix3x3 PitchYawRoll( const TVector3D<T>& pyr )
		{
			return RotationX( pyr.x ) * RotationY( pyr.y ) * RotationZ( pyr.z );
		}

		/// \if DE
		/// @brief Ausgabe Stream Operator
		/// \else
		/// @brief output stream operator
		/// \endif
		friend std::ostream& operator<< ( std::ostream& os, const TMatrix3x3& m )
		{
			for ( int x = 0; x < 3; x++ )
			{
				os << "[";
				for ( int y = 0; y < 3; y++ )
				{
					os << m[y][x];
					if (y < 2)
					{
						os<<", "; 
					}
				}
				os << "]" << std::endl;
			}
			return os;
		}

	private:
		inline int Check( const int index ) const
		{
			// Check bounds in debug build
			assert( index >= 0 && index < 3 );

			return index;
		}

		inline int Check9( const int index ) const
		{
			// Check bounds in debug build
			assert( index >= 0 && index < 9 );

			return index;
		}
		
	};
}

#endif //_ZFXMATH_INCLUDE_MATRIX3X3_H_


