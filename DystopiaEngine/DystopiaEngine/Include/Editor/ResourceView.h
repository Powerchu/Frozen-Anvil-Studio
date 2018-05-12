/* HEADER *********************************************************************************/
/*!
\file	ResourceView.h
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
#ifndef _RESOURCE_VIEW_H_
#define _RESOURCE_VIEW_H_

namespace Dystopia
{
	class ResourceViewer
	{
	public:
		ResourceViewer();
		~ResourceViewer();

		void Init();
		void Update(const float&);
		void Shutdown();

	private:


	};
}

#endif //_RESOURCE_VIEW_H_
#endif //EDITOR





