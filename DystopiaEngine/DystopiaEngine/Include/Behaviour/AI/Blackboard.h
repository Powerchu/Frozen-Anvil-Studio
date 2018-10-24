/* HEADER *********************************************************************************/
/*!
\file	Blackboard.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Base class for all Neural Trees (Behaviour Tree). Referred to
https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
for basic understanding. Using Agents to share data context

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _BLACKBOARD_H_
#define _BLACKBOARD_H_

//#include "Globals.h"
#include <string>
#include <unordered_map>
#include "DataStructure/SharedPtr.h"
#include "Math/Vector4.h"

// TODO: use our reflection stuff
namespace Dystopia
{
	namespace NeuralTree
	{
		class Blackboard
		{
		public:
			/*
			 * Bool-String Map
			 */
			void setBool(const std::string& key, const bool value) { bools[key] = value; }
			bool getBool(const std::string& key)
			{
				if (bools.find(key) == bools.end()) {
					bools[key] = false;
				}
				return bools[key];
			}
			bool hasBool(const std::string& key) const { return bools.find(key) != bools.end(); }

			/*
			* Int-String Map
			*/
			void setInt(const std::string& key, const int value) { ints[key] = value; }
			int getInt(const std::string& key)
			{
				if (ints.find(key) == ints.end()) {
					ints[key] = 0;
				}
				return ints[key];
			}
			bool hasInt(const std::string& key) const { return ints.find(key) != ints.end(); }

			/*
			* Float-String Map
			*/
			void setFloat(const std::string& key, const float value) { floats[key] = value; }

			float getFloat(const std::string& key)
			{
				if (floats.find(key) == floats.end()) {
					floats[key] = 0.0f;
				}
				return floats[key];
			}
			bool hasFloat(const std::string& key) const { return floats.find(key) != floats.end(); }

			/*
			* Double-String Map
			*/
			void setDouble(const std::string& key, const double value) { doubles[key] = value; }
			double getDouble(const std::string& key)
			{
				if (doubles.find(key) == doubles.end()) {
					doubles[key] = 0.0f;
				}
				return doubles[key];
			}
			bool hasDouble(const std::string& key) const { return doubles.find(key) != doubles.end(); }

			/*
			* String-String Map
			*/
			void setString(const std::string& key, const std::string& value) { strings[key] = value; }
			std::string getString(const std::string& key)
			{
				if (strings.find(key) == strings.end()) {
					strings[key] = "";
				}
				return strings[key];
			}
			bool hasString(const std::string& key) const { return strings.find(key) != strings.end(); }

			/*
			* Void*-String Map
			*/
			void setPointer(const std::string& key, void* value) { pointers[key] = value; }
			void* getPointer(const std::string& key)
			{
				if (pointers.find(key) == pointers.end()) {
					pointers[key] = nullptr;
				}
				return pointers[key];
			}
			bool hasPointer(const std::string& key) const { return pointers.find(key) != pointers.end(); }

			/*
			* Vector4-String Map
			*/
			void setVector(const std::string& key, const Math::Vector4& value) { vectors[key] = value; }
			Math::Vector4 getVector(const std::string& key)
			{
				if (vectors.find(key) == vectors.end()) {
					vectors[key] = {0,0,0,0};
				}
				return vectors[key];
			}
			bool hasVector(const std::string& key) const { return vectors.find(key) != vectors.end(); }

			using Ptr = SharedPtr<Blackboard>;

		protected:
			std::unordered_map<std::string, void*> pointers;
			std::unordered_map<std::string, bool> bools;
			std::unordered_map<std::string, int> ints;
			std::unordered_map<std::string, float> floats;
			std::unordered_map<std::string, double> doubles;
			std::unordered_map<std::string, Math::Vector4> vectors;
			std::unordered_map<std::string, std::string> strings;
		};
	}
}

#endif
