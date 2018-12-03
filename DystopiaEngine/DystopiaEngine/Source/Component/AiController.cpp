/* HEADER *********************************************************************************/
/*!
\file	AiController.cpp
\author AARON CHU MING SAN (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
Wrapper class for AI Tree functions.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/AiController.h"		// File header

#include "System/AI/AISystem.h"
#include "System/Driver/Driver.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"

#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

#include "Math/MathUtility.h"
#include "Editor/RuntimeMeta.h"

#if EDITOR
#include "Editor/EGUI.h"
#endif

namespace Dystopia
{
	AiController::AiController(void)
		: mpBlackboard(Ctor::CreateShared<Blackboard>()), mNodeStatus(Node::eStatus::INVALID)
	{
	};

	/*void AiController::Load()
	{
	}*/

	void AiController::Awake()
	{
		bTree.SetBlackboard(mpBlackboard);

		/*const auto blackboard = mpBlackboard;

		NeuralTree::Builder()
			.composite<Selector>()
				.decorator<Succeeder>()
					.task<EatFood>(blackboard)
				.end()
			.end()
		.Build(bTree);*/
	}

	void AiController::Init()
	{
		
	}

	/*void AiController::Unload()
	{
	}*/

	AiController* AiController::Duplicate() const
	{
		const auto cc = EngineCore::GetInstance()->GetSystem<AISystem>()->RequestComponent(*this);
		return cc;
	}

	void AiController::Serialise(TextSerialiser& _out) const
	{
		_out.InsertStartBlock("AI Controller");
		Component::Serialise(_out);
		mpBlackboard->Serialise(_out);
		_out.InsertEndBlock("AI Controller");
	}

	void AiController::Unserialise(TextSerialiser& _in)
	{
		_in.ConsumeStartBlock();
		Component::Unserialise(_in);
		mpBlackboard->Unserialise(_in);
		_in.ConsumeEndBlock();
	}

	void AiController::Update(float)
	{
		if (bTree.IsValidTree())
		{
			mNodeStatus = bTree.Update();
		}
	}

	void AiController::LateUpdate(float) const
	{

	}

	BehaviourTree& AiController::GetTreeAsRef()
	{
		return bTree;
	}

	BehaviourTree::Ptr AiController::GetTreeAsPtr()
	{
		return Ctor::CreateShared<BehaviourTree>(bTree);
	}

	Blackboard::Ptr AiController::GetBlackboard() const
	{
		return bTree.GetBlackboard();
	}

	void AiController::ClearTree(void)
	{
		if (bTree.GetRoot() != nullptr)
			bTree.GetRoot() = nullptr;
	}

	
