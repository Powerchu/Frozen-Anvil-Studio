/* HEADER *********************************************************************************/
/*!
\file	Image.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Class to hold raw image data loaded into memory

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "IO/Image.h"

#include "Utility/Utility.h"
#include "Allocator/DefaultAlloc.h"


Dystopia::Image::Image() noexcept
	: mstrName{}, mbChanged{ false }, mbCompressed{}, mbRGB{}, mnRawFormat{}, mnFormat{},
	mnWidth{}, mnHeight{}, mnChannels{}, mnMipMaps{}, mpImageData{ nullptr }
{
}

Dystopia::Image::Image(HashString const& _str, bool _bCompressed, bool _bRGB, 
	unsigned _rawFormat, unsigned _format, unsigned _w, unsigned _h, unsigned _chnls, unsigned _mips, void* _ptr) noexcept
	: mstrName{ _str }, mbChanged{ false }, mbCompressed{ _bCompressed }, mbRGB{ _bRGB }, mnRawFormat{ _rawFormat }, mnFormat{ _format },
	mnWidth{ _w }, mnHeight{ _h }, mnChannels{ _chnls }, mnMipMaps{ _mips }, mpImageData{ _ptr }
{
}

//Dystopia::Image::Image(Image const& _o) noexcept
//	: mstrName{ _o.mstrName }, mbCompressed{ _o.mbCompressed }, mbRGB{ _o.mbRGB }, 
//	mnRawFormat{ _o.mnRawFormat }, mnFormat{ _o.mnFormat }, mnWidth{ _o.mnWidth }, mnHeight{ _o.mnHeight }, 
//	mnChannels{ _o.mnChannels }, mnMipMaps{ _o.mnMipMaps }, mpImageData{ _o.mpImageData }
//{
//}

Dystopia::Image::Image(Image&& _o) noexcept
	: mstrName{ Ut::Move(_o.mstrName) }, mbChanged{ Ut::Move(_o.mbChanged) }, mbCompressed{ Ut::Move(_o.mbCompressed) }, mbRGB{ Ut::Move(_o.mbRGB) },
	mnRawFormat{ Ut::Move(_o.mnRawFormat) }, mnFormat{ Ut::Move(_o.mnFormat) }, mnWidth{ Ut::Move(_o.mnWidth) }, mnHeight{ Ut::Move(_o.mnHeight) },
	mnChannels{ Ut::Move(_o.mnChannels) }, mnMipMaps{ Ut::Move(_o.mnMipMaps) }, mpImageData{ Ut::Move(_o.mpImageData) }
{
	_o.mpImageData = nullptr;
}


Dystopia::Image::~Image(void) noexcept
{

}

void Dystopia::Image::OnEditorUI(void)
{

}

Dystopia::Image& Dystopia::Image::operator = (Image&& _obj) noexcept
{
	std::memcpy(this, &_obj, sizeof(Image) - sizeof(void*));
	Ut::Swap(mpImageData, _obj.mpImageData);

	return *this;
}

