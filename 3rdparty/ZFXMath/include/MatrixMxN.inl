// Datei:         TMatrixMxN.inl
// ///////////////////////////////////////////////////////////////////////////
// Autor:         Patrick Ullmann
// Erstellt:      05.04.04
// Änderungen:    05.04.04 (Patrick)  Datei erstellt
// ///////////////////////////////////////////////////////////////////////////
// Beschreibung:  Struktur für eine Matrix mit frei belegbaren Spalten 
//                und Zeilenanzahl (Inline-File)

#ifndef _TMATRIXMXN_INL__25_07_2004_
#define _TMATRIXMXN_INL__25_07_2004_

// I N C L U D E S ///////////////////////////////////////////////////////////
#include <cassert>

// F U N K T I O N E N ///////////////////////////////////////////////////////
namespace ZFXMath
{
// ///////////////////////////////////////////////////////////////////////////
// PUBLIC Constructor
//
// Default Constructor
// ///////////////////////////////////////////////////////////////////////////
template<int M, int N, class T> inline TMatrixMxN<M, N, T>::TMatrixMxN (void)
{	
    identity ();
}

// ///////////////////////////////////////////////////////////////////////////
// PUBLIC Constructor
//
// Erstellt eine Matrix mit dem übergebenen Wert.
//
// [in] value: Wert womit alle Zeilen und Spalten gefüllt werden soll.
// ///////////////////////////////////////////////////////////////////////////
template<int M, int N, class T> inline TMatrixMxN<M, N, T>::TMatrixMxN (const T& value)
{	
        // Druchlaufe alle Einträge und setze sie auf den jeweiligen Wert
    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<N; ++j)
        {
            (*this).matrix[i][j] = value;
        }
    }
}

// ///////////////////////////////////////////////////////////////////////////
// PUBLIC Constructor
//
// Ein Copy-Constructor
//
// [in] other: Matrix die Kopiert werden soll
// ///////////////////////////////////////////////////////////////////////////
template<int M, int N, class T> inline TMatrixMxN<M, N, T>::TMatrixMxN (TMatrixMxN<M, N, T> const &other)
{
    (*this) = other;
}

// ///////////////////////////////////////////////////////////////////////////
// PUBLIC Constructor
//
// Erstellt eine Matrix mit Werten eines Übergebenen 1D Array der größe M*N
//
// [in] value: Ein 1D Array der größe M*N
// ///////////////////////////////////////////////////////////////////////////
template<int M, int N, class T> inline TMatrixMxN<M, N, T>::TMatrixMxN (const T (&value)[M*N])
{	
        // Druchlaufe alle Einträge und setze sie auf den jeweiligen Wert
    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<N; ++j)
        {
            (*this).matrix[i][j] = value[i*4+j];
        }
    }
}

// ///////////////////////////////////////////////////////////////////////////
// PUBLIC Destructor
//
// Der Destructor
// ///////////////////////////////////////////////////////////////////////////
template<int M, int N, class T> inline TMatrixMxN<M, N, T>::~TMatrixMxN (void)
{
}


// ///////////////////////////////////////////////////////////////////////////
// M A T R I Z E N   -   F U N K T I O N E N #################################
// ///////////////////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////////////////
// PUBLIC
//
// Erstellt eine Null-Matrix
// ///////////////////////////////////////////////////////////////////////////
template<int M, int N, class T> inline void TMatrixMxN<M, N, T>::zero (void)
{
        // alles auf 0 setzen
    ::memset ((*this).matrix, 0, sizeof(T&)*M*N);
}

// ///////////////////////////////////////////////////////////////////////////
// PUBLIC
//
// Erstellt eine Null-Matrix
// ///////////////////////////////////////////////////////////////////////////
template<int M, int N, class T> inline void TMatrixMxN<M, N, T>::identity (void)
{
        // alles auf 0 setzen
    ::memset ((*this).matrix, 0, sizeof(T&)*M*N);

        // Quadratisch?
    if (M == N)
    {
            // Hauptdiagonale auf 1 setzen
        for (int i=0; i<N; ++i)
        {
            (*this).matrix[i][i] = 1.0f;
        }
    }
    else if (M > N)	// Mehr Spalten als Zeilen
    {
        // Hauptdiagonale auf 1 setzen
        for (int i=0; i<N; ++i)
        {
            (*this).matrix[i][i] = 1.0f;
        }

            // Rest auf 1 setzen:
            // Beginnend bei N und endend bei M
        for (int i=N; i<M; ++i)
        {
            (*this).matrix[N-1][i] = 1.0f;
        }
    }
    else// if (N > M)	// Mehr Zeilen als Spalten
    {
            // Hauptdiagonale auf 1 setzen
        for (int i=0; i<M; ++i)
        {
            (*this).matrix[i][i] = 1.0f;
        }

            // Rest auf 1 setzen:
            // Beginnend bei N und endend bei M
        for (int i=M; i<N; ++i)
        {
            (*this).matrix[i][M-1] = 1.0f;
        }
    }
}

