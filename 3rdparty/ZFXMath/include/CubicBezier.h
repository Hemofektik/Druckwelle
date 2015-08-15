/// \file
///
/// \if DE
/// @brief TCubicBezier
///
/// TCubicBezier: n-dimensionale kubische Bezier-Kurve
/// \else
/// @brief TCubicBezier
///
/// TCubicBezier: n-dimensional Cubic Bezier-Curve
/// \endif


#ifndef _ZFXMATH_INCLUDE_CUBICBEZIER_H_
#define _ZFXMATH_INCLUDE_CUBICBEZIER_H_

#include <vector>

namespace ZFXMath
{

	/// \if DE
	/// @brief n-dimensionale kubische Bezier-Kurve
	///
	/// \param PrecisionType Genauigkeit der Funktionsauswertung (z.B. float)
	/// \param FuncValueType Typ der Funktion (z.B. TVector3D<float>)
	/// \else
	/// @brief n-dimensional Cubic Bezier-Curve
	///
	/// \param PrecisionType precision of function evaluation (e.g. float)
	/// \param FuncValueType Type of function (e.g. TVector3D<float>)
	/// \endif
	template<class PrecisionType, class FuncValueType>
	class TCubicBezier
	{

	public:

		TCubicBezier() {};

		/// \if DE
		/// @brief Konstruktor mit allen vier Kontrollpunkten
		/// \else
		/// @brief Constructor with all four control points
		/// \endif
		TCubicBezier(	const FuncValueType& cP0,
						const FuncValueType& cP1,
						const FuncValueType& cP2,
						const FuncValueType& cP3 ) { m_ControlPoint[0] = cP0;
													 m_ControlPoint[1] = cP1;
													 m_ControlPoint[2] = cP2;
													 m_ControlPoint[3] = cP3; }

		/// \if DE
		/// @brief Konstruktor mit den zwei End-Kontrollpunkten
		/// 
		/// Erzeugt eine Linie zwischen den beiden Punkten
		/// \else
		/// @brief Constructor with the two end control points
		/// 
		/// Creates a line between the wo points
		/// \endif
		TCubicBezier(	const FuncValueType& cP0,
						const FuncValueType& cP3 ) { m_ControlPoint[0] = cP0;
													 m_ControlPoint[1] = cP0 + ( cP3 - cP0 ) / 3.0;
													 m_ControlPoint[2] = cP0 + ( cP3 - cP0 ) / 1.5;
													 m_ControlPoint[3] = cP3; }


		TCubicBezier( const FuncValueType* pCP ) { memcpy( m_ControlPoint, pCP, sizeof( TCubicBezier ) ); }

		TCubicBezier( const TCubicBezier& cB ) { memcpy( m_ControlPoint, cB.m_ControlPoint, sizeof( TCubicBezier ) ); }

		/// \if DE
		/// @brief Konstanter Zugriff über Index
		/// \else
		/// @brief Const accessors by index
		/// \endif
		FuncValueType operator () ( const int i ) const
		{
			return m_ControlPoint[ Check( i ) ];
		}

		/// \if DE
		/// @brief Nichtkonstanter Zugriff über Index
		/// \else
		/// @brief Non-const accessors by index
		/// \endif
		FuncValueType& operator () ( const int i )
		{
			return m_ControlPoint[ Check( i ) ];
		}

		/// \if DE
		/// @brief Auswertung der Kurve am übergebenen Punkt
		/// \else
		/// @brief Evaluation of the curve at the given point
		/// \endif
		FuncValueType operator () ( const PrecisionType& u ) const
		{
			return Evaluate( u );
		}

		/// \if DE
		/// @brief Auswertung der Kurve am übergebenen Punkt
		/// \else
		/// @brief Evaluation of the curve at the given point
		/// \endif
		FuncValueType operator () ( const PrecisionType& u )
		{
			return Evaluate( u );
		}


		/// \if DE
		/// @brief Auswertung der Kurve am übergebenen Punkt
		/// \else
		/// @brief Evaluation of the curve at the given point
		/// \endif
		FuncValueType Evaluate( const PrecisionType& u ) const
		{
			PrecisionType invu = 1 - u;
			return (  m_ControlPoint[0] * invu * invu * invu
					+ m_ControlPoint[1] * 3 * u * invu * invu
					+ m_ControlPoint[2] * 3 * u * u * invu
					+ m_ControlPoint[3] * u * u * u );
		}

	private:
		inline int Check( const int index ) const
		{
			// Check bounds in debug build
			assert( index >= 0 && index < 4 );

			return index;
		}

