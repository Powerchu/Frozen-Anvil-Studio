/* HEADER *********************************************************************************/
/*!
\file	EditorFactory.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorFactory.h"
#include "Editor/EditorMain.h"
#include "Editor/RuntimeMeta.h"

#include "System/Driver/Driver.h"
#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
#include "System/Behaviour/BehaviourSystemHelper.h"

#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

#include "Utility/GUID.h"
#include "Utility/DebugAssert.h"

#include <fstream>
#include <iostream>

Editor::EditorFactory::EditorFactory(void)
	: mpSceneSys{ nullptr }
{}

Editor::EditorFactory::~EditorFactory(void)
{}

void Editor::EditorFactory::Load(void)
{}

bool Editor::EditorFactory::Init(void)
{
	mpSceneSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
	return true;
}

void Editor::EditorFactory::StartFrame(void)
{}

void Editor::EditorFactory::Update(float)
{}

void Editor::EditorFactory::EndFrame(void)
{}

void Editor::EditorFactory::Shutdown(void)
{}

void Editor::EditorFactory::Message(eEMessage)
{}

void Editor::EditorFactory::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::EditorFactory::LoadSettings(Dystopia::TextSerialiser&)
{}

bool Editor::EditorFactory::SaveAsPrefab(const uint64_t& _objID, Dystopia::TextSerialiser& _out)
{
	return false;
}

bool Editor::EditorFactory::LoadFromPrefab(Dystopia::GameObject&, Dystopia::TextSerialiser&)
{
	return false;
}

bool Editor::EditorFactory::SavePrefab(const uint64_t& _objID, Dystopia::TextSerialiser& _out)
{
	Dystopia::GameObject *pObject = mpSceneSys->GetCurrentScene().FindGameObject(_objID);

	if (pObject)
	{
		const auto& allComponents = pObject->GetAllComponents();
		const auto& allBehaviours = pObject->GetAllBehaviours();

		SaveSegment(*pObject, _out);
		SaveSegment(allComponents, _out);
		SaveSegment(allBehaviours, _out);

		return true;
	}
	return false;
}

bool Editor::EditorFactory::LoadPrefab(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	unsigned count = LoadSegment(_obj, _in);
	LoadSegmentC(_obj, count, _in);
	LoadSegmentB(_obj, _in);
	return true;
}

void Editor::EditorFactory::SaveSegment(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _out)
{
	const auto& allComponents = _obj.GetAllComponents();
	_out.InsertStartBlock("GO_PREFAB");
	_out << allComponents.size();
	_obj.Serialise(_out);
	_out.InsertEndBlock("GO_PREFAB");
}

void Editor::EditorFactory::SaveSegment(const AutoArray<Dystopia::Component*>& _arrComponents, Dystopia::TextSerialiser& _out)
{
	for (const auto& c : _arrComponents)
	{
		_out.InsertStartBlock("Component");
		_out << c->GetRealComponentType();
		c->Serialise(_out);
		_out.InsertEndBlock("Component");
	}
}

void Editor::EditorFactory::SaveSegment(const AutoArray<Dystopia::Behaviour*>& _arrBehaviours, Dystopia::TextSerialiser& _out)
{
	_out.InsertStartBlock("All Behaviours Block");
	_out << _arrBehaviours.size();
	for (const auto& b : _arrBehaviours)
	{
		HashString name{ b->GetBehaviourName() };
		_out << name;
		_out.InsertStartBlock("Behaviour Block");
		auto metaData = b->GetMetaData();
		if (metaData)
		{
			auto allNames = metaData.GetAllNames();
			_out << allNames.size();
			for (const auto& n : allNames)
			{
				if (metaData[n])
				{
					std::string memVarName{ n };
					_out << memVarName;
					_out.InsertStartBlock("Member Variable");
					metaData[n].Serialise(b, _out, Dystopia::BehaviourHelper::SuperSerialiseFunctor{});
					_out.InsertEndBlock("Member Variable");
				}
			}
		}
		_out.InsertEndBlock("Behaviour Block");
	}
	_out.InsertEndBlock("All Behaviours Block");
}

unsigned Editor::EditorFactory::LoadSegment(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	unsigned n = 0;
	_in.ConsumeStartBlock();
	_in >> n;
	_obj.Unserialise(_in);
	_in.ConsumeEndBlock();

	_obj.SetID(Dystopia::GUIDGenerator::GetUniqueID());
	return n;
}

void Editor::EditorFactory::LoadSegmentC(Dystopia::GameObject& _obj, unsigned _count, Dystopia::TextSerialiser& _in)
{
	Dystopia::ComponentList cList;
	unsigned sysID = 0;
	for (unsigned i = 0; i < _count; ++i)
	{
		_in.ConsumeStartBlock();
		_in >> sysID;
		Dystopia::Component *pComponent = cList.GetComponent(sysID, &_obj);
		_obj.AddComponent(pComponent, typename Dystopia::Component::TAG{});
		pComponent->Unserialise(_in);
		_in.ConsumeEndBlock();
	}
}

void Editor::EditorFactory::LoadSegmentB(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	unsigned num;
	_in.ConsumeStartBlock();
	_in >> num;
	for (unsigned i = 0; i < num; ++i)
	{
		HashString name;
		_in >> name;
		if (auto ptr = Dystopia::EngineCore::GetInstance()->Get<Dystopia::BehaviourSystem>()->RequestBehaviour(_obj.GetID(), name.c_str()))
		{
			unsigned size = 0;
			_in.ConsumeStartBlock();
			_in >> size;
			auto BehaviourMetadata = ptr->GetMetaData();
			if (BehaviourMetadata)
			{
				for (unsigned u = 0; u < size; ++u)
				{
					std::string MemVarName;
					_in >> MemVarName;
					if (BehaviourMetadata[MemVarName.c_str()])
					{
						_in.ConsumeStartBlock();
						/*Call Unserialise*/
						BehaviourMetadata[MemVarName.c_str()].Unserialise(ptr, _in, Dystopia::BehaviourHelper::SuperUnserialiseFunctor{});
						_in.ConsumeEndBlock();
					}
					else
					{
						_in.ConsumeStartBlock();
						_in.ConsumeEndBlock();
					}
				}
			}
			ptr->SetOwner(&_obj);
			_obj.AddComponent(ptr, Dystopia::BehaviourTag{});
			_in.ConsumeEndBlock();
		}
		else
		{
			_in.ConsumeStartBlock();
			_in.ConsumeEndBlock();
		}
	}
	_in.ConsumeEndBlock();
	_obj.Identify();
}





#endif














