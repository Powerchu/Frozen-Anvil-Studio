/* HEADER *********************************************************************************/
/*!
\file	ParticleEditor.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/ParticleEditor.h"
#include "Editor/EditorMain.h"
#include "Editor/Payloads.h"
#include "Editor/EGUI.h"
#include "Editor/RuntimeMeta.h"

#include "System/Driver/Driver.h"
#include "System/Particle/ParticleAffector.h"
#include "System/Particle/SpawnAffector.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"

#include "System/Particle/Emitter.h"

#include <string>

namespace
{
	static inline constexpr Dystopia::ParticleAffector* DUMMY = nullptr;
}

Editor::ParticleEditor::ParticleEditor(void)
	: mLabel{ "Particle System" },
	mnObjectID{ 0 },
	mpObject{ nullptr },
	mArrUsedAffectors{}
{
}

Editor::ParticleEditor::~ParticleEditor(void)
{
}

void Editor::ParticleEditor::Load(void)
{
	mArrUsedAffectors.reserve(Dystopia::AffectorGet::size);
}

bool Editor::ParticleEditor::Init(void)
{
	return true;
}

void Editor::ParticleEditor::Update(float)
{
}

void Editor::ParticleEditor::EditorUI(void)
{
	EGUI::Display::EmptyBox("Game Object: ", 200.f, mnObjectID ? std::to_string(mnObjectID).c_str() : "- empty -");
	if (const auto id = EGUI::Display::StartPayloadReceiver<uint64_t>(EGUI::GAME_OBJ))
	{
		mnObjectID = Dystopia::CORE::Get<Dystopia::SceneSystem>()->FindGameObject(*id) ? *id : 0;
		EGUI::Display::EndPayloadReceiver();
	}

	if (!mnObjectID || !ValidateEmitter())
	{
		mpObject = nullptr;
		EGUI::Display::HorizontalSeparator();
		return;
	}

	EGUI::Display::HorizontalSeparator();

	AddAffectors();

}

void Editor::ParticleEditor::Shutdown(void)
{
}

void Editor::ParticleEditor::Message(eEMessage)
{
}

void Editor::ParticleEditor::SaveSettings(Dystopia::TextSerialiser&) const
{
}

void Editor::ParticleEditor::LoadSettings(Dystopia::TextSerialiser&)
{
}

HashString Editor::ParticleEditor::GetLabel(void) const
{
	return mLabel;
}

bool Editor::ParticleEditor::ValidateEmitter(void)
{
	if (auto object = Dystopia::CORE::Get<Dystopia::SceneSystem>()->FindGameObject(mnObjectID))
	{
		if (!object->GetComponent<Dystopia::ParticleEmitter>())
		{
			EGUI::Display::Label("No Emitter found on Game Object - Please add the component!");
			return false;
		}
		mpObject = object;
		return true;
	}

	EGUI::Display::Label("Game Object ID not found");
	return false;
}

void Editor::ParticleEditor::AddAffectors(void)
{
	//Dystopia::Emitter* pEmitter = mpObject->GetComponent<Dystopia::ParticleEmitter>();
	//
	//const auto& existingSpawnAffectors = pEmitter->GetSpawnAffectors();
	//const auto& existingUpdateAffectors = pEmitter->GetUpdateAffectors();
	//const auto& existingFixedUpdateAffectors = pEmitter->GetFixedUpdateAffectors();
	//
	//mArrUsedAffectors.clear();
	//
	//for (const auto & elem : existingSpawnAffectors)
	//	UpdateAffectorUsed(elem.GetID());
	//for (const auto & elem : existingUpdateAffectors)
	//	UpdateAffectorUsed(elem.GetID());
	//for (const auto & elem : existingFixedUpdateAffectors)
	//	UpdateAffectorUsed(elem.GetID());
	//
	//if (EGUI::Display::StartTreeNode("List of Affectors"))
	//{
	//	ListOfAffectors();
	//	EGUI::Display::EndTreeNode();
	//}
	//
	//if (EGUI::Display::StartTreeNode("Affectors Attached to Emitter"))
	//{
	//	EmitterAffectors();
	//	EGUI::Display::EndTreeNode();
	//}
}

void Editor::ParticleEditor::UpdateAffectorUsed(int _id)
{
	if (AffectorIsUsed(_id))
		return;

	mArrUsedAffectors.Insert(_id);
}

bool Editor::ParticleEditor::AffectorIsUsed(int _id)
{
	for (const auto & elem : mArrUsedAffectors)
		if (elem == _id)
			return true;
	return false;
}

void Editor::ParticleEditor::ListOfAffectors(void)
{
	//static Dystopia::AffectorGet affectorGet;
	//static const auto affectorNames = Dystopia::AffectorUI<Dystopia::AffectorList>::GetUIName();
	//
	//for (int i = 0; i < static_cast<int>(Dystopia::AffectorGet::size); i++)
	//{
	//	EGUI::PushID(i);
	//	if (AffectorIsUsed(i))
	//	{
	//		EGUI::Display::Label((DUMMY->*affectorNames[i])());
	//		EGUI::SameLine();
	//		EGUI::Display::Label("already added!");
	//	}
	//	else
	//	{
	//		EGUI::Display::Label((DUMMY->*affectorNames[i])());
	//		EGUI::SameLine(5.f, 150.f);
	//		if (EGUI::Display::Button("Add", Math::Vec2{ 100.f, 24.f }))
	//			AddAffector(i);
	//	}
	//	EGUI::PopID();
	//}
	//EGUI::Display::HorizontalSeparator();
}

void Editor::ParticleEditor::AddAffector(int )
{
	//Dystopia::Emitter* pEmitter = mpObject->GetComponent<Dystopia::Emitter>();
	//static Dystopia::AffectorGet affectorGet;
	//affectorGet.Get(_id, *pEmitter);
}

void Editor::ParticleEditor::EmitterAffectors(void)
{
	//Dystopia::Emitter* pEmitter = mpObject->GetComponent<Dystopia::Emitter>();
	//
	//static Dystopia::AffectorGet affectorGet;
	//static const auto affectorNames = Dystopia::AffectorUI<Dystopia::AffectorList>::GetUIName();
	//static const auto affectorUI = Dystopia::AffectorUI<Dystopia::AffectorList>::GetPtrsToUIFunction();
	//
	//auto& existingSpawnAffectors = pEmitter->GetSpawnAffectors();
	//auto& existingUpdateAffectors = pEmitter->GetUpdateAffectors();
	//auto& existingFixedUpdateAffectors = pEmitter->GetFixedUpdateAffectors();
	//
	////ImGui::BeginGroup();
	////ImGui::Columns(3, "Columns Affectors");
	//if (EGUI::Display::StartTreeNode("Spawners"))
	//{
	//	ImGui::BeginGroup();
	//	auto count = static_cast<int>(existingSpawnAffectors.size());
	//	ImGui::Columns(count ? count : 1);
	//	for (int i = 0; i < existingSpawnAffectors.size(); ++i)
	//	{
	//		EGUI::PushID(static_cast<int>(i));
	//		bool open = EGUI::Display::StartTreeNode((DUMMY->*affectorNames[existingSpawnAffectors[i].GetID()])());
	//		bool toRemove = false;
	//		if (ImGui::BeginPopupContextItem())
	//		{
	//			if (EGUI::Display::SelectableTxt("Remove"))
	//				toRemove = true;
	//			ImGui::EndPopup();
	//		}
	//		if (open)
	//		{
	//			(existingSpawnAffectors[i].*affectorUI[existingSpawnAffectors[i].GetID()])();
	//			EGUI::Display::EndTreeNode();
	//		}
	//		EGUI::PopID();
	//		if (toRemove)
	//			existingSpawnAffectors.FastRemove(i--);
	//		ImGui::NextColumn();
	//	}
	//	ImGui::NextColumn();
	//	ImGui::EndGroup();
	//	EGUI::Display::EndTreeNode();
	//}
	//ImGui::Columns(1);
	//EGUI::Display::HorizontalSeparator();
	////ImGui::NextColumn();
	//if (EGUI::Display::StartTreeNode("Updaters"))
	//{
	//	ImGui::BeginGroup();
	//	auto count = static_cast<int>(existingUpdateAffectors.size());
	//	ImGui::Columns(count ? count : 1);
	//	for (int i = 0; i < existingUpdateAffectors.size(); ++i)
	//	{
	//		EGUI::PushID(static_cast<int>(i));
	//		bool open = EGUI::Display::StartTreeNode((DUMMY->*affectorNames[existingUpdateAffectors[i].GetID()])());
	//		bool toRemove = false;
	//		if (ImGui::BeginPopupContextItem())
	//		{
	//			if (EGUI::Display::SelectableTxt("Remove"))
	//				toRemove = true;
	//			ImGui::EndPopup();
	//		}
	//		if (open)
	//		{
	//			(existingUpdateAffectors[i].*affectorUI[existingUpdateAffectors[i].GetID()])();
	//			EGUI::Display::EndTreeNode();
	//		}
	//		EGUI::PopID();
	//		if (toRemove)
	//			existingUpdateAffectors.FastRemove(i--);
	//		ImGui::NextColumn();
	//	}
	//	ImGui::NextColumn();
	//	ImGui::EndGroup();
	//	EGUI::Display::EndTreeNode();
	//}
	//ImGui::Columns(1);
	//EGUI::Display::HorizontalSeparator();
	////ImGui::NextColumn();
	//if (EGUI::Display::StartTreeNode("Fixers"))
	//{
	//	ImGui::BeginGroup();
	//	auto count = static_cast<int>(existingFixedUpdateAffectors.size());
	//	ImGui::Columns(count ? count : 1);
	//	for (int i = 0; i < existingFixedUpdateAffectors.size(); ++i)
	//	{
	//		EGUI::PushID(static_cast<int>(i));
	//		bool open = EGUI::Display::StartTreeNode((DUMMY->*affectorNames[existingFixedUpdateAffectors[i].GetID()])());
	//		bool toRemove = false;
	//		if (ImGui::BeginPopupContextItem())
	//		{
	//			if (EGUI::Display::SelectableTxt("Remove"))
	//				toRemove = true;
	//			ImGui::EndPopup();
	//		}
	//		if (open)
	//		{
	//			(existingFixedUpdateAffectors[i].*affectorUI[existingFixedUpdateAffectors[i].GetID()])();
	//			EGUI::Display::EndTreeNode();
	//		}
	//		EGUI::PopID();
	//		if (toRemove)
	//			existingFixedUpdateAffectors.FastRemove(i--);
	//		ImGui::NextColumn();
	//	}
	//	ImGui::NextColumn();
	//	ImGui::EndGroup();
	//	EGUI::Display::EndTreeNode();
	//}
	//ImGui::Columns(1);
	////ImGui::EndGroup();
	////ImGui::Columns(1);
}

#endif 















