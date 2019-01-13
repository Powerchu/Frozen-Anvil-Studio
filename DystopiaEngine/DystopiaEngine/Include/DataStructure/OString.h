
#ifndef _O_STRING_H_
#define _O_STRING_H_

#define U_BUFSIZE  11
#define ULL_BUFSIZE 21
/**/
#include <cstdint>
#include <iostream>
#include "Globals.h"
#include "Utility/Utility.h"

#include "Allocator/DefaultAlloc.h"
#include "System/Time/Timer.h"

typedef uint64_t HashID;

static constexpr HashID FNV_PRIME = 1099511628211u;
static constexpr HashID OFFSET_BASIS = 0xcbf29ce484222325;
template<unsigned int N>
constexpr HashID StringHasher(const char(&_s)[N], unsigned int I = N)
{
	return I == 1 ? (OFFSET_BASIS ^ _s[0]) * FNV_PRIME :
		(StringHasher(_s, I - 1) ^ (_s[I - 1])) * FNV_PRIME;
}

_DLL_EXPORT HashID StringHasher(const char* _s);

class _DLL_EXPORT OString
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
	template<typename T, typename = Ut::EnableIf_t<Ut::IsCString<T>::value>>
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
	const char* cbegin(void) const;
	const char*	begin(void);
	const char* cend(void) const;
	const char*	end(void);
	const char* clast(void) const;
	const char*	last(void);
	char * access_c(void);

	/* modifiers */

	template<unsigned N>
	OString& operator+=(const char(&_s)[N]);
	template<typename T>
	OString& operator+=(T);
	OString& operator+=(const OString&);
	OString& erase(size_t _pos, size_t _len = nPos);


	/* useful functions */

	size_t		find(const OString&, size_t _pos = 0) const;
	size_t		find(const char* _s, size_t _pos = 0) const;
	size_t		find(char _c, size_t _pos = 0) const;
	size_t		rfind(const OString&, size_t _pos = nPos) const;
	size_t		rfind(const char* _s, size_t _pos = nPos) const;
	size_t		rfind(char _c, size_t _pos = nPos) const;
	bool		compare(const OString&) const;
	bool		compare(const char* _s) const;
	size_t		find_first_of(const OString&, size_t _pos = 0) const;
	size_t		find_first_of(const char* _s, size_t _pos = 0) const;
	size_t		find_last_of(const OString&, size_t _pos = nPos) const;
	size_t		find_last_of(const char* _s, size_t _pos = nPos) const;
	OString		substr(size_t _pos, size_t _len = nPos) const;

	/* get functions */

	size_t			bufferSize(void) const;
	size_t			length(void) const;
	size_t			size(void) const;
	const char *	c_str(void) const;
	HashID			id(void) const;


	void Rehash(void);

private:
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

_DLL_EXPORT bool operator==(const OString& _lhs, const OString& _rhs);
_DLL_EXPORT bool operator==(const OString& _lhs, const char * _rhs);
_DLL_EXPORT bool operator==(const char * _lhs, const OString& _rhs);
_DLL_EXPORT bool operator==(HashID _id, const OString& _rhs);
_DLL_EXPORT bool operator==(const OString& _lhs, HashID _id);
_DLL_EXPORT bool operator==(OString&& _lhs, OString&& _rhs);
_DLL_EXPORT bool operator==(const OString& _lhs, OString&& _rhs);
_DLL_EXPORT bool operator==(OString&& _lhs, const OString& _rhs);
_DLL_EXPORT bool operator!=(const OString& _lhs, const OString& _rhs);
_DLL_EXPORT bool operator<(const OString& _lhs, const OString& _rhs);
_DLL_EXPORT bool operator>(const OString& _lhs, const OString& _rhs);
std::ostream& operator<<(std::ostream& _os, const OString& _rhs);

_DLL_EXPORT OString operator+(const OString& _lhs, const OString& _rhs);
_DLL_EXPORT OString operator+(const OString& _lhs, const char*);
_DLL_EXPORT OString operator+(const char*, const OString& _rhs);



/******************************************************* template function definition *******************************************************/

template<size_t N>
inline OString::OString(char const (&_literal)[N])
	: mnCurSize{ 0 }, mnBufferSize{ 0 }, mnID{ StringHasher(_literal) }, mpLiteral{ &_literal[0] },
	mpCharBuffer{ nullptr }, mbRehash{ true }
{
	while (_literal[mnCurSize] != '\0')
		mnCurSize++;
}

template<typename T, typename>
inline OString::OString(T _str)
	: mnCurSize{ strlen(_str) }, mnBufferSize{ mnCurSize + 1 }, mnID{ 0 }, mpLiteral{ nullptr },
	mpCharBuffer{ Dystopia::DefaultAllocator<char[]>::Alloc(mnBufferSize) }, mbRehash{ true }
{
	if (mpCharBuffer)
	{
		size_t i = 0;
		for (; i < mnCurSize; ++i)
			mpCharBuffer[i] = _str[i];
		for (; i < mnBufferSize; ++i)
			mpCharBuffer[i] = '\0';
	}
}

