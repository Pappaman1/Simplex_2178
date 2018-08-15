#pragma once
#include <list>
#include <string>
#include <cctype>
template<typename T, typename S>
class Heap
{

private:
	int size;
	static const int MAX_SIZE = 100;
	T keys[MAX_SIZE];
	S values[MAX_SIZE];


	void swap(int i, int j)
	{
		S k = values[i];
		values[i] = values[j];
		values[j] = k;

		T l = keys[i];
		keys[i] = keys[j];
		keys[j] = l;



	}
	int parent(int i)
	{
		return (i - 1) / 2;

	}

	int childLeft(int i)
	{
		return 2 * i + 1;

	}

	int childRight(int i)
	{
		return 2 * i + 2;

	}
	void heapifyUp(int i)
	{
		if (i <= 0)
		{
			return;
		}
		int j = parent(i);
		if (keys[i] < keys[j])
		{
			swap(i, j);
		}
		heapifyUp(j);
	}
	void heapifyDn(int i)
	{
		int j;

		// If no children...
		if (childLeft(i) > size - 1) return;

		// If no right child...
		if (childRight(i) > size - 1) {
			j = childLeft(i);
		}
		else {
			// If both right and left children
			j = (keys[childLeft(i)] < keys[childRight(i)]) ? (childLeft(i)) : (childRight(i));
		}

		if (keys[i] > keys[j]) {
			swap(i, j);
		}

		heapifyDn(j);
	}
public:
	int getsize() {
		return size;
	};

	Heap<T, S>::Heap()
	{
		size = 0;
	}


	Heap<T, S>::~Heap()
	{
	}
	void push(T value1, S value2)
	{
		if (size < MAX_SIZE) {
			keys[size] = value1;
			values[size] = value2;
			size++;
		}
		heapifyUp(size - 1);
	}


	S pop()
	{
		if (size > 0)
		{
			
			swap(0, size - 1);
			size--;
			heapifyDn(0);
			return values[size];
			
		}
		size = 0;
		
		return 0;
	}
};

