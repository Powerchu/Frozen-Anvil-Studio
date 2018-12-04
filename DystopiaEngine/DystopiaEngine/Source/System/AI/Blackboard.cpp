#include "System/AI/Blackboard.h"

namespace Dystopia
{
	namespace NeuralTree
	{
		/*
		* Bool-String Map
		*/
		void Blackboard::SetBool(const HashString& key, const bool value)
		{
			bools[key] = value;
		}

		bool Blackboard::GetBool(const HashString& key)
		{
			if (bools.find(key) == bools.end()) {
				this->SetBool(key, false);
			}
			return bools[key];
		}
		bool Blackboard::HasBool(const HashString& key) const { return bools.find(key) != bools.end(); }

		/*
		* Int-String Map
		*/
		void Blackboard::SetInt(const HashString& key, const int value) { ints[key] = value; }
		int Blackboard::GetInt(const HashString& key)
		{
			if (ints.find(key) == ints.end()) {
				this->SetInt(key, 0);
			}
			return ints[key];
		}
		bool Blackboard::HasInt(const HashString& key) const { return ints.find(key) != ints.end(); }

		/*
		* Float-String Map
		*/
		void Blackboard::SetFloat(const HashString& key, const float value) { floats[key] = value; }

		float Blackboard::GetFloat(const HashString& key)
		{
			if (floats.find(key) == floats.end()) {
				this->SetFloat(key, 0.f);
			}
			return floats[key];
		}
		bool Blackboard::HasFloat(const HashString& key) const { return floats.find(key) != floats.end(); }

		/*
		* Double-String Map
		*/
		void Blackboard::SetDouble(const HashString& key, const double value) { doubles[key] = value; }
		double Blackboard::GetDouble(const HashString& key)
		{
			if (doubles.find(key) == doubles.end()) {
				this->SetDouble(key, 0.0);
			}
			return doubles[key];
		}
		bool Blackboard::HasDouble(const HashString& key) const { return doubles.find(key) != doubles.end(); }

		/*
		* String-String Map
		*/
		void Blackboard::SetString(const HashString& key, const HashString value) { strings[key] = value; }
		HashString Blackboard::GetString(const HashString key)
		{
			if (strings.find(key) == strings.end()) {
				this->SetString(key, "New Key");
			}
			return strings[key];
		}
		bool Blackboard::HasString(const HashString& key) const { return strings.find(key) != strings.end(); }

		/*
		* Void*-String Map
		*/
		void Blackboard::SetPointer(const HashString& key, void* value) { pointers[key] = value; }
		void* Blackboard::GetPointer(const HashString& key)
		{
			if (pointers.find(key) == pointers.end()) {
				return nullptr;
			}
			return pointers[key];
		}
		bool Blackboard::HasPointer(const HashString& key) const { return pointers.find(key) != pointers.end(); }

		/*
		* Vector4-String Map
		*/
		void Blackboard::SetVector(const HashString& key, const Math::Vector4 value) { vectors[key] = value; }
		Math::Vector4 Blackboard::GetVector(const HashString key)
		{
			if (vectors.find(key) == vectors.end()) {
				this->SetVector(key, { 0,0,0,0 });
			}
			return vectors[key];
		}
		bool Blackboard::HasVector(const HashString& key) const { return vectors.find(key) != vectors.end(); }

		/*
		* GameObject-String Map
		*/
		void Blackboard::SetObject(const HashString& key, const uint64_t value) { objs[key] = value; }
		uint64_t Blackboard::GetObject(const HashString& key)
		{
			if (objs.find(key) == objs.end()) {
				this->SetObject(key, static_cast<uint64_t>(-1));
			}
			return objs[key];
		}
		bool Blackboard::HasObject(const HashString key) const { return objs.find(key) != objs.end(); }

		std::unordered_map<HashString, void*>			& Blackboard::GetMapToPointers() { return pointers; }
		std::unordered_map<HashString, bool>			& Blackboard::GetMapToBools() { return bools; }
		std::unordered_map<HashString, int>				& Blackboard::GetMapToInts() { return ints; }
		std::unordered_map<HashString, float>			& Blackboard::GetMapToFloats() { return floats; }
		std::unordered_map<HashString, double>			& Blackboard::GetMapToDoubles() { return doubles; }
		std::unordered_map<HashString, Math::Vector4>   & Blackboard::GetMapToVectors()  { return vectors; }
		std::unordered_map<HashString, uint64_t>		& Blackboard::GetMapToObjs()  { return objs; }
		std::unordered_map<HashString, HashString>		& Blackboard::GetMapToNames() { return strings; }

