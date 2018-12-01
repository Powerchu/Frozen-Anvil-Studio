/* HEADER *********************************************************************************/
/*!
\file   HashString.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "DataStructure/HashString.h"
#include "Utility/Utility.h"
#include "Utility/DebugAssert.h"

#include <stdlib.h>

#define U_BUFFER_SIZE  11
#define ULL_BUFFER_SIZE 16

#if !EDITOR 

HashID StringHasher(const char* _s)
{
	if (!_s)
		return 0;

	HashID hash = OFFSET_BASIS;
	size_t len = strlen(_s) + 1;
	for (size_t i = 0; i < len; ++i)
	{
		hash ^= *_s++;
		hash *= FNV_PRIME;
	}
	return hash;
}

/*************************************************************************************************************/


/* constructors */
HashString::HashString(void)
	: mHashedID{ 0 }, mCharBuffer{ nullptr }, mSize{ 0 }, mbRehash{false}, mpLiteral{nullptr}
{}

HashString::HashString(const HashString& _rhs)
	: mHashedID{ _rhs.mHashedID }, mCharBuffer{ _rhs.mpLiteral ? nullptr : _rhs.mSize ? Dystopia::DefaultAllocator<char[]>::Alloc(_rhs.mSize + 1) : nullptr },
	mSize{ _rhs.mSize }, mbRehash{ _rhs.mbRehash }, mpLiteral{ _rhs.mpLiteral }
{
	if (!mpLiteral && mCharBuffer)
	{
		strcpy_s(mCharBuffer, _rhs.mSize + 1, _rhs.mCharBuffer);
		mbRehash = true;
	}
	//if (mSize)
	//{
	//	//strcpy_s(mCharBuffer, _rhs.mSize + 1, _rhs.mCharBuffer);
	//	mbRehash = true;
	//}
}

HashString::HashString(HashString&& _rhs) noexcept
	: mHashedID{ _rhs.mHashedID }, mCharBuffer{ _rhs.mCharBuffer },
	mSize{ _rhs.mSize }, mbRehash{ _rhs.mbRehash }, mpLiteral{ _rhs.mpLiteral }
{
	_rhs.mHashedID = 0;
	_rhs.mCharBuffer = nullptr;
	_rhs.mSize = 0;
	_rhs.mbRehash = true;
	_rhs.mpLiteral = nullptr;
}

HashString::HashString(const char * const _s)
	: mHashedID{ 0 }, mCharBuffer{ _s ? Dystopia::DefaultAllocator<char[]>::Alloc(strlen(_s) + 1) : nullptr },
	mSize{ _s ? strlen(_s) : 0 }, mbRehash{ false }, mpLiteral{ _s }
{
	if (_s)
	{
		mpLiteral = _s;
		mbRehash = true;
		//strcpy_s(mCharBuffer, mSize + 1, _s);
	}
}

HashString::HashString(const char * const _start, const char* const _end)
	: mHashedID{ 0 }, mCharBuffer{ Dystopia::DefaultAllocator<char[]>::Alloc(_end - _start + 1) },
	mSize{ static_cast<size_t>(_end - _start) }, mbRehash{ true }
{
	size_t i = 0;
	while (i < mSize)
		mCharBuffer[i++] = *(_start + i);
	mCharBuffer[i] = '\0';
	//mHashedID = StringHasher(mCharBuffer);
}

HashString::~HashString(void)
{
	Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);
}

/* operators */
HashString& HashString::operator=(const HashString& _rhs)
{
	if (mpLiteral)
	{
		mCharBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(mSize + _rhs.mSize + 1);

		mpLiteral = nullptr;
	}
	else
	{
		Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);

		mHashedID = _rhs.mHashedID;
		mCharBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(_rhs.mSize + 1);
		strcpy_s(mCharBuffer, _rhs.mSize + 1, _rhs.mCharBuffer);
		mSize = _rhs.mSize;
		mbRehash = _rhs.mbRehash;
	}
	return *this;
}

HashString& HashString::operator=(HashString&& _rhs) noexcept
{
	Ut::Swap(mCharBuffer, _rhs.mCharBuffer);
	Ut::Swap(mHashedID, _rhs.mHashedID);
	Ut::Swap(mSize, _rhs.mSize);
	Ut::Swap(mbRehash, _rhs.mbRehash);
	return *this;
}

