/* HEADER *********************************************************************************/
/*!
\file	Emitter.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Particle Emitter

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
	mShaderName{ "Default Particle" }, mnParticleLimit{ 1000 }, mbIsAlive{ true }, mbUVChanged{ false },
	mpOwner{ _owner }, mpTransform{ nullptr }
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(5, &mClrBuffer);
	_EDITOR_CODE(if (_owner)
		mpTransform = mpOwner->GetOwner()->GetComponent<Transform>();
	)
}

Dystopia::Emitter::Emitter(Dystopia::Emitter const& _rhs) noexcept
	: mColour{ _rhs.mColour }, mPosition{ _rhs.mPosition }, mVelocity{ _rhs.mVelocity }, mAccel{ _rhs.mAccel },
	mLifetime{ _rhs.mLifetime }, mSpawnCount{ _rhs.mSpawnCount }, mbUVChanged{ _rhs.mbUVChanged },
	mSpawn{ _rhs.mSpawn }, mUpdate{ _rhs.mUpdate }, mFixedUpdate{ _rhs.mFixedUpdate },
	mpShader{ _rhs.mpShader }, mpTexture{ _rhs.mpTexture },
	mInitialLife{ _rhs.mInitialLife }, mTextureName{ _rhs.mTextureName }, 
	mShaderName{ _rhs.mTextureName }, mnParticleLimit{ _rhs.mnParticleLimit }, mbIsAlive{ _rhs.mbIsAlive },
	mpOwner{ nullptr }, mpTransform{ nullptr }
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(5, &mClrBuffer);
}

Dystopia::Emitter::~Emitter(void) noexcept
{
	Bind();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);

	glDeleteBuffers(5, &mClrBuffer);
	glDeleteVertexArrays(1, &mVAO);

	Unbind();
}

void Dystopia::Emitter::Awake(void)
{
	if (!mpShader)
		mpShader = CORE::Get<ShaderSystem>()->GetShader(mShaderName.c_str());

	if (!mpTexture)
		mpTexture = CORE::Get<TextureSystem>()->GetTexture(mTextureName.c_str());
}

void Dystopia::Emitter::Init(void)
{
	BaseInit();
	InitArrays();
	InitBuffers();
}

void Dystopia::Emitter::BaseInit(void)
{
#   if EDITOR
	if (!mpShader)
	{
		mpShader = CORE::Get<ShaderSystem>()->GetShader(mShaderName.c_str());
	}
	if (!mpTexture)
	{
		auto path = CORE::Get<FileSystem>()->Normalize(CORE::Get<FileSystem>()->GetFullPath(mTextureName.c_str(), eFileDir::eResource));
		mpTexture = CORE::Get<TextureSystem>()->LoadTexture(path.c_str());
	}

	if (!mpOwner)
		__debugbreak();
#   endif

#if EDITOR
	mParticle.mnLimit = mnParticleLimit;
#endif
	mpTransform = mpOwner->GetOwner()->GetComponent<Transform>();

	mbUpdatedPositions = false;
}

void Dystopia::Emitter::InitArrays(void)
{
	mInitialLife.clear();
	mLifetime   .clear();
	mRotVel		.clear();
	mRotAcc		.clear();
	mUV         .clear();
	mSize       .clear();
	mColour     .clear();
	mAccel      .clear();
	mVelocity   .clear();
	mPosition   .clear();
	mRotation   .clear();

	mInitialLife.reserve(mParticle.mnLimit);
	mLifetime   .reserve(mParticle.mnLimit);
	mRotVel		.reserve(mParticle.mnLimit);
	mRotAcc		.reserve(mParticle.mnLimit);
	mUV         .reserve(mParticle.mnLimit);
	mSize       .reserve(mParticle.mnLimit);
	mColour     .reserve(mParticle.mnLimit);
	mAccel      .reserve(mParticle.mnLimit);
	mVelocity   .reserve(mParticle.mnLimit);
	mPosition   .reserve(mParticle.mnLimit);
	mRotation	.reserve(mParticle.mnLimit);
}

void Dystopia::Emitter::InitBuffers(void) const noexcept
{
	Bind();

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glBufferData(GL_ARRAY_BUFFER, mParticle.mnLimit * sizeof(decltype(mPosition)::Val_t), nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(0, 1);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mClrBuffer);
	glBufferData(GL_ARRAY_BUFFER, mParticle.mnLimit * sizeof(decltype(mColour)::Val_t), nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, mSzBuffer);
	glBufferData(GL_ARRAY_BUFFER, mParticle.mnLimit * sizeof(decltype(mSize)::Val_t), nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, mRotBuffer);
	glBufferData(GL_ARRAY_BUFFER, mParticle.mnLimit * sizeof(decltype(mRotation)::Val_t), nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, mParticle.mnLimit * sizeof(decltype(mUV)::Val_t), nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(4, 1);

#   if defined(DEBUG) | defined(_DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#    endif

	Unbind();
}


void Dystopia::Emitter::FixedUpdate(float _fDT) noexcept
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
		mPosition[n] += mVelocity[n].xyz * _fDT;
	}

#else
	auto pAcc = mAccel.begin();
	auto pVel = mVelocity.begin();
	auto pRotAcc = mRotAcc.begin();
	auto pRotVel = mRotVel.begin();

	for (auto& e : mVelocity)
	{
		e += *pAcc * _fDT;
		++pAcc;
	}

	for (auto& e : mPosition)
	{
		e += *pVel * _fDT;
		++pVel;
	}

	auto ptr = reinterpret_cast<Math::Vec4*>(mRotVel.begin());
	auto sz = mRotVel.size() >> 2;
	size_t n;
	for (n = 0; n < sz; ++n)
	{
		*ptr = *reinterpret_cast<Math::Vec4*&>(pRotAcc)++ * _fDT;
	}
	auto ptr2 = reinterpret_cast<float*>(ptr);
	for (n <<= 2; n < mRotVel.size(); ++n)
	{
		*ptr2 = *pRotAcc * _fDT;
		++pRotAcc;
	}

	ptr = reinterpret_cast<Math::Vec4*>(mRotation.begin());
	sz = mRotation.size() >> 2;
	for (n = 0; n < sz; ++n)
	{
		*ptr = *reinterpret_cast<Math::Vec4*&>(pRotVel)++ * _fDT;
	}
	ptr2 = reinterpret_cast<float*>(ptr);
	for (n <<= 2; n < mRotation.size(); ++n)
	{
		*ptr2 = *pRotVel * _fDT;
		++pRotVel;
	}
#endif

	mbUpdatedPositions = true;
}

void Dystopia::Emitter::Bind(void) const noexcept
{
	glBindVertexArray(mVAO);
	_EDITOR_CODE(if (mpTexture))
	mpTexture->Bind();
}

void Dystopia::Emitter::Unbind(void) const noexcept
{
	//mpTexture->Unbind();
	//glBindVertexArray(0);
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

void Dystopia::Emitter::UploadBuffers(void) const noexcept
{
	ScopedTimer<ProfilerAction> timeKeeper{ "Emitter", "Upload" };

	UploadBufferAux(mSzBuffer, mSize);
	UploadBufferAux(mClrBuffer, mColour);

	if (mbUVChanged)
		UploadBufferAux(mUVBuffer, mUV);

	if (mbUpdatedPositions)
	{
		UploadBufferAux(mPosBuffer, mPosition);
		UploadBufferAux(mRotBuffer, mRotation);
	}

	const_cast<bool&>(mbUVChanged)        = false;
	const_cast<bool&>(mbUpdatedPositions) = false;
}

void Dystopia::Emitter::Render(void) const noexcept
{
	glDrawArraysInstanced(GL_POINTS, 0, 1, static_cast<GLsizei>(mPosition.size()));
}

void Dystopia::Emitter::KillParticle(size_t _nIdx) noexcept
{
	DEBUG_ASSERT(!mSpawnCount, "Particle System Error: No particles to kill!");
	--mSpawnCount;

	mInitialLife.FastRemove(_nIdx);
	mLifetime   .FastRemove(_nIdx);
	mRotVel     .FastRemove(_nIdx);
	mRotAcc     .FastRemove(_nIdx);
	mUV         .FastRemove(_nIdx);
	mSize       .FastRemove(_nIdx);
	mColour     .FastRemove(_nIdx);
	mAccel      .FastRemove(_nIdx);
	mVelocity   .FastRemove(_nIdx);
	mPosition   .FastRemove(_nIdx);
	mRotation   .FastRemove(_nIdx);

	mbUVChanged        = true;
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
		auto&& trs = mpTransform->GetTransformMatrix();

		mInitialLife.EmplaceBackUnsafe(mParticle.mfLifeDur);
		mLifetime   .EmplaceBackUnsafe(mParticle.mfLifeDur);
		mRotVel     .EmplaceBackUnsafe(mParticle.mRotVel  );
		mRotAcc     .EmplaceBackUnsafe(mParticle.mRotAccel);
	    mUV         .EmplaceBackUnsafe(mParticle.mUV      );
		mSize       .EmplaceBackUnsafe(mParticle.mSize    );
		mColour     .EmplaceBackUnsafe(mParticle.mColour  );
		mAccel      .EmplaceBackUnsafe(mParticle.mAccel   );
		mVelocity   .EmplaceBackUnsafe(mParticle.mVelocity);
		mPosition   .EmplaceBackUnsafe((trs * vpos).xyz   );
		mRotation   .EmplaceBackUnsafe(mParticle.mRotation);

		mbUVChanged        = true;
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
	_EDITOR_CODE(if (_pOwner)
		mpTransform = mpOwner->GetOwner()->GetComponent<Transform>();
	else
		mpTransform = nullptr;
	)
}

void Dystopia::Emitter::NotifyUVChanged(void) noexcept
{
	mbUVChanged = true;
}

Dystopia::Transform const& Dystopia::Emitter::GetOwnerTransform(void) const noexcept
{
	return *mpTransform;
}

void Dystopia::Emitter::SetShader(Shader* _p) noexcept
{
	mpShader = _p;
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

AutoArray<float>& Dystopia::Emitter::GetLifetime(void) noexcept
{
	return mLifetime;
}

AutoArray<float>& Dystopia::Emitter::GetInitialLifetime(void) noexcept
{
	return mInitialLife;
}

AutoArray<Math::Vec4>& Dystopia::Emitter::GetUV(void) noexcept
{
	return mUV;
}

AutoArray<Math::Vec3>& Dystopia::Emitter::GetPosition(void) noexcept
{
	return mPosition;
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

AutoArray<float>& Dystopia::Emitter::GetRotation(void) noexcept
{
	return mRotation;
}

AutoArray<float>& Dystopia::Emitter::GetRotationalVelocity(void) noexcept
{
	return mRotVel;
}

AutoArray<float>& Dystopia::Emitter::GetRotationalAcceleration(void) noexcept
{
	return mRotAcc;
}

AutoArray<Math::Vec2>& Dystopia::Emitter::GetSize(void) noexcept
{
	return mSize;
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
	if (GetTexture())
	{
		EGUI::Display::EmptyBox("Texture", 150, GetTexture()->GetName().c_str(), true);
	}
	else
	{
		EGUI::Display::EmptyBox("Texture", 150, "-empty-", true);
	}

	if (auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::ALL_IMG))
	{
		SetTexture(CORE::Get<TextureSystem>()->LoadTexture(t->mPath));
		EGUI::Display::EndPayloadReceiver();
	}

	EGUI::SameLine();
	if (EGUI::Display::IconCross("Clear", 8.f))
	{
		SetTexture(nullptr);
	}

	if (auto t = GetTexture())
	{
		EGUI::Display::Label("Preview");
		EGUI::SameLine(DefaultAlighnmentSpacing, 80);
		float ratio = static_cast<float>(t->GetHeight()) / static_cast<float>(t->GetWidth());
		EGUI::Display::Image(t->GetID(), Math::Vec2{ 140, 140 * ratio }, false, true);
	}
	EGUI::Display::Label("Count : %d", GetSpawnCount()); 
	EGUI::Display::DragInt("Maximum Limit", &mnParticleLimit, 1.f, 0, INT_MAX);
	
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

Dystopia::Texture * Dystopia::Emitter::GetTexture(void) const noexcept
{
	return mpTexture;
}


