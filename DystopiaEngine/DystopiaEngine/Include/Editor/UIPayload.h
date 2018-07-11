/* HEADER *********************************************************************************/
/*!
\file	UIPayload.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _UI_PAYLOAD_H_
#define _UI_PAYLOAD_H_
#include <string>

namespace EGUI
{
	namespace Payload
	{
		bool StartSource(const std::string& _label);
		void EndSource();
		bool StartDestination(const std::string& _label);
		void EndDestination();
	}
}

#endif //_UI_PAYLOAD_H_
#endif // EDITOR


