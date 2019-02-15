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
#include "System/Particle/Emitter.h"

#include "System/Driver/Driver.h"
#include "System/Particle/ParticleAffector.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Particle/SpawnAffector.h"
#include "System/Time/ScopedTimer.h"
#include "System/Profiler/ProfilerAction.h"

#include "Math/Vectors.h"
#include "Math/Matrix4.h"
#include "Utility/Meta.h"
#include "Utility/MetaAlgorithms.h"
#include "Utility/DebugAssert.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"

#include "Editor/RuntimeMeta.h"

#include <GL/glew.h>

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#include "Editor/Payloads.h"
#endif 


Dystopia::Emitter::Emitter(ParticleEmitter* _owner) noexcept
	: mColour{}, mPosition{}, mVelocity{}, mAccel{}, mLifetime{}, mSpawnCount{},
	mSpawn{}, mUpdate{}, mFixedUpdate{}, mpShader{ nullptr }, mpTexture{ nullptr },
	mInitialLife{}, mbUpdatedPositions{ false }, mTextureName{ "EditorStartup.png" }, 
	mShaderName{ "Default Particle" }, mnParticleLimit{ 1000 }, mbIsAlive{ true },
	mpOwner{ _owner }, mpTransform{ nullptr }
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(2, &mColourBuffer);
}

Dystopia::Emitter::Emitter(Dystopia::Emitter const& _rhs) noexcept
	: mColour{ _rhs.mColour }, mPosition{ _rhs.mPosition }, mVelocity{ _rhs.mVelocity }, mAccel{ _rhs.mAccel },
	mLifetime{ _rhs.mLifetime }, mSpawnCount{ _rhs.mSpawnCount },
	mSpawn{ _rhs.mSpawn }, mUpdate{ _rhs.mUpdate }, mFixedUpdate{ _rhs.mFixedUpdate },
	mpShader{ _rhs.mpShader }, mpTexture{ _rhs.mpTexture },
	mInitialLife{ _rhs.mInitialLife }, mTextureName{ _rhs.mTextureName }, 
	mShaderName{ _rhs.mTextureName }, mnParticleLimit{ _rhs.mnParticleLimit }, mbIsAlive{ _rhs.mbIsAlive },
	mpOwner{ nullptr }, mpTransform{ nullptr }
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(2, &mColourBuffer);
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
		mpShader = CORE::Get<ShaderSystem>()->GetShader(mShaderName.c_str());
	}

	if (!mpTexture)
	{
		mpTexture = CORE::Get<TextureSystem>()->GetTexture(mTextureName.c_str());
	}
}

