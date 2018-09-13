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
#include "Editor\PLogger.h"
#include "Editor\ConsoleDebugger.h"
#include "Editor\EGUI.h"
#include "System\Profiler\ProfilerAction.h"
#include "System\Profiler\Profiler.h"
#include "Math\MathUtility.h"
#include <algorithm>

namespace Dystopia
{
	static PerformanceLog* gpInstance = 0; // singleton
	namespace Performance
	{
		/* ===================================================== Performance Logger definitions ===================================================== */
		void LogDataS(const std::string& _category, const std::string& _graphLabel,
			float _val, float _min, float _max)
		{
			DEBUG_ASSERT(!gpInstance, "No instance of Performance Log found!");
			gpInstance->LogData(_category, _graphLabel, _val, _min, _max, false);
		}

		void LogDataG(const std::string& _catMainGraph, float _val, float _min, float _max)
		{
			DEBUG_ASSERT(!gpInstance, "No instance of Performance Log found!");
			gpInstance->LogData(_catMainGraph, _val, _min, _max, true);
		}

	}

	/* ===================================================== The Performance Logger for handling items/datas ===================================================== */
	PerformanceLog* PerformanceLog::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new PerformanceLog{};
		return gpInstance;
	}

	PerformanceLog::PerformanceLog()
		: EditorTab{ false },
		mLabel{ "Performance" },
		mGraphSizeB{ Math::Vec2{0,0} },
		mGraphSizeS{ Math::Vec2{0,0} },
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
	}

	void PerformanceLog::Update(const float& _dt)
	{
		static constexpr float offset	= -60.f;
		mGraphSizeB.x					= Math::Clamp(Size().x + offset, 50.f, Size().x);
		mGraphSizeS.x					= mGraphSizeB.x;
		mGraphSizeS.x					= Math::Clamp(mGraphSizeS.x, 50.f, Size().x -110.f);
	}

	void PerformanceLog::EditorUI()
	{
		for (const auto& item : mArrLoggedData)
		{
			if (item.mShowGeneric)
			{
				ShowLog(item.mGenericOverview, mGraphSizeB);
			}
			if (item.mData.size() && EGUI::Display::StartTreeNode(item.mLabel))
			{
				for (unsigned int i = 0; i < item.mData.size(); ++i)
				{
					ShowLog(item.mData[i], mGraphSizeS);
				}
				EGUI::Display::EndTreeNode();
			}
			EGUI::Display::HorizontalSeparator();
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
								 float _val, float _min, float _max, bool _bigGraph)
	{
		for (auto& item : mArrLoggedData)
		{
			if (item.mGenericOverview.mLabel == _cat)
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

	void PerformanceLog::LogData(const std::string& _catMainGraph, float _val,
								 float _min, float _max, bool _bigGraph)
	{
		for (auto& item : mArrLoggedData)
		{
			if (item.mGenericOverview.mLabel == _catMainGraph)
			{
				item.UpdateG(_val, _min, _max, _bigGraph);
				return;		//early out to avoid adding new PLogData;
			}
		}
		PLogItem newItem{ _catMainGraph };
		newItem.UpdateG(_val, _min, _max, _bigGraph);
		mArrLoggedData.push_back(newItem);
	}

	void PerformanceLog::SortLogs()
	{
		mArrLoggedData.Sort([](const PLogItem& p1, const PLogItem& p2) { return p1.mLabel < p2.mLabel; });
	}

	void PerformanceLog::ShowLog(const PLogData& _log, Math::Vec2 _size)
	{
		EGUI::Indent(10);
		_size.y = (_log.mIsBigGraph) ? mGraphBigY : mGraphSmallY;
		EGUI::Display::LineGraph(_log.mLabel.c_str(), _log.mArrValues, _log.mMin, _log.mMax, _size,
			std::to_string(_log.mArrValues[_log.maxLogs - 1]));
		EGUI::UnIndent(10);
	}
}


#endif //EDITOR

