/* HEADER *********************************************************************************/
/*!
\file	NeuralTree.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Base class for all Neural Trees (Behaviour Tree). Referred to 
https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
for basic understanding.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _NEURAL_TREE_H_
#define _NEURAL_TREE_H_

#include <utility>
#include "Globals.h"
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

			virtual ~Node() = default;

			virtual void Init() {};
			virtual eStatus Update() = 0;
			virtual void Exit(eStatus) {};

			eStatus Tick() 
			{
				if (mStatus != eStatus::RUNNING) {
					Init();
				}

				mStatus = Update();

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
				//return std::nullptr_t{};
			}

		protected:
			eStatus mStatus = eStatus::INVALID;
		private:
			uint64_t mnID{};
			uint64_t mnParentID{};
			HashString mnName;

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
			Composite() : iter(mparrChildren.begin()) {}
			virtual ~Composite() = default;

			void AddChild(const Node::Ptr& child) { mparrChildren.Insert(child); }
			bool HasChildren() const { return !mparrChildren.IsEmpty(); }

		protected:
			MagicArray<Node::Ptr> mparrChildren;
			MagicArray<Node::Ptr>::Itor_t iter;
		};

/*
 * Decorator Node - Can only have a single child node. Their function is either to 
 * transform the result they receive from their child node's status, to terminate the 
 * child, or repeat processing of the child, depending on the type of decorator node.
 */
		class Decorator : public Node
		{
		public:
			virtual ~Decorator() = default;

			void SetChild(const Node::Ptr& node) { mpChild = node; }
			bool HasChild() const { return mpChild != nullptr; }

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
			Task() = default;

			Task(Blackboard::Ptr _blackboard) 
				: mpBlackboard(std::move(_blackboard))
			{
			
			}
			virtual ~Task() = default;
			eStatus Update() override = 0;

		protected:
			Blackboard::Ptr mpBlackboard;
		};


		class BehaviourTree : public Node
		{
		public:
			BehaviourTree()
				: mpBlackboard(Ctor::CreateShared<Blackboard>())
				, mnID(0)
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

			eStatus Update() override { return mpRoot->Tick();	}

			void SetRoot(const Node::Ptr& node)
			{
				mpRoot = node;
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

		private:
			Node::Ptr mpRoot;
			Blackboard::Ptr mpBlackboard;
			Blackboard::Ptr mpSharedboard;

			uint64_t mnID;
		};

	}
}
	

#endif
