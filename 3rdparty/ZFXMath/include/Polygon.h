
#ifndef _ZFXMATH_INCLUDE_POLYGON_H_
#define _ZFXMATH_INCLUDE_POLYGON_H_

#include "Geometry.h"

namespace ZFXMath
{
	template<typename T>
	struct TPolygon2D
	{
	public:

	private:

		struct Edge
		{
			TVector2D<T> center;
			TVector2D<T> direction;
			T			 extent;
		};

		TVector2D<T>*	m_vertex;
		TVector2D<T>*	m_normal;
		Edge*			m_edge;
		int				m_vertexCount;

	public:

		TPolygon2D() :
		m_vertex(NULL),
			m_normal(NULL),
		m_edge(NULL),
		m_vertexCount(0)
		{}

		~TPolygon2D() 
		{
			delete[] m_vertex;
			delete[] m_normal;
			delete[] m_edge;
		}

		void AddVertex( const TVector2D<T>& v )
		{
			TVector2D<T>*	oldVertices = m_vertex;
			m_vertex = new TVector2D<T>[m_vertexCount + 1];

			if( oldVertices )
			{
				memcpy( m_vertex, oldVertices, m_vertexCount * sizeof( TVector2D<T> ) );
				delete[] oldVertices;
			}

			m_vertex[m_vertexCount] = v;
			m_vertexCount++;
		}

		void SetVertex( int index, const TVector2D<T>& v )
		{
			assert( index >= 0 && index < m_vertexCount );

			m_vertex[index] = v;
		}

		// removes latest vertex if it is equal to the first vertex
		// creates normal and edge information needed for any call to ComputeSqrDistance
		void CloseRing()
		{
			// "delete" last vertex if it is a duplicate of the first one
			{
				const TVector2D<T>& firstVertex = m_vertex[0];
				const TVector2D<T>& lastVertex = m_vertex[m_vertexCount - 1];

				const TVector2D<T> delta = firstVertex - lastVertex;

				if (delta.x == 0 && delta.y == 0)
				{
					m_vertexCount--;
				}
			}

			// create edges
			delete[] m_edge;
			const int numEdges = GetNumEdges();
			m_edge = new Edge[numEdges];
			for (int n = 0; n < numEdges; n++)
			{
				auto& v0 = m_vertex[n];
				auto& v1 = m_vertex[(n < m_vertexCount - 1) ? n + 1 : 0];

				Edge& edge = m_edge[n];
				edge.center = (v0 + v1) * (T)0.5;
				edge.direction = v1 - v0;
				edge.extent = edge.direction.Normalize() * (T)0.5;
			}

			// compute normals
			delete[] m_normal;
			const int numNormals = GetNumEdges();
			m_normal = new TVector2D<double>[m_vertexCount];
			for (int n = 0; n < m_vertexCount; n++)
			{
				auto& e0 = m_edge[(n > 0) ? n - 1 : m_vertexCount - 1];
				auto& e1 = m_edge[n];

				m_normal[n] = -(e0.direction + e1.direction).GetOrthogonal();
				m_normal[n].Normalize();
			}
		}

		void SetNumVertices( int numVertices )
		{
			assert( numVertices > 0 );

			TVector2D<T>*	oldVertices = m_vertex;
			m_vertex = new TVector2D<T>[numVertices];

			if( oldVertices )
			{
				memcpy( m_vertex, oldVertices, m_vertexCount * sizeof( TVector2D<T> ) );
				delete[] oldVertices;
			}

			m_vertexCount = numVertices;
		}

		MathResult Check( const TVector2D<T>& v ) const
		{
			int numCrossingEdges = 0;
			for( int n = 0; n < GetNumEdges(); n++ )
			{
				TVector2D<T> v0;
				TVector2D<T> v1;

				GetEdge( n, v0, v1 );
				
				TVector2D<T> vMin = v0;
				TVector2D<T> vMax = v1;

				vMin.Min( v1 );
				vMax.Max( v0 );

				if( v.x > vMin.x && v.y > vMin.y && v.y < vMax.y )
				{
					numCrossingEdges++;
				}
			}

			return ( numCrossingEdges & 1 ) ? INSIDE : OUTSIDE;
		}

		T ComputeSqrDistance(const TVector2D<T>& v, /*out*/ bool& pointIsRightOfEdge) const
		{
			T minSqrDistance = numeric_limits<T>::max();
			bool minDistancePointIsRightOfEdge = false;
			for (int n = 0; n < GetNumEdges(); n++)
			{
				bool pointIsRightOfTestEdge = false;
				T sqrDistance = SqrDistanceToEdge(v, n, pointIsRightOfTestEdge);
				if (sqrDistance < minSqrDistance)
				{
					minSqrDistance = sqrDistance;
					minDistancePointIsRightOfEdge = pointIsRightOfTestEdge;
				}
			}

			pointIsRightOfEdge = minDistancePointIsRightOfEdge;
			return minSqrDistance;
		}

