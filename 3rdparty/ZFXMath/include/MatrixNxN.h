/// \file
///
/// @brief TMatrixNxN
///
/// TMatrixNxN : N:N Dimensions TMatrixNxN Structure
// ///////////////////////////////////////////////////////////////////////////
// Autor:         Wassja A. Kopp
// Erstellt:      14.01.06
// Änderungen:    Fairerweise muss gesagt werden, dass das meiste aus MxN übernommen wurde
// ///////////////////////////////////////////////////////////////////////////
// Beschreibung:  Struktur für eine quadratische Matrix mit frei belegbaren Spalten 
//                und Zeilenanzahl (Inline-File)


#ifndef _ZFXMATH_INCLUDE_MATRIXNXN_H_
#define _ZFXMATH_INCLUDE_MATRIXNXN_H_

// I N C L U D E S ///////////////////////////////////////////////////////////

#include "ZFXMemory.h"

// S T R U K T U R E N ///////////////////////////////////////////////////////
namespace ZFXMath
{

	// Achtung: Diese Datei befindet sich im Versuchsstadium und wird es auch noch lange bleiben!

	// N = Spalten
	template<int N, typename T>struct TMatrixNxN
	{

		private:

		union	Matrix
		{
			T	m[N*N];
			T	matrix[N][N];
		};

		union	// Inverse der Matrix
		{
			T	minv[N*N];	
			T	matrixinv[N][N];
		};

		T det;	// Determinante der Matrix

		public:

		TMatrixNxN(){}

		TMatrixNxN(const T *p)
		{
			CopyMemory(m,p,N*N*sizeof(T));
			CopyMemory(minv,NULL,N*N*sizeof(T));
		}

		TMatrixNxN(const T &val)
		{
			for(register int i = 0; i < M*N; i++)
			{
				m[i] = val;
				minv[i] = NULL;
			}
		}

		TMatrixNxN(const TMatrixNxN &mat)
		{
			for(register int i = 0; i < N*N; i++)
			{
				m[i] = mat.m[i];
			}
		}

		// Casting zum Array
		operator T*(){return m;}
		operator const T*()const{return m;}

		// Indexbasierter Elementzugriff
		const T& operator()(int i,int j)const{return m[ CheckN(i) * N + CheckN(j) ];}
		T& operator()(int i,int j){return m[ CheckN(i) * N + CheckN(j) ];}
		const T& operator()(int i)const{return m[ CheckNxN(i) ];}
		T& operator()(int i){return m[ CheckNxN(i) ];}
		const T* operator[](int i)const{return m + N*CheckN(i);}
		const T* operator[](int i)const{return m + N*CheckN(i);}

		TMatrixNxN	operator+()const	{return (*this);}

		TMatrixNxN	operator+(const TMatrixNxN &mat)const
		{
			TMatrixNxN	ret(*this);
			ret += mat;
			return ret;
		}

		TMatrixNxN&	operator+=(const TMatrixNxN &mat)
		{
			for(register int i = 0; i < N * N; i++)
			{
				m[i] += mat(i);
			}
			return (*this);
		}

		TMatrixNxN	operator-()const	{return Negate();}

		TMatrixNxN	operator-(const TMatrixNxN &mat)const	{return (*this) + mat.Negate();}

		TMatrixNxN	operator-=(const TMatrixNxN &mat)		{(*this) += mat.Negate();return *this}

		TMatrixNxN	operator*(const T &val)const
		{
			TMatrixNxN	ret(*this);
			ret *= val;
			return ret;
		}

		TMatrixNxN&	operator*=(const T &val)
		{
			for(register int i = 0; i < N*N; i++)
			{
				m[i] *= i;
			}
		}

		template TMatrixNxN<N,T> operator*(const TMatrixMxN &mat)const	{return Multiply(mat);}

		TMatrixMxN&	operator *= (const TMatrixMxN<M,M,T> &mat){(*this) = Multiply(mat);}

		TVectorN<M,T> operator * (const TVectorN<N,T> &vec){return (*this) * (TMatrixMxN<M,1,T>)vec;}

		static TMatrixMxN Null(){return MatrixMxN(0.0);}

		static TMatrixMxN Identity()
		{
			TMatrixMxN ret = TMatrixMxN::Null();

			      // Quadratisch?
		    if (M == N)
			{
			    // Hauptdiagonale auf 1 setzen
				for (int i=0; i<N; ++i)
				{
				    ret(i,i) = 1.0f;
			    }
		    }
		    else if (M > N)	// Mehr Spalten als Zeilen
		    {
		        // Hauptdiagonale auf 1 setzen
		        for (int i=0; i<N; ++i)
		        {
		            ret(i,i) = 1.0f;
		        }
		
		            // Rest auf 1 setzen:
		            // Beginnend bei N und endend bei M
		        for (int i=N; i<M; ++i)
		        {
		            ret(N-1,i) = 1.0f;
		        }
		    }
		    else// if (N > M)	// Mehr Zeilen als Spalten
		    {
		            // Hauptdiagonale auf 1 setzen
		     for (int i=0; i<M; ++i)
		     {
		         ret(i,i) = 1.0f;
		     }
	
	            // Rest auf 1 setzen:
	            // Beginnend bei N und endend bei M
		        for (int i=M; i<N; ++i)
		        {
		            ret(i,M-1) = 1.0f;
		        }
		    }
		
			return ret;
		}

		TMatrixMxN	Negate()const
		{
			TMatrixMxN ret;
			for(register int i = 0; i < M * N; i++)
			{
				ret(i) = -m[i];
			}
			return ret;
		}

		TMatrixMxN<N,M,T> Transpose()
		{
			TMatrixMxN<N,M,T> ret;
			for(register int i = 0; i < M; i++)
			{
				for(register int j = 0; j < N; j++)
				{
					ret(j,i) = (*this)(i,j);
				}
			}
			return ret;
		}

		template<int K>TMatrixMxN<M,K,T> Multiply(const TMatrixMxN<N,K,T> &mat)
		{
			TMatrixMxN<M,K,T> ret(0.0);
			for(register int k = 0; k < K; k++)
			{
				for(register int i = 0; i < M; i++)
				{
					for(register int j = 0; j < N; j++)
					{
						ret(i,k) += (*this)(i,j) * mat(j,k);
					}
				}
			}
			return ret;
		}

		// Gibt eine Zeile zurück
		TVectorN<N,T> GetRow(int index)
		{
			TVectorN<N, T> result;
			for(register int i=0;i<N;i++)
				result(i) = (*this)(index,i);
			return result;
		}

		// Gibt eine Spalte zurück
		TVectorN<M,T> GetCol(int index)
		{
			TVectorN<M, T> result;
			for(register int i = 0; i < M; i++)
				result(i) = (*this)(i,index);
			return result;
		}

		// Setzt eine Zeile
		void SetRow(int index, const TVector<N, T> &row)
		{
			for(register int i = 0; i < N; i++)
				(*this)(index,i) = row(i);
		}

		// Setzt eine Spalte
		void SetCol(int index, const TVector<M, T> &col)
		{
			for(register int i = 0; i < M; i++)
				(*this)(i,index) = col(i);
		}

		// Prüft, ob eine Zeile Null ist (Name mit Row wäre angebracht)
		bool IsNull(int row)
		{
			for(register int i = 0; i < N; i++)
			{
				if(0.0 != (*this)(row,i))return false;
			}
			return true;
		}

		// Multipliziert eine Zeile mit einem Skalar
		void ScaleRow(int row, const T &factor)
		{
			for(register int i = 0; i < N; i++)
			{
				(*this)(row,i) *= factor;
			}
		}

		// Addiert zu einer Zeile eine skalierte andere
		void AddScaledRow(int rowa, int rowb, const T &factor)
		{
			for(register int i = 0; i < N; i++)
			{
				(*this)(rowa,i) += (*this)(rowb,i) * factor;
			}
		}

		// Vertauscht zwei Zeilen (Name mit Row wäre angebracht)
		void Interchange(int rowa, int rowb)
		{
			TVectorN<N,T> temp = GetRow(rowa);
			SetRow(rowa, GetRow(rowb));
			SetRow(rowb, temp);
		}

		// Ausgabeoperator
		friend std::ostream& operator<< ( std::ostream& os, const TMatrixMxN& m )
		{
			for ( int x = 0; x < M; x++ )
			{
				os << "[";
				for ( int y = 0; y < N; y++ )
				{
					os << m[ x * 3 + y ];
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

		inline int CheckM(int i)
		{
			assert(i >= 0 && i < M);
			return i;
		}

		inline int CheckN(int i)
		{
			assert(i >= 0 && i < N);
			return i;
		}

		inline int CheckMxN(int i)
		{
			assert(i >= 0 && i < M*N);
			return i;
		}
	};

} // namespace ZFXMath


#endif // _ZFXMATH_INCLUDE_MATRIXMXN_H_
