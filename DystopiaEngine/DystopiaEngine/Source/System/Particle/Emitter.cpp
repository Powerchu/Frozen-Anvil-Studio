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
#include "System/Particle/TrailEmitter.h"

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


namespace
{
	static const auto DEFAULT_TEXTURE = "EditorStartup.png";
}

Dystopia::Emitter::Emitter(ParticleEmitter* _owner) noexcept
	: Emitter{ _owner, GL_POINTS, 1, "Default Particle" }
{
}

#define COMMA ,
Dystopia::Emitter::Emitter(ParticleEmitter * _owner, int _drawMode, int _div, char const* _shader) noexcept
	: mColour{}, mPosition{}, mVelocity{}, mAccel{}, mLifetime{}, mInitialLife{}, mCommand{},
	mnParticleLimit{ 1000 }, mSpawnCount{}, mSpawn{}, mUpdate{}, mFixedUpdate{}, 
	mbUpdatedPositions{ false }, mbUVChanged{ false }, mbIsAlive{ true }, _EDITOR_CODE(mbBuffers{ false } COMMA bEditorInit{ false } COMMA)
	mpShader{ nullptr }, mShaderName{ _shader }, mpTexture{ nullptr }, mTextureName{ DEFAULT_TEXTURE },
	mpOwner{ _owner }, mpTransform{ nullptr }, mDrawMode{ _drawMode }, mDiv{ _div }
{
	if (_owner)
	{
		GenBuffers();
		_EDITOR_CODE(mpTransform = mpOwner->GetOwner()->GetComponent<Transform>());
	}
}

Dystopia::Emitter::Emitter(Dystopia::Emitter const& _rhs) noexcept
	: mColour{}, mPosition{}, mVelocity{}, mAccel{}, mLifetime{}, mInitialLife{}, mCommand{},
	mnParticleLimit{ _rhs.mnParticleLimit }, mSpawnCount{ _rhs.mSpawnCount }, 
	mSpawn{ _rhs.mSpawn }, mUpdate{ _rhs.mUpdate }, mFixedUpdate{ _rhs.mFixedUpdate },
	mbUpdatedPositions{ false }, mbUVChanged{ false }, mbIsAlive{ _rhs.mbIsAlive }, _EDITOR_CODE(mbBuffers{ false } COMMA bEditorInit{ false } COMMA)
	mpShader{ _rhs.mpShader }, mShaderName{ _rhs.mTextureName }, mpTexture{ _rhs.mpTexture }, mTextureName{ _rhs.mTextureName },
	mpOwner{ nullptr }, mpTransform{ nullptr }, mDrawMode{ _rhs.mDrawMode }, mDiv{ _rhs.mDiv }
{
	GenBuffers();
}
#undef COMMA

Dystopia::Emitter::~Emitter(void) noexcept
{
	_EDITOR_CODE(if (mbBuffers) {)

	Bind();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);

	Unbind();

	glDeleteBuffers(6, &mClrBuffer);
	glDeleteVertexArrays(1, &mVAO);

	_EDITOR_CODE(})
}

void Dystopia::Emitter::GenBuffers(void) noexcept
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(6, &mClrBuffer);
	_EDITOR_CODE(mbBuffers = true);
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
	mCommand.clear();
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

	if (!mpOwner) __debugbreak();

	mParticle.mnLimit = mnParticleLimit;
#   endif

	mpTransform = mpOwner->GetOwner()->GetComponent<Transform>();
	mbUVChanged        = false;
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
	mCommand    .clear();

	mInitialLife.reserve(mParticle.mnLimit);
	mLifetime   .reserve(mParticle.mnLimit);
	mRotVel		.reserve(mParticle.mnLimit);
	mRotAcc		.reserve(mParticle.mnLimit);
	mUV         .reserve(mParticle.mnLimit);
	mSize       .reserve(mParticle.mnLimit);
	mColour     .reserve(mParticle.mnLimit);
	mAccel      .reserve(mParticle.mnLimit);
	mVelocity   .reserve(mParticle.mnLimit);
	mPosition   .reserve(mParticle.mnLimit * mDiv);
	mRotation	.reserve(mParticle.mnLimit);
	mCommand    .reserve(mParticle.mnLimit);

	for (unsigned n = 0; n < mParticle.mnLimit; ++n)
		mCommand.EmplaceBackUnsafe(static_cast<unsigned>(mDiv), 1u, n * mDiv, n);
}

