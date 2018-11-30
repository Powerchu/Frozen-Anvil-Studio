
#ifndef _O_STRING_H_
#define _O_STRING_H_

#include "Allocator/DefaultAlloc.h"
#include "System/Time/Timer.h"
#include "DataStructure/HashString.h"

class OString
{
public:
	static constexpr size_t nPos = static_cast<size_t>(-1);
	/* Constructors */

	OString(void);
	OString(OString&& _moveCtor) noexcept;
	OString(const OString& _copyCtor);
	OString(const char* _cstrStart, const char* _cstrEnd);
	~OString(void);

	template<size_t N>
	OString(char const (&_literal)[N]);
	template<typename T>
	OString(T _str);

	/* Assignment operators */

	template <unsigned N>
	OString& operator=(const char(&_s)[N]);
	template<typename T>
	OString& operator=(T _str);
	OString& operator=(const OString&);
	OString& operator=(OString&&) noexcept;

	/* set functions */

	void clear(void);

	/* element access */

	char&   operator[](size_t _pos);
	char	operator[](size_t _pos) const;
	char&   back(void);
	char	back(void) const;
	char&   front(void);
	char	front(void) const;

	/* modifiers */

	template<unsigned N>
	OString& operator+=(const char(&_s)[N]);
	template<typename T>
	OString& operator+=(T);
	OString& operator+=(const OString&);
	OString& erase(size_t _pos, size_t _len = nPos);
	OString& replace(size_t _pos, size_t _len, const OString&);
	OString& replace(size_t _pos, size_t _len, const char * _s);


	/* get functions */

	size_t			bufferSize(void) const;
	size_t			len(void) const;
	size_t			size(void) const;
	const char *	c_str(void) const;
	HashID			id(void) const;


	void Rehash(void);

private:
	static constexpr size_t shortArr = 8;

	//update new full size before calling this
	void MyStrCopy(const char *_toCopyFrom, const size_t& _newCurSize);
	void Realloc(size_t _newBufferSize);
	void Alloc(size_t _newSize);
	void CopyFromLiteral(void);

	size_t		mnCurSize;			// 8 bytes
	size_t		mnBufferSize;		// 8 bytes
	HashID		mnID;				// 8 bytes
	const char	*mpLiteral;			// 8 bytes
	char		*mpCharBuffer;		// 8 bytes
	bool		mbRehash;			// 2 bytes
};

template<size_t N>
inline OString::OString(char const (&_literal)[N])
	: mnCurSize{ N - 1 }, mnBufferSize{ 0 }, mnID{ StringHasher(_literal) }, mpLiteral{ &_literal[0] },
	mpCharBuffer{ nullptr }, mbRehash{ false }
{}

template<typename T>
inline OString::OString(T _str)
{
	static_assert(false, "Not accepted");
}

template<>
inline OString::OString(const char * _cstr)
	: mnCurSize{ strlen(_cstr) }, mnBufferSize{ mnCurSize + 1 }, mnID{ 0 }, mpLiteral{ nullptr },
	mpCharBuffer{ Dystopia::DefaultAllocator<char[]>::Alloc(mnBufferSize) }, mbRehash{ true }
{
	if (mpCharBuffer)
	{
		size_t i = 0;
		for (; i < mnCurSize; ++i)
			mpCharBuffer[i] = _cstr[i];
		for (; i < mnBufferSize; ++i)
			mpCharBuffer[i] = '\0';
	}
}

template <unsigned N>
inline OString& OString::operator=(const char(&_s)[N])
{
	mnCurSize = N - 1;
	mbRehash = true;
	mpLiteral = &_s[0];
	return *this;
}

template<typename T>
inline OString& OString::operator=(T _str)
{
	static_assert(false, "Not accepted");
	return *this;
}

