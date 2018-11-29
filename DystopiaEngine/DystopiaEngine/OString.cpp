#include "OString.h"

OString::OString(void)
	: mnCurSize{ 0 }, mnFullSize{ 0 }, mnID{ 0 }, mpLiteral{ nullptr }, mpCharBuffer{ nullptr }, mbRehash{ false }
{}

OString::OString(OString&& _moveCtor) noexcept
	: mnCurSize{ _moveCtor.mnCurSize }, mnFullSize{ _moveCtor.mnFullSize }, mnID{ _moveCtor.mnID }, mpLiteral{ _moveCtor.mpLiteral },
	mpCharBuffer{ _moveCtor.mpCharBuffer }, mbRehash{ _moveCtor.mbRehash }
{
	_moveCtor.mnCurSize = 0;
	_moveCtor.mnFullSize = 0;
	_moveCtor.mnID = 0;
	_moveCtor.mpLiteral = nullptr;
	_moveCtor.mpCharBuffer = nullptr;
	_moveCtor.mbRehash = false;
}

OString::OString(const OString& _copyCtor)
	: mnCurSize{ _copyCtor.mnCurSize }, mnFullSize{ _copyCtor.mnFullSize }, mnID{ _copyCtor.mnID }, mpLiteral{ _copyCtor.mpLiteral },
	mpCharBuffer{ _copyCtor.mpCharBuffer }, mbRehash{ _copyCtor.mbRehash }
{}

OString::OString(const char* _cstrStart, const char* _cstrEnd)
	: mnCurSize{ static_cast<size_t>(_cstrEnd - _cstrStart) }, mnFullSize{ mnCurSize + 1 }, mnID{ 0 }, mpLiteral{ nullptr },
	mpCharBuffer{ Dystopia::DefaultAllocator<char[]>::Alloc(mnFullSize) }, mbRehash{ true }
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

const char * OString::c_str(void)
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





































