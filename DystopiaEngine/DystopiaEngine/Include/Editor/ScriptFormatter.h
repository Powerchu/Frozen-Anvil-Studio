/* HEADER *********************************************************************************/
/*!
\file	ScriptFormatter.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _Script_Formatter_H_
#define _Script_Formatter_H_
#include <string>

namespace Dystopia
{
	bool GenerateScript(const std::string& _className, const std::string& _creatorName, const std::string& _creatorLogin);
}


#endif //_Script_Formatter_H_
#endif //EDITOR

