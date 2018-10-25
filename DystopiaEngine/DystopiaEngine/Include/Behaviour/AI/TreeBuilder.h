/* HEADER *********************************************************************************/
/*!
\file	TreeBuilder.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Builder Template Interface for all Neural Trees (Behaviour Tree). Referred to
https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
for basic understanding.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TREE_BUILDER_H_
#define _TREE_BUILDER_H_

#include "Behaviour/AI/NeuralTree.h"

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
			CompositeBuilder<Parent> leaf(Args... args)
			{
				auto child = std::make_shared<NodeType>((args)...);
				mpNode->AddChild(child);
				return *this;
			}

			template <class CompositeType, typename... Args>
			CompositeBuilder<CompositeBuilder<Parent>> composite(Args... args)
			{
				auto child = std::make_shared<CompositeType>((args)...);
				mpNode->AddChild(child);
				return CompositeBuilder<CompositeBuilder<Parent>>(this, reinterpret_cast<CompositeType*>(child.get()));
			}

			template <class DecoratorType, typename... Args>
			DecoratorBuilder<CompositeBuilder<Parent>> decorator(Args... args)
			{
				auto child = std::make_shared<DecoratorType>((args)...);
				mpNode->AddChild(child);
				return DecoratorBuilder<CompositeBuilder<Parent>>(this, reinterpret_cast<DecoratorType*>(child.get()));
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
			DecoratorBuilder<Parent> leaf(Args... args)
			{
				auto child = std::make_shared<NodeType>((args)...);
				mpNode->SetChild(child);
				return *this;
			}

			template <class CompositeType, typename... Args>
			CompositeBuilder<DecoratorBuilder<Parent>> composite(Args... args)
			{
				auto child = std::make_shared<CompositeType>((args)...);
				mpNode->SetChild(child);
				return CompositeBuilder<DecoratorBuilder<Parent>>(this, reinterpret_cast<CompositeType*>(child.get()));
			}

			template <class DecoratorType, typename... Args>
			DecoratorBuilder<DecoratorBuilder<Parent>> decorator(Args... args)
			{
				auto child = std::make_shared<DecoratorType>((args)...);
				mpNode->SetChild(child);
				return DecoratorBuilder<DecoratorBuilder<Parent>>(this, reinterpret_cast<DecoratorType*>(child.get()));
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
			Builder leaf(Args... args)
			{
				mpRoot = std::make_shared<NodeType>((args)...);
				return *this;
			}

			template <class CompositeType, typename... Args>
			CompositeBuilder<Builder> composite(Args... args)
			{
				mpRoot = std::make_shared<CompositeType>((args)...);
				return CompositeBuilder<Builder>(this, reinterpret_cast<CompositeType*>(mpRoot.get()));
			}

			template <class DecoratorType, typename... Args>
			DecoratorBuilder<Builder> decorator(Args... args)
			{
				mpRoot = std::make_shared<DecoratorType>((args)...);
				return DecoratorBuilder<Builder>(this, reinterpret_cast<DecoratorType*>(mpRoot.get()));
			}

			Node::Ptr Build() const
			{
				assert(mpRoot != nullptr && "The Behavior Tree is empty!");
				auto tree = std::make_shared<BehaviourTree>();
				tree->SetRoot(mpRoot);
				return tree;
			}

		private:
			Node::Ptr mpRoot;
		};
	}
}


#endif