#if EDITOR
	void AiController::EditorUI() noexcept
	{
		EditorCurrentStatus();
		AddKeyToMap();
		EGUI::SameLine(0);
		if (EGUI::Display::CollapsingHeader("Blackboard", false))
		{
			EGUI::Indent(10);
			EditorFunctionNode();
			EditorBoolNode();
			EditorIntNode();
			EditorFloatNode();
			EditorDoubleNode();
			EditorVectorNode();
			EditorObjectNode();
			EditorStringNode();
			EGUI::UnIndent(10);

			if (EGUI::Display::Button("Clear All", {256,24}))
			{
				mpBlackboard->ClearAll();
			}
		}

		EditorTreeView();

	}

	void AiController::EditorFunctionNode()
	{
		const auto map = mpBlackboard->GetMapToPointers();
		const size_t map_size = map.size();

		
		EGUI::Display::Label(std::to_string(map_size).c_str());
		EGUI::SameLine();

		if (EGUI::Display::StartTreeNode("Functions", nullptr, false, false, false, true))
		{
			std::vector<std::string> keys(map_size);
			std::vector<void*> values(map_size);

			transform(map.begin(), map.end(), keys.begin(), AI::key_select);
			transform(map.begin(), map.end(), values.begin(), AI::value_select);

			EGUI::Display::EndTreeNode();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove Key"))
			{
				mpBlackboard->ClearSingle(ePointer);
			}
			ImGui::EndPopup();
		}
	}

	void AiController::EditorStringNode()
	{
		const auto map = mpBlackboard->GetMapToNames();
		const size_t map_size = map.size();

		
		EGUI::Display::Label(std::to_string(map_size).c_str());
		EGUI::SameLine();

		if (EGUI::Display::StartTreeNode("Strings", nullptr, false, false, false, true))
		{
			std::vector<std::string> keys(map_size);
			std::vector<std::string> values(map_size);

			transform(map.begin(), map.end(), keys.begin(), AI::key_select);
			transform(map.begin(), map.end(), values.begin(), AI::value_select);

			for (unsigned i = 0; i < map_size; ++i)
			{
				EGUI::PushID(i + static_cast<unsigned>(map_size));
				const auto str = values[i];
				std::string _nName{ std::to_string(i) + ".K" };
				EGUI::Display::EmptyBox(_nName.c_str(), 80.f, keys[i].c_str());
				EGUI::SameLine(AI::DEFAULT_ALIGN);
				EGUI::ChangeAlignmentYOffset(0);
				EGUI::Display::EmptyBox("V", AI::BTN_SZ, str.c_str());
				EGUI::ChangeAlignmentYOffset();
				EGUI::PopID();
			}

			EGUI::Display::EndTreeNode();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove Key"))
			{
				mpBlackboard->ClearSingle(eStrings);
			}
			ImGui::EndPopup();
		}
	}

	void AiController::EditorObjectNode()
	{
		const auto map = mpBlackboard->GetMapToObjs();
		const size_t map_size = map.size();

		
		EGUI::Display::Label(std::to_string(map_size).c_str());
		EGUI::SameLine();

		if (EGUI::Display::StartTreeNode("Objects", nullptr, false, false, false, true))
		{
			std::vector<std::string> keys(map_size);
			std::vector<uint64_t> values(map_size);

			transform(map.begin(), map.end(), keys.begin(), AI::key_select);
			transform(map.begin(), map.end(), values.begin(), AI::value_select);

			for (unsigned i = 0; i < map_size; ++i)
			{
				EGUI::PushID(i + static_cast<unsigned>(map_size));
				const auto val = values[i];
				std::string _nName{ std::to_string(i) + ".K" };
				EGUI::Display::EmptyBox(_nName.c_str(), 80.f, static_cast<const char*>(keys[i].c_str()));
				EGUI::SameLine(AI::DEFAULT_ALIGN);
				EGUI::ChangeAlignmentYOffset(0);
				if (const auto obj = EngineCore::Get<SceneSystem>()->FindGameObject(val))
				{
					const auto gObjName = obj->GetName();
					EGUI::Display::EmptyBox("V", AI::BTN_SZ, gObjName.c_str());
				}
				else
				{
					EGUI::Display::EmptyBox("V", AI::BTN_SZ, "Invalid");
				}
				EGUI::ChangeAlignmentYOffset();
				EGUI::PopID();
			}

			EGUI::Display::EndTreeNode();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove Key"))
			{
				mpBlackboard->ClearSingle(eObject);
			}
			ImGui::EndPopup();
		}
	}

	void AiController::EditorVectorNode()
	{
		const auto map = mpBlackboard->GetMapToVectors();
		const size_t map_size = map.size();

		
		EGUI::Display::Label(std::to_string(map_size).c_str());
		EGUI::SameLine();

		if (EGUI::Display::StartTreeNode("Vectors", nullptr, false, false, false, true))
		{
			std::vector<std::string> keys(map_size);
			std::vector<Math::Vector4> values(map_size);

			transform(map.begin(), map.end(), keys.begin(), AI::key_select);
			transform(map.begin(), map.end(), values.begin(), AI::value_select);

			for (unsigned i = 0; i < map_size; ++i)
			{
				EGUI::PushID(i + static_cast<unsigned>(map_size));
				const auto val = values[i];
				std::string _nName{ std::to_string(i) + ".K" };
				std::string _nVal{ "X:" + std::to_string(static_cast<int>(val.x)) +
					" Y:" + std::to_string(static_cast<int>(val.y)) +
					" Z:" + std::to_string(static_cast<int>(val.z)) +
					" W:" + std::to_string(static_cast<int>(val.w))
				};
				EGUI::Display::EmptyBox(_nName.c_str(), 80.f, static_cast<const char*>(keys[i].c_str()));
				EGUI::SameLine(AI::DEFAULT_ALIGN);
				EGUI::ChangeAlignmentYOffset(0);
				EGUI::Display::EmptyBox("V", AI::BTN_SZ, _nVal.c_str());
				EGUI::ChangeAlignmentYOffset();
				EGUI::PopID();
			}

			EGUI::Display::EndTreeNode();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove Key"))
			{
				mpBlackboard->ClearSingle(eVector);
			}
			ImGui::EndPopup();
		}
	}

	void AiController::EditorDoubleNode()
	{
		const auto map = mpBlackboard->GetMapToDoubles();
		const size_t map_size = map.size();

		
		EGUI::Display::Label(std::to_string(map_size).c_str());
		EGUI::SameLine();

		if (EGUI::Display::StartTreeNode("Doubles", nullptr, false, false, false, true))
		{
			std::vector<std::string> keys(map_size);
			std::vector<double> values(map_size);

			transform(map.begin(), map.end(), keys.begin(), AI::key_select);
			transform(map.begin(), map.end(), values.begin(), AI::value_select);

			for (unsigned i = 0; i < map_size; ++i)
			{
				EGUI::PushID(i + static_cast<unsigned>(map_size));
				const auto val = values[i];
				std::string _nName{ std::to_string(i) + ".K" };
				EGUI::Display::EmptyBox(_nName.c_str(), 80.f, static_cast<const char*>(keys[i].c_str()));
				EGUI::SameLine(AI::DEFAULT_ALIGN);
				EGUI::ChangeAlignmentYOffset(0);
				EGUI::Display::EmptyBox("V", AI::BTN_SZ, std::to_string(val).c_str());
				EGUI::ChangeAlignmentYOffset();
				EGUI::PopID();
			}

			EGUI::Display::EndTreeNode();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove Key"))
			{
				mpBlackboard->ClearSingle(eDouble);
			}
			ImGui::EndPopup();
		}
	}

	void AiController::EditorFloatNode()
	{
		const auto map = mpBlackboard->GetMapToFloats();
		const size_t map_size = map.size();

		
		EGUI::Display::Label(std::to_string(map_size).c_str());
		EGUI::SameLine();

		if (EGUI::Display::StartTreeNode("Floats", nullptr, false, false, false, true))
		{
			std::vector<std::string> keys(map_size);
			std::vector<float> values(map_size);

			transform(map.begin(), map.end(), keys.begin(), AI::key_select);
			transform(map.begin(), map.end(), values.begin(), AI::value_select);

			for (unsigned i = 0; i < map_size; ++i)
			{
				EGUI::PushID(i + static_cast<unsigned>(map_size));
				const auto val = values[i];
				std::string _nName{ std::to_string(i) + ".K" };
				EGUI::Display::EmptyBox(_nName.c_str(), 80.f, static_cast<const char*>(keys[i].c_str()));
				EGUI::SameLine(AI::DEFAULT_ALIGN);
				EGUI::ChangeAlignmentYOffset(0);
				EGUI::Display::EmptyBox("V", AI::BTN_SZ, std::to_string(val).c_str());
				EGUI::ChangeAlignmentYOffset();
				EGUI::PopID();
			}

			EGUI::Display::EndTreeNode();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove Key"))
			{
				mpBlackboard->ClearSingle(eFloat);
			}
			ImGui::EndPopup();
		}
	}

	void AiController::EditorIntNode()
	{
		const auto map = mpBlackboard->GetMapToInts();
		const size_t map_size = map.size();

		
		EGUI::Display::Label(std::to_string(map_size).c_str());
		EGUI::SameLine();

		if (EGUI::Display::StartTreeNode("Ints", nullptr, false, false, false, true))
		{
			std::vector<std::string> keys(map_size);
			std::vector<int> values(map_size);

			transform(map.begin(), map.end(), keys.begin(), AI::key_select);
			transform(map.begin(), map.end(), values.begin(), AI::value_select);

			for (unsigned i = 0; i < map_size; ++i)
			{
				EGUI::PushID(i + static_cast<unsigned>(map_size));
				const auto val = values[i];
				std::string _nName{ std::to_string(i) + ".K" };
				EGUI::Display::EmptyBox(_nName.c_str(), 80.f, static_cast<const char*>(keys[i].c_str()));
				EGUI::SameLine(AI::DEFAULT_ALIGN);
				EGUI::ChangeAlignmentYOffset(0);
				EGUI::Display::EmptyBox("V", AI::BTN_SZ, std::to_string(val).c_str());
				EGUI::ChangeAlignmentYOffset();
				EGUI::PopID();
			}
			EGUI::Display::EndTreeNode();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove Key"))
			{
				mpBlackboard->ClearSingle(eInt);
			}
			ImGui::EndPopup();
		}
	}

	void AiController::EditorBoolNode()
	{
		const auto map = mpBlackboard->GetMapToBools();
		const size_t map_size = map.size();

		
		EGUI::Display::Label(std::to_string(map_size).c_str());
		EGUI::SameLine();

		if (EGUI::Display::StartTreeNode("Bools", nullptr, false, false, false, true))
		{
			std::vector<std::string> keys(map_size);
			std::vector<bool> values(map_size);

			transform(map.begin(), map.end(), keys.begin(), AI::key_select);
			transform(map.begin(), map.end(), values.begin(), AI::value_select);

			for (unsigned i = 0; i < map_size; ++i)
			{
				EGUI::PushID(i + static_cast<unsigned>(map_size));
				const bool b = values[i];
				std::string _nName{ std::to_string(i) + ".K" };
				EGUI::Display::EmptyBox(_nName.c_str(), 80.f, keys[i].c_str());
				EGUI::SameLine(AI::DEFAULT_ALIGN);
				EGUI::ChangeAlignmentYOffset(0);
				EGUI::Display::EmptyBox("V", AI::BTN_SZ, (b ? "true" : "false"));
				EGUI::ChangeAlignmentYOffset();
				EGUI::PopID();
			}

			EGUI::Display::EndTreeNode();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove Key"))
			{
				mpBlackboard->ClearSingle(eBool);
			}
			ImGui::EndPopup();
		}
	}

	void AiController::AddKeyToMap()
	{
		if (EGUI::Display::Button("Add Key", { 60,24 }))
		{
			EGUI::Display::OpenPopup("Add Key", false);
		}

		if (EGUI::Display::StartPopupModal("Add Key", "Adding New Key"))
		{
			//TODO clean up this damn mess
			static int keyTypeIndex = 0;
			static char buffer[256];
			static bool Booler = false;
			static int Inter = 0;
			static float Floater = 0.0f;
			static Math::Vector4 Vectorer{ 0,0,0 };
			static char GameObjectBuf[256];
			static char Stringer[256];

			std::string keyTypes[8]{ "Pointer", "Bool", "Int", "Float", "Double",
				"Vector", "Object", "Names" };

			if (EGUI::Display::DropDownSelection("Key Type:", keyTypeIndex, keyTypes))
			{

			}

			if (keyTypeIndex != 0 && keyTypeIndex != 4)
			{
				EGUI::Display::TextField("Key", buffer /*mSearchText*/, Editor::MAX_SEARCH, true, 128.f);
				EGUI::SameLine(0);
			}
			else
			{
				EGUI::Display::Label("Key not yet available.");
			}

			{
				switch (keyTypeIndex)
				{
				case 0:
					break;
				case 1:
					EGUI::Display::CheckBox("Bool", &Booler,false);
					break;
				case 2:
					EGUI::Display::DragInt("Int", &Inter, 1, -INT_MAX, INT_MAX,true);
					break;
				case 3:
					EGUI::Display::DragFloat("Float", &Floater, 0.1f, -FLT_MAX, FLT_MAX, true);
					break;
				case 4:
					break;
				case 5:
					EGUI::Display::VectorFields("Vector", &Vectorer, 0.1f, -FLT_MAX, FLT_MAX, true);
					break;
				case 6:
					EGUI::Display::TextField("Obj Name", GameObjectBuf, Editor::MAX_SEARCH, false, 100.f,false);
					break;
				case 7:
					EGUI::Display::TextField("String", Stringer, Editor::MAX_SEARCH, false, 100.f,false);
					break;
				default:
					break;
				}
			}

			if (EGUI::Display::Button("Confirm", { 60,24 }))
			{
				switch (keyTypeIndex)
				{
				case 0:
					break;
				case 1:
					mpBlackboard->SetBool(buffer, Booler);
					break;
				case 2:
					mpBlackboard->SetInt(buffer, Inter);
					break;
				case 3:
					mpBlackboard->SetFloat(buffer, Floater);
					break;
				case 4:
					break;
				case 5:
					mpBlackboard->SetVector(buffer, Vectorer);
					break;
				case 6:
				{
					if (const auto pObj = EngineCore::Get<SceneSystem>()->FindGameObject_cstr(GameObjectBuf))
					{
						mpBlackboard->SetObject(buffer, pObj->GetID());
					}
				}
				break;
				case 7:
					mpBlackboard->SetString(buffer, Stringer);
					break;
				default:
					break;
				}

				EGUI::Display::CloseCurrentPopup();
			}

			EGUI::SameLine(15);
			if (EGUI::Display::Button("Cancel", { 60,24 }))
			{
				EGUI::Display::CloseCurrentPopup();
			}
			EGUI::Display::EndPopup();
		}
	}

	void AiController::EditorCurrentStatus()
	{
		if (bTree.IsValidTree())
		{
			HashString _eStatus = "INVALID";
			switch (mNodeStatus)
			{
			case Node::eStatus::INVALID: break;
			case Node::eStatus::CANCELLED: _eStatus = "CANCELLED"; break;
			case Node::eStatus::RUNNING: _eStatus = "RUNNING"; break;
			case Node::eStatus::SUCCESS: _eStatus = "SUCCESS"; break;
			case Node::eStatus::FAIL: _eStatus = "FAIL"; break;
			default:;
			}
			EGUI::Display::EmptyBox("Current eStatus:", 128.f, _eStatus.c_str());
			EGUI::Display::Dummy(0, 15);
		}
	}

	void AiController::RecursiveTree(Node::Ptr _node) const
	{
		if (_node) // if node is valid
		{
			if (_node->GetNodeType() == 0)	// Is Composite Node
			{
				const Composite* tempC = dynamic_cast<Composite*>(_node.GetRaw());
				if (tempC)
				{
					HashString CompositeName;
					if (tempC->GetID() == bTree.GetID()) //First Node
					{
						CompositeName = { "ROOT: " + tempC->GetEditorName() };
					}
					else
					{
						CompositeName = { "Composite: " + tempC->GetEditorName() };
					}
					
					if (EGUI::Display::CollapsingHeader(CompositeName.c_str()))
					{
						for (const auto& child : tempC->GetAllChildren())
						{
							if (child)
								RecursiveTree(child);
						}
					}
				}
			}

			else if (_node->GetNodeType() == 1) // Is Decorator Node
			{
				const Decorator* tempD = dynamic_cast<Decorator*>(_node.GetRaw());
				EGUI::Indent();
				HashString DecoratorName = { "Decorator: " + tempD->GetEditorName() };
				if (EGUI::Display::CollapsingHeader(DecoratorName.c_str()))
				{
					if (tempD)
					{
						if (const auto child = tempD->GetChild())
						{
							RecursiveTree(child);
						}
					}
				}
				EGUI::UnIndent();
			}

			else // Is Task Node
			{
				EGUI::Indent();
				const Task* tempT = dynamic_cast<Task*>(_node.GetRaw());
				if (tempT)
				{
					HashString TaskName = { "Task: " + tempT->GetEditorName() };
					ImGui::CollapsingHeader(TaskName.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet);
				}
				EGUI::UnIndent();
			}
		}
	}

	void AiController::EditorTreeView()
	{
		if (bTree.IsValidTree())
		{
			EGUI::Display::Label("AI Tree View");
			RecursiveTree(bTree.GetRoot());
		}
	
	}

#endif // EDITOR

}


