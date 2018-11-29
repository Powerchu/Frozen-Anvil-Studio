
#ifndef _O_STRING_H_
#define _O_STRING_H_

#include "Allocator/DefaultAlloc.h"
#include "System/Time/Timer.h"
#include "DataStructure/HashString.h"

class OString
{
public:
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


	const char *	c_str(void);
	HashID			id(void) const;

private:
	static constexpr size_t shortArr = 8;

	//update new full size before calling this
	void MyStrCopy(const char *_toCopyFrom, const size_t& _newCurSize);
	void Realloc(size_t _newBufferSize);
	void Rehash(void);

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
		if (mpCharBuffer)
		{
			if (mnBufferSize <= newSize)
				Realloc(newSize + 1);
		}
		else
		{
			mnBufferSize = newSize + 1;
			mpCharBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(mnBufferSize);
		}
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
	return *this;
}

#endif 
















