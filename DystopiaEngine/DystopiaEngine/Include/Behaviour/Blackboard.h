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

#include "Globals.h"
#include <string>
#include <unordered_map>
#include "DataStructure/SharedPtr.h"

// TODO: use our reflection stuff
namespace Dystopia
{
	namespace NeuralTree
	{
		class Blackboard
		{
		public:
			void setBool(std::string key, bool value) { bools[key] = value; }
			bool getBool(std::string key)
			{
				if (bools.find(key) == bools.end()) {
					bools[key] = false;
				}
				return bools[key];
			}
			bool hasBool(std::string key) const { return bools.find(key) != bools.end(); }

			void setInt(std::string key, int value) { ints[key] = value; }
			int getInt(std::string key)
			{
				if (ints.find(key) == ints.end()) {
					ints[key] = 0;
				}
				return ints[key];
			}
			bool hasInt(std::string key) const { return ints.find(key) != ints.end(); }

			void setFloat(std::string key, float value) { floats[key] = value; }
			float getFloat(std::string key)
			{
				if (floats.find(key) == floats.end()) {
					floats[key] = 0.0f;
				}
				return floats[key];
			}
			bool hasFloat(std::string key) const { return floats.find(key) != floats.end(); }

			void setDouble(std::string key, double value) { doubles[key] = value; }
			double getDouble(std::string key)
			{
				if (doubles.find(key) == doubles.end()) {
					doubles[key] = 0.0f;
				}
				return doubles[key];
			}
			bool hasDouble(std::string key) const { return doubles.find(key) != doubles.end(); }

			void setString(std::string key, std::string value) { strings[key] = value; }
			std::string getString(std::string key)
			{
				if (strings.find(key) == strings.end()) {
					strings[key] = "";
				}
				return strings[key];
			}
			bool hasString(std::string key) const { return strings.find(key) != strings.end(); }

			using Ptr = SharedPtr<Blackboard>;

		protected:
			std::unordered_map<std::string, bool> bools;
			std::unordered_map<std::string, int> ints;
			std::unordered_map<std::string, float> floats;
			std::unordered_map<std::string, double> doubles;
			std::unordered_map<std::string, std::string> strings;
		};
	}
}

#endif