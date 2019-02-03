/* HEADER *********************************************************************************/
/*!
\file	Renderer.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Basic Renderer.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/Renderer.h"
#include "Component/Transform.h"

#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Mesh.h"
#include "System/Graphics/MeshSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/Texture2D.h"
#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include "Math/MathLib.h"
#include "DataStructure/Tuple.h"
#include "DataStructure/Variant.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/HashString.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"
#include "IO/TextSerialiser.h"
#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#include "Editor/EGUI.h"
#endif 


Dystopia::Renderer::Renderer(void) noexcept
	: Component{}, mnUnique{ 0 }, mpMesh{ nullptr }, mpShader{ nullptr },
	mpTexture{ nullptr }, mTexturePath{""}
{
}

Dystopia::Renderer::Renderer(Dystopia::Renderer&& _rhs) noexcept
	: Component{ Ut::Move(_rhs) }, mnUnique{ _rhs.mnUnique }, mpMesh{ _rhs.mpMesh }, mpShader{ _rhs.mpShader }, 
	mpTexture{ _rhs.mpTexture }, mTexturePath{ _rhs.mTexturePath }
{
	_rhs.mpMesh    = nullptr;
	_rhs.mpShader  = nullptr;
	_rhs.mpTexture = nullptr;
}

Dystopia::Renderer::Renderer(const Renderer& _rhs) noexcept
	: Component{ _rhs }, mnUnique{ 0 }, mpMesh{ nullptr }, mpShader{ nullptr }, 
	mpTexture{ nullptr }, mTexturePath{ _rhs.mTexturePath }
{
}

void Dystopia::Renderer::Awake(void)
{
	SetMesh("Quad");
	SetShader(CORE::Get<ShaderSystem>()->GetShader("Default Shader"));

	if (mTexturePath.length())
	{
		mpTexture = CORE::Get<GraphicsSystem>()->LoadTexture(mTexturePath);
	}
}

void Dystopia::Renderer::Init(void)
{
}

void Dystopia::Renderer::Draw(void) const noexcept
{
	if (mpMesh)
	{
		mpMesh->DrawMesh(GraphicsSystem::GetDrawMode());
	}
}


void Dystopia::Renderer::SetMesh(Mesh* _pMesh) noexcept
{
	mpMesh = _pMesh;
}

void Dystopia::Renderer::SetMesh(const std::string& _strMesh) noexcept
{
	mpMesh = CORE::Get<MeshSystem>()->GetMesh(_strMesh);
}


void Dystopia::Renderer::SetShader(Shader* _p) noexcept
{
	mpShader = _p;
}

void Dystopia::Renderer::SetShader(const std::string&) noexcept
{
	// TODO
	__debugbreak();
}

Dystopia::Shader* Dystopia::Renderer::GetShader(void) const noexcept
{
#   if EDITOR
	if (!mpShader || !mpShader->IsValid())
		return CORE::Get<ShaderSystem>()->GetShader("Error Shader");
#   endif

	return mpShader;
}


void Dystopia::Renderer::SetTexture(Texture* _pTexture) noexcept
{
	mpTexture = _pTexture;

	if (_pTexture)
	{
		mTexturePath = _pTexture->GetPath();
	}
	else
	{
		mTexturePath.clear(); 
	}
}

Dystopia::Texture* Dystopia::Renderer::GetTexture(void) const noexcept
{
	return mpTexture;
}


bool Dystopia::Renderer::HasTransparency(void) const noexcept
{
	return (mnFlags & eObjFlag::FLAG_RESERVED) != 0;
}

std::pair<OString, ::Gfx::eUniform_t> const* Dystopia::Renderer::FindUniformInShader(const char* _strName)
{
	for (auto& e : mpShader->GetVariables())
		if (e.first == _strName)
			return &e;

	return nullptr;
}

void Dystopia::Renderer::ResetOverride(void)
{
	for(auto& e : Ut::Range(mOverride).Reverse())
		if (!FindUniformInShader(e.Get<0>().c_str()))
			mOverride.FastRemove(&e);
}


AutoArray<Tuple<OString, ::Gfx::eUniform_t, Dystopia::Renderer::ShaderVariant_t>>& Dystopia::Renderer::GetOverrides(void)
{
	return mOverride;
}

Dystopia::Renderer* Dystopia::Renderer::Duplicate(void) const
{
	return static_cast<ComponentDonor<Renderer>*>(
		CORE::Get<typename Renderer::SYSTEM>()
	)->RequestComponent(*this);
}

void Dystopia::Renderer::Serialise(TextSerialiser& _out) const
{
	_out.InsertStartBlock("Renderer");
	Component::Serialise(_out);
	//_out << EngineCore::GetInstance()->Get<FileSystem>()->ConvertToRelative(mTexturePath);
	std::string rp = CORE::Get<FileSystem>()->ConvertToRelative(std::string{ mTexturePath.c_str() });
	auto pos = rp.find_last_of("/\\");
	if (pos != std::string::npos)
		_out << rp.substr(pos + 1);
	_out.InsertEndBlock("Renderer");
}

void Dystopia::Renderer::Unserialise(TextSerialiser& _in)
{
	std::string path;
	_in.ConsumeStartBlock();
	Component::Unserialise(_in);
	_in >> path;
	_in.ConsumeEndBlock();
	auto pos = path.find_last_of("/\\");
	if (pos != std::string::npos)
		path = path.substr(pos + 1);
	mTexturePath = CORE::Get<FileSystem>()->Normalize(CORE::Get<FileSystem>()->GetFullPath(path, eFileDir::eResource)).c_str();
}

void Dystopia::Renderer::EditorUI(void) noexcept
{
#if EDITOR
	/*EGUI::PushLeftAlign(80);

	TextureField();
	MeshField();
	ShaderField();

	EGUI::PopLeftAlign();*/
