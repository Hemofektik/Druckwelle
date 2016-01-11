
#include <vector>


template<typename T>
class Quadtree {
public:

	class Object {
	public:
		T x;
		T y;
		T width;
		T height;

		Object(T x, T y, T width, T height)
			: x(x)
			, y(y)
			, width(width)
			, height(height)
		{
		}

		virtual ~Object() {}

	};

	Quadtree(T x, T y, T width, T height, int maxLevel, int level = 0);
	~Quadtree();

	void AddObject(Object *object);
	void GetObjectsAt(T x, T y, std::vector<Object*>& intersectingObjects);
	void Clear();

private:
	T x;
	T y;
	T width;
	T height;
	int level;
	int	maxLevel;
	std::vector<Object*> objects;

	Quadtree<T>* NW;
	Quadtree<T>* NE;
	Quadtree<T>* SW;
	Quadtree<T>* SE;

	bool Contains(Quadtree<T> *child, Object *object);
};



template<typename T>
Quadtree<T>::Quadtree(T x, T y, T width, T height, int maxLevel, int level) :
	x		(x),
	y		(y),
	width	(width),
	height	(height),
	level	(level),
	maxLevel(maxLevel),
	NW(NULL),
	NE(NULL),
	SW(NULL),
	SE(NULL)
{
	if (level == maxLevel)
		return;

	NW = new Quadtree(x, y, width / 2.0f, height / 2.0f, maxLevel, level + 1);
	NE = new Quadtree(x + width / 2.0f, y, width / 2.0f, height / 2.0f, maxLevel, level + 1);
	SW = new Quadtree(x, y + height / 2.0f, width / 2.0f, height / 2.0f, maxLevel, level + 1);
	SE = new Quadtree(x + width / 2.0f, y + height / 2.0f, width / 2.0f, height / 2.0f, maxLevel, level + 1);
}

template<typename T>
Quadtree<T>::~Quadtree()
{
	if (level == maxLevel)
		return;

	delete NW;
	delete NE;
	delete SW;
	delete SE;
}

template<typename T>
void Quadtree<T>::AddObject(Object *object) {
	if (level == maxLevel) {
		objects.push_back(object);
		return;
	}

	if (Contains(NW, object)) {
		NW->AddObject(object); return;
	} else if (Contains(NE, object)) {
		NE->AddObject(object); return;
	} else if (Contains(SW, object)) {
		SW->AddObject(object); return;
	} else if (Contains(SE, object)) {
		SE->AddObject(object); return;
	}

	if (level == 0) {
		return;
	}

	objects.push_back(object);
}

template<typename T>
void Quadtree<T>::GetObjectsAt(T _x, T _y, std::vector<Object*>& intersectingObjects)
{
	if (level == maxLevel)
	{
		intersectingObjects.insert(intersectingObjects.end(), objects.begin(), objects.end());
		return;
	}

	if (_x > x + width / 2.0f && _x < x + width) {
		if (_y > y + height / 2.0f && _y < y + height) {
			intersectingObjects.insert(intersectingObjects.end(), objects.begin(), objects.end());
			SE->GetObjectsAt(_x, _y, intersectingObjects);
		} else if (_y > y && _y <= y + height / 2.0f) {
			intersectingObjects.insert(intersectingObjects.end(), objects.begin(), objects.end());
			NE->GetObjectsAt(_x, _y, intersectingObjects);
		}
	} else if (_x > x && _x <= x + width / 2.0f) {
		if (_y > y + height / 2.0f && _y < y + height) {
			intersectingObjects.insert(intersectingObjects.end(), objects.begin(), objects.end());
			SW->GetObjectsAt(_x, _y, intersectingObjects);
		} else if (_y > y && _y <= y + height / 2.0f) {
			intersectingObjects.insert(intersectingObjects.end(), objects.begin(), objects.end());
			NW->GetObjectsAt(_x, _y, intersectingObjects);
		}
	}
}

template<typename T>
bool Quadtree<T>::Contains(Quadtree<T> *child, Object *object) {
	return	 !(object->x < child->x ||
		   object->y < child->y ||
		   object->x > child->x + child->width  ||
		   object->y > child->y + child->height ||
		   object->x + object->width < child->x ||
		   object->y + object->height < child->y ||
		   object->x + object->width > child->x + child->width ||
		   object->y + object->height > child->y + child->height);
}