void Dystopia::Emitter::Init(void)
{
#if EDITOR
	mParticle.mnLimit = mnParticleLimit;
#endif

	mbUpdatedPositions = false;

	mColour     .clear();
	mPosition   .clear();
	mVelocity   .clear();
	mAccel      .clear();
	mLifetime   .clear();
	mInitialLife.clear();

	mColour     .reserve(mParticle.mnLimit);
	mPosition   .reserve(mParticle.mnLimit);
	mVelocity   .reserve(mParticle.mnLimit);
	mAccel      .reserve(mParticle.mnLimit);
	mLifetime   .reserve(mParticle.mnLimit);
	mInitialLife.reserve(mParticle.mnLimit);

	if (!mpShader)
	{
		mpShader = CORE::Get<ShaderSystem>()->GetShader(mShaderName.c_str());
	}
	if (!mpTexture)
	{
		mpTexture = CORE::Get<TextureSystem>()->LoadTexture(CORE::Get<FileSystem>()->FindFilePath(mTextureName.c_str(), eFileDir::eResource).c_str());
	}
#   if EDITOR
	if (!mpOwner)
		__debugbreak();
#   endif
	mpTransform = mpOwner->GetOwner()->GetComponent<Transform>();

	Bind();

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glBufferData(GL_ARRAY_BUFFER, mParticle.mnLimit * sizeof(decltype(mPosition)::Val_t), nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(0, 1);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, mParticle.mnLimit * sizeof(decltype(mColour)::Val_t), nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(1, 1);

#   if defined(DEBUG) | defined(_DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#    endif

	Unbind();
}

void Dystopia::Emitter::FixedUpdate(float _fDT)
{
#if defined(_OPENMP)
	long long const l = mPosition.size();

#	pragma omp parallel for
	for (long long n = 0; n < l; ++n)
	{
		mVelocity[n] += mAccel[n] * _fDT;
	}

#	pragma omp parallel for
	for (long long n = 0; n < l; ++n)
	{
		mPosition[n] += mVelocity[n].xyz0 * _fDT;
	}
#else
	auto pAcc = mAccel.begin();
	auto pVel = mVelocity.begin();

	for (auto& e : mVelocity)
	{
		e += *pAcc * _fDT;
		++pAcc;
	}

	for (auto& e : mPosition)
	{
		e += (*pVel).xyz0 * _fDT;
		++pVel;
	}
#endif

	mbUpdatedPositions = true;
}

void Dystopia::Emitter::Bind(void) const noexcept
{
	glBindVertexArray(mVAO);
	mpTexture->Bind();
}

void Dystopia::Emitter::Unbind(void) const noexcept
{
	mpTexture->Unbind();
	glBindVertexArray(0);
}

namespace
{
	template <typename T, typename U>
	inline void UploadBufferAux(T& buf, U& arr) noexcept
	{
		using val_t = Ut::RemoveRef_t<decltype(arr[0])>;

		glBindBuffer(GL_ARRAY_BUFFER, buf);
		auto MapPtr = glMapBufferRange(GL_ARRAY_BUFFER, 0, arr.size() * sizeof(val_t), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

#if defined(_OPENMP)
		long long const lim = arr.size();
#	pragma omp parallel for
		for (long long n = 0; n < lim; ++n)
			static_cast<val_t*>(MapPtr)[n] = *(arr.begin() + n);
#else
		std::memcpy(MapPtr, arr.begin(), arr.size() * sizeof(val_t));
#endif

		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
}

void Dystopia::Emitter::UploadPositionBuffer(void) const noexcept
{
	ScopedTimer<ProfilerAction> timeKeeper{ "Emitter", "Position Upload" };

	if (!mbUpdatedPositions)
		return;

	UploadBufferAux(mPosBuffer, mPosition);
	const_cast<bool&>(mbUpdatedPositions) = false;
}

void Dystopia::Emitter::UploadColourBuffer(void) const noexcept
{
	ScopedTimer<ProfilerAction> timeKeeper{ "Emitter", "Colour Upload" };
	UploadBufferAux(mColourBuffer, mColour);
}

void Dystopia::Emitter::Render(void) const noexcept
{
	glDrawArraysInstanced(GL_POINTS, 0, 1, static_cast<GLsizei>(mPosition.size()));
}

void Dystopia::Emitter::KillParticle(unsigned _nIdx) noexcept
{
	DEBUG_ASSERT(!mSpawnCount, "Particle System Error: No particles to kill!");
	--mSpawnCount;

	mLifetime.FastRemove(_nIdx);

	mColour[_nIdx].w = 0;
	mColour.FastRemove(_nIdx);

	mAccel.FastRemove(_nIdx);
	mVelocity.FastRemove(_nIdx);

	mPosition[_nIdx].w = 0;
	mPosition.FastRemove(_nIdx);

	mInitialLife.FastRemove(_nIdx);
	mbUpdatedPositions = true;
}

void Dystopia::Emitter::SpawnParticle(void) noexcept
{
	if (mParticle.mnLimit > mSpawnCount)
	{
		++mSpawnCount;

		for (auto& e : mSpawn)
			e.Update(*this, 0);

		Math::Vec4 vpos = mParticle.mPos.xyz1;
		auto pos = mpTransform->GetTransformMatrix() * mParticle.mPos.xyz1;
		pos.w = mParticle.mfSize;

		mLifetime   .EmplaceBackUnsafe(mParticle.mfLifeDur);
		mColour     .EmplaceBackUnsafe(mParticle.mColour  );
		mAccel      .EmplaceBackUnsafe(mParticle.mAccel   );
		mVelocity   .EmplaceBackUnsafe(mParticle.mVelocity);
		mPosition   .EmplaceBackUnsafe(pos                );
		mInitialLife.EmplaceBackUnsafe(mParticle.mfLifeDur);

		mbUpdatedPositions = true;
	}
}

void Dystopia::Emitter::SetTexture(Texture* _texture) noexcept
{
	mpTexture = _texture;
	if (mpTexture)
		mTextureName = mpTexture->GetName();
	else
	{
		mTextureName = "EditorStartup.png";
		mpTexture = CORE::Get<TextureSystem>()->GetTexture(mTextureName.c_str());
	}
}

void Dystopia::Emitter::SetOwner(ParticleEmitter* _pOwner) noexcept
{
	mpOwner = _pOwner;
}

Dystopia::Transform const& Dystopia::Emitter::GetOwnerTransform(void) const noexcept
{
	return *mpTransform;
}

Dystopia::Shader& Dystopia::Emitter::GetShader(void) noexcept
{
	return *mpShader;
}

size_t Dystopia::Emitter::GetSpawnCount(void) const noexcept
{
	return mSpawnCount;
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


void Dystopia::Emitter::Serialise(TextSerialiser& _out) const noexcept
{
	_out.InsertStartBlock("Emitter");

	if (mpShader)
		_out << mpShader->GetName();
	else
		_out << "Default Particle";
	if (mpTexture)
		_out << mpTexture->GetName();
	else
		_out << "EditorStartup.png";
	_out << mnParticleLimit;
	_out << mbIsAlive;
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

void Dystopia::Emitter::Unserialise(TextSerialiser& _in) noexcept
{
	_in.ConsumeStartBlock();

	std::string buf;
	buf.reserve(128);

	_in >> buf;
	mShaderName = buf.c_str();
	mpShader = CORE::Get<ShaderSystem>()->GetShader(mShaderName.c_str());
	_in >> buf;
	mTextureName = CORE::Get<FileSystem>()->GetFullPath(buf.c_str(), eFileDir::eResource).c_str();
	if (!mTextureName.length())
	{
		mTextureName = "EditorStartup.png";
		mpTexture = CORE::Get<TextureSystem>()->GetTexture(mTextureName);
	}
	else
		mpTexture = CORE::Get<TextureSystem>()->LoadTexture(mTextureName);
	int n = 0;
	_in >> n;
	mnParticleLimit = n;
	mParticle.mnLimit = static_cast<size_t>(mnParticleLimit);
	_in >> mbIsAlive;
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
	//static const auto affectorNames = Dystopia::AffectorUI<Dystopia::AffectorList>::GetUIName();
	//static bool bDebug = false;
	//
	//auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();
	//
	//EGUI::Display::EmptyBox("Particle Texture", 150, (mpTexture) ? mpTexture->GetName().c_str() : "-empty-", true);
	//if (const auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::ALL_IMG))
	//{
	//	cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand(&Emitter::SetTexture, mpTexture),
	//		cmd->MakeFnCommand(&Emitter::SetTexture,
	//			CORE::Get<TextureSystem>()->GetTexture(t->mName.c_str())));
	//	EGUI::Display::EndPayloadReceiver();
	//}
	//
	//OString buffer = "No Shader";
	//if (mpShader)
	//	buffer = mpShader->GetName().c_str();
	//
	//if (EGUI::Display::EmptyBox("Shader", 150, buffer.c_str(), true))
	//	bDebug = !bDebug;
	//
	//buffer += "                               ";
	//if (bDebug && EGUI::Display::TextField("Manual", buffer, true, 150))
	//	if (auto pShader = CORE::Get<ShaderSystem>()->GetShader(buffer.c_str()))
	//		mpShader = pShader;
	//
	//switch (EGUI::Display::DragInt("Maximum Particle Count", &mnParticleLimit, 1.f, 0, INT_MAX))
	//{
	//case EGUI::eDragStatus::eSTART_DRAG:
	//	cmd->StartRec(&Emitter::mnParticleLimit, this);
	//	break;
	//case EGUI::eDragStatus::eEND_DRAG:
	//case EGUI::eDragStatus::eDEACTIVATED:
	//case EGUI::eDragStatus::eENTER:
	//	cmd->EndRec(&Emitter::mnParticleLimit, this);
	//	break;
	//}
	//
	//EGUI::Display::Label("Particle Count: %u", mSpawnCount);
	//EGUI::Display::HorizontalSeparator();
	//if (EGUI::Display::StartTreeNode("Spawn Affectors"))
	//{
	//	for (int i = 0; i < mSpawn.size(); ++i)
	//	{
	//		EGUI::PushID(static_cast<int>(i));
	//		EGUI::Indent(30.f);
	//		EGUI::Display::Label((mSpawn[i].*affectorNames[mSpawn[i].GetID()])());
	//		EGUI::UnIndent(30.f);
	//		EGUI::PopID();
	//	}
	//	EGUI::Display::EndTreeNode();
	//}
	//EGUI::Display::HorizontalSeparator();
	//if (EGUI::Display::StartTreeNode("Update Affectors"))
	//{
	//	for (int i = 0; i < mUpdate.size(); ++i)
	//	{
	//		EGUI::PushID(static_cast<int>(i));
	//		EGUI::Indent(30.f);
	//		EGUI::Display::Label((mUpdate[i].*affectorNames[mUpdate[i].GetID()])());
	//		EGUI::UnIndent(30.f);
	//		EGUI::PopID();
	//	}
	//	EGUI::Display::EndTreeNode();
	//}
	//EGUI::Display::HorizontalSeparator();
	//if (EGUI::Display::StartTreeNode("Fixed Update Affectors"))
	//{
	//	for (int i = 0; i < mFixedUpdate.size(); ++i)
	//	{
	//		EGUI::PushID(static_cast<int>(i));
	//		EGUI::Indent(30.f);
	//		EGUI::Display::Label((mFixedUpdate[i].*affectorNames[mFixedUpdate[i].GetID()])());
	//		EGUI::UnIndent(30.f);
	//		EGUI::PopID();
	//	}
	//	EGUI::Display::EndTreeNode();
	//}
#endif 
}

void Dystopia::Emitter::StopEmission(void) noexcept
{
	mbIsAlive = false;
}

void Dystopia::Emitter::StartEmission(void) noexcept
{
	mbIsAlive = true;
}

bool Dystopia::Emitter::IsAlive(void) const noexcept
{
	return mbIsAlive;
}