#endif
}

#if EDITOR
void Dystopia::Renderer::TextureField()
{
	::Editor::File *t = nullptr;
	EGUI::Display::EmptyBox("Texture", 150, (mpTexture) ? mpTexture->GetName().c_str() : "-empty-", true);
	auto cmd = ::Editor::EditorMain::GetInstance()->GetSystem<::Editor::EditorCommands>();
	t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PNG);
	if (t)  EGUI::Display::EndPayloadReceiver();

	if (!t)
	{
		t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::DDS);
		if (t) EGUI::Display::EndPayloadReceiver();
	}
	if (!t)
	{
		t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::BMP);
		if (t) EGUI::Display::EndPayloadReceiver();
	}
	if (t)
	{
		Texture *pTex = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(t->mPath.c_str());
		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand(&Renderer::SetTexture, mpTexture),
										   cmd->MakeFnCommand(&Renderer::SetTexture, pTex));
	}

	EGUI::SameLine();
	if (EGUI::Display::IconCross("Clear", 8.f))
	{
		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand(&Renderer::SetTexture, mpTexture),
										   cmd->MakeFnCommand(&Renderer::SetTexture, nullptr));
	}

	if (mpTexture)
	{
		EGUI::Display::Label("Preview");
		EGUI::SameLine(DefaultAlighnmentSpacing, 80);
		float ratio = static_cast<float>(mpTexture->GetHeight()) / static_cast<float>(mpTexture->GetWidth());
		EGUI::Display::Image(mpTexture->GetID(), Math::Vec2{ 140, 140 * ratio }, false, true);

		EGUI::SameLine();
		if (EGUI::Display::Button("Auto", Math::Vec2{ 35, 20 }))
		{
			auto w = static_cast<float>(mpTexture->GetWidth());
			auto h = static_cast<float>(mpTexture->GetHeight());
			w /= 10;
			h /= 10;
			cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand<Transform, const Math::Vec4&>(&Transform::SetScale, GetOwner()->GetComponent<Transform>()->GetScale()),
											   cmd->MakeFnCommand<Transform, const Math::Vec4&>(&Transform::SetScale, Math::Vec4{ w, h, 1.f }));
		}
	}
}

void Dystopia::Renderer::MeshField()
{
	if (EGUI::Display::EmptyBox("Mesh", 150, (mpMesh) ? mpMesh->GetName().c_str() : "", true))
	{

	}
	if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::FILE))
	{
		EGUI::Display::EndPayloadReceiver();
	}
}

