#include "OString.h"

#include "Utility/DebugAssert.h"
#include "Utility/Utility.h"

#include <stdlib.h>

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
/****** Constructors ******/
OString::OString(void)
	: mnCurSize{ 0 }, mnBufferSize{ 0 }, mnID{ 0 }, mpLiteral{ nullptr }, mpCharBuffer{ nullptr }, mbRehash{ false }
{}

OString::OString(OString&& _moveCtor) noexcept
	: mnCurSize{ _moveCtor.mnCurSize }, mnBufferSize{ _moveCtor.mnBufferSize }, mnID{ _moveCtor.mnID }, mpLiteral{ _moveCtor.mpLiteral },
	mpCharBuffer{ _moveCtor.mpCharBuffer }, mbRehash{ _moveCtor.mbRehash }
{
	_moveCtor.mnCurSize = 0;
	_moveCtor.mnBufferSize = 0;
	_moveCtor.mnID = 0;
	_moveCtor.mpLiteral = nullptr;
	_moveCtor.mpCharBuffer = nullptr;
	_moveCtor.mbRehash = false;
}

OString::OString(const OString& _copyCtor)
	: mnCurSize{ _copyCtor.mnCurSize }, mnBufferSize{ _copyCtor.mnBufferSize }, mnID{ _copyCtor.mnID }, mpLiteral{ _copyCtor.mpLiteral },
	mpCharBuffer{ _copyCtor.mpCharBuffer }, mbRehash{ _copyCtor.mbRehash }
{}

OString::OString(const char* _cstrStart, const char* _cstrEnd)
	: mnCurSize{ static_cast<size_t>(_cstrEnd - _cstrStart) }, mnBufferSize{ mnCurSize + 1 }, mnID{ 0 }, mpLiteral{ nullptr },
	mpCharBuffer{ Dystopia::DefaultAllocator<char[]>::Alloc(mnBufferSize) }, mbRehash{ true }
{
	auto start = mpCharBuffer;
	const auto end = mpCharBuffer + mnCurSize;
	while (start != end)
		*start++ = *_cstrStart++;
	start[mnCurSize] = '\0';
}

OString::~OString(void)
{
	if (mpCharBuffer)
		Dystopia::DefaultAllocator<char[]>::Free(mpCharBuffer);
}

/****** Assignment operators ******/

OString& OString::operator=(const OString& _rhs)
{
	if (_rhs.mpLiteral)
		mpLiteral = _rhs.mpLiteral;
	else
	{
		Alloc(_rhs.mnCurSize);
		mpLiteral = nullptr;
		MyStrCopy(_rhs.c_str(), _rhs.mnCurSize);
	}
	return *this;
}

OString& OString::operator=(OString&& _rhs) noexcept
{
	Ut::Swap(mnCurSize, _rhs.mnCurSize);
	Ut::Swap(mnBufferSize, _rhs.mnBufferSize);
	Ut::Swap(mnID, _rhs.mnID);
	Ut::Swap(mpLiteral, _rhs.mpLiteral);
	Ut::Swap(mpCharBuffer, _rhs.mpCharBuffer);
	Ut::Swap(mbRehash, _rhs.mbRehash);
	return *this;
}

void OString::clear(void)
{
	if (mpLiteral)
		mpLiteral = nullptr;
	else
	{
		if (mpCharBuffer)
			for (size_t i = 0; i < mnCurSize; i++)
				mpCharBuffer[i] = '\0';
	}
	mbRehash = true;
	mnCurSize = 0;
}


/* element access */

char& OString::operator[](size_t _pos)
{
	DEBUG_ASSERT(_pos >= mnCurSize, "Index out of range!");
	if (mpLiteral)
		CopyFromLiteral();
	return mpCharBuffer[_pos];
}

char OString::operator[](size_t _pos) const
{
	DEBUG_ASSERT(_pos >= mnCurSize, "Index out of range!");
	if (mpLiteral)
		return mpLiteral[_pos];
	return mpCharBuffer[_pos];
}

char& OString::back(void)
{
	DEBUG_ASSERT(0 == mnCurSize, "Empty string!");
	if (mpLiteral)
		CopyFromLiteral();
	return mpCharBuffer[mnCurSize - 1];
}

char OString::back(void) const
{
	DEBUG_ASSERT(0 == mnCurSize, "Empty string!");
	return mpLiteral ? mpLiteral[mnCurSize - 1] : mpCharBuffer[mnCurSize - 1];
}

char& OString::front(void)
{
	DEBUG_ASSERT(0 == mnCurSize, "Empty string!");
	if (mpLiteral)
		CopyFromLiteral();
	return mpCharBuffer[0];
}