		/*
		* Member Functions
		*/

		Blackboard::Ptr Blackboard::Clone() const
		{
			return Ctor::CreateShared<Blackboard>(*this);
		}

		void Blackboard::ClearAll(void)
		{
			for (int i = 0; i < BlackBoardSelector::TOTAL; ++i)
			{
				ClearSingle(static_cast<BlackBoardSelector>(i));
			}
		}

		void Blackboard::ClearSingle(BlackBoardSelector _select)
		{
			switch (_select)
			{
			case 0:
				pointers.clear();
				break;
			case 1:
				bools.clear();
				break;
			case 2:
				ints.clear();
				break;
			case 3:
				floats.clear();
				break;
			case 4:
				doubles.clear();
				break;
			case 5:
				vectors.clear();
				break;
			case 6:
				objs.clear();
				break;
			case 7:
				strings.clear();
				break;
			default:
				break;
			}
		}

		void Blackboard::Serialise(TextSerialiser& _out) const
		{
			size_t map_size = bools.size();

			/*
			* Bools
			*/
			_out << map_size;
			for (auto& x : bools)
			{
				_out << x.first;
				_out << x.second;
			}

			/*
			* Ints
			*/
			map_size = ints.size();
			_out << map_size;
			for (auto& x : ints)
			{
				_out << x.first;
				_out << x.second;
			}

			/*
			* Floats
			*/
			map_size = floats.size();
			_out << map_size;
			for (auto& x : floats)
			{
				_out << x.first;
				_out << x.second;
			}

			/*
			* Doubles
			*/
			map_size = doubles.size();
			_out << map_size;
			for (auto& x : doubles)
			{
				_out << x.first;
				_out << x.second;
			}

			/*
			* Vectors
			*/
			map_size = vectors.size();
			_out << map_size;
			for (auto& x : vectors)
			{
				_out << x.first;
				_out << x.second;
			}

			/*
			* GObjects
			*/
			map_size = objs.size();
			_out << map_size;
			for (auto& x : objs)
			{
				_out << x.first;
				_out << x.second;
			}

			/*
			* Names
			*/
			map_size = strings.size();
			_out << map_size;
			for (auto& x : strings)
			{
				_out << x.first;
				_out << x.second;
			}
		}

		void Blackboard::Unserialise(TextSerialiser& _in)
		{
			size_t map_size{};
			/*
			* Bools
			*/
			_in >> map_size;
			for (size_t i = 0; i<map_size; ++i)
			{
				HashString first;
				bool second;
				_in >> first;
				_in >> second;

				SetBool(first, second);
			}

			/*
			* Ints
			*/
			_in >> map_size;
			for (size_t i = 0; i<map_size; ++i)
			{
				HashString first;
				int second;
				_in >> first;
				_in >> second;

				SetInt(first, second);
			}

			/*
			* Floats
			*/
			_in >> map_size;
			for (size_t i = 0; i<map_size; ++i)
			{
				HashString first;
				float second;
				_in >> first;
				_in >> second;

				SetFloat(first, second);
			}

			/*
			* Doubles
			*/
			_in >> map_size;
			for (size_t i = 0; i<map_size; ++i)
			{
				HashString first;
				double second;
				_in >> first;
				_in >> second;

				SetDouble(first, second);
			}

			/*
			* Vectors
			*/
			_in >> map_size;
			for (size_t i = 0; i<map_size; ++i)
			{
				HashString first;
				Math::Vec4 second;
				_in >> first;
				_in >> second;

				SetVector(first, second);
			}

			/*
			* GObjects
			*/
			_in >> map_size;
			for (size_t i = 0; i<map_size; ++i)
			{
				HashString first;
				uint64_t second;
				_in >> first;
				_in >> second;

				SetObject(first, second);
			}

			/*
			* Names
			*/
			_in >> map_size;
			for (size_t i = 0; i<map_size; ++i)
			{
				HashString first;
				HashString second;
				_in >> first;
				_in >> second;

				SetString(first, second);
			}
		}

	}
}
