/* HEADER *********************************************************************************/
/*!
\file	XMLParser.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Reads XMLs

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "XMLParser.h"
#include "Allocator/DefaultAlloc.h"
#include "Utility/MetaAlgorithms.h"
#include "Utility/MetaDataStructures.h"

#include <cctype>
#include <fstream>


namespace
{
	template <typename Pred>
	struct Not {
		static inline bool f (char const* _buf)
		{
			return !Pred::f(_buf);
		}
	};

	template <typename PredA, typename PredB>
	struct Or {
		static inline bool f(char const* _buf)
		{
			return PredA::f(_buf) || PredB::f(_buf);
		}
	};

	template <char ch>
	struct FindChar {
		static inline bool f(char const* _buf)
		{
			return ch == *_buf;
		}
	};

	namespace
	{
		template <typename, typename>
		struct FindSeqAux;

		template <template <unsigned...> class Set, unsigned ... R, typename T, T ... Ch>
		struct FindSeqAux<Set<R...>, Ut::IntegralList<T, Ch...>> {
			static inline bool f(char const* _buf)
			{
				return ((Ch == _buf[R]) && ...);
			}
		};
	}

	template <char ...SEQ>
	struct FindSeq : public FindSeqAux<Ut::MetaMakeRange_t<sizeof...(SEQ)>, Ut::IntegralList<char, SEQ...>>
	{};

	using SkipMeta    = FindSeq<'?', '>'>;
	using SkipCData   = FindSeq<']', ']', '>'>;
	using SkipComment = FindSeq<'-', '-', '>'>;

	using SkipToEnd = FindChar<'>'>;

	struct WhiteSpace {
		static inline bool f(char const* _buf)
		{
			return !std::isspace(*_buf);
		}
	};
}


Dystopia::NodeXML* Dystopia::XMLParser::Parse(char const* _strFile)
{
	using ios = std::ios;
	using Alloc_t = Dystopia::DefaultAllocator<char[]>;
	std::basic_ifstream<char> file{ _strFile, ios::binary };

	if (!file.good())
	{
		__debugbreak();
		return nullptr;
	}

	file.seekg(0, ios::end);
	auto sz = static_cast<size_t>(file.tellg());
	char* buf = Alloc_t::Alloc(1 + sz);

	file.seekg(0);
	file.read(buf, sz);
	buf[sz] = '\0';

	file.close();

	return ActuallyParse(buf);
}

Dystopia::NodeXML* Dystopia::XMLParser::ParseNode(char *& _buf, NodeXML* _curr)
{
	using Alloc_t = Dystopia::DefaultAllocator<NodeXML>;
	_buf = Skip<WhiteSpace>(_buf);

	_curr->mstrName = _buf;
	_buf = Skip<Not<WhiteSpace>>(_buf);
	*_buf++ = '\0';

	while (*_buf)
	{
		_buf = Skip<WhiteSpace>(_buf);

		switch (*_buf)
		{
		case '/':
			return _curr->GetParent();
			break;

		case '>':
			++_buf;
			return _curr;
			break;

		default:
			auto name = _buf;
			_buf = Skip<FindChar<'='>>(_buf);
			*_buf++ = '\0';
			_buf += '"' == *_buf;
			_curr->mFields.EmplaceBack(name, _buf);
			_buf = Skip<Or<FindChar<'"'>, Not<WhiteSpace>>>(_buf);
			*_buf = '\0';
			break;
		}

		++_buf;
	}

	return _curr;
}

Dystopia::NodeXML* Dystopia::XMLParser::ActuallyParse(char* _buf)
{
	using Alloc_t = Dystopia::DefaultAllocator<NodeXML>;
	NodeXML* root = Alloc_t::ConstructAlloc();
	NodeXML* curr = root;

	root->mpParent = nullptr;
	while (curr && *_buf)
	{
		_buf = Skip<WhiteSpace>(_buf);

		if ('<' != *_buf)
			return root;

		if (auto& e = *_buf)
		{
			switch (e)
			{
			case '!':
			{
				++_buf;
				if ('-' == *_buf) // Comment
				{
					_buf = Skip<SkipComment>(_buf + 2);
					if (!*_buf)
						return root;

					_buf += 3; // Skip "-->"
				}
				else if ('[' == *_buf)
				{
					_buf = Skip<SkipCData>(_buf + 7);
					_buf += 3; // Skip "]]>"
				}
				else // Don't know what on earth is it
				{
					_buf = Skip<SkipToEnd>(_buf + 1);
					if (!*_buf)
						return root;
					
					++_buf; // Skip ">"
				}
			}
			break;

			case '?': // XML Decl or PI node
			{
				_buf = Skip<SkipMeta>(_buf + 1);
				if (!*_buf)
					return root;
				
				_buf += 2; // Skip "?>"
			}
			break;

			case '/': // End of tag
			{
				_buf = Skip<SkipToEnd>(_buf + 1);
				++_buf; // Skip '>'

				curr = curr->GetParent();
			}
			break;

			default:
			{
				auto temp = Alloc_t::ConstructAlloc();
				curr->mChildren.Insert(temp);
				temp->mpParent = curr;
				curr = ParseNode(++_buf, temp);
			}
			break;
			}
		}
	}

	return root;
}


char const* Dystopia::NodeXML::GetName() const noexcept
{
	return mstrName;
}

Dystopia::NodeXML* Dystopia::NodeXML::GetParent() const noexcept
{
	return mpParent;
}

AutoArray<Dystopia::NodeXML*> const& Dystopia::NodeXML::GetChildren() const noexcept
{
	return mChildren;
}

AutoArray<std::pair<char const*, char const*>> const& Dystopia::NodeXML::GetFields() const noexcept
{
	return mFields;
}