// ///////////////////////////////////////////////////////////////////////////
// PUBLIC
//
// Transponiert die Matrix
//
// [ret] Die Transponierte Matrix
// ///////////////////////////////////////////////////////////////////////////
template<int M, int N, class T> inline TMatrixMxN<N,M,T> TMatrixMxN<M, N, T>::transpose(void) const
{
        // Achtung: Hier geben wir nicht die Grundform (M,N) zurück sondern die
        //          Entgegengesetzte Form (N,M)!!
    TMatrixMxN<N,M,T> ret;
        
        // Druchlaufe alle Einträge und vertausche sie
    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<N; ++j)
        {
            ret.matrix[i][j] = (*this).matrix[j][i];
        }
    }
        
    return (ret);
}

// ///////////////////////////////////////////////////////////////////////////
// PUBLIC
//
// Rechnet alle Werte der Matrix |*(-1)
//
// [ret] Die Transponierte Matrix
// ///////////////////////////////////////////////////////////////////////////
template<int M, int N, class T> inline void TMatrixMxN<M, N, T>::negate (void)
{        
        // Druchlaufe alle Einträge und rechne |*(-1)
    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<N; ++j)
        {
            (*this).matrix[i][j] = -(*this).matrix[i][j];
        }
    }
}

// ///////////////////////////////////////////////////////////////////////////
// M A T R I Z E N   -   O P E R A T O R E N #################################
// ///////////////////////////////////////////////////////////////////////////

	// #######################################################################
	// Z U W E I S U N G S  -  O P E R A T O R E N
	// #######################################################################
template<int M, int N, class T> inline TMatrixMxN<M, N, T> const &TMatrixMxN<M, N, T>::operator= (TMatrixMxN<M, N, T> const &other)
{
        // Matrix gleichsetzen
    ::memcpy ((*this).matrix, other.matrix, sizeof(T&)*M*N);

    return (*this);
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> const &TMatrixMxN<M, N, T>::operator= (const T& value)
{
        // Druchlaufe alle Einträge und setze den wert 'value'
    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<N; ++j)
        {
            (*this).matrix[i][j] = value;
        }
    }

    return (*this);
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> const &TMatrixMxN<M, N, T>::operator= (const T (&value)[M*N])
{
        // Druchlaufe alle Einträge und setze den wert 'value'
    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<N; ++j)
        {
            (*this).matrix[i][j] = value[i*4+j];
        }
    }

    return (*this);
}

	// #######################################################################
	// V E R G L E I C H S  -  O P E R A T O R E N
	// #######################################################################
template<int M, int N, class T> inline bool TMatrixMxN<M, N, T>::operator== (TMatrixMxN<M, N, T> const &other)
{
        // Druchlaufe alle Einträge und teste sie zueinander
    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<N; ++j)
        {
            if ((*this).matrix[i][j] != other.matrix[i][j])
                return (false);
        }
    }

    return (true);
}

template<int M, int N, class T> inline bool TMatrixMxN<M, N, T>::operator== (const T& value)
{
        // Druchlaufe alle Einträge und teste sie zueinander
    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<N; ++j)
        {
            if ((*this).matrix[i][j] != value)
                return (false);
        }
    }

    return (true);
}

template<int M, int N, class T> inline bool TMatrixMxN<M, N, T>::operator== (const T (&value)[M*N])
{
        // Druchlaufe alle Einträge und teste sie zueinander
    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<N; ++j)
        {
            if ((*this).matrix[i][j] != value[i*4+j])
                return (false);
        }
    }

    return (true);
}

template<int M, int N, class T> inline bool TMatrixMxN<M, N, T>::operator!= (TMatrixMxN<M, N, T> const &other)
{
    return (!((*this) == other));
}

template<int M, int N, class T> inline bool TMatrixMxN<M, N, T>::operator!= (const T& value)
{
    return (!((*this) == TMatrixMxN<M,N,T>(value)));
}

template<int M, int N, class T> inline bool TMatrixMxN<M, N, T>::operator!= (const T (&value)[M*N])
{
    return (!((*this) == TMatrixMxN<M,N,T>(value)));
}


	// #######################################################################
	// A D D I T I O N S  -  O P E R A T O R E N
	// #######################################################################
