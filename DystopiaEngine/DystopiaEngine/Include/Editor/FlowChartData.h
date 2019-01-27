/* HEADER *********************************************************************************/
/*!
\file	FlowChartData.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _FLOW_CHART_DATA_H_
#define _FLOW_CHART_DATA_H_
#include "DataStructure/AutoArray.h"
#include "DataStructure/HashString.h"
#include "Utility/GUID.h"
#include "Math/Vectors.h"

struct UserLinks
{
	UserLinks(int _fromIndx, int _fromSlot, int _toIndx, int _toslot);
	int mnFromIndx;
	int mnFromSlot;
	int mnToIndx;
	int mnToSlot;
};

class UserItem
{
public:
	UserItem(const HashString& _name);
	~UserItem(void);

	void AddLink(const uint64_t&);
	uint64_t GetID(void) const;
	HashString GetName(void) const;

	Math::Vec2 GetPos(void) const;
	Math::Vec2 GetSize(void) const;
	int GetInputCount(void) const;
	int GetOutputCount(void) const;

	void SetPos(const Math::Vec2&);
	void SetSize(const Math::Vec2&);
	Math::Vec2 GetInputSlotPos(int slot_no) const; 
	Math::Vec2 GetOutputSlotPos(int slot_no) const;

	int IncrementInput(void);
	int IncrementOutput(void);
	const AutoArray<uint64_t>& GetLinks(void) const;

private:
	uint64_t mnID;
	HashString mName;
	Math::Vec2 mPos;
	Math::Vec2 mSize;
	int mnInputCount;
	int mnOutputCount;
	AutoArray<uint64_t> mLinks;

};





#endif







