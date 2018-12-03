/* HEADER *********************************************************************************/
/*!
\file	Blackboard.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Base class for all Neural Trees (Behaviour Tree). Referred to
https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
for basic understanding. Using Agents to share data context

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _BLACKBOARD_H_
#define _BLACKBOARD_H_

//#include "Globals.h"
#include <unordered_map>
#include "DataStructure/SharedPtr.h"
#include "Object/GameObject.h"
#include "Math/Vector4.h"
#include "DataStructure/OString.h"


namespace Dystopia
{
	namespace NeuralTree
	{
		enum BlackBoardSelector
		{
			ePointer = 0,
			eBool,
			eInt,
			eFloat,
			eDouble,
			eVector,
			eObject,
			eStrings,

			TOTAL
		};

		struct _DLL_EXPORT Blackboard
		{
		public:
			using Ptr = SharedPtr<Blackboard>;

			~Blackboard() = default;

			void SetBool(const HashString key, const bool value);
			bool GetBool(HashString key);
			bool HasBool(HashString key) const;

			void SetInt(HashString key, int value);
			int GetInt(HashString key);
			bool HasInt(HashString key) const;

			void SetFloat(HashString key, float value);
			float GetFloat(HashString key);
			bool HasFloat(HashString key) const;

			void SetDouble(HashString key, double value);
			double GetDouble(HashString key);
			bool HasDouble(HashString key) const;

			void SetString(HashString key, HashString value);
			HashString GetString(HashString key);
			bool HasString(HashString key) const;

			void SetPointer(HashString key, void* value);
			void* GetPointer(HashString key);
			bool HasPointer(HashString key) const;

			void SetVector(HashString key, Math::Vector4 value);
			Math::Vector4 GetVector(HashString key);
			bool HasVector(HashString key) const;

			void SetObject(HashString key, uint64_t value);
			uint64_t GetObject(HashString key);
			bool HasObject(HashString key) const;


			std::unordered_map<HashString, void*>			& GetMapToPointers()	;
			std::unordered_map<HashString, bool>			& GetMapToBools()		;
			std::unordered_map<HashString, int>				& GetMapToInts() 		;
			std::unordered_map<HashString, float>			& GetMapToFloats()		;
			std::unordered_map<HashString, double>			& GetMapToDoubles()		;
			std::unordered_map<HashString, Math::Vector4>   & GetMapToVectors()		;
			std::unordered_map<HashString, uint64_t>		& GetMapToObjs()		;
			std::unordered_map<HashString, HashString>		& GetMapToNames()		;

			Blackboard Clone() const;

			void ClearAll();

			void ClearSingle(BlackBoardSelector _select);

			void Serialise(TextSerialiser& _out) const;
			void Unserialise(TextSerialiser& _in);

		protected:
			std::unordered_map<HashString, void*>			pointers;	// 0
			std::unordered_map<HashString, bool>			bools;		// 1
			std::unordered_map<HashString, int>				ints;		// 2
			std::unordered_map<HashString, float>			floats;		// 3
			std::unordered_map<HashString, double>			doubles;	// 4
			std::unordered_map<HashString, Math::Vector4>	vectors;	// 5
			std::unordered_map<HashString, uint64_t>		objs;		// 6
			std::unordered_map<HashString, HashString>		strings;	// 7
		};
	}
}

#endif
