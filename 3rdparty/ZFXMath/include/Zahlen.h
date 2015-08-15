#ifndef ZAHLEN_H
#define ZAHLEN_H

namespace ZFXMath
{

#include <math.h>

#define GOLDENERSCHNITT	1.6180339887498948482045868343656
#define GOLD_SCHN_QUER	-0.61803398874989484820458683436564
#define EINSDURCHWURZELFUENF	0.44721359549995793928183473374626

class Fibonacci	// Wer will, kann den langen Namen ja mit typedef abkürzen
{
public:
	Fibonacci(int _index){wert = EINDURCHWURZELFUENF*(pow(GOLDENERSCHNITT, _index)-pow(GOLD_SCHN_QUER, _index));index=_index;}
	long wert;
	int index;
	Fibonacci operator = (int _index) {return new Fibonacci(_index);}
};

class Lucas
{
public:
	Lucas(int _index){wert = pow(GOLDENERSCHNITT, _index)+pow(GOLD_SCHN_QUER, _index); index=_index;}
	long wert;
	int index;
	Lucas operator = (int _index) {return new Lucas(_index);}
};

class Polygonal
{
public:
	Polygonal(int _g){g=_g;}
	long n(long _n){return _n*((g-2)*_n)-(g-4)/2;}
	int g;
	Polygonal operator = (int _g) {return new Polygonal(_g);}
};



}

#endif