template<>
inline OString& OString::operator=(const char * _str)
{
	const size_t newSize = strlen(_str);
	if (mpLiteral)
	{
		Alloc(newSize);
		mpLiteral = nullptr;
	}
	else
	{
		if (mnBufferSize <= newSize)
			Realloc(newSize + 1);
	}
	MyStrCopy(_str, newSize);
	return *this;
}

template<>
inline OString& OString::operator=(const wchar_t * _str)
{
	const size_t newSize = strlen(reinterpret_cast<char const*>(_str));
	if (mpLiteral)
	{
		Alloc(newSize);
		mpLiteral = nullptr;
	}
	else
	{
		if (mnBufferSize <= newSize)
			Realloc(newSize + 1);
	}
	size_t i = 0;
	for (; i < newSize; ++i)
		mpCharBuffer[i] = static_cast<char>(_str[i]);
	for (; i < mnBufferSize; ++i)
		mpCharBuffer[i] = '\0';
	mbRehash = true;
	return *this;
}

template<unsigned N>
inline OString& OString::operator+=(const char(&_s)[N])
{
	const size_t newSize = mnCurSize + N - 1;
	if (mpLiteral)
	{ 
		if (mpCharBuffer && mnBufferSize > newSize)
		{
			CopyFromLiteral();
			size_t i = mnCurSize;
			for (; i < newSize; i++)
				mpCharBuffer[i] = _s[i - mnCurSize];
			for (; i < mnBufferSize; i++)
				mpCharBuffer[i] = '\0';
		}
		else
		{
			Alloc(newSize);
			CopyFromLiteral();
			for (size_t i = mnCurSize; i < mnBufferSize; i++)
				mpCharBuffer[i] = _s[i - mnCurSize];
		}
	}
	else
	{
		if (mpCharBuffer)
		{
			if (mnBufferSize <= newSize)
			{
				char *tempBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(newSize + 1);
				size_t i = 0, j = 0;
				for (; i < mnCurSize; ++i)
					tempBuffer[i] = mpCharBuffer[i];
				for (; j < N; j++)
					tempBuffer[i + j] = _s[j];

				tempBuffer[newSize] = '\0';
				Dystopia::DefaultAllocator<char[]>::Free(mpCharBuffer);
				mpCharBuffer = tempBuffer;
				mnBufferSize = newSize + 1;
			}
			else
			{
				size_t i = mnCurSize;
				for(; i < newSize; ++i)
					mpCharBuffer[i] = _s[i - mnCurSize];
				mpCharBuffer[newSize] = '\0';
			}
		}
		else
		{
			Alloc(newSize);
			size_t i = 0;
			for (; i < N; ++i)
				mpCharBuffer[i] = _s[i];
		}
	}
	mnCurSize = newSize;
	mbRehash = true;
	return *this;
}

template<typename T>
inline OString& OString::operator+=(T _element)
{
	static_assert(false, "Not accepted");
	return *this;
}

template<>
inline OString& OString::operator+=(T _element)
{
	
}

template<>
inline OString& OString::operator+=(const char * _str)
{
	size_t i = 0, j = 0;
	const size_t otherSize = strlen(_str);
	const size_t newSize = mnCurSize + otherSize;

	if (mpLiteral)
	{

	}

	//char* buffer = Dystopia::DefaultAllocator<char[]>::Alloc(otherSize + mSize + 1);
	//while (i < mSize)
	//{
	//	buffer[i] = mCharBuffer[i];
	//	++i;
	//}
	//while (j < otherSize)
	//{
	//	buffer[i + j] = *(_s + j);
	//	++j;
	//}
	//buffer[otherSize + mSize] = '\0';
	//mSize += otherSize;
	//Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);
	//mCharBuffer = buffer;
	//mbRehash = true;

	return *this;
}

template<>
inline OString& OString::operator+=(const char _c)
{
	return *this;
}

template<>
inline OString& OString::operator+=(unsigned _n)
{
	return *this;
}

template<>
inline OString& OString::operator+=(unsigned long long _ull)
{
	return *this;
}

#endif 
