HashString& HashString::operator=(const char * _s)
{
	Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);
	HashString temp{ _s };

	mCharBuffer = temp.mCharBuffer;
	mSize = temp.mSize;
	mHashedID = temp.mHashedID;
	mbRehash = temp.mbRehash;
	temp.mCharBuffer = nullptr;
	return *this;
}

HashString& HashString::operator=(const wchar_t *_s)
{
	size_t len = 0;
	const wchar_t * it = _s;
	while (*_s != '\0')
	{
		len++;
		_s++;
	}
	char* buffer = mCharBuffer;
	if (len > mSize)
	{
		buffer = Dystopia::DefaultAllocator<char[]>::Alloc(len + 1);
		Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);
		mCharBuffer = buffer;
	}
	mSize = 0;
	while (*it != '\0')
	{
		*buffer++ = static_cast<char>(*it++);
		mSize++;
	}
	*buffer = '\0';
	mbRehash = true;
	//mHashedID = StringHasher(mCharBuffer);
	return *this;
}

/* capacity */
size_t HashString::size(void) const
{
	return mSize;
}

size_t HashString::length(void) const
{
	return mSize;
}

void HashString::clear(void)
{
	Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);
	mCharBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(1);
	*mCharBuffer = '\0';
	mSize = 0;
	mbRehash = true;
	//mHashedID = StringHasher(mCharBuffer);
}

/* element access */
char& HashString::operator[](size_t _pos)
{
	DEBUG_ASSERT(_pos >= mSize, "Hash String operator[] out of range");
	return mCharBuffer[_pos];
}

const char& HashString::operator[](size_t _pos) const
{
	DEBUG_ASSERT(_pos >= mSize, "Hash String operator[] out of range");
	return mCharBuffer[_pos];
}

char& HashString::back(void)
{
	return mCharBuffer[mSize];
}

const char& HashString::back(void) const
{
	return mCharBuffer[mSize];
}

char& HashString::front(void)
{
	return mCharBuffer[0];
}

const char& HashString::front(void) const
{
	return mCharBuffer[0];
}

/* modifiers */
HashString& HashString::operator+=(const char * _s)
{
	size_t i = 0;
	size_t j = 0;
	size_t otherSize = strlen(_s);
	char* buffer = Dystopia::DefaultAllocator<char[]>::Alloc(otherSize + mSize + 1);
	while (i < mSize)
	{
		buffer[i] = mCharBuffer[i];
		++i;
	}
	while (j < otherSize)
	{
		buffer[i + j] = *(_s + j);
		++j;
	}
	buffer[otherSize + mSize] = '\0';
	mSize += otherSize;
	Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);
	mCharBuffer = buffer;
	mbRehash = true;
	//mHashedID = StringHasher(mCharBuffer);
	return *this;
}

HashString& HashString::operator+=(const HashString& _rhs)
{
	return operator+=(_rhs.mCharBuffer);
}

HashString& HashString::operator+=(const char _c)
{
	size_t i = 0;
	char* buffer = Dystopia::DefaultAllocator<char[]>::Alloc(mSize + 2);
	while (i < mSize)
	{
		buffer[i] = mCharBuffer[i];
		++i;
	}
	buffer[mSize] = _c;
	buffer[1 + mSize] = '\0';
	mSize += 1;
	Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);
	mCharBuffer = buffer;
	mbRehash = true;
	//mHashedID = StringHasher(mCharBuffer);
	return *this;
}

HashString& HashString::operator+=(unsigned _i)
{
	char buffer[U_BUFFER_SIZE];
	if (!_itoa_s(_i, buffer, U_BUFFER_SIZE, 10))
	{
		return operator+=(buffer);
	}
	return *this;
}

HashString& HashString::operator+=(unsigned long long _u)
{
	char buffer[ULL_BUFFER_SIZE];
	if (!_i64toa_s(_u, buffer, ULL_BUFFER_SIZE, 10))
	{
		return operator+=(buffer);
	}
	return *this;
}

HashString& HashString::erase(size_t _pos, size_t _len)
{
	DEBUG_ASSERT(_pos >= mSize, "Hash String Earse _pos out of range");
	if (!_len)
		return *this;
	size_t pad = 0;
	char *start = begin() + _pos;
	char *it = start;
	while (it < end() && _len)
	{
		*it++ = '\0';
		_len--;
		pad++;
	}
	if (!_len)
	{
		while (it < end())
		{
			*start = *(it);
			*(it) = '\0';
			it++;
			start++;
		}
	}
	mSize -= pad;
	mbRehash = true;
	//mHashedID = StringHasher(mCharBuffer);
	return *this;
}

