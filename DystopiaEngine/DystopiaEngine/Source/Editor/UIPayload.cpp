/* HEADER *********************************************************************************/
/*!
\file	UIPayload.cpp
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
#include "UIPayload.h"

namespace EGUI{
namespace Payload{

	bool StartSource(const std::string& _label)
	{
		return true;
	}
	
	void EndSource()
	{
	
	}
	
	bool StartDestination(const std::string& _label)
	{	
		return true;
	}
	
	void EndDestination()
	{
	
	}



}}	// namespace EGUI::Payload
#endif //EDITOR
