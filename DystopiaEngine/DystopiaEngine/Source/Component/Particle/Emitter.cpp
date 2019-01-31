/* HEADER *********************************************************************************/
/*!
\file	Emitter.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Particle Emitter

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/Emitter.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"

#include "System/Driver/Driver.h"
#include "System/Particle/ParticleAffector.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Particle/SpawnAffector.h"

#include "Math/Vectors.h"
#include "Math/Matrix4.h"
#include "Utility/DebugAssert.h"
#include "Utility/Meta.h"
#include "Utility/MetaAlgorithms.h"

#include "Editor/RuntimeMeta.h"

#include <GL/glew.h>

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#endif 


Dystopia::Emitter::Emitter(void) noexcept
	: mColour{}, mPosition{}, mVelocity{}, mAccel{}, mLifetime{}, mSpawnCount{},
	mSpawn{}, mUpdate{}, mFixedUpdate{}, mpShader{ nullptr }, mpTexture{ nullptr },
	mInitialLife{}, mnParticleLimit{}
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(2, &mColourBuffer);

	glBindVertexArray(mVAO);

	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);

	//glVertexAttribDivisor(0, 4);
	//glVertexAttribDivisor(1, 4);

	glBindVertexArray(0);
}

Dystopia::Emitter::Emitter(Dystopia::Emitter const& _rhs) noexcept
	: mColour{ _rhs.mColour }, mPosition{ _rhs.mPosition }, mVelocity{ _rhs.mVelocity }, mAccel{ _rhs.mAccel }, 
	mLifetime{ _rhs.mLifetime }, mSpawnCount{ _rhs.mSpawnCount },
	mSpawn{ _rhs.mSpawn }, mUpdate{ _rhs.mUpdate }, mFixedUpdate{ _rhs.mFixedUpdate }, 
	mpShader{ _rhs.mpShader }, mpTexture{ _rhs.mpTexture },
	mInitialLife{ _rhs.mInitialLife }
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(2, &mColourBuffer);

	glBindVertexArray(mVAO);

	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);

	//glVertexAttribDivisor(0, 4);
	//glVertexAttribDivisor(1, 4);

	glBindVertexArray(0);
}

Dystopia::Emitter::~Emitter(void) noexcept
{
	Bind();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteBuffers(2, &mColourBuffer);
	glDeleteVertexArrays(1, &mVAO);

	Unbind();
}

void Dystopia::Emitter::Awake(void)
{
	if (!mpShader)
	{
		mpShader = CORE::Get<ShaderSystem>()->GetShader("Default Particle");
	}
	if (!mpTexture)
	{
		mpTexture = CORE::Get<TextureSystem>()->GetTexture("EditorStartup.png");
	}
}

void Dystopia::Emitter::Init(void)
{
	mColour  .clear();
	mPosition.clear();
	mVelocity.clear();
	mAccel   .clear();
	mLifetime.clear();

	mColour  .reserve(mParticle.mnLimit);
	mPosition.reserve(mParticle.mnLimit);
	mVelocity.reserve(mParticle.mnLimit);
	mAccel   .reserve(mParticle.mnLimit);
	mLifetime.reserve(mParticle.mnLimit);
	mInitialLife.reserve(mParticle.mnLimit);

	if (!mpShader)
	{
		mpShader = CORE::Get<ShaderSystem>()->GetShader("Default Particle");
	}
	if (!mpTexture)
	{
		mpTexture = CORE::Get<TextureSystem>()->GetTexture("EditorStartup.png");
	}

	mPosition.EmplaceBackUnsafe(-50.f, 50.f, 0.f);
	mPosition.EmplaceBackUnsafe(-50.f, -50.f, 0.f);
	mPosition.EmplaceBackUnsafe(50.f, 50.f, 0.f);

	Bind();
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, mParticle.mnLimit * sizeof(decltype(mColour)::Val_t), nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glBufferData(GL_ARRAY_BUFFER, mParticle.mnLimit * sizeof(decltype(mPosition)::Val_t), &mPosition[0], GL_STREAM_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//glVertexAttribDivisor(0, 4);
	//glVertexAttribDivisor(1, 4);

	Unbind();
}

void Dystopia::Emitter::FixedUpdate(float _fDT)
{
#if EDITOR
	mParticle.mnLimit = static_cast<size_t>(mnParticleLimit); // incase editor commands externally changes it 
#endif 

	auto pVel   = mVelocity.begin();
	auto pAccel = mAccel   .begin();

	for (auto& e : mPosition)
	{
		*pVel +=  *pAccel     * _fDT;
		 e    += (*pVel).xyz0 * _fDT;

		 ++pVel; ++pAccel;
	}
}

void Dystopia::Emitter::Bind(void) const noexcept
{
	glPointSize(100.f);
	mpShader->Bind();
	glBindVertexArray(mVAO);
	mpTexture->Bind();
}

void Dystopia::Emitter::Unbind(void) const noexcept
{
	mpTexture->Unbind();
	glBindVertexArray(0);
	mpShader->Unbind();
}

void Dystopia::Emitter::UploadBuffers(void) const noexcept
{
	glBindBuffer(GL_ARRAY_BUFFER, mColourBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mColour.size() * sizeof(Math::Vec4), mColour.begin());

	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mPosition.size() * sizeof(Math::Vec4), mPosition.begin());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Dystopia::Emitter::Render(void) const noexcept
{
	glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(mPosition.size()));

	if (auto err = glGetError())
		__debugbreak();
}

void Dystopia::Emitter::KillParticle(unsigned _nIdx) noexcept
{
	DEBUG_ASSERT(!mSpawnCount, "Particle System Error: No particles to kill!");
	--mSpawnCount;

	mLifetime.FastRemove(_nIdx);
	mColour  .FastRemove(_nIdx);
	mAccel   .FastRemove(_nIdx);
	mVelocity.FastRemove(_nIdx);
	mPosition.FastRemove(_nIdx);
	mInitialLife.FastRemove(_nIdx);
}

void Dystopia::Emitter::SpawnParticle(void) noexcept
{
	if (mParticle.mnLimit > mSpawnCount)
	{
		++mSpawnCount;

		for (auto& e : mSpawn)
			e.Update(*this, 0);

		auto transform = GetOwner()->GetComponent<Transform>();
		auto pos = transform->GetTransformMatrix() * mParticle.mPos.xyz1;
		pos.w    = mParticle.mfSize;

		mParticle.mfLifeDur = mParticle.mfInitialLife; // to always start at initial (lifeDur = counter)

		mLifetime.EmplaceBackUnsafe(mParticle.mfLifeDur);
		mColour  .EmplaceBackUnsafe(mParticle.mColour  );
		mAccel   .EmplaceBackUnsafe(                   );
		mVelocity.EmplaceBackUnsafe(mParticle.mVelocity);
		mPosition.EmplaceBackUnsafe(pos                );
		mInitialLife.EmplaceBackUnsafe(mParticle.mfInitialLife);
	}

}

Dystopia::Shader& Dystopia::Emitter::GetShader(void) noexcept
{
	return *mpShader;
}

Dystopia::GfxParticle& Dystopia::Emitter::GetSpawnDefaults(void) noexcept
{
	return mParticle;
}

AutoArray<float>& Dystopia::Emitter::GetInitialLifetime(void) noexcept
{
	return mInitialLife;
}

AutoArray<Math::Vec4>& Dystopia::Emitter::GetColour(void) noexcept
{
	return mColour;
}

AutoArray<Math::Vec4>& Dystopia::Emitter::GetPosition(void) noexcept
{
	return mPosition;
}

AutoArray<Math::Vec3>& Dystopia::Emitter::GetVelocity(void) noexcept
{
	return mVelocity;
}

AutoArray<Math::Vec3>& Dystopia::Emitter::GetAcceleration(void) noexcept
{
	return mAccel;
}

AutoArray<float>& Dystopia::Emitter::GetLifetime(void) noexcept
{
	return mLifetime;
}

AutoArray<Dystopia::ParticleAffector>& Dystopia::Emitter::GetSpawnAffectors(void) noexcept
{
	return mSpawn;
}

AutoArray<Dystopia::ParticleAffector>& Dystopia::Emitter::GetUpdateAffectors(void) noexcept
{
	return mUpdate;
}

AutoArray<Dystopia::ParticleAffector>& Dystopia::Emitter::GetFixedUpdateAffectors(void) noexcept
{
	return mFixedUpdate;
}

Dystopia::Emitter * Dystopia::Emitter::Duplicate(void) const
{
	return static_cast<ComponentDonor<Emitter> *>(EngineCore::GetInstance()->GetSystem<Emitter::SYSTEM>())->RequestComponent(*this);
}


void Dystopia::Emitter::Serialise(TextSerialiser& _out) const
{
	_out.InsertStartBlock("Emitter");
	Component::Serialise(_out);
	if (mpShader)
		_out << mpShader->GetName();
	else
		_out << "Default Particle";
	if (mpTexture)
		_out << mpTexture->GetName();
	else
		_out << "EditorStartup.png";
	_out << mnParticleLimit;
	_out.InsertEndBlock("Emitter");




	_out.InsertStartBlock("Spawn affectors");
	_out << mSpawn.size();
	for (auto& e : mSpawn)
		_out << e.GetID();
	_out.InsertEndBlock("Spawn affectors");
	for (auto& e : mSpawn)
		e.Serialise(_out);




	_out.InsertStartBlock("Update affectors");
	_out << mUpdate.size();
	for (auto& e : mUpdate)
		_out << e.GetID();
	_out.InsertEndBlock("Update affectors");
	for (auto& e : mUpdate)
		e.Serialise(_out);




	_out.InsertStartBlock("Fixed Update affectors");
	_out << mFixedUpdate.size();
	for (auto& e : mFixedUpdate)
		_out << e.GetID();
	_out.InsertEndBlock("Fixed Update affectors");
	for (auto& e : mFixedUpdate)
		e.Serialise(_out);
}

void Dystopia::Emitter::Unserialise(TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	Component::Unserialise(_in);

	std::string buf;
	buf.reserve(128);

	_in >> buf;
	mpShader = CORE::Get<ShaderSystem>()->GetShader(buf.c_str());
	_in >> buf;
	mpTexture = CORE::Get<TextureSystem>()->GetTexture(buf.c_str());
	int n = 0;
	_in >> n;
	mnParticleLimit = n;
	mParticle.mnLimit = static_cast<size_t>(mnParticleLimit);
	_in.ConsumeEndBlock();

	static AffectorGet affectorsList;
	unsigned id = 0;
	size_t size = 0;




	_in.ConsumeStartBlock();
	_in >> size;
	mSpawn.clear();
	for (size_t i = 0; i < size; i++)
	{
		_in >> id;
		affectorsList.Get(id, *this);
	}
	_in.ConsumeEndBlock();
	for (auto& elem : mSpawn)
		elem.UnSerialise(_in);




	_in.ConsumeStartBlock();
	_in >> size;
	mUpdate.clear();
	for (size_t i = 0; i < size; i++)
	{
		_in >> id;
		affectorsList.Get(id, *this);
	}
	_in.ConsumeEndBlock();
	for (auto& elem : mUpdate)
		elem.UnSerialise(_in);




	_in.ConsumeStartBlock();
	_in >> size;
	mFixedUpdate.clear();
	for (size_t i = 0; i < size; i++)
	{
		_in >> id;
		affectorsList.Get(id, *this);
	}
	_in.ConsumeEndBlock();
	for (auto& elem : mFixedUpdate)
		elem.UnSerialise(_in);

}

void Dystopia::Emitter::EditorUI(void) noexcept
{
#if EDITOR
	static const auto affectorNames = Dystopia::AffectorUI<Dystopia::AffectorList>::GetUIName();

	auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();
	switch (EGUI::Display::DragInt("Maximum Particle Count", &mnParticleLimit, 1.f, 0, INT_MAX))
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		cmd->StartRec(&Emitter::mnParticleLimit, this);
		break;
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eENTER:
		cmd->EndRec(&Emitter::mnParticleLimit, this);
		break;
	}

	EGUI::Display::Label("Particle Count: %u", mSpawnCount);
	EGUI::Display::HorizontalSeparator();
	if (EGUI::Display::StartTreeNode("Spawn Affectors"))
	{
		for (int i = 0; i < mSpawn.size(); ++i)
		{
			EGUI::PushID(static_cast<int>(i));
			EGUI::Indent(30.f);
			EGUI::Display::Label((mSpawn[i].*affectorNames[mSpawn[i].GetID()])());
			EGUI::UnIndent();
			EGUI::PopID();
		}
		EGUI::Display::EndTreeNode();
	}
	EGUI::Display::HorizontalSeparator();
	if (EGUI::Display::StartTreeNode("Update Affectors"))
	{
		for (int i = 0; i < mUpdate.size(); ++i)
		{
			EGUI::PushID(static_cast<int>(i));
			EGUI::Indent(30.f);
			EGUI::Display::Label((mUpdate[i].*affectorNames[mUpdate[i].GetID()])());
			EGUI::UnIndent();
			EGUI::PopID();
		}
		EGUI::Display::EndTreeNode();
	}
	EGUI::Display::HorizontalSeparator();
	if (EGUI::Display::StartTreeNode("Fixed Update Affectors"))
	{
		for (int i = 0; i < mFixedUpdate.size(); ++i)
		{
			EGUI::PushID(static_cast<int>(i));
			EGUI::Indent(30.f);
			EGUI::Display::Label((mFixedUpdate[i].*affectorNames[mFixedUpdate[i].GetID()])());
			EGUI::UnIndent();
			EGUI::PopID();
		}
		EGUI::Display::EndTreeNode();
	}
#endif 
}