//template<>
//inline OString::OString(const char * _cstr)
//	: mnCurSize{ strlen(_cstr) }, mnBufferSize{ mnCurSize + 1 }, mnID{ 0 }, mpLiteral{ nullptr },
//	mpCharBuffer{ Dystopia::DefaultAllocator<char[]>::Alloc(mnBufferSize) }, mbRehash{ true }
//{
//	if (mpCharBuffer)
//	{
//		size_t i = 0;
//		for (; i < mnCurSize; ++i)
//			mpCharBuffer[i] = _cstr[i];
//		for (; i < mnBufferSize; ++i)
//			mpCharBuffer[i] = '\0';
//	}
//}

template <unsigned N>
inline OString& OString::operator=(const char(&_s)[N])
{
	mbRehash = true;
	mpLiteral = &_s[0];
	mnCurSize = 0;
	while (_s[mnCurSize] != '\0')
		mnCurSize++;
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
	size_t newSize = 0;
	while (*(_str + newSize) != '\0')
		newSize++;
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
	mnCurSize = newSize;
	mbRehash = true;
	return *this;
}

template<>
inline OString& OString::operator=(wchar_t * _str)
{
	return operator=(static_cast<const wchar_t*>(_str));
}

template<unsigned N>
inline OString& OString::operator+=(const char(&_s)[N])
{
	size_t otherSize = 0;
	while (_s[otherSize] != '\0')
		otherSize++;
	const size_t newSize = mnCurSize + otherSize;
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
				for (; j < otherSize; j++)
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
			for (; i < otherSize; ++i)
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
inline OString& OString::operator+=<const char*>(const char * _str)
{
	const size_t newSize = mnCurSize + strlen(_str);

	if (mpLiteral)
	{
		if (mpCharBuffer && mnBufferSize > newSize)
		{
			CopyFromLiteral();
			size_t i = mnCurSize;
			for (; i < newSize; i++)
				mpCharBuffer[i] = _str[i - mnCurSize];
			for (; i < mnBufferSize; i++)
				mpCharBuffer[i] = '\0';
		}
		else
		{
			Alloc(newSize);
			CopyFromLiteral();
			for (size_t k = mnCurSize; k < mnBufferSize; k++)
				mpCharBuffer[k] = _str[k - mnCurSize];
			mpCharBuffer[newSize] = '\0';
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
				for (; j < strlen(_str); j++)
					tempBuffer[i + j] = _str[j];

				tempBuffer[newSize] = '\0';
				Dystopia::DefaultAllocator<char[]>::Free(mpCharBuffer);
				mpCharBuffer = tempBuffer;
				mnBufferSize = newSize + 1;
			}
			else
			{
				size_t i = mnCurSize;
				for (; i < newSize; ++i)
					mpCharBuffer[i] = _str[i - mnCurSize];
				mpCharBuffer[newSize] = '\0';
			}
		}
		else
		{
			Alloc(newSize);
			size_t i = 0;
			for (; i < strlen(_str); ++i)
				mpCharBuffer[i] = _str[i];
		}
	}
	mnCurSize = newSize;
	mbRehash = true;
	return *this;
}

template<>
inline OString& OString::operator+=<char>(char _c)
{
	const size_t newSize = mnCurSize + 1;
	if (mpLiteral)
	{
		if (!mpCharBuffer ||  mnBufferSize <= newSize)
		{
			Alloc(newSize);
		}
		CopyFromLiteral();
		mpCharBuffer[mnCurSize] = _c;
		mpCharBuffer[newSize] = '\0';
	}
	else
	{
		if (mpCharBuffer)
		{
			if (mnBufferSize <= newSize)
			{
				char *tempBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(newSize + 1);
				size_t i = 0;
				for (; i < mnCurSize; ++i)
					tempBuffer[i] = mpCharBuffer[i];
				tempBuffer[mnCurSize] = _c;
				tempBuffer[newSize] = '\0';
				Dystopia::DefaultAllocator<char[]>::Free(mpCharBuffer);
				mpCharBuffer = tempBuffer;
				mnBufferSize = newSize + 1;
			}
			else
			{
				mpCharBuffer[mnCurSize] = _c;
				mpCharBuffer[newSize] = '\0';
			}
		}
		else
		{
			Alloc(1);
			mpCharBuffer[0] = _c;
		}
	}
	mnCurSize++;
	mbRehash = true;
	return *this;
}

template<>
inline OString& OString::operator+=(unsigned _n)
{
	char buffer[U_BUFSIZE];
	if (!_itoa_s(_n, buffer, U_BUFSIZE, 10))
		return operator+=(buffer);
	return *this;
}

template<>
inline OString& OString::operator+=(unsigned long long _ull)
{
	char buffer[ULL_BUFSIZE];
	if (!_i64toa_s(_ull, buffer, ULL_BUFSIZE, 10))
		return operator+=(buffer);
	return *this;
}

template <unsigned N>
inline OString operator+(const char(&_s)[N], const OString& _rhs)
{
	OString s{ _s };
	return s + _rhs;
}

template <unsigned N>
inline OString operator+(const OString& _lhs, const char(&_s)[N])
{
	OString s{ _s };
	return _lhs + s;
}

namespace std
{
	template <typename>
	struct hash;

	template <>
	struct hash<OString>
	{
		inline size_t operator() (const OString& str) const
		{
			return str.id();
		}
	};
}

#endif 
















