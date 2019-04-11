/* HEADER *********************************************************************************/
/*!
\file	AiController.h
\author Aaron Chu Ming San (100%)
\par    email: m.chu\@digipen.edu
\brief
AITree Controller Component.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "Component/Component.h"
#include "Component/ComponentList.h"

#include "System/AI/AISystem.h"

#include "Utility/MetaAlgorithms.h"
#include "Utility/Meta.h"


namespace Dystopia
{
	using namespace NeuralTree;

	// AiController physics component for 2D sprites.
	class _DLL_EXPORT AiController : public Component
	{
	public:
		using SYSTEM = AISystem;
		unsigned GetComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, UsableComponents>::value;
		};
		static const std::string GetCompileName(void) { return "AI Controller"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }


		// ====================================== CONSTRUCTORS ======================================= // 
		AiController(void);
		AiController(const AiController& _copy);

		// ================================ VIRTUAL FUNCTIONS ================================== // 
		//void Load(void);
		void Awake() override;
		void Init(void) override;
		//void Unload(void);
		AiController* Duplicate() const override;
		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&) override;

		void Update(float _dt);
		void LateUpdate(float _dt) const;
		// ===================================== MEMBER FUNCTIONS ==================================== // 
		SharedPtr<BehaviourTree>& GetTreeAsRef(void);
		BehaviourTree::Ptr GetTreeAsPtr(void) const;
		Blackboard::Ptr& GetBlackboard(void);

		void ClearTree(void);
		void SetTree(const SharedPtr<BehaviourTree>&  _root);

		Node::eStatus mNodeStatus;
		Blackboard::Ptr mpBlackboard;

		void SetLocked(bool _b);
		bool GetLock();
#if EDITOR
		/*=================Editor Stuff=====================*/
		void EditorCurrentStatus();
		void RecursiveTree(Node::Ptr);
		void EditorTreeView();
		void AddKeyToMap();

		void EditorFunctionNode();
		void EditorStringNode();
		void EditorObjectNode();
		void EditorVectorNode();
		void EditorDoubleNode();
		void EditorFloatNode();
		void EditorIntNode();
		void EditorBoolNode();

		// Editor UI		
		void EditorUI(void) noexcept override;

		
#endif // EDITOR
		SharedPtr<BehaviourTree> bTree;
	private:
		bool isLocked = false;
		Task * mCurrentTask = nullptr;

	};
}
#endif