char OString::front(void) const
{
	DEBUG_ASSERT(0 == mnCurSize, "Empty string!");
	return mpLiteral ? mpLiteral[0] : mpCharBuffer[0];
}

const char* OString::cbegin(void) const
{
	DEBUG_ASSERT(0 == mnCurSize, "Empty string!");
	if (mpLiteral)
		return mpLiteral;
	return mpCharBuffer;
}

const char*	OString::begin(void)
{
	DEBUG_ASSERT(0 == mnCurSize, "Empty string!");
	if (mpLiteral)
		return mpLiteral;
	return mpCharBuffer;
}

const char* OString::cend(void) const
{
	DEBUG_ASSERT(0 == mnCurSize, "Empty string!");
	if (mpLiteral)
		return mpLiteral + mnCurSize;
	return mpCharBuffer + mnCurSize;
}

const char*	OString::end(void)
{
	DEBUG_ASSERT(0 == mnCurSize, "Empty string!");
	if (mpLiteral)
		return mpLiteral + mnCurSize;
	return mpCharBuffer + mnCurSize;
}

const char* OString::clast(void) const
{
	DEBUG_ASSERT(0 == mnCurSize, "Empty string!");
	if (mpLiteral)
		return mpLiteral + mnCurSize - 1;
	return mpCharBuffer + mnCurSize - 1;
}

const char*	OString::last(void)
{
	DEBUG_ASSERT(0 == mnCurSize, "Empty string!");
	if (mpLiteral)
		return mpLiteral + mnCurSize - 1;
	return mpCharBuffer + mnCurSize - 1;
}

char * OString::access_c(void)
{
	if (mpLiteral)
		CopyFromLiteral();
	return &mpCharBuffer[0];
}

/* modifiers */

OString& OString::operator+=(const OString& _rhs)
{
	return operator+=(_rhs.mpLiteral ? _rhs.mpLiteral : _rhs.mpCharBuffer);
}

OString& OString::erase(size_t _pos, size_t _len)
{
	DEBUG_ASSERT(_pos >= mnCurSize, "Index out of range");
	DEBUG_ASSERT(!_len, "Useless function call");

	if (mpLiteral)
		CopyFromLiteral();

	size_t pad = 0;
	char *start = mpCharBuffer + _pos;
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
	mnCurSize -= pad;
	mbRehash = true;
	return *this;
}

/* useful functions */
size_t OString::find(const OString& _rhs, size_t _pos) const
{
	return find(_rhs.c_str(), _pos);
}

size_t OString::find(const char* _s, size_t _pos) const
{
	DEBUG_ASSERT(_pos >= mnCurSize, "Index out of range");

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
	return OString::nPos;
}

size_t OString::find(char _c, size_t _pos) const
{
	DEBUG_ASSERT(_pos >= mnCurSize, "Index out of range");
	const char *searchPos = (cbegin() + _pos < cend()) ? cbegin() + _pos : cbegin();
	while (searchPos < cend())
	{
		if (*searchPos == _c)
			return searchPos - cbegin();
		searchPos++;
	}
	return OString::nPos;
}

size_t OString::rfind(const OString& _rhs, size_t _pos) const
{
	return rfind(_rhs.c_str(), _pos);
}

size_t OString::rfind(const char* _s, size_t _pos) const
{
	const char *searchPos = (cbegin() + _pos < cend() && _pos != OString::nPos) ? cbegin() + _pos : cend();
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
	return OString::nPos;
}

size_t OString::rfind(char _c, size_t _pos) const
{
	const char *searchPos = (cbegin() + _pos < cend() && _pos != OString::nPos) ? cbegin() + _pos : cend();
	while (searchPos > cbegin())
	{
		if (*searchPos == _c)
			return searchPos - cbegin();
		searchPos--;
	}
	return OString::nPos;
}

bool OString::compare(const OString& _rhs) const
{
	if (_rhs.mbRehash)
		const_cast<OString*>(&_rhs)->Rehash();
	if (mbRehash)
		const_cast<OString*>(this)->Rehash();
	return (mnID == _rhs.mnID);
}

bool OString::compare(const char* _s) const
{
	if (mbRehash)
		const_cast<OString*>(this)->Rehash();
	return (mnID == StringHasher(_s));
}

size_t OString::find_first_of(const OString& _rhs, size_t _pos) const
{
	return find_first_of(_rhs.c_str(), _pos);
}

size_t OString::find_first_of(const char* _s, size_t _pos) const
{
	DEBUG_ASSERT(_pos >= mnCurSize, "Index out of range");
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
	return OString::nPos;
}

size_t OString::find_last_of(const OString& _rhs, size_t _pos) const
{
	return find_last_of(_rhs.c_str(), _pos);
}