void Dystopia::Emitter::InitBuffers(void) noexcept
{
	//_EDITOR_CODE(if (bEditorInit) return static_cast<void>(bEditorInit = !bEditorInit);)

	_EDITOR_CODE(if (::Editor::EditorMain::GetInstance()->GetCurState() != ::Editor::eState::PLAY  && ::Editor::EditorMain::GetInstance()->GetNextState() != ::Editor::eState::PLAY) return;)

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mCmdBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, mParticle.mnLimit * sizeof(decltype(mCommand)::Val_t), mCommand.begin(), GL_STATIC_DRAW);

	Bind();

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glBufferData(GL_ARRAY_BUFFER, mDiv * mParticle.mnLimit * sizeof(decltype(mPosition)::Val_t), nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(0, 0);

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
		*ptr += *reinterpret_cast<Math::Vec4*&>(pRotAcc)++ * _fDT;
	}
	auto ptr2 = reinterpret_cast<float*>(ptr);
	for (n <<= 2; n < mRotVel.size(); ++n)
	{
		*ptr2 += *pRotAcc * _fDT;
		++pRotAcc;
	}

	ptr = reinterpret_cast<Math::Vec4*>(mRotation.begin());
	sz = mRotation.size() >> 2;
	for (n = 0; n < sz; ++n)
	{
		*ptr += *reinterpret_cast<Math::Vec4*&>(pRotVel)++ * _fDT;
		//Math::Wrap(*ptr, Math::Vec4{ -Math::pi }, Math::Vec4{ Math::pi });
		++ptr;
	}
	ptr2 = reinterpret_cast<float*>(ptr);
	for (n <<= 2; n < mRotation.size(); ++n)
	{
		*ptr2 += *pRotVel * _fDT;
		//Math::Wrap(*ptr2, -Math::pi, Math::pi);

		++pRotVel;
	}
#endif

	mbUpdatedPositions = true;
}

void Dystopia::Emitter::Bind(void) const noexcept
{
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mCmdBuffer);
	glBindVertexArray(mVAO);
	_EDITOR_CODE(if (mpTexture))
	mpTexture->Bind();
}