		FuncValueType	m_ControlPoint[4];
	};


	/// \if DE
	/// @brief n-dimensionale Reihe von Punkten die mit einer 
	///		   kubischen Bezier-Kurve interpoliert werden können
	///
	/// \param PrecisionType Genauigkeit der Funktionsauswertung (z.B. float)
	/// \param FuncValueType Typ der Funktion (z.B. TVector3D<float>)
	/// \else
	/// @brief n-dimensional Cubic Bezier-Curve
	///
	/// \param PrecisionType precision of function evaluation (e.g. float)
	/// \param FuncValueType Type of function (e.g. TVector3D<float>)
	/// \endif
	template<class PrecisionType, class FuncValueType>
	class TCubicBezierSet
	{
		public:

			TCubicBezierSet() {};

			void AddPoint( const FuncValueType& point )
			{
				m_point.push_back( point );
				m_splinePoint.clear();
				if( m_point.size() > 1 )
				{
					ComputeSplinePoints();
				}
			}

			/// \if DE
			/// @brief Auswertung der Kurve am übergebenen Punkt
			/// \else
			/// @brief Evaluation of the curve at the given point
			/// \endif
			FuncValueType operator () ( const PrecisionType& u ) const
			{
				return Evaluate( u );
			}

			/// \if DE
			/// @brief Auswertung der Kurve am übergebenen Punkt
			/// \else
			/// @brief Evaluation of the curve at the given point
			/// \endif
			FuncValueType operator () ( const PrecisionType& u )
			{
				return Evaluate( u );
			}


			/// \if DE
			/// @brief Auswertung der Kurve am übergebenen Punkt
			/// \else
			/// @brief Evaluation of the curve at the given point
			/// \endif
			FuncValueType Evaluate( const PrecisionType& u ) const
			{
				int pointIndex = (int)u;
				int splinePointIndex = pointIndex * 3;

				assert( ( m_point.size() > 1 ) && pointIndex >= 0 && pointIndex < (int)m_point.size() );

				TCubicBezier<PrecisionType, FuncValueType> cb(	m_splinePoint[splinePointIndex + 0], 
																m_splinePoint[splinePointIndex + 1], 
																m_splinePoint[splinePointIndex + 2],
																m_splinePoint[splinePointIndex + 3] );

				return cb.Evaluate( u - pointIndex );
			}

			int GetNumPoints() const
			{
				return (int)m_point.size();
			}

		private:

			void ComputeSplinePoints()
			{
				if( m_splinePoint.size() == 0 )
				{
					int numPoints = (int)m_point.size();
					if( numPoints == 2 )
					{
						TCubicBezier<PrecisionType, FuncValueType> cb( m_point[0], m_point[1] );
						m_splinePoint.push_back( cb(0) );
						m_splinePoint.push_back( cb(1) );
						m_splinePoint.push_back( cb(2) );
						m_splinePoint.push_back( cb(3) );
					}
					else
					{
						m_splinePoint.push_back( m_point[0] );

						FuncValueType nextTangent = ( m_point[0] - m_point[2] ) * (PrecisionType)0.5 + m_point[1];
						m_splinePoint.push_back( Interpolate( (PrecisionType)0.5, m_point[0], nextTangent ) );

						for ( int n = 1; n < numPoints - 1; n++ )
						{
							FuncValueType prevTangent = ( m_point[n - 1] - m_point[n] ) * (PrecisionType)0.5;
							FuncValueType nextTangent = -prevTangent;

							m_splinePoint.push_back( m_point[n] + prevTangent );
							m_splinePoint.push_back( m_point[n] );
							m_splinePoint.push_back( m_point[n] + nextTangent );
						}

						FuncValueType prevTangent = ( m_point[numPoints - 3] - m_point[numPoints - 1] ) * (PrecisionType)0.5 + m_point[numPoints - 2];
						m_splinePoint.push_back( Interpolate( (PrecisionType)0.5, prevTangent, m_point[numPoints - 1] ) );

						m_splinePoint.push_back( m_point[numPoints - 1] );
					}

					// for interpolation safety
					m_splinePoint.push_back( m_point[numPoints - 1] );
					m_splinePoint.push_back( m_point[numPoints - 1] );
					m_splinePoint.push_back( m_point[numPoints - 1] );
				}
			}

			std::vector<FuncValueType>				m_point;
			std::vector<FuncValueType>				m_splinePoint;

	};


}

#endif //_ZFXMATH_INCLUDE_CUBICBEZIER_H_


