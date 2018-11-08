/* HEADER *********************************************************************************/
/*!
\file	HashString.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "DataStructure/HashString.h"
#include "Utility/Utility.h"
#include "Utility/DebugAssert.h"

#include <stdlib.h>

HashID StringHasher(const char* _s)
{
	HashID hash = OFFSET_BASIS;
	size_t len = strlen(_s) + 1;
	for (size_t i = 0; i < len; ++i)
	{
		hash ^= *_s++;
		hash *= FNV_PRIME;
	}
	return hash;
}

/* constructors */
HashString::HashString(void)
	: mHashedID{ 0 }, mCharBuffer{ nullptr }, mSize{ 0 }
{}

HashString::HashString(const HashString& _rhs)
	: mHashedID{ _rhs.mHashedID }, mCharBuffer{ _rhs.mSize ? Dystopia::DefaultAllocator<char[]>::Alloc(_rhs.mSize + 1) : nullptr },
	mSize{ _rhs.mSize }
{
	if (mSize)
	{
		strcpy_s(mCharBuffer, _rhs.mSize + 1, _rhs.mCharBuffer);
	}
}

HashString::HashString(HashString&& _rhs) noexcept
	: mHashedID{ _rhs.mHashedID }, mCharBuffer{ _rhs.mCharBuffer }, mSize{ _rhs.mSize }
{
	_rhs.mHashedID = 0;
	_rhs.mCharBuffer = nullptr;
	_rhs.mSize = 0;
}

HashString::HashString(const char * _s)
	: mHashedID{ StringHasher(_s) }, mCharBuffer{ Dystopia::DefaultAllocator<char[]>::Alloc(strlen(_s) + 1) },
	mSize{ strlen(_s) }
{
	strcpy_s(mCharBuffer, mSize + 1, _s);
}

HashString::HashString(const char * _start, const char* _end)
	: mHashedID{ 0 }, mCharBuffer{ Dystopia::DefaultAllocator<char[]>::Alloc(_start - _end + 1) },
	mSize{ static_cast<size_t>(_end - _start) }
{
	size_t i = 0;
	while (i < mSize)
		mCharBuffer[i++] = *(_start + i);
	mCharBuffer[i] = '\0';
	mHashedID = StringHasher(mCharBuffer);
}

HashString::~HashString(void)
{
	Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);
}

/* operators */
HashString& HashString::operator=(const HashString& _rhs)
{
	Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);

	mHashedID = _rhs.mHashedID;
	mCharBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(_rhs.mSize + 1);
	strcpy_s(mCharBuffer, _rhs.mSize + 1, _rhs.mCharBuffer);
	mSize = _rhs.mSize;
	return *this;
}

HashString& HashString::operator=(HashString&& _rhs) noexcept
{
	Ut::Swap(mCharBuffer, _rhs.mCharBuffer);
	Ut::Swap(mHashedID, _rhs.mHashedID);
	Ut::Swap(mSize, _rhs.mSize);
	return *this;
}

HashString& HashString::operator=(const char * _s)
{
	Dystopia::DefaultAllocator<char[]>::Free(mCharBuffer);
	HashString temp{ _s };

	mCharBuffer = temp.mCharBuffer;
	mSize = temp.mSize;
	mHashedID = temp.mHashedID;
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
	mHashedID = StringHasher(mCharBuffer);
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
	if (mCharBuffer)
		mCharBuffer[0] = '\0';
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
	mHashedID = StringHasher(mCharBuffer);
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
	mHashedID = StringHasher(mCharBuffer);
	return *this;
}

HashString& HashString::operator+=(unsigned _i)
{
	char buffer[11];
	if (!_itoa_s(_i, buffer, 11, 10))
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
	size_t pad = _len;
	char *start = begin() + _pos;
	char *it = start;
	while (it < end() && _len)
	{
		*it++ = '\0';
		_len--;
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
	mHashedID = StringHasher(mCharBuffer);
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
	mHashedID = StringHasher(mCharBuffer);
	return *this;
}

HashString& HashString::replace(size_t _pos, size_t _len, const char * _s)
{
	HashString temp{ _s };
	return replace(_pos, _len, temp);
}

/* operations */
const char* HashString::c_str(void) const
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
	return (mHashedID == _rhs.mHashedID);
}

bool HashString::compare(const char* _s) const
{
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

const char* HashString::cbegin(void) const
{
	return mCharBuffer;
}

char* HashString::begin(void)
{
	return mCharBuffer;
}

const char* HashString::cend(void) const
{
	return mCharBuffer + mSize;
}

char* HashString::end(void)
{
	return mCharBuffer + mSize;
}

const char* HashString::clast(void) const
{
	return mCharBuffer + mSize -1;
}

char* HashString::last(void)
{
	return mCharBuffer + mSize -1;
}

HashID HashString::id(void) const
{
	return mHashedID;
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




