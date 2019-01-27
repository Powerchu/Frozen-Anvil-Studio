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
#include "Math/Vectors.h"
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

			void SetBool(const HashString& key, const bool value);
			bool GetBool(const HashString& key);
			bool HasBool(const HashString& key) const;

			void SetInt(const HashString& key, int value);
			int GetInt(const HashString& key);
			bool HasInt(const HashString& key) const;

			void SetFloat(const HashString& key, float value);
			float GetFloat(const HashString& key);
			bool HasFloat(const HashString& key) const;

			void SetDouble(const HashString& key, double value);
			double GetDouble(const HashString& key);
			bool HasDouble(const HashString& key) const;

			void SetString(const HashString& key, HashString value);
			HashString GetString(HashString key);
			bool HasString(const HashString& key) const;

			void SetPointer(const HashString& key, void* value);
			void* GetPointer(const HashString& key);
			bool HasPointer(const HashString& key) const;

			void SetVector(const HashString& key, Math::Vector4 value);
			Math::Vector4 GetVector(HashString key);
			bool HasVector(const HashString& key) const;

			void SetObject(const HashString& key, uint64_t value);
			uint64_t GetObject(const HashString& key);
			bool HasObject(HashString key) const;


			std::unordered_map<HashString, void*>			& GetMapToPointers()	;
			std::unordered_map<HashString, bool>			& GetMapToBools()		;
			std::unordered_map<HashString, int>				& GetMapToInts() 		;
			std::unordered_map<HashString, float>			& GetMapToFloats()		;
			std::unordered_map<HashString, double>			& GetMapToDoubles()		;
			std::unordered_map<HashString, Math::Vector4>   & GetMapToVectors()		;
			std::unordered_map<HashString, uint64_t>		& GetMapToObjs()		;
			std::unordered_map<HashString, HashString>		& GetMapToNames()		;

			Blackboard::Ptr Blackboard::Clone() const;

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