HashString& HashString::replace(size_t _pos, size_t _len, const HashString& _rhs)
{
	DEBUG_ASSERT(_pos >= mSize, "Hash String replace _pos out of range");
	size_t size = mSize + _rhs.mSize + 1;
	bool realloc = static_cast<size_t>(end() - begin()) < size;
	char *buffer = mCharBuffer;

	if (realloc)
		buffer = Dystopia::DefaultAllocator<char[]>::Alloc(size);

	char *track = buffer;
	char *cont = begin() + _pos + _len;
	size_t i = 0;
	while (i < _pos)
		*buffer++ = *(mCharBuffer + i++);
	i = 0;
	while (i < _len && i < _rhs.mSize)
		*buffer++ = *(_rhs.mCharBuffer + i++);
	if (_len)
	{
		i = 0;
		while (cont + i < end())
			*buffer++ = *(cont + i++);
		while (buffer < track + size)
			*buffer++ = '\0';
	}

	if (realloc)
	{
		Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);
		mCharBuffer = track;
	}
	mSize = 0;
	char *it = mCharBuffer;
	while (*it++ != '\0')
		mSize++;

	mbRehash = true;
	//mHashedID = StringHasher(mCharBuffer);
	return *this;
}

HashString& HashString::replace(size_t _pos, size_t _len, const char * _s)
{
	HashString temp{ _s };
	return replace(_pos, _len, temp);
}

/* operations */

const char* const HashString::c_str(void) const
{
	return mCharBuffer;
}

size_t HashString::find(const HashString& _rhs, size_t _pos) const
{
	return find(_rhs.c_str(), _pos);
}

size_t HashString::find(const char* _s, size_t _pos) const
{
	DEBUG_ASSERT(_pos >= mSize, "Hash String find _pos out of range");
	const char *searchPos = (cbegin() + _pos < cend()) ? cbegin() + _pos : cbegin();
	size_t len = strlen(_s);
	while (searchPos < cend())
	{
		const char *it1 = searchPos;
		const char *it2 = _s;
		size_t passes = 0;
		while (it1 < cend())
		{
			if (*it1++ == *it2++)
				passes++;
			else
				break;
		}
		if (passes == len)
			return searchPos - cbegin();
		searchPos++;
	}
	return HashString::nPos;
}

size_t HashString::find(char _c, size_t _pos) const
{
	DEBUG_ASSERT(_pos >= mSize, "Hash String find _pos out of range");
	const char *searchPos = (cbegin() + _pos < cend()) ? cbegin() + _pos : cbegin();
	while (searchPos < cend())
	{
		if (*searchPos == _c)
			return searchPos - cbegin();
		searchPos++;
	}
	return HashString::nPos;
}

size_t HashString::rfind(const HashString& _rhs, size_t _pos) const
{
	return rfind(_rhs.c_str(), _pos);
}

size_t HashString::rfind(const char* _s, size_t _pos) const
{
	const char *searchPos = (cbegin() + _pos < cend() && _pos != HashString::nPos) ? cbegin() + _pos : cend();
	size_t len = strlen(_s);
	while (searchPos > cbegin())
	{
		const char *it1 = searchPos;
		const char *it2 = _s + len - 1;
		size_t passes = 0;
		while (it1 >= cbegin() && it2 >= _s)
		{
			if (*it1-- == *it2--)
				passes++;
			else
				break;
		}
		if (passes == len)
			return searchPos - cbegin() - (len - 1);
		searchPos--;
	}
	return HashString::nPos;
}

size_t HashString::rfind(char _c, size_t _pos) const
{
	const char *searchPos = (cbegin() + _pos < cend() && _pos != HashString::nPos) ? cbegin() + _pos : cend();
	while (searchPos > cbegin())
	{
		if (*searchPos == _c)
			return searchPos - cbegin();
		searchPos--;
	}
	return HashString::nPos;
}

bool HashString::compare(const HashString& _rhs) const
{
	if (_rhs.mbRehash)
		const_cast<HashString*>(&_rhs)->Rehash();
	if (mbRehash)
		const_cast<HashString*>(this)->Rehash();
	return (mHashedID == _rhs.mHashedID);
}

bool HashString::compare(const char* _s) const
{
	if (mbRehash)
		const_cast<HashString*>(this)->Rehash();
	return (mHashedID == StringHasher(_s));
}