		// assumes that the polygon is convex
		void Split( int edgeIndex, T lerp, TPolygon2D<T>& newPolygon0, TPolygon2D<T>& newPolygon1 ) const
		{
			TVector2D<T> edgeV0;
			TVector2D<T> edgeV1;

			GetEdge( edgeIndex, edgeV0, edgeV1 );

			TVector2D<T> rayOrigin = edgeV0.Interpolate( edgeV1, lerp );
			TVector2D<T> rayDir = ( edgeV0 - edgeV1 ).GetOrthogonal();

			// find the other intersection point
			for( int n = 0; n < GetNumEdges(); n++ )
			{
				if( n == edgeIndex ) continue;

				TVector2D<T> v0;
				TVector2D<T> v1;

				GetEdge( n, v0, v1 );

				TVector2D<T> intersection;
				if( IntersectsLineRay( v0, v1, rayOrigin, rayDir, intersection ) )
				{
					int minIndex = Min( n, edgeIndex );
					int maxIndex = Max( n, edgeIndex );

					newPolygon0.SetNumVertices( maxIndex - minIndex + 2 );
					newPolygon1.SetNumVertices( GetNumVertices() - maxIndex + minIndex + 2 );

					// left polygon
					int originVertexIndex = minIndex;
					for( int vi = 0; vi < newPolygon0.GetNumVertices() - 2; vi++ )
					{
						originVertexIndex++;

						newPolygon0.SetVertex( vi, m_vertex[originVertexIndex] );
					}
					newPolygon0.SetVertex( newPolygon0.GetNumVertices() - 2, intersection );
					newPolygon0.SetVertex( newPolygon0.GetNumVertices() - 1, rayOrigin );

					//test winding
					{
						TVector2D<T> edge0V0;
						TVector2D<T> edge0V1;
						TVector2D<T> edge1V0;
						TVector2D<T> edge1V1;
						newPolygon0.GetEdge( newPolygon0.GetNumEdges() - 2, edge0V0, edge0V1 );
						newPolygon0.GetEdge( newPolygon0.GetNumEdges() - 1, edge1V0, edge1V1 );

						TVector2D<T> edge0Dir = ( edge0V0 - edge0V1 );
						edge0Dir.Normalize();
						TVector2D<T> edge1Dir = ( edge1V0 - edge1V1 );
						edge1Dir.Normalize();
						
						if( edge0Dir.GetOrthogonal().DotProduct( edge1Dir ) > 0.0f )
						{	// correct winding order
							newPolygon0.SetVertex( newPolygon0.GetNumVertices() - 2, rayOrigin );
							newPolygon0.SetVertex( newPolygon0.GetNumVertices() - 1, intersection );
						}
					}

					// right polygon
					originVertexIndex = maxIndex;
					for( int vi = 0; vi < newPolygon1.GetNumVertices() - 2; vi++ )
					{
						originVertexIndex = ( originVertexIndex + 1 ) % GetNumVertices();

						newPolygon1.SetVertex( vi, m_vertex[originVertexIndex] );
					}
					newPolygon1.SetVertex( newPolygon1.GetNumVertices() - 2, rayOrigin );
					newPolygon1.SetVertex( newPolygon1.GetNumVertices() - 1, intersection );

					//test winding
					{
						TVector2D<T> edge0V0;
						TVector2D<T> edge0V1;
						TVector2D<T> edge1V0;
						TVector2D<T> edge1V1;
						newPolygon1.GetEdge( newPolygon1.GetNumEdges() - 2, edge0V0, edge0V1 );
						newPolygon1.GetEdge( newPolygon1.GetNumEdges() - 1, edge1V0, edge1V1 );

						TVector2D<T> edge0Dir = ( edge0V0 - edge0V1 );
						edge0Dir.Normalize();
						TVector2D<T> edge1Dir = ( edge1V0 - edge1V1 );
						edge1Dir.Normalize();
						
						if( edge0Dir.GetOrthogonal().DotProduct( edge1Dir ) > 0.0f )
						{	// correct winding order
							newPolygon1.SetVertex( newPolygon1.GetNumVertices() - 2, intersection );
							newPolygon1.SetVertex( newPolygon1.GetNumVertices() - 1, rayOrigin );
						}
					}

					return;
				}
			}
		}

