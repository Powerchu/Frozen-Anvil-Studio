/* HEADER *********************************************************************************/
/*!
\file	FlowChartData.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Editor/FlowChartData.h"

UserLinks::UserLinks(int _ii, int _is, int _oi, int _os)
	: mnFromIndx{ _ii }, mnFromSlot{ _is }, mnToIndx{ _oi }, mnToSlot{ _os }
{}

UserItem::UserItem(const HashString& _name)
	: mName{ _name }, mnID{ Dystopia::GUIDGenerator::GetUniqueID() },
	mLinks{}, mPos{ 0,0 }, mSize{ 100, 40 }, mnInputCount{ 0 }, mnOutputCount{ 0 }
{
}

UserItem::~UserItem(void)
{
}

void UserItem::AddLink(const uint64_t& _id)
{
	for (const auto& elem : mLinks)
		if (elem == _id)
			return;

	mLinks.Insert(_id);
}

uint64_t UserItem::GetID(void) const
{
	return mnID;
}

HashString UserItem::GetName(void) const
{
	return mName;
}

Math::Vec2 UserItem::GetPos(void) const
{
	return mPos;
}

Math::Vec2 UserItem::GetSize(void) const
{
	return mSize;
}

int UserItem::GetInputCount(void) const
{
	return mnInputCount;
}

int UserItem::GetOutputCount(void) const
{
	return mnOutputCount;
}

void UserItem::SetPos(const Math::Vec2& _pos)
{
	mPos = _pos;
}

void UserItem::SetSize(const Math::Vec2& _size)
{
	mSize = _size;
}

Math::Vec2 UserItem::GetInputSlotPos(int _n) const
{ 
	return Math::Vec2(mPos.x, mPos.y + mSize.y * (static_cast<float>(_n) + 1) / (static_cast<float>(mnInputCount) + 1));
}

Math::Vec2 UserItem::GetOutputSlotPos(int _n) const
{ 
	return Math::Vec2(mPos.x + mSize.x, mPos.y + mSize.y * (static_cast<float>(_n) + 1) / (static_cast<float>(mnOutputCount) + 1));
}

int UserItem::IncrementInput(void)
{
	int temp = mnInputCount;
	mnInputCount++;
	return temp;
}

int UserItem::IncrementOutput(void)
{
	int temp = mnOutputCount;
	mnOutputCount++;
	return temp;
}

const AutoArray<uint64_t>& UserItem::GetLinks(void) const
{
	return mLinks;
}



