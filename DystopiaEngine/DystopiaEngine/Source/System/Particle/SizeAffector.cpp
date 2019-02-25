/* HEADER ********************************************************************************* /
/*!
\file	SizeAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
		data[0,1,2,3] for delta
		data[4,5] for min x
		data[6,7] for min y
		data[8,9] for max x
		data[10,11] for max y

		reserved[0]
			- 1 << 0 for random

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/SizeAffector.h"

#include "System/Particle/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#if EDITOR
#include "Editor/EGUI.h"
#endif 

Dystopia::InitialSizeAffector::InitialSizeAffector(void)
	: ParticleAffector{ &InitialSizeAffector::AffectorSpawn }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
}

Dystopia::InitialSizeAffector::~InitialSizeAffector(void)
{}

void Dystopia::InitialSizeAffector::SetMinX(float _val)
{
	*reinterpret_cast<float*>(data) = _val;
}

void Dystopia::InitialSizeAffector::SetMinY(float _val)
{
	*reinterpret_cast<float*>(data + 4) = _val;
}

void Dystopia::InitialSizeAffector::SetMaxX(float _val)
{
	*reinterpret_cast<float*>(data + 8) = _val;
}

void Dystopia::InitialSizeAffector::SetMaxY(float _val)
{
	*reinterpret_cast<float*>(data + 12) = _val;
}

float Dystopia::InitialSizeAffector::GetMinX(void) const
{
	return *reinterpret_cast<const float*>(data);
}

float Dystopia::InitialSizeAffector::GetMinY(void) const
{
	return *reinterpret_cast<const float*>(data + 4);
}

float Dystopia::InitialSizeAffector::GetMaxX(void) const
{
	return *reinterpret_cast<const float*>(data + 8);
}

float Dystopia::InitialSizeAffector::GetMaxY(void) const
{
	return *reinterpret_cast<const float*>(data + 12);
}

void Dystopia::InitialSizeAffector::ToggleRandom(void)
{
	reserved[0] ^= (1 << 0);
}

bool Dystopia::InitialSizeAffector::IsRandom(void) const
{
	return reserved[0] & (1 << 0);
}

void Dystopia::InitialSizeAffector::AffectorSpawn(Emitter& _emitter, float)
{
	if (IsRandom())
	{
		std::random_device rDev;
		std::mt19937 gen{ rDev() };
		std::uniform_real_distribution<float> distrX{ GetMinX(), GetMaxX() };
		std::uniform_real_distribution<float> distrY{ GetMinY(), GetMaxY() };
		_emitter.GetSpawnDefaults().mSize = Math::Vec2{ distrX(gen), distrY(gen) };
	}
	else
		_emitter.GetSpawnDefaults().mSize = Math::Vec2{ GetMinX(), GetMinY() };
}

const char * Dystopia::InitialSizeAffector::EditorDisplayLabel(void) const
{
	return "Initial Size";
}

void Dystopia::InitialSizeAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);
	EGUI::PushID(53);

	bool rand = IsRandom();
	if (EGUI::Display::CheckBox("Random Size", &rand))
		ToggleRandom();

	Math::Vec2 min{ GetMinX(), GetMinY()};

	auto result = EGUI::Display::VectorFields("Min Size", &min, 0.1f, 0.f, USHRT_MAX);
	switch (result[0])
	{
		case EGUI::eDragStatus::eDRAGGING:
		case EGUI::eDragStatus::eSTART_DRAG:
		case EGUI::eDragStatus::eDEACTIVATED:
			SetMinX(min.x);
			if (GetMaxX() < GetMinX())
				SetMaxX(GetMinX());
			break;
		default:
			break;
	}
	switch (result[1])
	{
		case EGUI::eDragStatus::eDRAGGING:
		case EGUI::eDragStatus::eSTART_DRAG:
		case EGUI::eDragStatus::eDEACTIVATED:
			SetMinY(min.y);
			if (GetMaxY() < GetMinY())
				SetMaxY(GetMinY());
			break;
		default:
			break;
	}

	if (IsRandom())
	{
		Math::Vec2 max{ GetMaxX(), GetMaxY() };
		auto res2 = EGUI::Display::VectorFields("Max Size", &max, 0.1f, 0.f, USHRT_MAX);
		switch (res2[0])
		{
		case EGUI::eDragStatus::eDRAGGING:
		case EGUI::eDragStatus::eSTART_DRAG:
		case EGUI::eDragStatus::eDEACTIVATED:
			SetMaxX(max.x);
			if (GetMinX() > GetMaxX())
				SetMinX(GetMaxX());
			break;
		default:
			break;
		}
		switch (res2[1])
		{
		case EGUI::eDragStatus::eDRAGGING:
		case EGUI::eDragStatus::eSTART_DRAG:
		case EGUI::eDragStatus::eDEACTIVATED:
			SetMaxY(max.y);
			if (GetMinY() > GetMaxY())
				SetMinY(GetMaxY());
			break;
		default:
			break;
		}
	}

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif 
}

/********************************************************************************************************************/

Dystopia::SizeOverLifeAffector::SizeOverLifeAffector(void)
	: ParticleAffector{ &SizeOverLifeAffector::AffectorUpdate }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
}

Dystopia::SizeOverLifeAffector::~SizeOverLifeAffector(void)
{}

void Dystopia::SizeOverLifeAffector::SetDeltaX(float _f)
{
	*reinterpret_cast<float*>(data) = _f;
}

void Dystopia::SizeOverLifeAffector::SetDeltaY(float _f)
{
	*reinterpret_cast<float*>(data + 4) = _f;
}

float Dystopia::SizeOverLifeAffector::GetDeltaX(void) const
{
	return *reinterpret_cast<const float*>(data);
}

float Dystopia::SizeOverLifeAffector::GetDeltaY(void) const
{
	return *reinterpret_cast<const float*>(data + 4);
}

void Dystopia::SizeOverLifeAffector::AffectorUpdate(Emitter& _emitter, float _dt)
{
	auto& allSz = _emitter.GetSize();
	for (auto& p : allSz)
	{
		p = Math::Max(p + Math::Vec2{ GetDeltaX() * _dt, GetDeltaY() * _dt }, Math::Vec2{ 0.f });
	}
}

const char * Dystopia::SizeOverLifeAffector::EditorDisplayLabel(void) const
{
	return "Size Over Life";
}

void Dystopia::SizeOverLifeAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);
	EGUI::PushID(58);

	Math::Vec2 del{ GetDeltaX(), GetDeltaY() };
	auto result = EGUI::Display::VectorFields("Change per (s)", &del, 0.1f, -FLT_MAX, FLT_MAX);
	switch (result[0])
	{
	case EGUI::eDragStatus::eDRAGGING:
	case EGUI::eDragStatus::eSTART_DRAG:
	case EGUI::eDragStatus::eDEACTIVATED:
		SetDeltaX(del.x);
		break;
	default:
		break;
	}
	switch (result[1])
	{
	case EGUI::eDragStatus::eDRAGGING:
	case EGUI::eDragStatus::eSTART_DRAG:
	case EGUI::eDragStatus::eDEACTIVATED:
		SetDeltaY(del.y);
		break;
	default:
		break;
	}

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif 
}


