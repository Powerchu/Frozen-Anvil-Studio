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
#include "DataStructure/MagicArray.h"
#include "DataStructure/SharedPtr.h"

#include "Behaviour/AI/Blackboard.h"
#include "Behaviour/AI/TreeBuilder.h"

namespace Dystopia
{
	namespace NeuralTree
	{
		// Base Node that contains statuses
		class Node
		{
		public:
			enum class eStatus
			{
				INVALID = -1,
				READY,
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

			using Ptr = SharedPtr<Node>;

		protected:
			eStatus mStatus = eStatus::INVALID;
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
 * Leaf Node - These are the lowest level node type, and are incapable of having any children.
 * AI Logic must be implemented here.
 */
		class Leaf : public Node
		{
		public:
			Leaf() = default;
			Leaf(Blackboard::Ptr _blackboard) : mpBlackboard(std::move(_blackboard)) {}
			virtual ~Leaf() = default;

		protected:
			Blackboard::Ptr mpBlackboard;
		};


		class BehaviorTree : public Node
		{
		public:
			BehaviorTree() : mpBlackboard(CreateShared<Blackboard>()) {}
			BehaviorTree(const Node::Ptr &rootNode) : BehaviorTree() { mpRoot = rootNode; }

			eStatus Update() override { return mpRoot->Tick();	}

			void SetRoot(const Node::Ptr &node) { mpRoot = node; }
			Blackboard::Ptr GetBlackboard() const { return mpBlackboard; }

		private:
			Node::Ptr mpRoot;
			Blackboard::Ptr mpBlackboard;
		};

	}
}
	

#endif
