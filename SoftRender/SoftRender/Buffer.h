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
	const size_t GetWidth()const;
	const size_t GetHeight()const;

	T** GetppBuffer();
	void Initialize(size_t w, size_t h, T t);
	void Resize(size_t w, size_t h);
	void Resize(size_t w, size_t h, T t);
	void Clear(T);
	void Release();
	const T& operator()(size_t x,size_t y)const;
	T& operator()(size_t x, size_t y);
private:
	T* buffer;
	size_t width;
	size_t height;
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
const size_t Buffer<T>::GetWidth()const
{
	return width;
}

template<typename T>
const size_t Buffer<T>::GetHeight()const
{
	return height;
}

template<typename T>
void Buffer<T>::Initialize(size_t w, size_t h, T t)
{
	buffer = (T*)malloc(w*h * sizeof(T));
	//buffer.resize(w*h);
	width = w;
	height = h;
	Clear(t);
}

template<typename T>
void Buffer<T>::Resize(size_t w, size_t h, T t)
{
	Release();
	//buffer.resize(w*h);
	buffer = (T*)malloc(w*h * sizeof(T));
	width = w;
	height = h;
	Clear(t);
}

template<typename T>
void Buffer<T>::Resize(size_t w, size_t h)
{
	Release();
	//buffer.resize(w*h);
	buffer = (T*)malloc(w*h * sizeof(T));
	width = w;
	height = h;
}

template<typename T>
void Buffer<T>::Release()
{

}

template<typename T>
void Buffer<T>::Clear(T t)
{
	for (int i = 0; i < width*height; ++i)
	{
		buffer[i] = t;
	}
}

template<typename T>
const T& Buffer<T>::operator()(size_t x, size_t y)const
{
	size_t index = y*width + x;
	assert(index < height*width);
	return buffer[index];
} 

template<typename T>
T& Buffer<T>::operator()(size_t x, size_t y)
{
	return const_cast<T&>
		(static_cast<const Buffer<T>&>
		(*this)(x, y));
}

template<typename T>
T** Buffer<T>::GetppBuffer()
{
	return &buffer;
}

#endif