size_t HashString::find_first_of(const HashString& _rhs, size_t _pos) const
{
	return find_first_of(_rhs.c_str(), _pos);
}

size_t HashString::find_first_of(const char* _s, size_t _pos) const
{
	DEBUG_ASSERT(_pos >= mSize, "Hash String find_first_of _pos out of range");
	const char* it1 = cbegin() + _pos;
	const char* it2 = _s;
	while (it1 < cend())
	{
		it2 = _s;
		while (*it2 != '\0')
		{
			if (*it1 == *it2)
				return it1 - cbegin();
			it2++;
		}
		it1++;
	}
	return HashString::nPos;
}

size_t HashString::find_last_of(const HashString& _rhs, size_t _pos) const
{
	return find_last_of(_rhs.c_str(), _pos);
}

size_t HashString::find_last_of(const char* _s, size_t _pos) const
{
	const char *it1 = (cbegin() + _pos < cend() && _pos != HashString::nPos) ? cbegin() + _pos : cend();
	const char* it2 = _s;
	while (it1 >= cbegin())
	{
		it2 = _s;
		while (*it2 != '\0')
		{
			if (*it1 == *it2)
				return it1 - cbegin();
			it2++;
		}
		it1--;
	}
	return HashString::nPos;
}

HashString HashString::substr(size_t _pos, size_t _len) const
{
	DEBUG_ASSERT(_pos >= mSize, "Hash String substr _pos out of range");
	const char * ending = cbegin() + _len;
	if (_len > mSize || _pos + _len > mSize)
		ending = cend();
	return HashString{ cbegin() + _pos,  ending };
}

const char* const HashString::cbegin(void) const
{
	return mCharBuffer;
}

char* const HashString::begin(void)
{
	return mCharBuffer;
}

const char* const HashString::cend(void) const
{
	return mCharBuffer + mSize;
}

char* const HashString::end(void)
{
	return mCharBuffer + mSize;
}

const char* const HashString::clast(void) const
{
	return mCharBuffer + mSize - 1;
}

char* const HashString::last(void)
{
	return mCharBuffer + mSize - 1;
}

HashID HashString::id(void) const
{
	if (mbRehash)
		const_cast<HashString*>(this)->Rehash();
	return mHashedID;
}

bool HashString::NeedRehash(void) const
{
	return mbRehash;
}

void HashString::Rehash(void) const
{
	const_cast<HashString*>(this)->mbRehash = false;
	const_cast<HashString*>(this)->mHashedID = StringHasher(mCharBuffer);
}

bool operator==(const HashString& _lhs, const HashString& _rhs)
{
	return (_lhs.id() == _rhs.id());
}

bool operator==(const HashString& _lhs, const char * _rhs)
{
	return _lhs.id() == StringHasher(_rhs);
}

bool operator==(const char * _lhs, const HashString& _rhs)
{
	return _rhs == _lhs;
}

bool operator==(HashID _id, const HashString& _rhs)
{
	return _rhs.id() == _id;
}

bool operator==(const HashString& _lhs, HashID _id)
{
	return _lhs.id() == _id;
}

bool operator==(HashString&& _lhs, HashString&& _rhs)
{
	return _lhs.id() == _rhs.id();
}

bool operator==(const HashString& _lhs, HashString&& _rhs)
{
	return _lhs.id() == _rhs.id();
}

bool operator==(HashString&& _lhs, const HashString& _rhs)
{
	return _lhs.id() == _rhs.id();
}

bool operator!=(const HashString& _lhs, const HashString& _rhs)
{
	return !(_lhs.id() == _rhs.id());
}

HashString operator+(const HashString& _lhs, const HashString& _rhs)
{
	HashString temp{ _lhs };
	temp += _rhs;
	return temp;
}

HashString operator+(const HashString& _lhs, const char* _s)
{
	HashString s{ _s };
	return _lhs + s;
}

HashString operator+(const char* _s, const HashString& _rhs)
{
	HashString s{ _s };
	return s + _rhs;
}

std::ostream& operator<<(std::ostream& _os, const HashString& _rhs)
{
	_os << _rhs.c_str();
	return _os;
}

bool operator<(const HashString& _lhs, const HashString& _rhs)
{
	return std::strcmp(_lhs.c_str(), _rhs.c_str()) < 0;
}

bool operator>(const HashString& _lhs, const HashString& _rhs)
{
	return std::strcmp(_lhs.c_str(), _rhs.c_str()) > 0;
}
#endif 







