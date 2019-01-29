#include "Component/AudioListener.h"
#include "System/Driver/Driver.h"
#include "System/Base/ComponentDonor.h"
#include "System/Sound/SoundSystem.h"
#include "Math/Vectors.h"

#include "fmod.hpp"

#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#include "Editor/Payloads.h"
#include "Editor/ConsoleLog.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"

#define MAX_RANGE 99999.9f
#endif

namespace Dystopia
{
	Dystopia::AudioListener::AudioListener(void)
		:mMaxRange{0.f},mPos{new FMOD_VECTOR{0,0,0}}
	{
		EngineCore::Get<SoundSystem>()->RegisterNewListener(this);
	}

	AudioListener::AudioListener(float _MaxRange)
		:mMaxRange{_MaxRange}, mPos{ new FMOD_VECTOR{0,0,0} }
	{
		EngineCore::Get<SoundSystem>()->RegisterNewListener(this);
	}

	Dystopia::AudioListener::AudioListener(const AudioListener & _other)
		: mMaxRange{ _other.mMaxRange }, mPos{ new FMOD_VECTOR{_other.mPos->x,_other.mPos->y,_other.mPos->z} }
	{
		EngineCore::Get<SoundSystem>()->RegisterNewListener(this);
	}

	Dystopia::AudioListener::~AudioListener(void)
	{
		delete mPos;
		mPos = nullptr;
	}

	void Dystopia::AudioListener::Awake(void)
	{
	}

	void Dystopia::AudioListener::Load(void)
	{
	}

	void Dystopia::AudioListener::Init(void)
	{
	}

	void Dystopia::AudioListener::GameObjectDestroy(void)
	{
	}

	void Dystopia::AudioListener::Update(float)
	{
	}

	Dystopia::AudioListener * Dystopia::AudioListener::Duplicate() const
	{
		return static_cast<ComponentDonor<AudioListener>*>(EngineCore::GetInstance()->Get<SoundSystem>())->RequestComponent(*this);
	}

	void Dystopia::AudioListener::Serialise(TextSerialiser & _out) const
	{
		_out.InsertStartBlock("Audio Listener");
		Component::Serialise(_out);
		_out.InsertEndBlock("Audio Listener");
	}

	void Dystopia::AudioListener::Unserialise(TextSerialiser & _in)
	{
		_in.ConsumeStartBlock();
		Component::Unserialise(_in);
		_in.ConsumeEndBlock();
	}

#if EDITOR
	void Dystopia::AudioListener::EditorUI(void) noexcept
	{
		//auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();

		//EGUI::PushLeftAlign(80);
		//EGUI::PushID(1);

		//switch (EGUI::Display::DragFloat("Volume", &mMaxRange, 0.1f, 0.f, MAX_RANGE))
		//{
		//case EGUI::eSTART_DRAG:
		//	cmd->StartRec(&AudioListener::mMaxRange, this);
		//	break;
		//case EGUI::eDRAGGING:
		//	break;
		//case EGUI::eEND_DRAG:
		//case EGUI::eENTER:
		//case EGUI::eDEACTIVATED:
		//case EGUI::eTABBED:
		//	cmd->EndRec(&AudioListener::mMaxRange, this);
		//	break;
		//}

		//EGUI::PopID();
		//EGUI::PopLeftAlign();
	}
#endif

	float AudioListener::GetMaxRange() const
	{
		return mMaxRange;
	}

	void AudioListener::SetMaxRange(float const & _Range)
	{
		mMaxRange = _Range;
	}

	FMOD_VECTOR * AudioListener::GetFMODPos() const
	{
		return mPos;
	}

	void AudioListener::UpdateFMODPos(Math::Vec3D const & _vec)
	{
		mPos->x = _vec.x;
		mPos->y = _vec.y;
		mPos->z = _vec.z;
	}

}
