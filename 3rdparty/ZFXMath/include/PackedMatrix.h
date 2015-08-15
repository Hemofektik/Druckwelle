/// \file
///
/// \if DE
/// @brief TPackedMatrix
///
/// TPackedMatrix: Matrix4x4 Ersatz um Speicher zu sparen
/// \else
/// @brief TPackedMatrix
///
/// TPackedMatrix: Matrix4x4 replacement to save memory
/// \endif

#ifndef _ZFXMATH_INCLUDE_PACKETMATRIX_H_	
#define _ZFXMATH_INCLUDE_PACKETMATRIX_H_

namespace ZFXMath
{

	/// \if DE
	/// @brief TPackedMatrix
	///
	/// TPackedMatrix: Matrix4x4 Ersatz um Speicher zu sparen
	/// \else
	/// @brief TPackedMatrix
	///
	/// TPackedMatrix: Matrix4x4 replacement to save memory
	/// \endif
	template<typename T>
	struct TPackedMatrix
	{
		TQuaternion<T>	rotation;
		T				scale;
		TVector3D<T>	position;

		TPackedMatrix()
		{
		};

		TPackedMatrix(	const TQuaternion<T>&	rotation,
						const T&				scale,
						const TVector3D<T>&		position ) :
		rotation(rotation),
		scale(scale),
		position(position)
		{
		}

		TPackedMatrix( const TVector3D<T>&		position ) :
		rotation(TQuaternion<T>::Identity()),
		scale((T)1.0),
		position(position)
		{
		}

		TPackedMatrix( const TMatrix4x4<T>& m )
		{
			rotation = m.SubMatrix(3,3);
			scale = (T)1.0;
			position = m.GetCol(3);
		}

		/// \if DE
		/// @brief automatische Typkonvertierung
		/// \else
		/// @brief auto coercion
		/// \endif
		template<typename T2>
		operator TPackedMatrix<T2>() const
		{
			return TPackedMatrix<T2>( TQuaternion<T2>(rotation.x, rotation.y, rotation.z, rotation.w), (T2)scale, (TVector3D<T2>)position );
		}

		template<typename T2>
		operator TMatrix4x4<T2>() const
		{
			TMatrix4x4<T2> ret( TMatrix3x3<T2>( (TQuaternion<T2>)rotation ) );
			ret.Scale( scale );
			ret.Translate( position );
			return ret;
		}

		TPackedMatrix operator * ( const TPackedMatrix& pm ) const { return Multiply( pm );	}
		TPackedMatrix& operator *= ( const TPackedMatrix& pm ) { *this = Multiply( pm ); return *this; }

		TVector3D<T> operator * (const TVector4D<T> &vec)const
		{
			return ( rotation * TVector3D<T>(vec) ) * scale + position * vec.w;
		}

		TVector3D<T> operator * (const TVector3D<T> &vec)const
		{
			return ( rotation * vec ) * scale;
		}

		/// \if DE
		/// @brief Multipliziert zwei Packed Matrizen miteinander
		/// \else
		/// @brief Multiplies two packed matrices
		/// \endif
		TPackedMatrix Multiply( const TPackedMatrix& pm ) const
		{
			TPackedMatrix result;

			result.rotation		= rotation * pm.rotation;
			result.scale		= scale * pm.scale;
			result.position		= position + ( rotation * pm.position ) * scale;

			return result;
		}

		TPackedMatrix Inverse() const
		{
			TPackedMatrix inv;
			inv.rotation		= rotation.Inverse();
			inv.scale			= (T)1.0 / scale;
			inv.position		= rotation * -position;
			return inv;
		}

		static TPackedMatrix LookAtLH( const TVector3D<T>& eyePos, const TVector3D<T>& eyeDir, const TVector3D<T>& up )
		{
			return TPackedMatrix( TQuaternion<T>::LookAtLH( eyeDir, up ), 1.0f, -eyePos );
		}

		static TPackedMatrix Identity()
		{
			return TPackedMatrix(  TQuaternion<T>::Identity(), (T)1.0, TVector3D<T>( (T)0.0, (T)0.0, (T)0.0 ) );
		}

	};

}

#endif //_ZFXMATH_INCLUDE_PACKETMATRIX_H_






