/* HEADER *********************************************************************************/
/*!
\file	PLogInfo.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/PLogInfo.h"
#include "Math/MathLib.h"
#include "Utility/Utility.h"
#include <algorithm>

namespace Dystopia
{
	static constexpr int def_max = 100;
	/* ===================================================== A single log's Data ===================================================== */
	PLogData::PLogData(const std::string& _name, bool _big)
		: mLabel{ _name }, mArrValues{ 0.f }, mCurrentIndex{ 0 }, mIsBigGraph{ _big }, 
		mMax{ def_max }
	{}

	PLogData::PLogData(const PLogData& _rhs)
		: mLabel{ _rhs.mLabel }, mArrValues{ 0.f }, mCurrentIndex{ _rhs.mCurrentIndex },
		mIsBigGraph{ _rhs.mIsBigGraph }, mMax{ _rhs.mMax }
	{
		for (unsigned int i = 0; i < maxLogs; ++i)
			mArrValues[i] = _rhs.mArrValues[i];
	}

	PLogData::PLogData(PLogData&& _rhs)
		: mLabel{}, mArrValues{ 0.f }, mCurrentIndex{}, mIsBigGraph{ false }, mMax{ def_max }
	{
		Ut::Swap(mArrValues, _rhs.mArrValues);
		Ut::Swap(mCurrentIndex, _rhs.mCurrentIndex);
		Ut::Swap(mLabel, _rhs.mLabel);
		Ut::Swap(mIsBigGraph, _rhs.mIsBigGraph);
		Ut::Swap(mMax, _rhs.mMax);
	}

	PLogData& PLogData::operator=(PLogData&& _rhs)
	{
		Ut::Swap(mArrValues, _rhs.mArrValues);
		Ut::Swap(mCurrentIndex, _rhs.mCurrentIndex);
		Ut::Swap(mLabel, _rhs.mLabel);
		Ut::Swap(mIsBigGraph, _rhs.mIsBigGraph);
		Ut::Swap(mMax, _rhs.mMax);
		return*this;
	}

	PLogData& PLogData::operator=(const PLogData& _rhs)
	{
		mCurrentIndex = _rhs.mCurrentIndex;
		for (unsigned int i = 0; i < maxLogs; ++i)
			mArrValues[i] = _rhs.mArrValues[i];
		mLabel = _rhs.mLabel;
		mIsBigGraph = _rhs.mIsBigGraph;
		mMax = _rhs.mMax;
		return *this;
	}

	void PLogData::UpdateLog(float _val)
	{
		mArrValues[mCurrentIndex] = _val;
		mMax = (_val >= mMax) ? mMax * 2 : mMax;
		mMax = (_val * 4 < mMax) ? mMax  / 2 : mMax ;
		mMax = mMax < def_max ? def_max : mMax;

		if (mCurrentIndex != (maxLogs - 1)) mCurrentIndex++;
		else
			std::rotate(&mArrValues[0], &mArrValues[1], &mArrValues[maxLogs]);
	}

	/* ===================================================== A log's item for category ===================================================== */
	PLogItem::PLogItem(const std::string& _cat)
		: mLabel{ _cat + " Details" }, mData{}, mGenericOverview{ _cat, true }, mShowGeneric{ false }
	{}

	PLogItem::PLogItem(const PLogItem& _rhs)
		: mLabel{ _rhs.mLabel }, mData{ _rhs.mData }, mGenericOverview{ _rhs.mGenericOverview },
		mShowGeneric{ _rhs.mShowGeneric }
	{}

	PLogItem::PLogItem(PLogItem&& _rhs)
		: mLabel{}, mData{}, mGenericOverview{ "", true }, mShowGeneric{ false }
	{
		Ut::Swap(mLabel, _rhs.mLabel);
		Ut::Swap(mData, _rhs.mData);
		Ut::Swap(mGenericOverview, _rhs.mGenericOverview);
		Ut::Swap(mShowGeneric, _rhs.mShowGeneric);
	}

	PLogItem::~PLogItem()
	{
		mData.clear();
	}

	void PLogItem::SortLogs()
	{
		mData.Sort([](const PLogData& p1, const PLogData& p2) { return p1.mLabel < p2.mLabel; });
	}

	void PLogItem::InsertLog(const PLogData& _graph)
	{
		mData.push_back(_graph);
	}

	void PLogItem::UpdateG(float _val, bool _bigGraph)
	{
		mShowGeneric = true;
		mGenericOverview.mIsBigGraph = _bigGraph;
		mGenericOverview.UpdateLog(_val);
	}

	void PLogItem::UpdateLog(const std::string& _graph, float _val, bool _bigGraph)
	{
		for (auto& e : mData)
		{
			if (e.mLabel == _graph)
			{
				e.UpdateLog(_val);
				return;	//early out to avoid adding new PLogData;
			}
		}
		// If never early out, means no log of that name is found
		PLogData newData{ _graph, _bigGraph };
		newData.UpdateLog(_val);
		InsertLog(newData);
		SortLogs();
	}

	PLogItem& PLogItem::operator=(const PLogItem& _rhs)
	{
		mLabel = _rhs.mLabel;
		mData.clear();
		mData = _rhs.mData;
		mGenericOverview = _rhs.mGenericOverview;
		mShowGeneric = _rhs.mShowGeneric;
		return *this;
	}

	PLogItem& PLogItem::operator=(PLogItem&& _rhs)
	{
		Ut::Swap(mLabel, _rhs.mLabel);
		Ut::Swap(mData, _rhs.mData);
		Ut::Swap(mGenericOverview, _rhs.mGenericOverview);
		Ut::Swap(mShowGeneric, _rhs.mShowGeneric);
		return *this;
	}
}

#endif //EDITOR
