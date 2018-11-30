#include "OString.h"
#include "Utility/DebugAssert.h"

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

/* modifiers */

OString& OString::operator+=(const OString& _rhs)
{
	return *this;
}

OString& OString::erase(size_t _pos, size_t _len)
{
	return *this;
}

OString& OString::replace(size_t _pos, size_t _len, const OString& _rhs)
{
	return *this;
}

OString& OString::replace(size_t _pos, size_t _len, const char * _s)
{
	return *this;
}




/****** get functions ******/

size_t OString::bufferSize(void) const
{
	return mnBufferSize;
}

size_t OString::len(void) const
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





































