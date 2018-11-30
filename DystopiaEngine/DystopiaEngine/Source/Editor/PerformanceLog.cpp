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
#include "Editor/PLogger.h"
#include "Editor/PerformanceLog.h"
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"

#include "Math/MathUtility.h"

#include <algorithm>
#include <string>

namespace Editor //Dystopia
{
	//static PerformanceLog* gpInstance = 0; // singleton
	namespace Performance
	{
		/* ===================================================== Performance Logger definitions ===================================================== */
		void LogDataS(const HashString& _category, const HashString& _graphLabel,
			float _val)
		{
			auto gpInstance = EditorMain::GetInstance()->GetPanel<PerformanceLog>();
			if (gpInstance)
				gpInstance->LogData(_category, _graphLabel, _val, false);
		}

		void LogDataG(const HashString& _catMainGraph, float _val)
		{
			auto gpInstance = EditorMain::GetInstance()->GetPanel<PerformanceLog>();
			if (gpInstance)
				gpInstance->LogData(_catMainGraph, _val, true);
		}

		void LogTaskMgr(const PLogTaskManager& _data)
		{
			auto gpInstance = EditorMain::GetInstance()->GetPanel<PerformanceLog>();
			if (gpInstance)
				gpInstance->LogTaskMgr(_data);
		}
	}

	/* ===================================================== The Performance Logger for handling items/datas ===================================================== */

	PerformanceLog::PerformanceLog(void)
		: //EditorTab{ false },
		mLabel{ "Performance" },
		mGraphSizeB{ Math::Vec2{0,0} },
		mGraphSizeS{ Math::Vec2{0,0} },
		mArrLoggedData{},
		mGraphBigY{ 50 },
		mGraphSmallY{ 20 },
		mTaskMgrDetails{}
	{}

	PerformanceLog::~PerformanceLog(void)
	{
		//gpInstance = nullptr;
	}

	void PerformanceLog::Load(void)
	{}

	bool PerformanceLog::Init(void)
	{
		return true;
	}

	void PerformanceLog::Update(float)
	{
		static constexpr float offset = -80.f;
		mGraphSizeB.x = Math::Clamp(Size().x + offset, 50.f, Size().x);
		mGraphSizeS.x = mGraphSizeB.x;
		mGraphSizeS.x = Math::Clamp<float>(mGraphSizeS.x, 50.f, Size().x - 120.f);
	}

	void PerformanceLog::EditorUI(void)
	{
		ShowTaskMgrBreakdown();
		for (const auto& item : mArrLoggedData)
		{
			if (item.mShowGeneric && EGUI::Display::StartTreeNode(item.mGenericOverview.mLabel.c_str(),nullptr,false,false,true,true))
			{
				EGUI::Indent(10);
				ShowLog(item.mGenericOverview, mGraphSizeB);
				if (item.mData.size() && EGUI::Display::StartTreeNode(item.mLabel.c_str(), nullptr, false, false, false, true))
				{
					for (const auto& i : item.mData)
					{
						EGUI::Display::Label(i.mLabel.c_str());
						ShowLog(i, mGraphSizeS);
					}
					EGUI::Display::EndTreeNode();
				}
				EGUI::UnIndent(10);
				EGUI::Display::EndTreeNode();
			}
			EGUI::Display::HorizontalSeparator();
		}
	}

	void PerformanceLog::Shutdown(void)
	{
		mArrLoggedData.clear();
	}

	void PerformanceLog::Message(eEMessage)
	{}

	void PerformanceLog::SaveSettings(Dystopia::TextSerialiser&) const
	{}

	void PerformanceLog::LoadSettings(Dystopia::TextSerialiser&)
	{}

	HashString PerformanceLog::GetLabel(void) const
	{
		return mLabel;
	}

	void PerformanceLog::LogData(const HashString& _cat, const HashString& _label, float _val, bool _bigGraph)
	{
		for (auto& item : mArrLoggedData)
		{
			if (item.mGenericOverview.mLabel == _cat)
			{
				item.UpdateLog(_label, _val, _bigGraph);
				return;		//early out to avoid adding new PLogData;
			}
		}
		// If never early out, means no log of that name is found
		PLogItem newItem{ _cat };
		newItem.UpdateLog(_label, _val, _bigGraph);
		mArrLoggedData.push_back(newItem);
		SortLogs();
	}

	void PerformanceLog::LogData(const HashString& _catMainGraph, float _val, bool _bigGraph)
	{
		for (auto& item : mArrLoggedData)
		{
			if (item.mGenericOverview.mLabel == _catMainGraph)
			{
				item.UpdateG(_val, _bigGraph);
				return;		//early out to avoid adding new PLogData;
			}
		}
		PLogItem newItem{ _catMainGraph };
		newItem.UpdateG(_val, _bigGraph);
		mArrLoggedData.push_back(newItem);
	}

	void PerformanceLog::LogTaskMgr(const PLogTaskManager& _data)
	{
		mTaskMgrDetails = _data;
	}

	void PerformanceLog::SortLogs(void)
	{
		mArrLoggedData.Sort([](const PLogItem& p1, const PLogItem& p2) { return std::string{p1.mLabel.c_str()} < std::string{ p2.mLabel.c_str() }; });
	}

	void PerformanceLog::ShowLog(const PLogData& _log, Math::Vec2 _size)
	{
		_size.y = (_log.mIsBigGraph) ? mGraphBigY : mGraphSmallY;
		EGUI::Display::LineGraph(_log.mLabel.c_str(), _log.mArrValues, 0, static_cast<float>(_log.mMax), _size,
								 std::to_string(static_cast<int>(_log.mArrValues[_log.mCurrentIndex])).c_str());
	}

	void PerformanceLog::ShowTaskMgrBreakdown(void)
	{
		//static constexpr float sizeY = 200;
		//Math::Vec2 size{Size().x - 7.f, sizeY };
		//if (EGUI::StartChild("Task Manager Breakdown", size))
		{
			EGUI::Display::Dummy(0, 5.f);

			EGUI::Display::Label(" Frame Rate (s)   : %.1f FPS", 1.f / mTaskMgrDetails.mFrameRate);
			EGUI::Display::Label(" CPU Idle (%%)     : %.1f", mTaskMgrDetails.mCPUIdle);
			EGUI::Display::Label(" CPU Busy (%%)     : %.1f", mTaskMgrDetails.mCPUBusy);
			EGUI::Display::Label(" CPU OS   (%%)     : %.1f", mTaskMgrDetails.mCPUOS);
			EGUI::Display::Label(" CPU Proc (%%)     : %.1f", mTaskMgrDetails.mCPUProc);
			EGUI::Display::Label(" Page Faults      : %d", mTaskMgrDetails.mPageFaults);
			EGUI::Display::Label(" Memory Used (MB) : %d", mTaskMgrDetails.mMemUsed);
			EGUI::Display::Label(" RAM Used (MB)    : %d", mTaskMgrDetails.mRamUsed);
			EGUI::Display::Label(" Memory Available : %d", mTaskMgrDetails.mMemAvail);
			EGUI::Display::Label(" Memory Load      : %.1f", mTaskMgrDetails.mMemLoad);

			EGUI::Display::Dummy(0, 5.f);
			EGUI::Display::HorizontalSeparator();
			EGUI::Display::Dummy(0, 5.f);
		}
		//EGUI::EndChild();
	}
}


#endif //EDITOR

