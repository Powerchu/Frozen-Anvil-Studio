/* HEADER *********************************************************************************/
/*!
\file	AudioSource.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Sound/SoundSystem.h"
#include "System/Sound/Audio.h"
#include "System/Driver/Driver.h"
#include "Component/AudioSource.h"
#include "Object/GameObject.h"
#include "fmod.hpp"

#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#include "Editor/Payloads.h"
#include "Editor/ConsoleLog.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#endif 

#define MAX_VOLUME 3
#define MAX_CAP_SCALE 5
#define FREQUENCY 44100
#define MAX_RANGE 10000.f
Dystopia::AudioSource::AudioSource(void) 
	: Component{}, 
	mpSound{ nullptr }, mSoundName{ "Samsara.mp3" }, mChannel{},
	mSoundType{ 0 }, mPlayOnStart{ true }, mLoop{ false },
	mPaused{ false }, mIsPlaying{ false }, mReady{ false }, mVolume{ 1.f },
	mFrequency{ 1.f }, mPitch{ 1.f }, mChanged{ true }, mRange{0.1f}, mMaxRange{10000.f}, mPos{ new FMOD_VECTOR{0,0,0} }
{
}

Dystopia::AudioSource::AudioSource(const AudioSource& _rhs)
	: Component{ _rhs }, 
	mpSound{ nullptr }, mSoundName{ _rhs.mSoundName }, mChannel{},
	mSoundType{ _rhs.mSoundType }, mPlayOnStart{ _rhs.mPlayOnStart }, mLoop{ _rhs.mLoop },
	mPaused{ false }, mIsPlaying{ false }, mReady{ false }, mVolume{ _rhs.mVolume },
	mFrequency{ _rhs.mFrequency }, mPitch{ _rhs.mPitch }, mChanged{ true }, mRange{_rhs.mRange}, mMaxRange{ _rhs.mMaxRange }, mPos{ new FMOD_VECTOR{ _rhs.mPos->x, _rhs.mPos->y, _rhs.mPos->z } }
{
}

Dystopia::AudioSource::~AudioSource(void)
{
	delete mPos;
	mPos = nullptr;
}

void Dystopia::AudioSource::Awake(void)
{
}

void Dystopia::AudioSource::Load(void)
{
}

void Dystopia::AudioSource::Init(void)
{
	if (mSoundName.size())
		SetSound(EngineCore::GetInstance()->GetSystem<SoundSystem>()->LoadSound(HashString{ mSoundName.c_str() }));
	mReady = mPlayOnStart;
}

void Dystopia::AudioSource::Update(float)
{
	if (!mChannel.mpChannel || !mpSound) return;

	mChannel.mpChannel->isPlaying(&mIsPlaying);
	mChannel.mpChannel->setVolume(mVolume);
	mChannel.mpChannel->setFrequency(FREQUENCY * mFrequency);
	mChannel.mpChannel->setPitch(mPitch);
	mpSound->mpSound->set3DMinMaxDistance(mRange, mMaxRange);

	if (!mIsPlaying && mLoop)
		mReady = true;
}

void Dystopia::AudioSource::GameObjectDestroy(void)
{
	Component::DestroyComponent();
	if (mChannel.mpChannel) 
		mChannel.mpChannel->stop();
}

Dystopia::AudioSource* Dystopia::AudioSource::Duplicate() const
{
	return static_cast<ComponentDonor<AudioSource>*>(EngineCore::GetInstance()->Get<SoundSystem>())->RequestComponent(*this);
}

void Dystopia::AudioSource::Serialise(TextSerialiser& _out) const
{
	_out.InsertStartBlock("Audio");
	Component::Serialise(_out);
	_out << mSoundType;
	_out << mPlayOnStart;
	_out << mLoop;
	_out << mSoundName;
	_out << mVolume;
	_out << mFrequency;
	_out << mPitch;
	_out << mRange;
	_out << mMaxRange;
	_out.InsertEndBlock("Audio");
}

void Dystopia::AudioSource::Unserialise(TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	Component::Unserialise(_in);
	_in >> mSoundType;
	_in >> mPlayOnStart;
	_in >> mLoop;
	_in >> mSoundName;
	_in >> mVolume;
	_in >> mFrequency;
	_in >> mPitch;
	_in >> mRange;
	_in >> mMaxRange;
	_in.ConsumeEndBlock();
}

void Dystopia::AudioSource::EditorUI(void) noexcept
{
#if EDITOR
	
	auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();
	EGUI::PushLeftAlign(80);
	EGUI::PushID(1);

	EGUI::Display::EmptyBox("Audio", 150, (mpSound) ? mSoundName.c_str() : "-empty-", true);
	if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::ALL_AUDIO))
	{
		Sound *pSound = EngineCore::GetInstance()->GetSystem<SoundSystem>()->LoadSound(t->mName.c_str());
		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand(&AudioSource::SetSound, mpSound),
										   cmd->MakeFnCommand(&AudioSource::SetSound, pSound));
		EGUI::Display::EndPayloadReceiver();
	}
	EGUI::SameLine();
	if (EGUI::Display::IconCross("Clear", 8.f))
	{
		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand(&AudioSource::SetSound, mpSound),
										   cmd->MakeFnCommand(&AudioSource::SetSound, nullptr));
	}
	int old = mSoundType;
	if (EGUI::Display::DropDownSelection("Category", mSoundType, g_AudioCategory, 150))
	{
		cmd->ChangeValue(GetOwnerID(), &AudioSource::mSoundType, old, mSoundType);
	}
	if (EGUI::Display::CheckBox("On Awake", &mPlayOnStart))
	{
		cmd->ChangeValue(GetOwnerID(), &AudioSource::mPlayOnStart, !mPlayOnStart, mPlayOnStart);
	}
	if (EGUI::Display::CheckBox("Loop", &mLoop))
	{
		cmd->ChangeValue(GetOwnerID(), &AudioSource::mLoop, !mLoop, mLoop);
		mChanged = true;
	}
	switch (EGUI::Display::DragFloat("Volume", &mVolume, 0.1f, 0.f, MAX_VOLUME))
	{
	case EGUI::eSTART_DRAG:
		cmd->StartRec(&AudioSource::mVolume, this);
		break;
	case EGUI::eDRAGGING:
		break;
	case EGUI::eEND_DRAG:
	case EGUI::eENTER:
	case EGUI::eDEACTIVATED:
	case EGUI::eTABBED:
		cmd->EndRec(&AudioSource::mVolume, this);
		break;
	}
	switch (EGUI::Display::DragFloat("Frequency", &mFrequency, 0.1f, 0.f, MAX_CAP_SCALE))
	{
	case EGUI::eSTART_DRAG:
		cmd->StartRec(&AudioSource::mFrequency, this);
		break;
	case EGUI::eDRAGGING:
		break;
	case EGUI::eEND_DRAG:
	case EGUI::eENTER:
	case EGUI::eDEACTIVATED:
	case EGUI::eTABBED:
		cmd->EndRec(&AudioSource::mFrequency, this);
		break;
	}
	switch (EGUI::Display::DragFloat("Pitch", &mPitch, 0.1f, 0.f, MAX_CAP_SCALE))
	{
	case EGUI::eSTART_DRAG:
		cmd->StartRec(&AudioSource::mPitch, this);
		break;
	case EGUI::eDRAGGING:
		break;
	case EGUI::eEND_DRAG:
	case EGUI::eENTER:
	case EGUI::eDEACTIVATED:
	case EGUI::eTABBED:
		cmd->EndRec(&AudioSource::mPitch, this);
		break;
	}
	switch (EGUI::Display::DragFloat("Range", &mRange, 0.1f, 0.f, MAX_RANGE))
	{
	case EGUI::eSTART_DRAG:
		cmd->StartRec(&AudioSource::mPitch, this);
		break;
	case EGUI::eDRAGGING:
		break;
	case EGUI::eEND_DRAG:
	case EGUI::eENTER:
	case EGUI::eDEACTIVATED:
	case EGUI::eTABBED:
		cmd->EndRec(&AudioSource::mPitch, this);
		break;
	}
	switch (EGUI::Display::DragFloat("Max Range", &mMaxRange, 0.1f, 0.f, MAX_RANGE))
	{
	case EGUI::eSTART_DRAG:
		cmd->StartRec(&AudioSource::mPitch, this);
		break;
	case EGUI::eDRAGGING:
		break;
	case EGUI::eEND_DRAG:
	case EGUI::eENTER:
	case EGUI::eDEACTIVATED:
	case EGUI::eTABBED:
		cmd->EndRec(&AudioSource::mPitch, this);
		break;
	}
	EGUI::PopID();
	EGUI::PopLeftAlign();

#endif 
}

void Dystopia::AudioSource::ChangeAudio(const char * _s)
{
	if (mChannel.mpChannel)
		mChannel.mpChannel->stop();
	SetSound(EngineCore::GetInstance()->GetSystem<SoundSystem>()->LoadSound(_s));
	SetReady(false);
}

Dystopia::Sound*& Dystopia::AudioSource::GetSound(void)
{
	return mpSound;
}

void Dystopia::AudioSource::SetSound(Sound* _s)
{
	mpSound = _s;
	mSoundName = mpSound ? mpSound->mName : "";
}

Dystopia::eSoundType Dystopia::AudioSource::GetSoundType(void) const
{
	return static_cast<eSoundType>(mSoundType);
}

void Dystopia::AudioSource::SetSoundType(eSoundType _t)
{
	mSoundType = static_cast<int>(_t);
}

Dystopia::Channel& Dystopia::AudioSource::GetChannel(void)
{
	return mChannel;
}
void Dystopia::AudioSource::SetChannel(const Channel & _c)
{
	mChannel = _c;
}

void Dystopia::AudioSource::SetMinMax(float _Min, float _Max)
{
	mRange    = _Min;
	mMaxRange = _Max;
}

void Dystopia::AudioSource::ResetMinMax()
{
}

void Dystopia::AudioSource::SetFrequency(float _freq)
{
	mFrequency = _freq;
}

float Dystopia::AudioSource::GetFrequency() const
{
	return mFrequency;
}

void Dystopia::AudioSource::SetPitch(float _pitch)
{
	mPitch = _pitch;
}

float Dystopia::AudioSource::GetPitch() const
{
	return mPitch;
}

void Dystopia::AudioSource::Play(void)
{
	if (!mpSound) return;

	if (mPaused)
	{
		if (mChannel.mpChannel)
			mChannel.mpChannel->setPaused(false);
		else
			return;
	}
	else
		mReady = true;
}

void Dystopia::AudioSource::Pause(void)
{
	if (!mChannel.mpChannel || !mpSound) return;

	mChannel.mpChannel->setPaused(true);
	mPaused = true;
}

void Dystopia::AudioSource::Stop(void)
{
	if (!mChannel.mpChannel || !mpSound) return;

	mChannel.mpChannel->stop();
	mChannel.mpChannel = nullptr;
}

void Dystopia::AudioSource::SetVolume(float _f)
{
	mChanged = true;
	mVolume = Math::Clamp(_f, 0.f, MAX_VOLUME);
}

bool Dystopia::AudioSource::IsReady(void) const
{
	return mReady;
}

bool Dystopia::AudioSource::IsPaused(void) const
{
	return mPaused;
}

bool Dystopia::AudioSource::IsPlaying(void) const
{
	return mIsPlaying;
}

void Dystopia::AudioSource::SetReady(bool _b)
{
	mReady = mIsPlaying ? false : _b;
}

FMOD_VECTOR * Dystopia::AudioSource::GetFMODPos() const
{
	return mPos;
}

void Dystopia::AudioSource::UpdateFMODPos(Math::Vec3D const & _vec)
{
	mPos->x = _vec.x;
	mPos->y = _vec.y;
	mPos->z = 1.f;
	GetChannel().mpChannel->set3DAttributes(mPos, NULL, NULL);
}


bool Dystopia::AudioSource::IsALoop(void) const
{
	return mLoop;
}
