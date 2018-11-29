
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

	void Rehash(void);

	size_t		mnCurSize;			// 8 bytes
	size_t		mnFullSize;			// 8 bytes
	HashID		mnID;				// 8 bytes
	const char	*mpLiteral;			// 8 bytes
	char		*mpCharBuffer;		// 8 bytes
	bool		mbRehash;			// 2 bytes
};

template<size_t N>
inline OString::OString(char const (&_literal)[N])
	: mnCurSize{ N - 1 }, mnFullSize{ N }, mnID{ StringHasher(_literal) }, mpLiteral{ &_literal[0] },
	mpCharBuffer{ nullptr }, mbRehash{ false }
{}

template<typename T>
inline OString::OString(T _str)
{
	static_assert("Not accepted");
}

template<>
inline OString::OString(const char * _cstr)
	: mnCurSize{ strlen(_cstr) }, mnFullSize{ mnCurSize + 1 }, mnID{ 0 }, mpLiteral{ nullptr },
	mpCharBuffer{ /*/ new char[mnFullSize] /*/ Dystopia::DefaultAllocator<char[]>::Alloc(mnFullSize) /**/ }, mbRehash{ true }
{
	if (mpCharBuffer)
	{
		size_t i = 0;
		for (; i < mnCurSize; ++i)
			mpCharBuffer[i] = _cstr[i];
		for (; i < mnFullSize; ++i)
			mpCharBuffer[i] = '\0';
	}
}

template <unsigned N>
inline OString& OString::operator=(const char(&_s)[N])
{
	return *this;
}

template<typename T>
inline OString& OString::operator=(T _str)
{
	static_assert("Not accepted");
	return *this;
}

template<>
inline OString& OString::operator=(const char * _str)
{
	const size_t newSize = strlen(_str);

	if (mpLiteral)
	{
		mpCharBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(mnFullSize > newSize ? mnFullSize : newSize);
		strcpy_s(mpCharBuffer, newSize, _str);
		if (mnFullSize <= newSize)
			mnFullSize = newSize + 1;
		mpLiteral = nullptr;
	}
	else
	{
		size_t i = 0;
		if (mnFullSize <= newSize)
		{
			Dystopia::DefaultAllocator<char[]>::Free(mpCharBuffer);
			mnFullSize = newSize + 1;
			mpCharBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(mnFullSize);
		}
		for (; i < newSize; ++i)
			mpCharBuffer[i] = _str[i];
		for (; i < mnFullSize; ++i)
			mpCharBuffer[i] = '\0';
	}
	mnCurSize = newSize;
	mbRehash = true;
	return *this;
}

template<>
inline OString& OString::operator=(const wchar_t * _str)
{
	return *this;
}

#endif 
