void Dystopia::Emitter::Unbind(void) const noexcept
{
	_EDITOR_CODE(if (mpTexture))
	mpTexture->Unbind();
	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

namespace
{
	template <typename T, typename U>
	inline void UploadBufferAux(T& buf, U& arr) noexcept
	{
		using val_t = Ut::RemoveRef_t<decltype(arr[0])>;

		auto s = sizeof(val_t);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		if (auto err = glGetError())
		{
			__debugbreak();
		}
		auto MapPtr = glMapBufferRange(GL_ARRAY_BUFFER, 0, arr.size() * s, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		if (auto err = glGetError())
		{
			__debugbreak();
		}

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
	glMultiDrawArraysIndirect(mDrawMode, 0, static_cast<GLsizei>(mPosition.size()) / mDiv, 0);
	//glDrawArraysInstanced(mDrawMode, 0, mDiv, static_cast<GLsizei>(mPosition.size()) / mDiv);
	//glDrawArraysInstancedBaseInstance(mDrawMode, 0, mDiv, static_cast<GLsizei>(mPosition.size()) / mDiv, 0);
	//glMultiDrawArrays(mDrawMode, first.begin(), count.begin(), static_cast<GLsizei>(mPosition.size()) / mDiv);

	// TODO: REMOVE
	if (auto err = glGetError())
		__debugbreak();
}

void Dystopia::Emitter::KillParticle(size_t _nIdx) noexcept
{
	DEBUG_ASSERT(!mSpawnCount, "Particle System Error: No particles to kill!");
	--mSpawnCount;

	for (auto& e : mDeath)
		e.Update(*this, 0);

	mInitialLife.FastRemove(_nIdx);
	mLifetime   .FastRemove(_nIdx);
	mRotVel     .FastRemove(_nIdx);
	mRotAcc     .FastRemove(_nIdx);
	mUV         .FastRemove(_nIdx);
	mSize       .FastRemove(_nIdx);
	mColour     .FastRemove(_nIdx);
	mAccel      .FastRemove(_nIdx);
	mVelocity   .FastRemove(_nIdx);
	mPosition   .FastRemoveRange(_nIdx * mDiv, mDiv);
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

		auto&& trs = mpTransform->GetTransformMatrix();
		auto vpos = (trs * mParticle.mPos.xyz1).xyz;

		mInitialLife.EmplaceBackUnsafe(mParticle.mfLifeDur);
		mLifetime   .EmplaceBackUnsafe(mParticle.mfLifeDur);
		mRotVel     .EmplaceBackUnsafe(mParticle.mRotVel  );
		mRotAcc     .EmplaceBackUnsafe(mParticle.mRotAccel);
	    mUV         .EmplaceBackUnsafe(mParticle.mUV      );
		mSize       .EmplaceBackUnsafe(mParticle.mSize    );
		mColour     .EmplaceBackUnsafe(mParticle.mColour  );
		mAccel      .EmplaceBackUnsafe(mParticle.mAccel   );
		mVelocity   .EmplaceBackUnsafe(mParticle.mVelocity);
		mPosition   .EmplaceBackMultiUnsafe(mDiv, vpos);
		mRotation   .EmplaceBackUnsafe(mParticle.mRotation);

		mbUVChanged        = true;
		mbUpdatedPositions = true;
	}
}
void Dystopia::Emitter::SpawnParticleGlobal(void) noexcept
{
	if (mParticle.mnLimit > mSpawnCount)
	{
		++mSpawnCount;

		for (auto& e : mSpawn)
			e.Update(*this, 0);

		mInitialLife.EmplaceBackUnsafe(mParticle.mfLifeDur);
		mLifetime   .EmplaceBackUnsafe(mParticle.mfLifeDur);
		mRotVel     .EmplaceBackUnsafe(mParticle.mRotVel  );
		mRotAcc     .EmplaceBackUnsafe(mParticle.mRotAccel);
		mUV         .EmplaceBackUnsafe(mParticle.mUV      );
		mSize       .EmplaceBackUnsafe(mParticle.mSize    );
		mColour     .EmplaceBackUnsafe(mParticle.mColour  );
		mAccel      .EmplaceBackUnsafe(mParticle.mAccel   );
		mVelocity   .EmplaceBackUnsafe(mParticle.mVelocity);
		mPosition   .EmplaceBackMultiUnsafe(mDiv, mParticle.mPos);
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
		mTextureName = DEFAULT_TEXTURE;
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
		_out << DEFAULT_TEXTURE;
	_out << mnParticleLimit;
	_out << mbIsAlive;
	_out << mDrawMode;
	_out << mDiv;
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

	_EDITOR_CODE(bEditorInit = true);

	std::string buf;
	buf.reserve(128);

	_in >> buf;
	mShaderName = buf.c_str();
	mpShader = CORE::Get<ShaderSystem>()->GetShader(mShaderName.c_str());
	_in >> buf;
	mTextureName = CORE::Get<FileSystem>()->GetFullPath(buf.c_str(), eFileDir::eResource).c_str();
	if (!mTextureName.length())
	{
		mTextureName = DEFAULT_TEXTURE;
		mpTexture = CORE::Get<TextureSystem>()->GetTexture(mTextureName);
	}
	else
		mpTexture = CORE::Get<TextureSystem>()->LoadTexture(mTextureName);
	int n = 0;
	_in >> n;
	mnParticleLimit = n;
	mParticle.mnLimit = static_cast<size_t>(mnParticleLimit);
	_in >> mbIsAlive;
	_in >> mDrawMode;
	_in >> mDiv;

	_EDITOR_CODE(mDiv += 0 == mDiv);

	if (mDiv > 1)
	{
		*reinterpret_cast<void**>(this) = TrailEmitter::GetVTablePtr();
	}

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

	EGUI::Display::CheckBox("Alive", &mbIsAlive);

	if (!GetTexture())
		EGUI::Display::ImageEmpty("-empty-", { 100, 100 });
	else
	{
		EGUI::Display::Label(GetTexture()->GetName().c_str());
		EGUI::SameLine(DefaultAlighnmentSpacing);
		EGUI::Display::Image(GetTexture()->GetID(), Math::Vec2{ 100, 100 }, false, true);
	}
	if (auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::ALL_IMG))
	{
		SetTexture(CORE::Get<TextureSystem>()->LoadTexture(t->mPath));
		EGUI::Display::EndPayloadReceiver();
	}

	ImGui::SameLine();
	if (EGUI::Display::IconCross("Clear", 8.f))
		SetTexture(nullptr);
	
	static char buf[512]{};
	if (EGUI::Display::TextField("Test", buf, 512))
	{
		SetShader(CORE::Get<ShaderSystem>()->GetShader(buf));
		std::memset(buf, 0, sizeof(buf));
	}

	//if (auto t = GetTexture())
	//{
	//	EGUI::Display::Label("Preview");
	//	EGUI::SameLine(DefaultAlighnmentSpacing, 80);
	//	float ratio = static_cast<float>(t->GetHeight()) / static_cast<float>(t->GetWidth());
	//	EGUI::Display::Image(t->GetID(), Math::Vec2{ 140, 140 * ratio }, false, true);
	//}
	EGUI::Display::Label("Count : %d", GetSpawnCount()); 
	EGUI::Display::DragInt("Maximum", &mnParticleLimit, 1.f, 0, INT_MAX);
	
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

int Dystopia::Emitter::GetStride(void) const noexcept
{
	return mDiv;
}


