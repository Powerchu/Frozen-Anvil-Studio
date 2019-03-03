/* HEADER ********************************************************************************* /
/*!
\file	ColorAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
		data[0,1] for red 1
		data[2,3] for green 1
		data[4,5] for blue 1
		data[6,7] for alpha 1
		data[8,9] for red 2
		data[10,11] for green 2
		data[12,13] for blue 2
		data[14,15] for alpha 2

		reserved[0] flags:
			- 1 << 0 random initial color

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/ColorAffector.h"

#include "System/Particle/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#if EDITOR
#include "Editor/EGUI.h"
#endif 

Dystopia::InitialColorAffector::InitialColorAffector(void)
	: ParticleAffector{ &InitialColorAffector::AffectorSpawn }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
	SetR1(255);
	SetG1(255);
	SetB1(255);
	SetA1(255);
	SetR2(255);
	SetG2(255);
	SetB2(255);
	SetA2(255);
}

Dystopia::InitialColorAffector::~InitialColorAffector(void)
{}


void Dystopia::InitialColorAffector::SetR1(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data) = Math::Clamp(_val, 0, 255);
}

void Dystopia::InitialColorAffector::SetG1(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 2) = Math::Clamp(_val, 0, 255);
}

void Dystopia::InitialColorAffector::SetB1(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 4) = Math::Clamp(_val, 0, 255);
}

void Dystopia::InitialColorAffector::SetA1(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 6) = Math::Clamp(_val, 0, 255);
}

void Dystopia::InitialColorAffector::SetR2(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 8) = Math::Clamp(_val, 0, 255);
}

void Dystopia::InitialColorAffector::SetG2(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 10) = Math::Clamp(_val, 0, 255);
}

void Dystopia::InitialColorAffector::SetB2(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 12) = Math::Clamp(_val, 0, 255);
}

void Dystopia::InitialColorAffector::SetA2(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 14) = Math::Clamp(_val, 0, 255);
}

unsigned int Dystopia::InitialColorAffector::GetR1(void) const
{
	return static_cast<unsigned int>(*reinterpret_cast<const unsigned short*>(data));
}

unsigned int Dystopia::InitialColorAffector::GetG1(void) const
{
	return static_cast<unsigned int>(*reinterpret_cast<const unsigned short*>(data + 2));
}

unsigned int Dystopia::InitialColorAffector::GetB1(void) const
{
	return static_cast<unsigned int>(*reinterpret_cast<const unsigned short*>(data + 4));
}

unsigned int Dystopia::InitialColorAffector::GetA1(void) const
{
	return static_cast<unsigned int>(*reinterpret_cast<const unsigned short*>(data + 6));
}

unsigned int Dystopia::InitialColorAffector::GetR2(void) const
{
	return static_cast<unsigned int>(*reinterpret_cast<const unsigned short*>(data + 8));
}

unsigned int Dystopia::InitialColorAffector::GetG2(void) const
{
	return static_cast<unsigned int>(*reinterpret_cast<const unsigned short*>(data + 10));
}

unsigned int Dystopia::InitialColorAffector::GetB2(void) const
{
	return static_cast<unsigned int>(*reinterpret_cast<const unsigned short*>(data + 12));
}

unsigned int Dystopia::InitialColorAffector::GetA2(void) const
{
	return static_cast<unsigned int>(*reinterpret_cast<const unsigned short*>(data + 14));
}

void Dystopia::InitialColorAffector::EnableRandom(bool _enable)
{
	if (_enable)
		reserved[0] |= (1 << 0);
	else
		reserved[0] &= ~(1 << 0);
}

void Dystopia::InitialColorAffector::AffectorSpawn(Emitter& _emitter, float)
{
	auto& p = _emitter.GetSpawnDefaults();
	if (reserved[0] & (1 << 0))
	{
		std::random_device rDev;
		std::mt19937 gen{ rDev() };
		std::uniform_int_distribution<unsigned int> distRed{ GetR1(), GetR2() };
		std::uniform_int_distribution<unsigned int> distGreen{ GetG1(), GetG2() };
		std::uniform_int_distribution<unsigned int> distBlue{ GetB1(), GetB2() };
		std::uniform_int_distribution<unsigned int> distAlpha{ GetA1(), GetA2() };

		p.mColour = Math::Vec4{ Math::Clamp(static_cast<float>(distRed(gen)) / 255.f, 0.f, 1.f),
								Math::Clamp(static_cast<float>(distGreen(gen)) / 255.f, 0.f, 1.f),
								Math::Clamp(static_cast<float>(distBlue(gen)) / 255.f, 0.f, 1.f),
								Math::Clamp(static_cast<float>(distAlpha(gen)) / 255.f, 0.f, 1.f), };
	}
	else
	{
		p.mColour = Math::Vec4{ Math::Clamp(static_cast<float>(GetR1()) / 255.f, 0.f, 1.f),
							    Math::Clamp(static_cast<float>(GetG1()) / 255.f, 0.f, 1.f), 
							    Math::Clamp(static_cast<float>(GetB1()) / 255.f, 0.f, 1.f), 
							    Math::Clamp(static_cast<float>(GetA1()) / 255.f, 0.f, 1.f), };
	}
}

const char * Dystopia::InitialColorAffector::EditorDisplayLabel(void) const
{
	return "Initial Color";
}

void Dystopia::InitialColorAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);

	EGUI::PushID(55);

	bool rand = reserved[0] & (1 << 0);
	EGUI::Display::CheckBox("Random Col", &rand);
		EnableRandom(rand);

	if (EGUI::Display::StartTreeNode("Color 1"))
	{
		int out = static_cast<int>(GetR1());
		if (EGUI::Display::DragInt("Red", &out, 1.f, 0, 255))
		{
			SetR1(static_cast<unsigned short>(out));
			SetR2(static_cast<unsigned short>(Math::Max(GetR2(), GetR1())));
		}

		out = static_cast<int>(GetG1());
		if (EGUI::Display::DragInt("Green", &out, 1.f, 0, 255))
		{
			SetG1(static_cast<unsigned short>(out));
			SetG2(static_cast<unsigned short>(Math::Max(GetG2(), GetG1())));
		}

		out = static_cast<int>(GetB1());
		if (EGUI::Display::DragInt("Blue", &out, 1.f, 0, 255))
		{
			SetB1(static_cast<unsigned short>(out));
			SetB2(static_cast<unsigned short>(Math::Max(GetB2(), GetB1())));

		}

		out = static_cast<int>(GetA1());
		if (EGUI::Display::DragInt("Alpha", &out, 1.f, 0, 255))
		{
			SetA1(static_cast<unsigned short>(out));
			SetA2(static_cast<unsigned short>(Math::Max(GetA2(), GetA1())));
		}

		EGUI::Display::EndTreeNode();
	}
	EGUI::PopID();

	if (reserved[0] & (1 << 0))
	{
		EGUI::PushID(56);
		if (EGUI::Display::StartTreeNode("Color 2"))
		{
			int out = static_cast<int>(GetR2());
			if (EGUI::Display::DragInt("Red", &out, 1.f, 0, 255))
			{
				SetR2(static_cast<unsigned short>(out));
				SetR1(static_cast<unsigned short>(Math::Min(GetR1(), GetR2())));
			}
			out = static_cast<int>(GetG2());
			if (EGUI::Display::DragInt("Green", &out, 1.f, 0, 255))
			{
				SetG2(static_cast<unsigned short>(out));
				SetG1(static_cast<unsigned short>(Math::Min(GetG1(), GetG2())));
			}
			out = static_cast<int>(GetB2());
			if (EGUI::Display::DragInt("Blue", &out, 1.f, 0, 255))
			{
				SetB2(static_cast<unsigned short>(out));
				SetB1(static_cast<unsigned short>(Math::Min(GetB1(), GetB2())));
			}
			out = static_cast<int>(GetA2());
			if (EGUI::Display::DragInt("Alpha", &out, 1.f, 0, 255))
			{
				SetA2(static_cast<unsigned short>(out));
				SetA1(static_cast<unsigned short>(Math::Min(GetA1(), GetA2())));
			}
			EGUI::Display::EndTreeNode();
		}
		EGUI::PopID();
	}


	EGUI::PopLeftAlign();
#endif 
}

/********************************************************************************************************************/

