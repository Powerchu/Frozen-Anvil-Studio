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

#include <cctype>
#include <fstream>


namespace
{
	struct SkipComment
	{
		static inline bool f (char const* _buf)
		{
			return _buf[0] == '-' && _buf[1] == '-' && _buf[2] == '>';
		}
	};
	struct SkipCData
	{
		static inline bool f(char const* _buf)
		{
			return _buf[0] == ']' && _buf[1] == ']' && _buf[2] == '>';
		}
	};
	struct SkipMeta
	{
		static inline bool f(char const* _buf)
		{
			return _buf[0] == '?' && _buf[1] == '>';
		}
	};
	struct SkipToEnd
	{
		static inline bool f(char const* _buf)
		{
			return _buf[0] == '>';
		}
	};
	struct WhiteSpace
	{
		static inline bool f(char const* _buf)
		{
			return std::isspace(*_buf);
		}
	};
}


Dystopia::NodeXML* Dystopia::XMLParser::Parse(char const* _strFile)
{
	using ios = std::ios;
	using Alloc_t = Dystopia::DefaultAllocator<char[]>;
	std::basic_ifstream<char> file{ _strFile, ios::binary };

	if (file.bad())
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

Dystopia::NodeXML* Dystopia::XMLParser::ParseNode(const char *& _buf, NodeXML* _curr)
{
	using Alloc_t = Dystopia::DefaultAllocator<NodeXML>;

	while (*_buf)
	{
		switch (*_buf)
		{
		case '"':
			break;
		case '=':
			break;
		case '/':
			return _curr->GetParent();
		case '>':
			break;
		}
	}

	return _curr;
}

Dystopia::NodeXML* Dystopia::XMLParser::ActuallyParse(char const* _buf)
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
					_buf = Skip<SkipToEnd>(_buf);
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

				curr = root->GetParent();
			}
			break;

			default:
			{
				curr = ParseNode(_buf, curr);
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

AutoArray<Dystopia::NodeXML> const& Dystopia::NodeXML::GetChildren() const noexcept
{
	return mChildren;
}

AutoArray<std::pair<char const*, char const*>> const& Dystopia::NodeXML::GetFields() const noexcept
{
	return mFields;
}

