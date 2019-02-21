/* HEADER *********************************************************************************/
/*!
\file	NeuralTree.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Base class for all Neural Trees (Behaviour Tree). Referred to 
https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
for basic understanding.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _NEURAL_TREE_H_
#define _NEURAL_TREE_H_

#include <utility>
#include "Globals.h"
#include "Utility/GUID.h"
#include "DataStructure/MagicArray.h"
#include "DataStructure/SharedPtr.h"

#include "System/AI/Blackboard.h"

namespace Dystopia
{
	namespace NeuralTree
	{
		// Base Node that contains statuses
		class Node
		{
		public:
			using Ptr = SharedPtr<Node>;

			enum class eStatus
			{
				INVALID = -1,
				CANCELLED,
				RUNNING,
				SUCCESS,
				FAIL
			};

			virtual unsigned GetNodeType(void) const { return unsigned(-1); }
			virtual HashString GetEditorName(void) const { return "Generic Node"; }

			uint64_t GetID(void) const { return mnID; }
			uint64_t GetParentID(void) const { return mnParentID; }
			
			void SetID(const uint64_t& _id) { mnID = _id; }
			void SetParentId(const uint64_t& _id) { mnParentID = _id; }

			virtual ~Node() = default;

			virtual void Init() {}
			virtual eStatus Update(float _deltaTime) = 0;
			
			virtual void Exit(eStatus) {}

			eStatus Tick(float _deltaTime)
			{
				if (mStatus != eStatus::RUNNING || mStatus == eStatus::INVALID) {
					Init();
				}

				mStatus = Update(_deltaTime);

				if (mStatus != eStatus::RUNNING) {
					Exit(mStatus);
				}

				return mStatus;
			}

			bool IsSuccess() const { return mStatus == eStatus::SUCCESS; }
			bool IsRunning() const { return mStatus == eStatus::RUNNING; }
			bool IsFailure() const { return mStatus == eStatus::FAIL; }
			bool IsExited() const  { return IsSuccess() || IsFailure(); }

			void Reset() { mStatus = eStatus::INVALID; }

			//For Hotloading TODO
			Node::Ptr CloneNode()
			{
				return Ctor::CreateShared<Node>(this);
			}

		protected:
			eStatus mStatus = eStatus::INVALID;
			uint64_t mnID{ GUIDGenerator::GetUniqueID() };
			uint64_t mnParentID{unsigned(-1)};

			SharedPtr<Node> mAssignedTree;
		};

/*
 * Composite Node - A composite node is a node that can have one or more children. 
 * They will process one or more of these children in either a first to last sequence 
 * or random order depending on the particular composite node in question, and at some 
 * stage will consider their processing complete and pass either success or failure to 
 * their parent, often determined by the success or failure of the child nodes. 
 * 
 * During the time they are processing children, they will continue to return Running to the 
 * parent.
 */
		class Composite : public Node
		{
		public:
			unsigned GetNodeType(void) const override { return unsigned(0); }
			HashString GetEditorName(void) const override { return "Composite Node"; }

			Composite() : iter(mparrChildren.begin()) {}
			virtual ~Composite() = default;

			void AddChild(const Node::Ptr& child) { mparrChildren.Insert(child); }
			bool HasChildren() const { return !mparrChildren.IsEmpty(); }

			auto GetAllChildren(void) const
			{
				MagicArray<Node::Ptr> ToRet;

				for (auto elem : mparrChildren)
				{
					ToRet.Emplace(elem);
				}

				return Ut::Move(ToRet);
			}

		protected:
			MagicArray<Node::Ptr> mparrChildren;
			MagicArray<Node::Ptr>::Itor_t iter;
			Node::Ptr mpChild;
		};

/*
 * Decorator Node - Can only have a single child node. Their function is either to 
 * transform the result they receive from their child node's status, to terminate the 
 * child, or repeat processing of the child, depending on the type of decorator node.
 */
		class Decorator : public Node
		{
		public:
			unsigned GetNodeType(void) const override { return unsigned(1); }
			HashString GetEditorName(void) const override { return "Decorator Node"; }

			virtual ~Decorator() = default;

			void SetChild(const Node::Ptr& node) { mpChild = node; }
			bool HasChild() const { return mpChild != nullptr; }

			Node::Ptr GetChild(void) const
			{
				return mpChild;
			}

		protected:
			Node::Ptr mpChild;
		};


/*
 * Task Node - These are the lowest level node type, and are incapable of having any children.
 * AI Logic must be implemented here.
 */
		class Task : public Node
		{
		public:
			unsigned GetNodeType(void) const override { return unsigned(2); }
			HashString GetEditorName(void) const override { return "Task Node"; }

			Task() = default;

			Task(Blackboard::Ptr _blackboard) 
				: mpBlackboard(Ut::Move(_blackboard))
			{
			
			}
			virtual ~Task() = default;
			eStatus Update(float _deltaTime) override = 0;

		protected:
			Blackboard::Ptr mpBlackboard;
		};


		class BehaviourTree : public Node
		{
		public:
			BehaviourTree()
				: mpBlackboard(Ctor::CreateShared<Blackboard>())
				, mnID{ unsigned(-1) }
			{

			}

			BehaviourTree(const HashString& _name)
				: mpBlackboard(Ctor::CreateShared<Blackboard>())
				, mnID{ unsigned(-1) }
				, mnName(_name)
			{

			}

			BehaviourTree(const Node::Ptr& rootNode) 
				: BehaviourTree()
			{
				mpRoot = rootNode;
			}

			BehaviourTree(const Blackboard::Ptr &shared) 
				: BehaviourTree()
			{
				mpSharedboard = shared;
			}

			eStatus Update(float _deltaTime) override { return mpRoot->Tick(_deltaTime);	}

			void SetRoot(const Node::Ptr& node)
			{
				mpRoot = node;
				mnID = mpRoot->GetID();
			}

			Blackboard::Ptr GetBlackboard() const
			{
				return mpBlackboard;
			}

			Blackboard::Ptr GetSharedBlackboard() const
			{
				return mpSharedboard;
			}
			
			void SetSharedBlackboard(const Blackboard::Ptr &shared)
			{
				mpSharedboard = shared;
			}

			void SetBlackboard(const Blackboard::Ptr &blackboard)
			{
				mpBlackboard = blackboard;
			}

			bool IsValidTree(void) const 
			{
				return (mpRoot.GetRaw() != nullptr);
			}

			HashString GetEditorName(void) const override { return mnName; }
			Ptr GetRoot() const { return mpRoot; }

		private:
			Node::Ptr mpRoot;
			Blackboard::Ptr mpBlackboard;
			Blackboard::Ptr mpSharedboard;

			uint64_t mnID;
			HashString mnName{ "Generic AI Tree" };
		};

	}
}
	

#endif