Dystopia::ColorOverLifeAffector::ColorOverLifeAffector(void)
	: ParticleAffector{ &ColorOverLifeAffector::AffectorUpdate }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
}

Dystopia::ColorOverLifeAffector::~ColorOverLifeAffector(void)
{}

void Dystopia::ColorOverLifeAffector::SetTargetR(unsigned short _n)
{
	*reinterpret_cast<unsigned short*>(data) = _n;
}

void Dystopia::ColorOverLifeAffector::SetTargetG(unsigned short  _n)
{
	*reinterpret_cast<unsigned short*>(data + 2) = _n;
}

void Dystopia::ColorOverLifeAffector::SetTargetB(unsigned short  _n)
{
	*reinterpret_cast<unsigned short*>(data + 4) = _n;
}

void Dystopia::ColorOverLifeAffector::SetTargetA(unsigned short  _n)
{
	*reinterpret_cast<unsigned short*>(data + 6) = _n;
}

void Dystopia::ColorOverLifeAffector::SetTargetLife(float _f)
{
	*reinterpret_cast<float*>(data + 8) = _f;
}

unsigned short Dystopia::ColorOverLifeAffector::GetTargetR(void) const
{
	return *reinterpret_cast<const unsigned short*>(data);
}

unsigned short Dystopia::ColorOverLifeAffector::GetTargetG(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 2);
}

