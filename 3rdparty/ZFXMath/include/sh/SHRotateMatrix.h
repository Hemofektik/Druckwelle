/// \file
///
/// \if DE
/// @brief SphericalHarmonics::TRotateMatrix
///
/// TRotateMatrix: führt die Rotation der SH-Koeffizienten durch
/// \else
/// @brief SphericalHarmonics::TRotateMatrix
///
/// TRotateMatrix: performs the rotation of the SH-Coefficients
/// \endif

#ifndef	ZFXMATH_INCLUDE_SH_ROTATEMATRIX_H
#define	ZFXMATH_INCLUDE_SH_ROTATEMATRIX_H

namespace ZFXMath
{

namespace SphericalHarmonics
{

/// \if DE
/// @brief Rotationsmatrix zum transformieren von SH Koeffizienten.
/// \else
/// @brief Rotation Matrix, to transform SH Coefficients.
/// \endif
template<class PrecisionType, class FuncValueType>
class TRotateMatrix
{
public:
	TRotateMatrix(const int numBands) :

		m_numBands(numBands),
		m_Matrices(0),
		m_Elements(0)

	{
		/// Allocate the sub-matrix array
		m_Matrices = (SubMatrix*)operator new (m_numBands * sizeof(SubMatrix));

		int size = 0;
		for (int i = 0; i < m_numBands; i++)
			size += (i * 2 + 1) * (i * 2 + 1);

		/// Allocate the entire element array
		m_Elements = new PrecisionType[size];

		/// Construct each sub-matrix
		for (int i = 0, j = 0; i < m_numBands; i++)
		{
			int w = i * 2 + 1;
			m_Matrices[i] = *new SubMatrix(m_Elements + j, w);
			j += (w * w);
		}

		// The first 1x1 sub-matrix is always 1 and so doesn't need to be stored in the rotation matrix
		// It's stored simply to make the indexing logic as easy as possible
		m_Elements[0] = 1;
	}


	~TRotateMatrix(void)
	{
		if (m_Elements)
			delete [] m_Elements;
		if (m_Matrices)
			delete [] m_Matrices;
	}


	// Element accessors which take the band index and (+,-) index relative to the centre of the sub-matrix
	PrecisionType&	operator () (const int l, const int m, const int n)
	{
		return (m_Matrices[l](m, n));
	}
	PrecisionType operator () (const int l, const int m, const int n) const
	{
		return (m_Matrices[l](m, n));
	}


	int	GetNumBands(void) const
	{
		return m_numBands;
	}

	/// \if DE
	/// @brief Rotiert die gegebenen Koeffizienten
	/// \else
	/// @brief Rotates the given coefficients
	/// \endif
	void Transform(const TCoeffs<FuncValueType>& source, TCoeffs<FuncValueType>& dest) const
	{
		// Check number of bands match
		int numBands = source.GetNumBands();
		assert(numBands == dest.GetNumBands());

		// Band 0 is always multiplied by 1 so it stays untouched
		dest(0) = source(0);

		// Loop through each band
		for (int l = 1; l < numBands; l++)
		{
			SubMatrix& M = m_Matrices[l];

			// Calculate band offset into coeff list
			int bandOffset = l * (l + 1);

			// Now through each argument of the destination coeff-list
			for (int mo = -l; mo <= l; mo++)
			{
				// Clear destination
				FuncValueType& d = dest(bandOffset + mo);
				d = 0;

				// Pre-calculate of mo's lookup in the sub-matrix, plus mi's shift
				int p_mo = (mo + M.GetShift()) * M.GetWidth() + M.GetShift();

				// Multiply-add with each argument of the source coeff-list
				for (int mi = -l; mi <= l; mi++)
					d += source(bandOffset + mi) * FuncValueType( M(p_mo + mi) );
			}
		}
	}

	private:
		class SubMatrix;

		// Number of bands stored
		int						m_numBands;

		// Sub-matrix for each band
		SubMatrix*				m_Matrices;

		// Element array for each sub-matrix
		PrecisionType*			m_Elements;

		class SubMatrix
		{
		public:
			// Construct that needs previously allocated element array
			SubMatrix(PrecisionType* elements, const int width)  :
				m_Elements(elements),
				m_Width(width),
				m_Size(width * width),
				m_Shift((width - 1) / 2)
			{
			}


			// Element accessors with (+,-) lookup
			PrecisionType& operator () (const int m, const int n)
			{
				return (m_Elements[Index(m, n)]);
			}
			PrecisionType operator () (const int m, const int n) const
			{
				return (m_Elements[Index(m, n)]);
			}


			// Linear element accessors
			PrecisionType& operator () (const int i)
			{
				return (m_Elements[Index(i)]);
			}
			PrecisionType operator () (const int i) const
			{
				return (m_Elements[Index(i)]);
			}


			int	GetWidth(void) const
			{
				return (m_Width);
			}

			int	GetShift(void) const
			{
				return (m_Shift);
			}


		private:

			int Index(const int m, const int n) const
			{
				// Check bounds in debug build
				assert(m >= -m_Shift && m <= m_Shift);
				assert(n >= -m_Shift && n <= m_Shift);

				return ((m + m_Shift) * m_Width + (n + m_Shift));
			}

			int Index(const int i) const
			{
				// Check bounds in debug build
				assert(i >= 0 && i < m_Size);

				return (i);
			}

			// Element array
			PrecisionType*		m_Elements;

			// Width of the sub-matrix
			int		m_Width;

			// Total size of the sub-matrix in multiples of PrecisionType
			int		m_Size;

			// Value to shift incoming matrix indices by
			int		m_Shift;
		};

}; // class TRotateMatrix
} // namespace SphericalHarmonics

} // namespace ZFXMath

#endif // ZFXMATH_INCLUDE_SH_ROTATEMATRIX_H
