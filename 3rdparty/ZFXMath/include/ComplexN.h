#ifndef COMPLEXN_H
#define COMPLEXN_H

/*
==========================================================================================================================================================
||                                                                         ComplexN.h                                                                   ||
||                                                                                                                                                      ||
||                              Enthält Definitionen und Funktionen von komplexen Zahlen                                                                ||
||                                                                                                                                                      ||
||                              Zuweisung                                                                                                               ||
||                              Vergleich                                                                                                               ||
||                              Addition                                                                                                                ||
||                              Multiplikation                                                                                                          ||
||                              Potenzieren                                                                                                             ||
||                                                                                                                                                      ||
||                                                                                                                                                      ||
==========================================================================================================================================================
*/

#include <iostream>

namespace ZFXMath
{
	template<typename T>
	struct TComplexN
	{
 		T re, im;
		TComplexN(T _re, T _im):re(_re), im(_im){}	// kartesische Koordinaten
		static TComplexN Polar(T bg, T arg){return TComplex(bg*Cos(arg), bg*Sin(arg));}	// Polarkoordinaten
		T Re(){return re;}
		T Im(){return im;}
		TComplexN Conjugate(){return TComplexN(re, -im);}	//	Gibt Konjugiert Komplexes der Funktion zurück
		T Abs(){return sqrt(im*im+re*re);}	//	Gibt Betrag der Funktion zurück
		double Arg(){return ATan2(im,re);}	// Gibt Argument der Funktion zurück
		
		TComplexN& operator = (const TComplexN<T> &a) {re = a.re; im = a.im; return (*this);}
  		TComplexN& operator = (const T &a) {re = a; im = 0.0; return (*this);}          // falls eine Zahl wie float, int etc. zugewiesen wird ist diese normalerweise reell.
		bool operator == (const TComplexN<T> &a) {if(re == a.re && im == a.im)return true; return false;}	// Vergleicht zwei komplexe Zahlen
		bool operator != (const TComplexN<T> &a) {if(re == a.re && im == a.im)return false; return true;}

        TComplexN operator + (const TComplexN& a) {return TComplexN(re + a.re, im + a.im);}
		TComplexN operator + (const T &a) {return TComplexN(re + a, im);}
        TComplexN operator - (const TComplexN& a) {return TComplexN(re - a.re, im - a.im);}
		TComplexN operator - (const T &a) {return TComplexN(re - a, im);}
		TComplexN operator * (const TComplexN& a) {return TComplexN(re * a.re - im * a.im, im * a.re + re * a.im);}
		TComplexN operator * (const T &a) {return TComplexN(re * a, im * a);}
		TComplexN operator / (const TComplexN& a) 
		{
			const T dividend = a.re*a.re+a.im*a.im;
			return TComplexN((re*a.re+im*a.im)/dividend, (im*a.re-re*a.im)/dividend);
		}
		TComplexN operator / (const T a) {return TComplexN(re / a, im / a);}
		TComplexN operator ^ (const T n) {return Polar(pow(this.Abs(), n), this.Arg*n);}	// Potenziert die komplexe Zahl




		friend std::ostream& operator<< ( std::ostream& os, const TComplexN& c)
		{
			if(c.re != 0)
				os << c.re;
			if(c.im == 1)
			{
				if(c.re != 0)
					os << "+";
				os << "i";
			}
			else if(c.im>0) 
				os << "+" << c.im << "i";
			if(c.im == -1)
				os << "-i";
			else if(c.im<0)	// Wenn c negativ ist, wird das Minus eh mit ausgegeben
				os << c.im << "i";

			// Wenn Imaginaerteil == 0 wird er nicht mit ausgegeben. 
			return os;
		}
		
	};
} 


#endif