template<int M, int N, class T> inline TMatrixMxN<M, N, T> TMatrixMxN<M, N, T>::operator+ (TMatrixMxN<M, N, T> const &other) const
{
    TMatrixMxN<M, N, T> ret;
            
        // Druchlaufe alle Einträge und addiere
    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<N; ++j)
        {
            ret.matrix[i][j] = (*this).matrix[i][j] + other.matrix[i][j];
        }
    }

    return (ret);
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> TMatrixMxN<M, N, T>::operator+ (const T& value) const
{
    return ((*this) + TMatrixMxN(value));
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> TMatrixMxN<M, N, T>::operator+ (const T (&value)[M*N]) const
{
    return ((*this) + TMatrixMxN(value));
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> const &TMatrixMxN<M, N, T>::operator+= (TMatrixMxN<M, N, T> const &other)
{
    (*this) = (*this) + other;

    return (*this);
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> const &TMatrixMxN<M, N, T>::operator+= (const T& value)
{
    (*this) = (*this) + TMatrixMxN(value);

    return (*this);
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> const &TMatrixMxN<M, N, T>::operator+= (const T (&value)[M*N])
{
    (*this) = (*this) + TMatrixMxN(value);

    return (*this);
}

	// #######################################################################
	// S U B T R A K T I O N S  -  O P E R A T O R E N
	// #######################################################################
template<int M, int N, class T> inline TMatrixMxN<M, N, T> TMatrixMxN<M, N, T>::operator- (void) const
{
    negate ();
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> TMatrixMxN<M, N, T>::operator- (TMatrixMxN<M, N, T> const &other) const
{
    TMatrixMxN<M, N, T> ret;

	// Druchlaufe alle Einträge und rechne minus der anderen Matrix
    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<N; ++j)
        {
            ret.matrix[i][j] = this->matrix[i][j] - other.matrix[i][j];
        }
    }

    return ret;
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> TMatrixMxN<M, N, T>::operator- (const T& value) const
{
    return ((*this) - TMatrixMxN(value));
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> TMatrixMxN<M, N, T>::operator- (const T (&value)[M*N]) const
{
    return ((*this) - TMatrixMxN(value));
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> const &TMatrixMxN<M, N, T>::operator-= (TMatrixMxN<M, N, T> const &other)
{
    (*this) = (*this) - other;

    return (*this);
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> const &TMatrixMxN<M, N, T>::operator-= (const T& value)
{
    (*this) = (*this) - TMatrixMxN(value);

    return (*this);
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> const &TMatrixMxN<M, N, T>::operator-= (const T (&value)[M*N])
{
    (*this) = (*this) - TMatrixMxN(value);

    return (*this);
}

	// #######################################################################
	// M U L T I P L I K A T I O N S  -  O P E R A T O R E N
	// #######################################################################
template<int M, int N, class T> inline TMatrixMxN<M, M, T> TMatrixMxN<M, N, T>::operator* (TMatrixMxN<N,M,T> const &other) const
{
    TMatrixMxN<M, M, T> ret;

    for (int i=0; i<M; ++i)
    {
        for (int j=0; j<M; ++j)
        {
            for (int k=0; k<N; ++k)
            {
                ret.matrix[i][j] += (*this).matrix[i][k] * other.matrix[k][j];
            }
        }
    }

	return (ret);
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> TMatrixMxN<M, N, T>::operator* (const T& value) const
{
    TMatrixMxN<M, N, T> ret;

        // Druchlaufe alle Einträge und rechne mal value
    for (int i=0; i<N; ++i)
    {
        for (int j=0; j<M; ++j)
        {
            ret.matrix[i][j] = (*this).matrix[i][j] * value;
        }
    }

    return (ret);
}

template<int M, int N, class T> inline TMatrixMxN<M, M, T> TMatrixMxN<M, N, T>::operator* (const T (&value)[M*N]) const
{
    return ((*this) * TMatrixMxN<M, N, T>(value));
}

template<int M, int N, class T> inline TMatrixMxN<M, M, T> const &TMatrixMxN<M, N, T>::operator*= (TMatrixMxN<M, N, T> const &other)
{    
    (*this) = (*this) * other;

    return (*this);
}

template<int M, int N, class T> inline TMatrixMxN<M, N, T> const &TMatrixMxN<M, N, T>::operator*= (const T& value)
{    
    (*this) = (*this) * value;

    return (*this);
}

template<int M, int N, class T> inline TMatrixMxN<M, M, T> const &TMatrixMxN<M, N, T>::operator*= (const T (&value)[M*N])
{    
    (*this) = (*this) * TMatrixMxN<M, N, T>(value);

    return (*this);
}

}; // Namespace

#endif // _TMATRIXMXN_INL__25_07_2004_
