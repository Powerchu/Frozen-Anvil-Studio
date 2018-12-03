/* HEADER *********************************************************************************/
/*!
\file	TreeBuilder.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Builder Template Interface for all Neural Trees (Behaviour Tree). Referred to
https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
http://aigamedev.com/open/article/type-safe-tree-builder/
for basic understanding.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TREE_BUILDER_H_
#define _TREE_BUILDER_H_

#include "System/AI/NeuralTree.h"

#include <cassert>

namespace Dystopia
{
	namespace NeuralTree
	{
		template<class Parent>
		class DecoratorBuilder;

		template <class Parent>
		class CompositeBuilder
		{
		public:
			CompositeBuilder(Parent* parent, Composite* node) : mpParent(parent), mpNode(node) {}

			template <class NodeType, typename... Args>
			CompositeBuilder<Parent> task(Args... args)
			{
				auto child = Ctor::CreateShared<NodeType>((args)...);
				mpNode->AddChild(child);
				return *this;
			}

			template <class CompositeType, typename... Args>
			CompositeBuilder<CompositeBuilder<Parent>> composite(Args... args)
			{
				auto child = Ctor::CreateShared<CompositeType>((args)...);
				mpNode->AddChild(child);
				return CompositeBuilder<CompositeBuilder<Parent>>(this, reinterpret_cast<CompositeType*>(child.GetRaw()));
			}

			template <class DecoratorType, typename... Args>
			DecoratorBuilder<CompositeBuilder<Parent>> decorator(Args... args)
			{
				auto child = Ctor::CreateShared<DecoratorType>((args)...);
				mpNode->AddChild(child);
				return DecoratorBuilder<CompositeBuilder<Parent>>(this, reinterpret_cast<DecoratorType*>(child.GetRaw()));
			}

			Parent& end() {	return *mpParent; }

		private:
			Parent * mpParent;
			Composite* mpNode;
		};

		template <class Parent>
		class DecoratorBuilder
		{
		public:
			DecoratorBuilder(Parent* parent, Decorator* node) : mpParent(parent), mpNode(node) {}

			template <class NodeType, typename... Args>
			DecoratorBuilder<Parent> task(Args... args)
			{
				auto child = Ctor::CreateShared<NodeType>((args)...);
				mpNode->SetChild(child);
				return *this;
			}

			template <class CompositeType, typename... Args>
			CompositeBuilder<DecoratorBuilder<Parent>> composite(Args... args)
			{
				auto child = Ctor::CreateShared<CompositeType>((args)...);
				mpNode->SetChild(child);
				return CompositeBuilder<DecoratorBuilder<Parent>>(this, reinterpret_cast<CompositeType*>(child.GetRaw()));
			}

			template <class DecoratorType, typename... Args>
			DecoratorBuilder<DecoratorBuilder<Parent>> decorator(Args... args)
			{
				auto child = Ctor::CreateShared<DecoratorType>((args)...);
				mpNode->SetChild(child);
				return DecoratorBuilder<DecoratorBuilder<Parent>>(this, reinterpret_cast<DecoratorType*>(child.GetRaw()));
			}

			Parent& end() { return *mpParent; }

		private:
			Parent * mpParent;
			Decorator* mpNode;
		};

		/*
		 * The Builder
		 */
		class Builder
		{
		public:
			template <class NodeType, typename... Args>
			Builder task(Args... args)
			{
				mpRoot = Ctor::CreateShared<NodeType>((args)...);
				return *this;
			}

			template <class CompositeType, typename... Args>
			CompositeBuilder<Builder> composite(Args... args)
			{
				mpRoot = Ctor::CreateShared<CompositeType>((args)...);
				return CompositeBuilder<Builder>(this, reinterpret_cast<CompositeType*>(mpRoot.GetRaw()));
			}

			template <class DecoratorType, typename... Args>
			DecoratorBuilder<Builder> decorator(Args... args)
			{
				mpRoot = Ctor::CreateShared<DecoratorType>((args)...);
				return DecoratorBuilder<Builder>(this, reinterpret_cast<DecoratorType*>(mpRoot.GetRaw()));
			}

			Node::Ptr Build() const
			{
				assert(mpRoot != nullptr && "The Behavior Tree is empty!");
				auto tree = Ctor::CreateShared<BehaviourTree>();
				tree->SetRoot(mpRoot);
				return tree;
			}

			void Build(BehaviourTree& _tree) const
			{
				assert(mpRoot != nullptr && "The Behavior Tree is empty!");
				_tree.SetRoot(mpRoot);
			}

		private:
			Node::Ptr mpRoot;
		};
	}
}


#endif
