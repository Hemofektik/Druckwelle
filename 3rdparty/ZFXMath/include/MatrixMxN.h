/// \file
///
/// @brief TMatrixMxN
///
/// TMatrixMxN : M:N Dimensions TMatrixMxN Structure
// ///////////////////////////////////////////////////////////////////////////
// Autor:         Patrick Ullmann
// Erstellt:      05.04.04
// Änderungen:    05.04.04 (Patrick)  Datei erstellt
//				  ......(weitere undokumentierte Änderungen)
//				  25.12.05 (oese) Matrix auf ZFXMath-Standard gebracht;
//								  Operantoren ausgemistet
// ///////////////////////////////////////////////////////////////////////////
// Beschreibung:  Struktur für eine Matrix mit frei belegbaren Spalten 
//                und Zeilenanzahl (Inline-File)

#ifndef _ZFXMATH_INCLUDE_MATRIXMXN_H_
#define _ZFXMATH_INCLUDE_MATRIXMXN_H_

// I N C L U D E S ///////////////////////////////////////////////////////////

#include "ZFXMemory.h"
#include <memory>

// S T R U K T U R E N ///////////////////////////////////////////////////////
namespace ZFXMath
{
	// M = Zeilen
	// N = Spalten
	template<int M, int N, typename T>struct TMatrixMxN
	{
	public:

		union
		{
			struct
			{
				T	m[M*N];
			};
			struct
			{
				T	matrix[M][N];
			};
		};

		TMatrixMxN(){}

		TMatrixMxN(const T *p)
		{
			CopyMemory(m,p,M*N*sizeof(T));
		}

		TMatrixMxN(const T &val)
		{
			for(register int i = 0; i < M*N; i++)
			{
				m[i] = val;
			}
		}

		TMatrixMxN(const TMatrixMxN &mat)
		{
			for(register int i = 0; i < M*N; i++)
			{
				m[i] = mat.m[i];
			}
		}

		// Casting zum Array
		operator T*(){return m;}
		operator const T*()const{return m;}

		// Indexbasierter Elementzugriff
		const T& operator()(int i,int j)const{return m[ CheckM(i) * N + CheckN(j) ];}
		T& operator()(int i, int j){ return m[CheckM(i) * N + CheckN(j)]; }
		const T& operator()(int i)const{return m[ CheckMxN(i) ];}
		T& operator()(int i){return m[ CheckMxN(i) ];}
		const T* operator[](int i)const{return m + N * CheckM(i);}

		TMatrixMxN	operator+()const	{return (*this);}

		TMatrixMxN	operator+(const TMatrixMxN &mat)const
		{
			TMatrixMxN	ret(*this);
			ret += mat;
			return ret;
		}

		TMatrixMxN&	operator+=(const TMatrixMxN &mat)
		{
			for(register int i = 0; i < M * N; i++)
			{
				m[i] += mat(i);
			}
			return (*this);
		}

		TMatrixMxN	operator-()const	{return Negate();}

		TMatrixMxN	operator-(const TMatrixMxN &mat)const	{ return (*this) + mat.Negate(); }

		TMatrixMxN&	operator-=(const TMatrixMxN &mat)		{ (*this) += mat.Negate(); return *this; }

		TMatrixMxN	operator*(const T &val)const
		{
			TMatrixMxN	ret(*this);
			ret *= val;
			return ret;
		}

		TMatrixMxN&	operator*=(const T &val)
		{
			for(register int i = 0; i < M*N; i++)
			{
				m[i] *= val;
			}

			return *this;
		}

		template<int K> TMatrixMxN<M,K,T> operator*(const TMatrixMxN<N,K,T> &mat)const	{return Multiply(mat);}

		TMatrixMxN&	operator *= (const TMatrixMxN<M, M, T> &mat){ (*this) = Multiply(mat); return *this;  }

		TVectorN<M,T> operator * (const TVectorN<N,T> &vec) const { return Multiply(vec); }

		static TMatrixMxN Null(){return TMatrixMxN(0.0);}

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

		/*
		Invertiert eine NxN Matrix mit Hilfe des Gauß-Jordan-Algorithmus.

		mat - Matrix die Invertiert werden soll.
		inv - Die Inverse der Matrix mat.
		det - out parameter der die determinante der Matrix beinhaltet

		return: false, falls die Matrix nicht invertierbar ist.
		*/
		bool ComputeInverseMatrix(TMatrixMxN<M,N,T>& inv, T& det) const
		{
			assert(N == M); // this algorithm works only for square sized matrices

			// Eine Nx2N Matrix für den Gauß-Jordan-Algorithmus aufbauen
			std::shared_ptr<TMatrixMxN<M, N * 2, T>> aPtr(new TMatrixMxN<M, N * 2, T>()); // create element on heap because it may become large
			TMatrixMxN<M, N * 2, T>& A = *aPtr;
			for (size_t i = 0; i < N; ++i)
			{
				for (size_t j = 0; j < N; ++j)
					A.matrix[i][j] = matrix[i][j];
				for (size_t j = N; j < 2 * N; ++j)
					A.matrix[i][j] = (i == j - N) ? (T)1.0 : (T)0.0;
			}

			// Gauß-Algorithmus.
			for (size_t k = 0; k < N - 1; ++k)
			{
				// Zeilen vertauschen, falls das Pivotelement eine Null ist
				if (A.matrix[k][k] == (T)0.0)
				{
					for (size_t i = k + 1; i < N; ++i)
					{
						if (A.matrix[i][k] != (T)0.0)
						{
							A.SwapLine(k, i);
							break;
						}
						else if (i == N - 1)
							return false; // Es gibt kein Element != 0
					}
				}

				// Einträge unter dem Pivotelement eliminieren
				for (size_t i = k + 1; i < N; ++i)
				{
					double p = A.matrix[i][k] / A.matrix[k][k];
					for (size_t j = k; j < 2 * N; ++j)
						A.matrix[i][j] -= A.matrix[k][j] * p;
				}
			}

			// Determinante der Matrix berechnen
			det = (T)1.0;
			for (size_t k = 0; k < N; ++k)
				det *= A.matrix[k][k];

			if (det == 0.0)  // Determinante ist =0 -> Matrix nicht invertierbar
				return false;

			// Jordan-Teil des Algorithmus durchführen
			for (size_t k = N - 1; k > 0; --k)
			{
				for (int i = (int)k - 1; i >= 0; --i)
				{
					double p = A.matrix[i][k] / A.matrix[k][k];
					for (size_t j = k; j < 2 * N; ++j)
						A.matrix[i][j] -= A.matrix[k][j] * p;
				}
			}

			// Einträge in der linker Matrix auf 1 normieren und in inv schreiben
			for (size_t i = 0; i < N; ++i)
			{
				const double f = A.matrix[i][i];
				for (size_t j = N; j < 2 * N; ++j)
					inv.matrix[i][j - N] = A.matrix[i][j] / f;
			}

			return true;
		}

		void ComputeCovarianceMatrix(TVectorN<N, T>& mean, const TVectorN<N, T>* samples, const size_t numSamples)
		{
			assert(N == M);

			memset(mean.val, 0, N * sizeof(T));
			memset(matrix, 0, N * N * sizeof(T));

			// compute mean vector
			for (size_t s = 0; s < numSamples; ++s)
			{
				mean += samples[s];
			}
			mean *= (T)1.0 / (T)numSamples;

			// compute upper triangle of covariance matrix
			for (size_t s = 0; s < numSamples; ++s)
			{
				for (size_t i = 0; i < N; ++i)
				{
					for (size_t j = i; j < N; ++j)
					{
						matrix[i][j] += (samples[s].val[i] - mean.val[i]) * (samples[s].val[j] - mean.val[j]);
					}
				}
			}

			// copy upper triangle transposed to lower half
			for (size_t i = 0; i < N; ++i)
			{
				for (size_t j = 0; j < i; ++j)
				{
					matrix[i][j] = matrix[j][i];
				}
			}

			(*this) *= (T)1.0 / (T)numSamples;
		}

		T ComputeEllipsoidVolume() const
		{
			const bool isOddDimension = ((N & 1) == 1);
			T volume = isOddDimension ? (T)2.0 * matrix[0][0] : (T)PI * matrix[0][0] * matrix[1][1];
			for (int ij = isOddDimension ? 2 : 3; ij < N; ij += 2)
			{
				volume *= matrix[ij - 1][ij - 1];
				volume *= ((T)2.0 * (T)PI * matrix[ij][ij]) / (T)(ij + 1);
			}

			return volume;
		}

		template<int K>TMatrixMxN<M,K,T> Multiply(const TMatrixMxN<N,K,T> &mat) const
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

		TVectorN<M, T> Multiply(const TVectorN<N, T> &vec) const
		{
			TVectorN<M, T> ret(0.0);
			for (register int i = 0; i < M; i++)
			{
				for (register int j = 0; j < N; j++)
				{
					ret(i) += (*this)(i, j) * vec(j);
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
		void SetRow(int index, const TVectorN<N, T> &row)
		{
			for(register int i = 0; i < N; i++)
				(*this)(index,i) = row(i);
		}

		// Setzt eine Spalte
		void SetCol(int index, const TVectorN<M, T> &col)
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

		// Bringt Matrix in die Hermitesche Normalform     --- Achtung --- Noch nicht benutzbar!
		void HNF()
		{
			int End;
			if(M>N)End=N; else End=M;
			// Eins schaffen:
			int i, j;
			for(i=0;i<End;i++)
			{
				if(IsNull(i) == false)
				{
					ScaleRow(i, 1/matrix[i][i]);
					for(j=i+1;j<N;j++)	// Alle Zeilen bis unten
					AddScaledRow(j, i, (-1)*matrix[i][j]);
				}
			}
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
		

		/*
		Vertauscht zwei Zeilen in einer NxM Matrix.

		line1, line2 - Index der Zeilen, die vertauscht werden sollten.

		return: false, falls line1 oder line2 nicht in der Matrix liegen.
		*/
		void SwapLine(size_t line1, size_t line2)
		{
			for (int i = 0; i < N; ++i)
			{
				T t = matrix[line1][i];
				matrix[line1][i] = matrix[line2][i];
				matrix[line2][i] = t;
			}
		}

	private:

		inline int CheckM(int i) const
		{
			assert(i >= 0 && i < M);
			return i;
		}

		inline int CheckN(int i) const
		{
			assert(i >= 0 && i < N);
			return i;
		}

		inline int CheckMxN(int i) const
		{
			assert(i >= 0 && i < M*N);
			return i;
		}
	};

} // namespace ZFXMath


#endif // _ZFXMATH_INCLUDE_MATRIXMXN_H_
