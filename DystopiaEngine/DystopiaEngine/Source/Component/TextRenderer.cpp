/* HEADER *********************************************************************************/
/*!
\file	TextRenderer.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Text Renderer.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/TextRenderer.h"
#include "System/Driver/Driver.h"

#include "System/Graphics/Mesh.h"
#include "System/Graphics/MeshSystem.h"
#include "System/Graphics/VertexDefs.h"
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/CharSpace.h"
#include "System/Graphics/FontSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/GraphicsSystem.h"

#include "Object/GameObject.h"

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/Payloads.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#endif

#include <GL/glew.h>
#include "Editor/EditorClipboard.h"


Dystopia::TextRenderer::TextRenderer(void) noexcept
	: Renderer{}, mPrevState(mCurrState), mpData{ nullptr },
	mText {}, mnAnchorX{ 0 }, mnAnchorY{ 0 }, mColor{ 1.f, 1.f, 1.f, 1.f }, mVerts{}
{
	auto pMeshSys = CORE::Get<MeshSystem>();
	auto pBaseMesh = pMeshSys->GetRaw(pMeshSys->GenerateRaw());
	mpMesh = pMeshSys->CreateMesh(pBaseMesh, 0);

	pBaseMesh->BuildEmpty<VertexBuffer, UVBuffer, IndexBuffer>();
}

Dystopia::TextRenderer::TextRenderer(const TextRenderer& _rhs) noexcept
	: Renderer{ _rhs }, mCurrState(_rhs.mCurrState), mPrevState(_rhs.mCurrState),
	mpData{ _rhs.mpData }, mText{ _rhs.mText }, mnAnchorX{ _rhs.mnAnchorX }, mnAnchorY{ _rhs.mnAnchorY }
	,mColor{ _rhs.mColor },mDefaultCol(_rhs.mDefaultCol),mHoverCol(_rhs.mHoverCol),mClickColor(_rhs.mClickColor),mDisabledColor(_rhs.mDisabledColor)
	,mfTintPerc(_rhs.mfTintPerc), mVerts{ _rhs.mVerts}
{
	//mTextureFields = Ut::Move(_rhs.mTextureFields);
}

void Dystopia::TextRenderer::Awake(void)
{
	mpShader = CORE::Get<ShaderSystem>()->GetShader("Font Shader");

	if (mpData)
		RegenMesh();
}

void Dystopia::TextRenderer::Draw(void) const noexcept
{
	GetShader()->UploadUniform("vColor", mColor*mfTintPerc);
	Renderer::Draw();
}


void Dystopia::TextRenderer::SetText(const char* _str)
{
	mText.clear();
	mText = _str;
	RegenMesh();
}

void Dystopia::TextRenderer::SetText(const std::string& _str)
{
	SetText(_str.c_str());
}

void Dystopia::TextRenderer::SetFont(const char* _strPath)
{
	SetFont(std::string{ _strPath });
}

void Dystopia::TextRenderer::SetFont(const std::string& _strPath)
{
	if (_strPath != "")
	{
		mpData = CORE::Get<FontSystem>()->LoadFont(_strPath);
		Renderer::SetTexture(mpData->mpAtlas->GetInternal());
		RegenMesh();
	}
	else
	{
		mpData    = nullptr;
		Renderer::SetTexture(nullptr);
	}
}

void Dystopia::TextRenderer::SetFontSize(float _sz)
{
	float z = GetOwner()->GetComponent<Transform>()->GetGlobalScale().z;
	GetOwner()->GetComponent<Transform>()->SetGlobalScale(_sz, _sz, z);
}

void Dystopia::TextRenderer::SetColor(const Math::Vec3D& _rgb)
{
	const float alpha = mColor.w;
	mColor = _rgb;
	mColor.w = alpha;
}

void Dystopia::TextRenderer::SetColorA(const Math::Vec4 _rgba)
{
	mColor = _rgba;
}

void Dystopia::TextRenderer::SetColor(float r, float g, float b)
{
	SetColor(Math::Vec3D{r,g,b});
}

void Dystopia::TextRenderer::SetColor(float r, float g, float b, float a)
{
	SetColorA(Math::Vec4{ r,g,b,a });
}

void Dystopia::TextRenderer::SetAlpha(float _a)
{
	mColor.w = _a;
}

void Dystopia::TextRenderer::SetAlphaPerc(float _perc)
{
	if (_perc <= 0.f) _perc = 0.f;
	else if (_perc >= 1.f) _perc = 1.0f;

	mfTintPerc = _perc;
}

void Dystopia::TextRenderer::RegenMesh(void)
{
	AutoArray<Gfx::Vertex> verts;
	AutoArray<Gfx::UV> uvs;
	AutoArray<short> indices;

	auto& atlas = mpData->mpAtlas->GetAllSections();

	short index = 0;

	float x = .0f, y =-1.0f, mx = .0f;
	constexpr float scale = 1.f / 70.f;
	for (auto b = mText.begin(), e = mText.end(); b != e; ++b)
	{
		if (*b == '\\')
		{
			if (++b == e)
				break;

			if (*b == 'n')
			{
				mx = Ut::Max(x, mx);
				x = .0f;
				y -= 1.1f;
				continue;
			}

			if (*b != '\\')
				continue;
		}

		auto const n = *b - ' ';
		auto& ch = mpData->mSpaces[n];

		float const dy = ch.mnBearingY * scale;
		float const dx = ch.mnBearingX * scale;
		float const w  = ch.mnWidth * scale + dx;
		float const h  = dy - ch.mnHeight * scale;
		
		verts.EmplaceBack(x + dx, y + dy, .0f);
		verts.EmplaceBack(x + dx, y + h, .0f);
		verts.EmplaceBack(x + w , y + h, .0f);
		verts.EmplaceBack(x + w , y + dy, .0f);

		uvs.EmplaceBack(atlas[n].uStart, atlas[n].vStart);
		uvs.EmplaceBack(atlas[n].uStart, atlas[n].vEnd);
		uvs.EmplaceBack(atlas[n].uEnd, atlas[n].vEnd);
		uvs.EmplaceBack(atlas[n].uEnd, atlas[n].vStart);

		indices.Insert(index);
		indices.Insert(index + 1);
		indices.Insert(index + 2);
		indices.Insert(index);
		indices.Insert(index + 2);
		indices.Insert(index + 3);

		index += 4;
		x += ch.mnAdvance * scale;
	}

	float(*alignOp[])(float) {
		[](float) { return .0f; },
		[](float x) { return x * .5f; },
		[](float x) { return x; }
	};

	mx = Ut::Max(x, mx);
	x = alignOp[mnAnchorX](mx);
	y = alignOp[mnAnchorY](y);
	for (auto& e : verts)
	{
		e.x -= x;
		e.y -= y;
	}

	mpMesh->UpdateBuffer<VertexBuffer>(verts);
	mpMesh->UpdateBuffer<UVBuffer>(uvs);
	mpMesh->UpdateBuffer<IndexBuffer>(indices);
	mpMesh->SetIndices(static_cast<unsigned>(indices.size()), 0);

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
}


Dystopia::TextRenderer* Dystopia::TextRenderer::Duplicate(void) const
{
	return static_cast<ComponentDonor<TextRenderer>*>(
		EngineCore::GetInstance()->Get<GraphicsSystem>()
	)->RequestComponent(*this);
}

void Dystopia::TextRenderer::Serialise(TextSerialiser& _out) const
{
	Component::Serialise(_out);

	_out << mpData->mstrName;
	_out << mText;
	_out << mColor;
	_out << mnAnchorX;
	_out << mnAnchorY;
	_out << static_cast<int>(mCurrState);
	_out << static_cast<int>(mPrevState);
	_out << mDefaultCol;
	_out << mHoverCol;
	_out << mClickColor;
	_out << mDisabledColor;
	_out << mfTintPerc;
}

void Dystopia::TextRenderer::Unserialise(TextSerialiser& _in)
{
	std::string font;
	int currState, prevState;
	Component::Unserialise(_in);

	_in >> font;
	_in >> mText;
	_in >> mColor;
	_in >> mnAnchorX;
	_in >> mnAnchorY;
	_in >> currState;
	_in >> prevState;
	_in >> mDefaultCol;
	_in >> mHoverCol;
	_in >> mClickColor;
	_in >> mDisabledColor;
	_in >> mfTintPerc;

	mCurrState = static_cast<TextEditor::eTextState>(currState);
	mPrevState = static_cast<TextEditor::eTextState>(prevState);
	ApplyChanges();
	SetFont(font);

}

void Dystopia::TextRenderer::ChangeState(TextEditor::eTextState _state)
{
	const auto temp = mCurrState;
	mCurrState = _state;
	mPrevState = temp;

	ApplyChanges();
}

void Dystopia::TextRenderer::RevertState()
{
	ChangeState(mPrevState);

	ApplyChanges();
}

void Dystopia::TextRenderer::ApplyChanges()
{
	switch (mCurrState)
	{
	case TextEditor::DEFAULT:
	{
		mColor = mDefaultCol;
		break;
	}
	case TextEditor::ON_CLICK:
	{
		mColor = mClickColor;
		break;
	}
	case TextEditor::ON_HOVER:
	{
		mColor = mHoverCol;
		break;
	}
	case TextEditor::DISABLED:
	{
		mColor = mDisabledColor;
		break;
	}
	default:;
	}
}

#if EDITOR
void Dystopia::TextRenderer::EditorUI(void) noexcept
{

	auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();

	bool bRegenMesh = false;
	static char buf[512]{ };
	static std::string anchorX[3]{ "Left", "Center", "Right" };
	static std::string anchorY[3]{ "Top", "Center", "Bottom" };
	namespace UI = EGUI::Display;

	if (UI::DropDownSelection("Anchor X", mnAnchorX, anchorX, 100))
	{
		bRegenMesh = true;
	}
	if (UI::DropDownSelection("Anchor Y", mnAnchorY, anchorY, 100))
	{
		bRegenMesh = true;
	}

	*(Ut::Copy(mText, &buf[0])) = '\0';

	if (EGUI::Display::TextField("Text ", buf, 512, true, 225, false))
	{
		mText.clear();
		mText = static_cast<const char *>(buf);

		bRegenMesh = true;
	}

	auto trans = GetOwner()->GetComponent<Transform>();
	auto scale = trans->GetGlobalScale();
	float z = scale.z;
	int tempF = static_cast<int>(scale.x);

	switch (EGUI::Display::SliderInt("Font Size", &tempF, 0, 1024))
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		break;
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eENTER:
		break;
	case EGUI::eDragStatus::eDRAGGING:
		trans->SetGlobalScale(static_cast<float>(tempF), static_cast<float>(tempF), z);
		break;
	default:
		break;
	}

	EGUI::Display::EmptyBox("Font ", 200, (mpData) ? mpData->mstrName.c_str() : "-empty-", true);

	::Editor::File* t = nullptr;
	if (auto i = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::TTF))
	{
		t = i;
		EGUI::Display::EndPayloadReceiver();
	}

	if (t)
	{
		char const* strName = "";

		if (mpData)
			strName = mpData->mpAtlas->GetName().c_str();

		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand<TextRenderer, const char*>(&TextRenderer::SetFont, strName),
										   cmd->MakeFnCommand<TextRenderer, const char*>(&TextRenderer::SetFont, t->mName.c_str()));
	}

	EGUI::SameLine();
	if (EGUI::Display::IconCross("Clear", 8.f) && mpData)
	{
		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand<TextRenderer, const char*>(&TextRenderer::SetFont, mpData->mpAtlas->GetName().c_str()),
										   cmd->MakeFnCommand<TextRenderer, const char*>(&TextRenderer::SetFont, ""));
	}

	int currIndex = static_cast<int>(mCurrState);
	static std::string textStates[4]{ "Default",  "Hover", "Clicked" , "Disabled" };

	if (EGUI::Display::DropDownSelection("Current State", currIndex, textStates))
	{
		ChangeState(static_cast<TextEditor::eTextState>(currIndex));
	}

	if (ImGui::ColorEdit4("Default Color", &mDefaultCol[0], (ImGuiColorEditFlags_NoSidePreview| ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions)))
	{
		ApplyChanges();
	}

	if (ImGui::ColorEdit4("Hover Color", &mHoverCol[0], (ImGuiColorEditFlags_NoSidePreview	| ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions)))
	{
		ApplyChanges();
	}

	if (ImGui::ColorEdit4("Clicked Color", &mClickColor[0], (ImGuiColorEditFlags_NoSidePreview	| ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions)))
	{
		ApplyChanges();
	}

	if (ImGui::ColorEdit4("Disabled Color", &mDisabledColor[0], (ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions)))
	{
		ApplyChanges();
	}

	if (bRegenMesh && mpData)
		RegenMesh();
}
#endif



