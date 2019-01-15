/* HEADER *********************************************************************************/
/*!
\file	DatabaseSystem.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _DATABASE_SYSTEM_H_
#define _DATABASE_SYSTEM_H_

#pragma warning(push)
#pragma warning(disable : 4251)

#include "System/Base/Systems.h"
#include "DataStructure/HashString.h"
#include "DataStructure/AutoArray.h"
#include "System/Database/DataSheet.h"

#include <utility> //std pair

namespace Dystopia
{
	class _DLL_EXPORT DatabaseSystem : public Systems
	{
	public:

		DatabaseSystem(void);
		~DatabaseSystem(void);

		bool Init(void);
		void Update(float);
		void Shutdown(void);

		void LoadDefaults(void) override;
		void LoadSettings(DysSerialiser_t&) override;
		void SaveSettings(DysSerialiser_t&) override;


		bool NewData(const HashString& _name);
		bool OpenData(const HashString& _name);
		bool CloseData(const HashString& _name);
		void UnloadData(const HashString& _name);
		DataSheet* GetDatabase(const HashString& _name);
		void SaveAllSheets(void);

		AutoArray<std::pair<HashString, DataSheet>>& GetAllSheets(void);
		AutoArray<const char *>& GetAllNames(void);

	private:

		AutoArray<std::pair<HashString, DataSheet>> mArrDataSheets;
		AutoArray<const char *> mArrSheetNames;

		HashString GetPathFormatted(const HashString& _name);

	};
}

#pragma warning(pop)
#endif 







