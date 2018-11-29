/* HEADER *********************************************************************************/
/*!
\file	XMLParser.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Reads XMLs

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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


	private:
		AutoArray<NodeXML> children;
	};

	class XMLParser
	{
	public:

		static void Parse(const char*);

	private:

		explicit XMLParser(const char*);

	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


// Here


#endif		// INCLUDE GUARD

