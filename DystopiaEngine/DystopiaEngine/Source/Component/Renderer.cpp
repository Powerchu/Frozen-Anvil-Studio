/* HEADER *********************************************************************************/
/*!
\file	Renderer.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Basic Renderer.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
#include "System/Graphics/TextureSystem.h"
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
	: Component{}, mnUnique{ 0 }, mpMesh{ CORE::Get<MeshSystem>()->GetMesh("Quad") }, mpShader{ nullptr },
	mTexturePaths{}, mTextureFields{}
{
	SetShader(CORE::Get<ShaderSystem>()->GetShader("Default Shader"));
}

Dystopia::Renderer::Renderer(Dystopia::Renderer&& _rhs) noexcept
	: Component{ Ut::Move(_rhs) }, mnUnique{ _rhs.mnUnique }, mpMesh{ _rhs.mpMesh }, mpShader{ _rhs.mpShader }, 
	mTexturePaths{ Ut::Move(_rhs.mTexturePaths) }, mTextureFields{ Ut::Move(_rhs.mTextureFields) }, mOverride{ Ut::Move(_rhs.mOverride) }
{
	_rhs.mpMesh    = nullptr;
	_rhs.mpShader  = nullptr;
}

Dystopia::Renderer::Renderer(const Renderer& _rhs) noexcept
	: Component{ _rhs }, mnUnique{ 0 }, mpMesh{ _rhs.mpMesh }, mpShader{ _rhs.mpShader },
	mTexturePaths{ _rhs.mTexturePaths }, mTextureFields{ _rhs.mTextureFields }, mOverride{ _rhs.mOverride }
{
}

void Dystopia::Renderer::Awake(void)
{
	// SetMesh("Quad");
	// SetShader(CORE::Get<ShaderSystem>()->GetShader("Default Shader"));
	// InitializeTextureFields();
}

void Dystopia::Renderer::Init(void)
{
}

void Dystopia::Renderer::Draw(void) const noexcept
{
	_EDITOR_CODE(if (mpMesh))
	mpMesh->DrawMesh(GraphicsSystem::GetDrawMode());
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
	if (mpShader == _p)
		return;

	mpShader = _p ? _p : CORE::Get<ShaderSystem>()->GetShader("Default Shader");

	const auto& texList = mpShader->GetTextureList();

	Texture* pMainTex = nullptr;
	if (mTextureFields.size())
		pMainTex = mTextureFields[0].Get<1>();

	mTextureFields.clear();
	mTextureFields.reserve(texList.size());

	//mTexturePaths.clear();
	//mTexturePaths.resize(texList.size);

	for (const auto & e : texList)
		mTextureFields.EmplaceBack(e.second, nullptr);

	SetTexture(pMainTex, 0);
}

void Dystopia::Renderer::SetShader(const std::string& _strName) noexcept
{
	SetShader(CORE::Get<ShaderSystem>()->GetShader(_strName.c_str()));
}

Dystopia::Shader* Dystopia::Renderer::GetShader(void) const noexcept
{
#   if EDITOR
	if (!mpShader || !mpShader->IsValid())
		return CORE::Get<ShaderSystem>()->GetShader("Error Shader");
#   endif

	return mpShader;
}

void Dystopia::Renderer::SetTexture(Texture* _pTexture, unsigned _idx) noexcept
{
	for (auto& e : mTextureFields)
		if (e.Get<0>() == _idx)
			e.Get<1>() = _pTexture;

	//if (mTexturePaths.Cap() <= _idx)
	//	mTexturePaths.resize(_idx + 1);
	//
	//if (_pTexture)
	//	mTexturePaths[_idx] = _pTexture->GetPath();
	//else
	//	mTexturePaths[_idx].clear();

	// InitializeTextureFields();
}

Dystopia::Texture* Dystopia::Renderer::GetTexture(unsigned _idx) const noexcept
{
	for (auto& e : mTextureFields)
		if (e.Get<0>() == _idx)
			return e.Get<1>();

	return nullptr;
}

AutoArray<Tuple<unsigned, Dystopia::Texture*>> const& Dystopia::Renderer::GetTextures(void) const noexcept
{
	return mTextureFields;
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

void Dystopia::Renderer::InitializeTextureFields(void)
{
	//unsigned n = 0;
	//
	//mTextureFields.clear();
	//mTextureFields.reserve(mTexturePaths.size());
	//
	//for (auto& e : mTexturePaths)
	//{
	//	if (e.length())
	//	{
	//		auto t = CORE::Get<TextureSystem>()->GetTexture(e);
	//		t = t ? t : CORE::Get<TextureSystem>()->LoadTexture(e);
	//
	//		if (!t)
	//			__debugbreak();
	//
	//		mTextureFields.EmplaceBack(n++, t);
	//	}
	//	else
	//		mTextureFields.EmplaceBack(n++, nullptr);
	//}
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
	auto pFileSys = CORE::Get<FileSystem>();

	_out.InsertStartBlock("Renderer");
	Component::Serialise(_out);

	_out << "SENTRY";
	//_out << mTextureFields.size();
	
	if (mpShader)
		_out << mpShader->GetName();
	else
		_out << "Error Shader";

	for (auto& e : mTextureFields)
	{
		_out << e.Get<0>() + 1;
		if (Texture*& ptr = e.Get<1>())
		{
			auto rp = pFileSys->ConvertToRelative(ptr->GetPath());
			auto pos = rp.find_last_of("/\\");

			if (pos != OString::nPos)
				_out << rp.substr(pos + 1);
			else
				_out << "";
		}
		else
			_out << "";
	}

	_out.InsertEndBlock("RENDERER_BASE");
	_out.InsertStartBlock("OVERRIDE");
	_out << mOverride.size();
	for (auto& e : mOverride)
	{
		_out << e.Get<0>();
		_out << static_cast<unsigned>(e.Get<1>());
		_out << e.Get<2>().GetTypeID();

		e.Get<2>().Visit([&_out](auto& _var) { _out << _var; });
	}

	_out.InsertEndBlock("Renderer");
}

void Dystopia::Renderer::Unserialise(TextSerialiser& _in)
{
	auto pFileSys = CORE::Get<FileSystem>();

	OString strOverride;
	std::string path;

	mTexturePaths.clear();
	
	_in.ConsumeStartBlock();
	Component::Unserialise(_in);
	_in >> path;

	if ("SENTRY" == path) // NEW VERSION
	{
		_in >> path; // shader saved name

		auto t = CORE::Get<ShaderSystem>()->GetShader(path.c_str());
		SetShader(t);

		size_t count = t->GetTextureList().size();
		//_in >> count;

		for (size_t i = 0; i < count; ++i)
		{
			unsigned id;
			_in >> id;
			_in >> path;

			id -= 1;

			if (Ut::Constant<decltype(id), -1>::value == id)
				break;

			path = pFileSys->Normalize(pFileSys->GetFullPath(path, eFileDir::eResource));
			//SetTexture(CORE::Get<TextureSystem>()->LoadTexture(path.c_str()), id);

			if (path.size())
				SetTexture(CORE::Get<TextureSystem>()->LoadTexture(path.c_str()), id);
			else
				SetTexture(nullptr, id);
		}

		_in.ConsumeEndBlock();
		_in.ConsumeStartBlock();

		ShaderVariant_t var;
		std::underlying_type_t<::Gfx::eUniform_t> type;
		ResetOverride();

		_in >> count;
		while (!_in.EndOfInput())
		{
			_in >> strOverride;
			_in >> type;
			_in >> const_cast<unsigned short&>(var.GetTypeID());

			var.Visit([&_in](auto& _var) { _in >> _var; });
			mOverride.EmplaceBack(strOverride, static_cast<::Gfx::eUniform_t>(type), Ut::Move(var));
		}
	}
	else // OLD SAVE
	{
		SetShader(nullptr);

		auto pos = path.find_last_of("/\\");
		if (pos != std::string::npos)
			path = path.substr(pos + 1);

		path = pFileSys->Normalize(pFileSys->GetFullPath(path, eFileDir::eResource));

		if (path.length() == 0)
		{
			//__debugbreak();
			SetTexture(nullptr, 0);
		}
		else
			SetTexture(CORE::Get<TextureSystem>()->LoadTexture(path.c_str()), 0);
	}

	_in.ConsumeEndBlock();
}

void Dystopia::Renderer::EditorUI(void) noexcept
{
#if EDITOR
	EGUI::PushLeftAlign(80);

	TextureField();
	//MeshField();
	ShaderField();

	EGUI::PopLeftAlign();
#endif
}

#if EDITOR
void Dystopia::Renderer::TextureField()
{
	auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();

	for (auto& [idx, mpTexture] : mTextureFields)
	{
		EGUI::PushID(idx);

		HashString displayName{"Texture"};
		if (mpShader)
		{
			const auto& texList = mpShader->GetTextureList();
			for (auto& tp : texList)
			{
				if (tp.second == idx)
				{
					displayName = tp.first;
				}
			}
		}

		if (!mpTexture)
			EGUI::Display::ImageEmpty(displayName.c_str(), { 100, 100 });
		else
		{
			EGUI::Display::Label(displayName.c_str());
			EGUI::SameLine(DefaultAlighnmentSpacing);
			//const float ratio = static_cast<float>(mpTexture->GetHeight()) / static_cast<float>(mpTexture->GetWidth());
			EGUI::Display::Image(mpTexture->GetID(), Math::Vec2{ 100, 100 }, false, true);
		}

		if (const auto t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::ALL_IMG))
		{
			cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand(&Renderer::SetTexture, mpTexture, idx),
											   cmd->MakeFnCommand(&Renderer::SetTexture, 
																  CORE::Get<GraphicsSystem>()->LoadTexture(t->mPath), idx));
			EGUI::Display::EndPayloadReceiver();
		}

		ImGui::SameLine();
		if (EGUI::Display::IconCross("ClearTexture", 8.f))
		{
			cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand(&Renderer::SetTexture, mpTexture, idx),
											   cmd->MakeFnCommand(&Renderer::SetTexture, nullptr, idx));
		}

		EGUI::PopID();
	}
}

void Dystopia::Renderer::MeshField()
{
	//if (EGUI::Display::EmptyBox("Mesh", 150, (mpMesh) ? mpMesh->GetName().c_str() : "", true))
	//{
	//
	//}
	//if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::FILE))
	//{
	//	EGUI::Display::EndPayloadReceiver();
	//}
}


namespace
{
	template <typename, typename>
	struct GenVariantList;

	template <template<unsigned...> class S, unsigned ... V, typename T>
	struct GenVariantList<S<V...>, T>
	{
		template <typename Variant_t>
		static inline void Init(Variant_t& v, unsigned _idx) noexcept
		{
			static void(*x[])(Variant_t&) noexcept {
				[](Variant_t& v) noexcept { v = typename Ut::MetaExtract_t<V, T>::type{}; }...
			};

			x[_idx](v);
		}
	};

	template <typename, typename>
	struct ShaderTypeGetAux;

	template <template<unsigned...> class S, unsigned ... V, typename T>
	struct ShaderTypeGetAux<S<V...>, T>
	{
		static inline Gfx::eUniform_t(*get[])(void) { [](void) { 
			return static_cast<::Gfx::eUniform_t>(Ut::MetaExtract_t<V, T>::value); 
		} ... };

		static inline constexpr auto Get(size_t _nIdx) noexcept
		{
			return get[_nIdx]();
		}
	};

	template <typename T>
	struct VariantList : GenVariantList<
		Ut::MetaMakeRange_t<Ut::SizeofList<T>::value>, T
	> {};

	template <typename T>
	struct ShaderTypeGet : ShaderTypeGetAux<
		Ut::MetaMakeRange_t<Ut::SizeofList<T>::value>, T
	> {};
}

void Dystopia::Renderer::ShaderField()
{
	auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();
	//static bool debug = false;

	if (EGUI::Display::EmptyBox("Shader", 150, (mpShader) ? mpShader->GetName().c_str() : "", true))
	{

	}
	if (unsigned *t = EGUI::Display::StartPayloadReceiver<unsigned>(EGUI::SHADER))
	{
		auto const & allShaders = Dystopia::CORE::Get<Dystopia::ShaderSystem>()->GetAllShaders();
		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand<Renderer, Shader*>(&Renderer::SetShader, mpShader),
										   cmd->MakeFnCommand<Renderer, Shader*>(&Renderer::SetShader, &allShaders[*t]));
		EGUI::Display::EndPayloadReceiver();
	}

	//char const* str = "No Shader";
	//if (mpShader)
	//{
	//	str = mpShader->GetName().c_str();
	//}
	//if (EGUI::Display::EmptyBox("Shader", 150, str, true))
	//{
	//	debug = !debug;
	//}

	//OString buffer{ str };
	//buffer += "                               ";
	//if (debug && EGUI::Display::TextField("Manual", buffer, true, 150))
	//{
	//	if (auto pShader = CORE::Get<ShaderSystem>()->GetShader(buffer.c_str()))
	//	{
	//		SetShader(pShader);
	//	}
	//}
	//if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::FILE))
	//{
	//	EGUI::Display::EndPayloadReceiver();
	//}

	auto& vars = mpShader->GetVariables();
	mOverrideNames.clear();
	mOverrideNames.reserve(vars.size() + 1);

	mOverrideNames.EmplaceBack("Select");
	for (auto& e : vars)
		mOverrideNames.EmplaceBack(e.first.c_str());

	int sele = 0;
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
				if (ShaderTypeGet<ShaderTypeList>::Get(n) == vars[sele - 1].second)
				{
					type = vars[sele - 1].second;
					VariantList<ShaderTypeList>::Init(myVariant, n);
					break;
				}

			mOverride.EmplaceBack(mOverrideNames[sele], type, Ut::Move(myVariant));
		}
	}

	int n = 0;
	for (auto& e : Ut::Range(mOverride).Reverse())
	{
		EGUI::PushID(++n);

		e.Get<2>().Visit(UIVisitor{ e.Get<0>() });
		ImGui::SameLine();
		if (EGUI::Display::IconCross("ClearOverrides", 9.f))
			mOverride.FastRemove(&e);

		EGUI::PopID();
	}
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
	EGUI::Display::VectorFields("", &_variant, 0.1f, -FLT_MAX, FLT_MAX, 50.f, true);
}
//template<>
//inline void Dystopia::Renderer::UIVisitor::operator()(std::pair<Texture*, int>& _variant)
//{
//	::Editor::File *t = nullptr;
//	EGUI::Display::LabelWrapped(strName.c_str());
//
//	EGUI::Display::EmptyBox("Texture", 150, (_variant.first) ? _variant.first->GetName().c_str() : "-empty-", true);
//	t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PNG);
//	if (t)  EGUI::Display::EndPayloadReceiver();
//
//	if (!t)
//	{
//		t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::DDS);
//		if (t) EGUI::Display::EndPayloadReceiver();
//	}
//
//	if (!t)
//	{
//		t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::BMP);
//		if (t) EGUI::Display::EndPayloadReceiver();
//	}
//
//	if (t)
//	{
//		_variant.first = CORE::Get<GraphicsSystem>()->LoadTexture(t->mPath.c_str());
//	}
//
//	EGUI::Display::DragInt(strName.c_str(), &_variant.second, 1, -INT_MAX, INT_MAX, true);
//}

#endif
