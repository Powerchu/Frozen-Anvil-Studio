/* HEADER *********************************************************************************/
/*!
\file	PerformanceLog.cpp
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
#include "Editor\PerformanceLog.h"
#include "Editor\EGUI.h"
#include "Math\MathLib.h"
#include "Utility\Utility.h"
#include <algorithm>

namespace Dystopia
{
	PLogData::PLogData(const std::string& _name, bool _big)
		: mLabel{ _name }, mArrValues{ 0.f }, mCurrentIndex{ 0 }, mIsBigGraph{ _big }, mMin{}, mMax{}
	{}

	PLogData::PLogData(const PLogData& _rhs)
		: mLabel{ _rhs.mLabel }, mArrValues{ 0.f }, mCurrentIndex{ _rhs.mCurrentIndex }, 
		mIsBigGraph{ _rhs.mIsBigGraph }, mMin{ _rhs.mMin }, mMax{ _rhs.mMax }
	{
		for (unsigned int i = 0; i < maxLogs; ++i)
			mArrValues[i] = _rhs.mArrValues[i];
	}
	
	PLogData::PLogData(PLogData&& _rhs)
		: mLabel{}, mArrValues{ 0.f }, mCurrentIndex{}, mIsBigGraph{false}, mMin{}, mMax{}
	{
		Utility::Swap(mArrValues, _rhs.mArrValues);
		Utility::Swap(mCurrentIndex, _rhs.mCurrentIndex);
		Utility::Swap(mLabel, _rhs.mLabel);
		Utility::Swap(mIsBigGraph, _rhs.mIsBigGraph);
		Utility::Swap(mMin, _rhs.mMin);
		Utility::Swap(mMax, _rhs.mMax);
	}

	PLogData& PLogData::operator=(PLogData&& _rhs)
	{
		Utility::Swap(mArrValues, _rhs.mArrValues);
		Utility::Swap(mCurrentIndex, _rhs.mCurrentIndex);
		Utility::Swap(mLabel, _rhs.mLabel);
		Utility::Swap(mIsBigGraph, _rhs.mIsBigGraph);
		Utility::Swap(mMin, _rhs.mMin);
		Utility::Swap(mMax, _rhs.mMax);
		return*this;
	}

	PLogData& PLogData::operator=(const PLogData& _rhs)
	{
		mCurrentIndex = _rhs.mCurrentIndex;
		for (unsigned int i = 0; i < maxLogs; ++i)
			mArrValues[i] = _rhs.mArrValues[i];
		mLabel = _rhs.mLabel;
		mIsBigGraph = _rhs.mIsBigGraph;
		mMin = _rhs.mMin;
		mMax = _rhs.mMax;
		return *this;
	}

	void PLogData::UpdateLog(float _val, float _min, float _max)
	{
		mMin = _min;
		mMax = _max;
		mArrValues[mCurrentIndex] = _val;

		if (mCurrentIndex != (maxLogs - 1)) mCurrentIndex++;
		else std::rotate(&mArrValues[0], &mArrValues[1], &mArrValues[maxLogs]);
	}

	PLogItem::PLogItem(const std::string& _cat)
		: mLabel{ _cat }, mData{}, mGenericOverview{}
	{}

	PLogItem::PLogItem(const PLogItem& _rhs)
		: mLabel{ _rhs.mLabel }, mData{ _rhs.mData }, mGenericOverview{ _rhs.mGenericOverview }
	{}
	
	PLogItem::PLogItem(PLogItem&& _rhs)
		: mLabel{}, mData{}, mGenericOverview{}
	{
		Utility::Swap(mLabel, _rhs.mLabel);
		Utility::Swap(mData, _rhs.mData);
		Utility::Swap(mGenericOverview, _rhs.mGenericOverview);
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

	void PLogItem::UpdateLog(const std::string& _graph, float _val, float _min, float _max, bool _bigGraph)
	{
		for (auto& e : mData)
		{
			if (e.mLabel == _graph)
			{
				e.UpdateLog(_val, _min, _max);
				return;	//early out to avoid adding new PLogData;
			}
		}
		// If never early out, means no log of that name is found
		PLogData newData{ _graph, _bigGraph };
		newData.UpdateLog(_val, _min, _max);
		InsertLog(newData);
		SortLogs();
	}

	PLogItem& PLogItem::operator=(const PLogItem& _rhs)
	{
		mLabel = _rhs.mLabel;
		mData.clear();
		mData = _rhs.mData;
		mGenericOverview = _rhs.mGenericOverview;
		return *this;
	}

	PLogItem& PLogItem::operator=(PLogItem&& _rhs)
	{
		Utility::Swap(mLabel, _rhs.mLabel);
		Utility::Swap(mData, _rhs.mData);
		Utility::Swap(mGenericOverview, _rhs.mGenericOverview);
		return *this;
	}

	static PerformanceLog* gpInstance = 0;
	PerformanceLog* PerformanceLog::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new PerformanceLog{};
		return gpInstance;
	}

	PerformanceLog::PerformanceLog()
		: EditorTab{ false },
		mLabel{ "Performance" },
		mGraphSize{ Math::Vec2{0,0} },
		mArrLoggedData{},
		mGraphBigY{ 100 },
		mGraphSmallY{ 40 }
	{}

	PerformanceLog::~PerformanceLog()
	{
		gpInstance = nullptr;
	}

	void PerformanceLog::Init()
	{
		LogData("Editor", "Func 1", 0, 0.f, 0.1f);
		LogData("Editor", "Func 2", 0, 0.f, 0.5f);
		LogData("Editor", "Func 3", 0, 0.f, 0.3f);
	}

	void PerformanceLog::Update(const float& _dt)
	{
		static constexpr float offset	= -50.f;
		mGraphSize.x					= Math::Clamp(Size().x + offset, 50.f, Size().x);

		LogData("Editor", "Func 1", _dt, 0.f, 0.1f);
		LogData("Editor", "Func 2", _dt * 2.f, 0.f, 0.2f);
		LogData("Editor", "Func 3", _dt * 2.f, 0.f, 0.1f);
	}

	void PerformanceLog::EditorUI()
	{
		for (const auto& item : mArrLoggedData)
		{
			if (EGUI::Display::StartTreeNode(item.mLabel))
			{
				for (unsigned int i = 0; i < item.mData.size(); ++i)
				{
					const PLogData& e = item.mData[i];
					mGraphSize.y = (e.mIsBigGraph) ? mGraphBigY : mGraphSmallY;
					EGUI::Display::LineGraph(e.mLabel.c_str(), e.mArrValues, e.mMin, e.mMax, mGraphSize,
											 std::to_string(e.mArrValues[e.maxLogs - 1]));
					EGUI::Display::HorizontalSeparator();
				}
				EGUI::Display::EndTreeNode();
			}
		}
	}

	void PerformanceLog::Shutdown()
	{
		mArrLoggedData.clear();
	}

	std::string PerformanceLog::GetLabel() const
	{
		return mLabel;
	}

	void PerformanceLog::LogData(const std::string& _cat, const std::string& _label, 
								 const float& _val, float _min, float _max, bool _bigGraph)
	{
		for (auto& item : mArrLoggedData)
		{
			if (item.mLabel == _cat)
			{
				item.UpdateLog(_label, _val, _min, _max, _bigGraph);
				return;		//early out to avoid adding new PLogData;
			}
		}
		// If never early out, means no log of that name is found
		PLogItem newItem{ _cat };
		newItem.UpdateLog(_label, _val, _min, _max, _bigGraph);
		mArrLoggedData.push_back(newItem);
		SortLogs();
	}

	void PerformanceLog::SortLogs()
	{
		mArrLoggedData.Sort([](const PLogItem& p1, const PLogItem& p2) { return p1.mLabel < p2.mLabel; });
	}
}


#endif //EDITOR

