#ifndef SET_H
#define SET_H

#include <malloc.h>

namespace ZFXMath
{
	template<typename T>
	class TSet	// Enthält Definition einer Menge
	{
	public:
		long card;	// Kardinalität = Anzahl der in der Menge enthaltenen Elemente
		T*Element;	// Enthält Elemente der Menge

		TSet(long _card, T*_Element)
		{
			card = _card;
			Element = (T*)malloc(sizeof(T)*card);
			int i;
			for(i=0;i<card;i++)
			{
				Element[i] = _Element[i];
			}
		}
		
		const T operator [] (int index)	//	Gibt index-tes Element der Menge zurück
		const { return Element[index];}
		
		bool operator==(TSet a)	// Vergleich ob 2 Mengen gleich sind
		{
			int i, j;
			for(i=0;i<card;i++)
			{
				bool exel = false;	// Existiert Element?
				for(j=0;j<a.card;j++)
				{
					if(Element[i] == a.Element[j])	{exel = true;break;}
				}
				if(exel == false)	// Dieser Fall tritt nur ein wenn ein Element von this nicht in a gefunden wurde
					return false;
			}
			return true;	// Die Fkt. kommt nur hierhin falls exel niemals falsch war. Klug, ne?
		}

		bool IsElement(T a)	// Gibt es in der Menge das Element a?
		{
			for(register int i = 0;i<card;i++)
			{
				if(Element[i] == a)
					return true;
			}
			return false;
		}

  		TSet& operator &(TSet a)	// Schneidet zwei Mengen
		{
			int tempcard, i, Zaehler;
			Zaehler = 0;
			if(card>a.card) tempcard = a.card; else tempcard = card;
			T*temp = (T*)malloc(tempcard*sizeof(T));
			for(i=0;i<tempcard;i++)
			{
				if(a.IsElement(Element[i]))
				{
					temp[Zaehler] = Element[i];
					Zaehler++;
				}
			}
			return TSet(Zaehler, temp);
		}

		TSet& operator |(TSet a)   // Vereinigt zwei Mengen
		{
			int tempcard, i, Zaehler;
			Zaehler = 0;
			if(card>a.card) tempcard = a.card; else tempcard = card;
			T*temp = (T*)malloc(tempcard*sizeof(T));
			for(i=0;i<tempcard;i++)
			{
				if(a.IsElement(Element[i]))
				{
					temp[Zaehler] = Element[i];
					Zaehler++;
				}
			}
			return TSet(Zaehler, temp);
		}

  		TSet& operator *(TSet a);   // Kartesisches Produkt

        TSet& C(TSet a);    // Eine Menge ohne die Andere (Muss die Schreibweise nochmal genau nachgucken)

        friend std::ostream& operator<< ( std::ostream& os, const TSet& a)
		{
            int i;
			os << "{";
            for(i=0;i<a.card;i++)
            {
				os << a.Element[i];
				if(i+1<a.card)
					os << ", ";
            }
			os << "}";
			return os;
		}


	};

}


#endif
