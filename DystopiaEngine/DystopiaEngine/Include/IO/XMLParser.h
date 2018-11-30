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
#ifndef _TEXT_SERIALISER_H_
#define _TEXT_SERIALISER_H_

#include "IO/Serialiser.h"
#include "DataStructure/AutoArray.h"


namespace Dystopia
{
	class NodeXML
	{
	public:

		char const* GetName() const noexcept;
		NodeXML* GetParent() const noexcept;
		AutoArray<NodeXML> const& GetChildren() const noexcept;
		AutoArray<std::pair<char const*, char const*>> const& GetFields() const noexcept;

	private:

		char const* mstrName;
		AutoArray<std::pair<char const*, char const*>> mFields;
		AutoArray<NodeXML*> mChildren;
		NodeXML* mpParent;

		friend class XMLParser;
	};

	class XMLParser
	{
	public:

		static NodeXML* Parse(const char*);

	private:

		static NodeXML* ParseNode(const char *&, NodeXML*);
		static NodeXML* ActuallyParse(const char *);

		template <typename Pred>
		static char const* Skip(char const*);

	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Pred>
inline char const* Dystopia::XMLParser::Skip(char const* _buf)
{
	while (*_buf && !Pred::f(_buf))
		++_buf;

	return _buf;
}


#endif		// INCLUDE GUARD

