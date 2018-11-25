/* HEADER *********************************************************************************/
/*!
\file   HashString.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _HASH_STRING_H_
#define _HASH_STRING_H_

#pragma warning(push)
#pragma warning(disable : 4251)

#include <cstdint>
#include <iostream>
#include "Globals.h"
#include "Allocator/DefaultAlloc.h"

typedef uint64_t HashID;

static constexpr HashID FNV_PRIME = 1099511628211u;
static constexpr HashID OFFSET_BASIS = 0xcbf29ce484222325;
template<unsigned int N>
constexpr HashID StringHasher(const char(&_s)[N], unsigned int I = N)
{
	return I == 1 ? (OFFSET_BASIS ^ _s[0]) * FNV_PRIME :
		(StringHasher(_s, I - 1) ^ (_s[I - 1])) * FNV_PRIME;
}

HashID StringHasher(const char* _s);

class _DLL_EXPORT_ONLY HashString
{
public:
	static constexpr size_t nPos = static_cast<size_t>(-1);
	/* constructors */
	HashString(void);
	HashString(const HashString&);
	HashString(HashString&&) noexcept;
	HashString(const char *);
	HashString(const char * const _start, const char* const _end);

	template <unsigned N>
	constexpr HashString(const char(&s)[N]);

	~HashString(void);

	/* operators */
	template <unsigned N>
	HashString& operator=(const char(&_s)[N]);
	HashString& operator=(const HashString&);
	HashString& operator=(HashString&&) noexcept;
	HashString& operator=(const char *);
	HashString& operator=(const wchar_t *);

	/* capacity */
	size_t      size(void) const;
	size_t      length(void) const;
	void        clear(void);

	/* element access */
	char&       operator[](size_t _pos);
	const char& operator[](size_t _pos) const;
	char&       back(void);
	const char& back(void) const;
	char&       front(void);
	const char& front(void) const;

	/* modifiers */
	template<unsigned N>
	HashString& operator+=(const char(&_s)[N]);
	HashString& operator+=(const char *);
	HashString& operator+=(const char);
	HashString& operator+=(unsigned);
	HashString& operator+=(const HashString&);
	HashString& operator+=(unsigned long long);
	HashString& erase(size_t _pos, size_t _len = -1);
	HashString& replace(size_t _pos, size_t _len, const HashString&);
	HashString& replace(size_t _pos, size_t _len, const char * _s);

	/* operations */
	const char* const c_str(void) const;
	size_t		find(const HashString&, size_t _pos = 0) const;
	size_t		find(const char* _s, size_t _pos = 0) const;
	size_t		find(char _c, size_t _pos = 0) const;
	size_t		rfind(const HashString&, size_t _pos = -1) const;
	size_t		rfind(const char* _s, size_t _pos = -1) const;
	size_t		rfind(char _c, size_t _pos = -1) const;
	bool		compare(const HashString&) const;
	bool		compare(const char* _s) const;
	size_t		find_first_of(const HashString&, size_t _pos = 0) const;
	size_t		find_first_of(const char* _s, size_t _pos = 0) const;
	size_t		find_last_of(const HashString&, size_t _pos = -1) const;
	size_t		find_last_of(const char* _s, size_t _pos = -1) const;
	HashString	substr(size_t _pos, size_t _len = -1) const;

	const char* const cbegin(void) const;
	char*		const begin(void);
	const char* const cend(void) const;
	char*		const end(void);
	const char* const clast(void) const;
	char*		const last(void);

	HashID      id(void) const;

private:
	size_t mSize;
	char* mCharBuffer;
	HashID mHashedID;
};

template <unsigned N>
constexpr HashString::HashString(const char(&_s)[N])
	: mSize{ N - 1 }
	, mCharBuffer{ Dystopia::DefaultAllocator<char[]>::Alloc(N) }
	, mHashedID{ StringHasher(_s) }
{
}

bool operator==(const HashString& _lhs, const HashString& _rhs);
bool operator==(const HashString& _lhs, const char * _rhs);
bool operator==(const char * _lhs, const HashString& _rhs);
bool operator==(HashID _id, const HashString& _rhs);
bool operator==(const HashString& _lhs, HashID _id);
bool operator!=(const HashString& _lhs, const HashString& _rhs);
bool operator<(const HashString& _lhs, const HashString& _rhs);
bool operator>(const HashString& _lhs, const HashString& _rhs);
//std::ostream& operator<<(std::ostream& _os, const HashString& _rhs);

HashString operator+(const HashString& _lhs, const HashString& _rhs);
HashString operator+(const HashString& _lhs, const char*);
HashString operator+(const char*, const HashString& _rhs);

template <unsigned N>
HashString operator+(const char(&_s)[N], const HashString& _rhs)
{
	HashString s{ _s };
	return s + _rhs;
}

template <unsigned N>
HashString operator+(const HashString& _lhs, const char(&_s)[N])
{
	HashString s{ _s };
	return _lhs + s;
}

template <unsigned N>
HashString& HashString::operator=(const char(&_s)[N])
{
	Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);

	mHashedID = StringHasher(_s);
	mCharBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(N);
	strcpy_s(mCharBuffer, N, &_s);
	mSize = N - 1;
	return *this;
}

template<unsigned N>
HashString& HashString::operator+=(const char(&_s)[N])
{
	size_t i = 0;
	size_t j = 0;
	char* buffer = Dystopia::DefaultAllocator<char[]>::Alloc(N + mSize);
	while (i < mSize)
	{
		buffer[i] = mCharBuffer[i];
		++i;
	}
	while (j < N)
	{
		buffer[i + j] = _s[j];
		++j;
	}
	mSize += N - 1;
	Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);
	mCharBuffer = buffer;
	mHashedID = StringHasher(mCharBuffer);
	return *this;
}

#endif //_HASH_STRING_H_