		int GetNumEdges() const
		{
			return m_vertexCount;
		}

		int GetNumVertices() const
		{
			return m_vertexCount;
		}

		const TVector2D<T>* GetVertices() const
		{
			return m_vertex;
		}

		TVector2D<T>* GetVertices()
		{
			return m_vertex;
		}

		const TVector2D<T>& GetVertex(int index) const
		{
			assert( index >= 0 && index < m_vertexCount );
			return m_vertex[index];
		}

		TVector2D<T>& GetVertex(int index)
		{
			assert( index >= 0 && index < m_vertexCount );
			return m_vertex[index];
		}

		void GetEdge(int index, TVector2D<T>& v0, TVector2D<T>& v1) const
		{
			assert( index >= 0 && index < GetNumEdges() );
			v0 = m_vertex[index];
			v1 = ( index < m_vertexCount - 1 ) ? m_vertex[index + 1] : m_vertex[0];
		}

	private:

		double SqrDistanceToEdge(const TVector2D<T>& point, int edgeIndex, /*out*/ bool& pointIsRightOfEdge) const
		{
			const Edge& edge = m_edge[edgeIndex];

			TVector2D<T> diff = point - edge.center;
			T segmentParameter = edge.direction.DotProduct(diff);
			TVector2D<T> segmentClosestPoint;
			TVector2D<T> segmentClosestPointNormal;
			if (-edge.extent < segmentParameter)
			{
				if (segmentParameter < edge.extent)
				{
					segmentClosestPoint = edge.center + segmentParameter * edge.direction;
					segmentClosestPointNormal = -edge.direction.GetOrthogonal();
				}
				else
				{
					// Vertex 1 of Edge
					segmentClosestPoint = m_vertex[(edgeIndex < m_vertexCount - 1) ? edgeIndex + 1 : 0];
					segmentClosestPointNormal = m_normal[(edgeIndex < m_vertexCount - 1) ? edgeIndex + 1 : 0];
				}
			}
			else
			{
				// Vertex 0 of Edge
				segmentClosestPoint = m_vertex[edgeIndex];
				segmentClosestPointNormal = m_normal[edgeIndex];
			}

			diff = point - segmentClosestPoint;
			T sqrDistance = diff.DotProduct(diff);

			pointIsRightOfEdge = segmentClosestPointNormal.DotProduct(diff) < 0.0;

			return sqrDistance;
		}

        bool IntersectsLineRay( TVector2D<T> linePoint1, TVector2D<T> linePoint2,
                                TVector2D<T> rayOrigin,  TVector2D<T> rayDir, TVector2D<T>& intersection) const
        { // Based on the 2d line intersection method from "comp.graphics.algorithms Frequently Asked Questions" 

            /* 
                    (Ay-Cy)(Dx-Cx)-(Ax-Cx)(Dy-Cy) 
                r = -----------------------------  (eqn 1) 
                    (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx) 
            */

            float q = (linePoint1.y - rayOrigin.y) * (rayDir.x) -
                      (linePoint1.x - rayOrigin.x) * (rayDir.y);
            float d = (linePoint2.x - linePoint1.x) * (rayDir.y) -
                      (linePoint2.y - linePoint1.y) * (rayDir.x);

            if (d == 0) // parallel lines so no intersection anywhere in space (in curved space, maybe, but not here in Euclidian space.) 
            {
                return false;
            }

            float r = q / d;

            /* 
                    (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay) 
                s = -----------------------------  (eqn 2) 
                    (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx) 
            */

            q = (linePoint1.y - rayOrigin.y) * (linePoint2.x - linePoint1.x) -
                (linePoint1.x - rayOrigin.x) * (linePoint2.y - linePoint1.y);
            float s = q / d;

            /* 
                    If r>1, P is located on extension of AB 
                    If r<0, P is located on extension of BA 
                    If s>1, P is located on extension of CD 
                    If s<0, P is located on extension of DC 

                    The above basically checks if the intersection is located at an extrapolated 
                    point outside of the line segments. To ensure the intersection is only within 
                    the line segments then the above must all be false, ie r between 0 and 1 
                    and s between 0 and 1. 
            */

            if (r < 0 || r > 1 || s < 0 /*|| s > 1*/ )
            {
                return false;
            }

            /* 
                    Px=Ax+r(Bx-Ax)
                    Py=Ay+r(By-Ay)
            */

            intersection.x = linePoint1.x + r * (linePoint2.x - linePoint1.x);
            intersection.y = linePoint1.y + r * (linePoint2.y - linePoint1.y);
            return true;
        }
	};
}

#endif
