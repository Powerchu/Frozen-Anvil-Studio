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

#include "Globals.h"
#include "DataStructure/MagicArray.h"
#include "DataStructure/SharedPtr.h"

namespace Dystopia
{
	namespace NeuralTree
	{
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
			virtual eStatus Update(float) = 0;
			virtual void Exit(eStatus) {};

			eStatus Tick(float);

			bool IsSuccess() const { return mStatus == eStatus::SUCCESS; }
			bool IsRunning() const { return mStatus == eStatus::RUNNING; }
			bool IsFailure() const { return mStatus == eStatus::FAIL; }
			bool IsExited() const  { return IsSuccess() || IsFailure(); }

			void Reset() { mStatus = eStatus::INVALID; }

			using Ptr = SharedPtr<Node>;

		protected:
			eStatus mStatus = eStatus::INVALID;
		};

		class Composite : public Node
		{
		public:
			Composite() : iter(children.begin()) {}
			virtual ~Composite() = default;

			void addChild(const Node::Ptr child) { children.Insert(child); }
			bool hasChildren() const { return !children.IsEmpty(); }

		protected:
			MagicArray<Node::Ptr> children;
			MagicArray<Node::Ptr>::Itor_t iter;
		};

		class Decorator : public Node
		{
		public:
			virtual ~Decorator() = default;

			void setChild(const Node::Ptr node) { child = node; }
			bool hasChild() const { return child != nullptr; }

		protected:
			Node::Ptr child;
		};



	}
}
	

#endif

