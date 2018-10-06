/* HEADER *********************************************************************************/
/*!
\file	Clipboard.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/Clipboard.h"

#include "Component/Component.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

#define DEFAULT_CLIP_SIZE 20

Dystopia::ClipData::ClipData(eClipDataTypes _dataType, void* _data, const size_t _dataSize)
	: mDataType{ _dataType }, mpData{ _data }, mDataSize{ _dataSize }
{
}

Dystopia::ClipData::~ClipData(void)
{
}

bool Dystopia::ClipData::IsSameSize(const size_t& _dataSize) const
{
	return _dataSize == mDataSize;
}

bool Dystopia::ClipData::IsSameType(eClipDataTypes _dataType) const
{
	return _dataType == mDataType;
}

void* Dystopia::ClipData::GetData()
{
	return mpData;
}

bool Dystopia::ClipData::operator==(const Dystopia::ClipData& _rhs) const
{
	return (mDataSize == _rhs.mDataSize) &&
		   (mDataType == _rhs.mDataType) &&
		   (mpData == _rhs.mpData);
}

Dystopia::Clipboard::Clipboard(void)
	: mArrClipData{ DEFAULT_CLIP_SIZE }, mArrSelectionID{ DEFAULT_CLIP_SIZE }
{
}

Dystopia::Clipboard::~Clipboard(void)
{
}

void Dystopia::Clipboard::Close(void)
{
	ClearData();
	ClearSelection();
}

void Dystopia::Clipboard::ClearData(void)
{
	mArrClipData.clear();
}
void Dystopia::Clipboard::ClearSelection(void)
{
	mArrSelectionID.clear();
}

void Dystopia::Clipboard::InsertSelection(const uint64_t& _uniqueID)
{
	for (const auto& id : mArrSelectionID)
	{
		if (id == _uniqueID) return;
	}
	mArrSelectionID.Insert(_uniqueID);
}

void Dystopia::Clipboard::InsertData(eClipDataTypes _dataType, void* _data, const size_t _dataSize)
{
	ClipData dat{ _dataType, _data, _dataSize };
	for (const auto& e : mArrClipData)
	{
		if (e == dat) return;
	}
	mArrClipData.Insert(dat);
}

AutoArray<void*> Dystopia::Clipboard::RetrieveDatas(eClipDataTypes _dataType, const size_t& _dataSize) const
{
	AutoArray<void*> ret{ DEFAULT_CLIP_SIZE };
	for (auto & e : mArrClipData)
	{
		if (e.IsSameSize(_dataSize) && e.IsSameType(_dataType))
		{
			ret.push_back(e.GetData());
		}
	}
	return ret;
}

const AutoArray<uint64_t>& Dystopia::Clipboard::RetrieveSelections(void) const
{
	return mArrSelectionID;
}



#endif