unsigned short Dystopia::ColorOverLifeAffector::GetTargetB(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 4);
}

unsigned short Dystopia::ColorOverLifeAffector::GetTargetA(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 6);
}

float Dystopia::ColorOverLifeAffector::GetTargetLife(void) const
{
	return *reinterpret_cast<const float*>(data + 8);
}

void Dystopia::ColorOverLifeAffector::AffectorUpdate(Emitter& _emitter, float _dt)
{
	auto& allInitialLife = _emitter.GetInitialLifetime();
	auto& allLife = _emitter.GetLifetime();
	auto& allCol = _emitter.GetColour();

	Math::Vec4 targetCol = Math::Vec4( GetTargetR(), GetTargetG(), GetTargetB(), GetTargetA() );

	targetCol /= 255.f;

	for (unsigned i = 0; i < allCol.size(); ++i)
	{
		float alphaLife = Math::Clamp(allLife[i] / allInitialLife[i], 0.f, 1.f);
		float ratioLeft = alphaLife - GetTargetLife();
		if (ratioLeft < Math::epsilon)
		{
			allCol[i] = targetCol;
		}
		else// if (ratioLeft > 0.f)
		{
			Math::Vec4 delta = (targetCol - allCol[i]) * _dt * ratioLeft;

			allCol[i] += delta;
			//float r = ratioLeft * _dt;

			//float change = r * ((GetTargetR() / 255.f) - allCol[i].x);
			//allCol[i].x = allCol[i].x + change;
			//
			//change = r * ((GetTargetG() / 255.f) - allCol[i].y);
			//allCol[i].y = allCol[i].y + change;
			//
			//change = r * ((GetTargetB() / 255.f) - allCol[i].z);
			//allCol[i].z = allCol[i].z + change;
			//
			//change = r * ((GetTargetA() / 255.f) - allCol[i].w);
			//allCol[i].w = allCol[i].w + change;
		}
	}
}

const char * Dystopia::ColorOverLifeAffector::EditorDisplayLabel(void) const
{
	return "Color Over Life";
}

void Dystopia::ColorOverLifeAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(80.f);
	EGUI::PushID(57);

	int targetR = static_cast<int>(GetTargetR());
	int targetG = static_cast<int>(GetTargetG());
	int targetB = static_cast<int>(GetTargetB());
	int targetA = static_cast<int>(GetTargetA());
	float targetL = GetTargetLife();

	if (EGUI::Display::DragInt("Red", &targetR, 1.f, 0, 255))
		SetTargetR(static_cast<unsigned short>(targetR));

	if (EGUI::Display::DragInt("Green", &targetG, 1.f, 0, 255))
		SetTargetG(static_cast<unsigned short>(targetG));

	if (EGUI::Display::DragInt("Blue", &targetB, 1.f, 0, 255))
		SetTargetB(static_cast<unsigned short>(targetB));

	if (EGUI::Display::DragInt("Alpha", &targetA, 0.1f, 0, 255))
		SetTargetA(static_cast<unsigned short>(targetA));

	if (EGUI::Display::DragFloat("Val", &targetL, 0.1f, 0.f, 1.f))
		SetTargetLife(targetL);

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif 
}