size_t OString::find_last_of(const char* _s, size_t _pos) const
{
	const char *it1 = (cbegin() + _pos < cend() && _pos != OString::nPos) ? cbegin() + _pos : cend();
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
	return OString::nPos;
}

OString OString::substr(size_t _pos, size_t _len) const
{
	DEBUG_ASSERT(_pos >= mnCurSize, "Index out of range");
	const char * ending = cbegin() + _len;
	if (_len > mnCurSize || _pos + _len > mnCurSize)
		ending = cend();
	return OString{ cbegin() + _pos,  ending };
}

/****** get functions ******/

size_t OString::bufferSize(void) const
{
	return mnBufferSize;
}

size_t OString::length(void) const
{
	return mnCurSize;
}

size_t OString::size(void) const
{
	return mnCurSize;
}

const char * OString::c_str(void) const
{
	if (mpLiteral)
		return mpLiteral;
	return mpCharBuffer;
}

HashID OString::id(void) const
{
	if (mbRehash)
		const_cast<OString*>(this)->Rehash();
	return mnID;
}

void OString::Rehash(void)
{
	mnID = mpCharBuffer ? StringHasher(mpCharBuffer) : 0;
	mbRehash = false;
}

/****** Private functions ******/

void OString::MyStrCopy(const char *_toCopyFrom, const size_t& _newCurSize)
{
	size_t i = 0;
	mnCurSize = _newCurSize;
	for (; i < mnCurSize; ++i)
		mpCharBuffer[i] = _toCopyFrom[i];
	for (; i < mnBufferSize; ++i)
		mpCharBuffer[i] = '\0';
	mbRehash = true;
}

void OString::Realloc(size_t _newBufferSize)
{
	if (mpCharBuffer)
		Dystopia::DefaultAllocator<char[]>::Free(mpCharBuffer);

	mnBufferSize = _newBufferSize;
	mpCharBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(mnBufferSize);
}

void OString::Alloc(size_t _newSize)
{
	if (mpCharBuffer)
	{
		if (mnBufferSize <= _newSize)
			Realloc(_newSize + 1);
	}
	else
	{
		mnBufferSize = _newSize + 1;
		mpCharBuffer = Dystopia::DefaultAllocator<char[]>::Alloc(mnBufferSize);
	}
}

void OString::CopyFromLiteral(void)
{
	DEBUG_ASSERT(!mpLiteral, "No literal found");

	if (mpCharBuffer)
	{
		if (mnBufferSize <= mnCurSize)
			Realloc(mnCurSize + 1);
	}
	else
		Realloc(mnCurSize);

	MyStrCopy(mpLiteral, mnCurSize);
	mpLiteral = nullptr;
	mbRehash = true;
}

/* non class member functions */

bool operator==(const OString& _lhs, const OString& _rhs)
{
	return _lhs.id() == _rhs.id();
}

bool operator==(const OString& _lhs, const char * _rhs)
{
	return _lhs.id() == StringHasher(_rhs);
}

bool operator==(const char * _lhs, const OString& _rhs)
{
	return _rhs.id() == StringHasher(_lhs);
}

bool operator==(HashID _id, const OString& _rhs)
{
	return _id == _rhs.id();
}

bool operator==(const OString& _lhs, HashID _id)
{
	return _id == _lhs.id();
}

bool operator==(OString&& _lhs, OString&& _rhs)
{
	return _lhs.id() == _rhs.id();
}

bool operator==(const OString& _lhs, OString&& _rhs)
{
	return _lhs.id() == _rhs.id();
}

bool operator==(OString&& _lhs, const OString& _rhs)
{
	return _lhs.id() == _rhs.id();
}

bool operator!=(const OString& _lhs, const OString& _rhs)
{
	return _lhs.id() != _rhs.id();
}

bool operator<(const OString& _lhs, const OString& _rhs)
{
	return std::strcmp(_lhs.c_str(), _rhs.c_str()) < 0;
}

bool operator>(const OString& _lhs, const OString& _rhs)
{
	return std::strcmp(_lhs.c_str(), _rhs.c_str()) > 0;
}

std::ostream& operator<<(std::ostream& _os, const OString& _rhs)
{
	_os << _rhs.c_str();
	return _os;
}

OString operator+(const OString& _lhs, const OString& _rhs)
{
	OString temp{ _lhs };
	temp += _rhs;
	return temp;
}

OString operator+(const OString& _lhs, const char* _str)
{
	OString s{ _str };
	s += _lhs;
	return s;
}

OString operator+(const char*_str, const OString& _rhs)
{
	OString s{ _str };
	s += _rhs;
	return s;
}



