void Dystopia::Renderer::ShaderField()
{
	static bool debug = false;
	EGUI::PushLeftAlign(80);
	static void(*x[])(ShaderVariant_t&) {
		[](ShaderVariant_t& v) { v = Ut::MetaExtract_t<0, ShaderTypeList>::type{}; },
		[](ShaderVariant_t& v) { v = Ut::MetaExtract_t<1, ShaderTypeList>::type{}; },
		[](ShaderVariant_t& v) { v = Ut::MetaExtract_t<2, ShaderTypeList>::type{}; },
		[](ShaderVariant_t& v) { v = Ut::MetaExtract_t<3, ShaderTypeList>::type{}; },
		[](ShaderVariant_t& v) { v = Ut::MetaExtract_t<4, ShaderTypeList>::type{}; },
		[](ShaderVariant_t& v) { v = Ut::MetaExtract_t<5, ShaderTypeList>::type{}; }
	};
	static ::Gfx::eUniform_t(*y[])(void) {
		[]() { return static_cast<::Gfx::eUniform_t>(Ut::MetaExtract_t<0, ShaderTypeList>::value); },
		[]() { return static_cast<::Gfx::eUniform_t>(Ut::MetaExtract_t<1, ShaderTypeList>::value); },
		[]() { return static_cast<::Gfx::eUniform_t>(Ut::MetaExtract_t<2, ShaderTypeList>::value); },
		[]() { return static_cast<::Gfx::eUniform_t>(Ut::MetaExtract_t<3, ShaderTypeList>::value); },
		[]() { return static_cast<::Gfx::eUniform_t>(Ut::MetaExtract_t<4, ShaderTypeList>::value); },
		[]() { return static_cast<::Gfx::eUniform_t>(Ut::MetaExtract_t<5, ShaderTypeList>::value); }
	};

	char const* str = "No Shader";
	if (mpShader)
	{
		str = mpShader->GetName().c_str();
	}
	if (EGUI::Display::EmptyBox("Shader", 150, str, true))
	{
		debug = !debug;
	}

	OString buffer{ str };
	buffer += "                               ";
	if (debug && EGUI::Display::TextField("Manual", buffer, true, 150))
	{
		if (auto pShader = CORE::Get<ShaderSystem>()->GetShader(buffer.c_str()))
		{
			SetShader(pShader);
		}
	}
	if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::FILE))
	{
		EGUI::Display::EndPayloadReceiver();
	}

	auto& vars = mpShader->GetVariables();
	mOverrideNames.clear();
	mOverrideNames.reserve(vars.size() + 1);

	mOverrideNames.EmplaceBack("Select");
	for (auto& e : vars)
		mOverrideNames.EmplaceBack(e.first.c_str());

	static int sele = 0;
	if (EGUI::Display::DropDownSelection("Overrides", sele, mOverrideNames) && sele)
	{
		::Gfx::eUniform_t type;
		ShaderVariant_t myVariant;

		if ([&]() {
			for (auto& e : mOverride)
				if (e.Get<0>() == mOverrideNames[sele])
					return false;
			return true;
		}())
		{
			for (int n = 0; n < Ut::SizeofList<ShaderTypeList>::value; ++n)
				if (y[n]() == vars[sele - 1].second)
				{
					type = vars[sele - 1].second;
					x[n](myVariant);
					break;
				}

			mOverride.EmplaceBack(mOverrideNames[sele], type, Ut::Move(myVariant));
		}
		sele = 0;
	}

	int n = 0;
	for (auto& e : Ut::Range(mOverride).Reverse())
	{
		EGUI::PushID(++n);

		e.Get<2>().Visit(UIVisitor{ e.Get<0>() });
		EGUI::SameLine();
		if (EGUI::Display::IconCross("Clear", 9.f))
			mOverride.FastRemove(&e);

		EGUI::PopID();
	}

	EGUI::PopLeftAlign();
}

template<>
inline void Dystopia::Renderer::UIVisitor::operator()(int& _variant)
{
	EGUI::Display::LabelWrapped(strName.c_str());
	EGUI::Display::DragInt(strName.c_str(), &_variant, 1, -INT_MAX, INT_MAX, true);
}
template<>
inline void Dystopia::Renderer::UIVisitor::operator()(float& _variant)
{
	EGUI::Display::LabelWrapped(strName.c_str());
	EGUI::Display::DragFloat(strName.c_str(), &_variant, 0.1f, -FLT_MAX, FLT_MAX, true);
}
template<>
inline void Dystopia::Renderer::UIVisitor::operator()(bool& _variant)
{
	EGUI::Display::CheckBox(strName.c_str(), &_variant);
}
template<>
inline void Dystopia::Renderer::UIVisitor::operator()(Math::Vec2& _variant)
{
	EGUI::Display::LabelWrapped(strName.c_str());
	EGUI::Display::VectorFields("", &_variant, 0.1f, -FLT_MAX, FLT_MAX);
}
template<>
inline void Dystopia::Renderer::UIVisitor::operator()(Math::Vec4& _variant)
{
	EGUI::Display::LabelWrapped(strName.c_str());
	EGUI::Display::VectorFields("", &_variant, 0.1f, -FLT_MAX, FLT_MAX);
}
template<>
inline void Dystopia::Renderer::UIVisitor::operator()(std::pair<Texture*, int>& _variant)
{
	::Editor::File *t = nullptr;
	EGUI::Display::LabelWrapped(strName.c_str());

	EGUI::Display::EmptyBox("Texture", 150, (_variant.first) ? _variant.first->GetName().c_str() : "-empty-", true);
	t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PNG);
	if (t)  EGUI::Display::EndPayloadReceiver();

	if (!t)
	{
		t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::DDS);
		if (t) EGUI::Display::EndPayloadReceiver();
	}

	if (!t)
	{
		t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::BMP);
		if (t) EGUI::Display::EndPayloadReceiver();
	}

	if (t)
	{
		_variant.first = CORE::Get<GraphicsSystem>()->LoadTexture(t->mPath.c_str());
	}

	EGUI::Display::DragInt(strName.c_str(), &_variant.second, 1, -INT_MAX, INT_MAX, true);
}
#endif
