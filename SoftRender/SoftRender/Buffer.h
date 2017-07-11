#pragma once
#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>
using namespace std;

template<typename T>
class Buffer
{
public:
	Buffer();
	~Buffer();

	void Resize(size_t r, size_t c, T t);
	void Clear(T);
	void Release();
	const T& operator()(size_t r,size_t c)const;
	T& operator()(size_t r, size_t c);
private:
	std::vector<T> buffer;
	size_t row;
	size_t col;
};
template<typename T>
Buffer<T>::Buffer()
{
}

template<typename T>
Buffer<T>::~Buffer()
{
}

template<typename T>
void Buffer<T>::Resize(size_t r, size_t c, T t)
{
	Release();
	buffer.resize(r*c);
	row = r;
	col = c;
	Clear(t);
}

template<typename T>
void Buffer<T>::Release()
{
	for (int i = 0; i < buffer.size(); ++i)
	{
		if (buffer[i])
		{
			buffer[i] = 0;
		}
	}
	buffer.clear();
}

template<typename T>
void Buffer<T>::Clear(T t)
{
	for (int i = 0; i < buffer.size(); ++i)
	{
		buffer[i] = t;
	}
}

template<typename T>
const T& Buffer<T>::operator()(size_t r, size_t c)const
{
	size_t index = r*row + c;
	assert(index < row*col);
	return buffer[r*row + c];
}

template<typename T>
T& Buffer<T>::operator()(size_t r, size_t c)
{
	return const_cast<T&>
		(static_cast<const Buffer<T>&>
		(*this)(r, c));
}

#endif