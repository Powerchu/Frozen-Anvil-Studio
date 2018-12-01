/* HEADER *********************************************************************************/
/*!
\file	XMLParser.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Reads XMLs

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include "IO/Serialiser.h"
#include "DataStructure/AutoArray.h"


namespace Dystopia
{
	class NodeXML
	{
	public:
		~NodeXML(void);
		char const* GetName() const noexcept;
		NodeXML* GetParent() const noexcept;
		AutoArray<NodeXML*> const& GetChildren() const noexcept;
		AutoArray<std::pair<char const*, char const*>> const& GetFields() const noexcept;

	private:

		char const* mpBuffer = nullptr;
		char const* mstrName;
		AutoArray<std::pair<char const*, char const*>> mFields;
		AutoArray<NodeXML*> mChildren;
		NodeXML* mpParent;

		friend class XMLParser;
	};

	class XMLParser
	{
	public:

		static NodeXML* Parse(const char* _strFile);

	private:

		static NodeXML* ParseNode(char *&, NodeXML*);
		static NodeXML* ActuallyParse(char *);

		template <typename Pred>
		static char* Skip(char*);

		XMLParser(void) = delete;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Pred>
inline char* Dystopia::XMLParser::Skip(char* _buf)
{
	while (*_buf && !Pred::f(_buf))
		++_buf;

	return _buf;
}


#endif		// INCLUDE GUARD

