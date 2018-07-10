/* HEADER *********************************************************************************/
/*!
\file	Commands.h
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
#ifndef _COMMAND_LIST_H_
#define _COMMAND_LIST_H_
#include "Math\Vector4.h"

namespace Dystopia
{
	struct Commands
	{
		virtual void ExecuteDo() = 0;
		virtual void ExecuteUndo() = 0;
		virtual bool Unchanged() const = 0;
	};

	template <typename T>
	struct ComdModifyValue : Commands
	{
		ComdModifyValue(T* _value, const T& _newV)
			: mValue{ _value }, mOldValue{ *_value }, mNewValue{ _newV }
		{}

		~ComdModifyValue()
		{}

		void ExecuteDo() override
		{
			*mValue = mNewValue;
		}
		void ExecuteUndo() override
		{
			*mValue = mOldValue;
		}

		bool Unchanged() const
		{
			return (mOldValue == mNewValue);
		}

	private:
		T mOldValue;
		T mNewValue;
		T* mValue;
	};

	template <typename T>
	struct ComdCreateObject : Commands
	{
		ComdCreateObject(T*& _object, const Math::Vec4& _pos)
			: mObject{ _object }
		{
			_object = nullptr;
		}
	
		~ComdCreateObject()
		{
			delete mObject;
		}
	
		void ExecuteDo() override
		{
			//mInstance = _object->Duplicate();
		}
		void ExecuteUndo() override
		{
			//*mValue = oldValue;
		}

		bool Unchanged() const
		{
			return false;
		}
	
	private:
		T* mObject;
	};


	struct RecordBase : Commands
	{
		virtual void EndRecord() = 0;
		virtual ~RecordBase() {}
	};

	template <typename T>
	struct ComdRecord : RecordBase
	{
		ComdRecord(T*& rhs)
			: mpTarget{ rhs }, mOldValue{ *mpTarget }, mNewValue{ mOldValue }
		{}

		void EndRecord()
		{
			mNewValue = *mpTarget;
		}

		T* GetPointer()
		{
			return mpTarget;
		}

		void ExecuteDo() override
		{
			*mpTarget = mNewValue;
		}

		void ExecuteUndo() override
		{
			*mpTarget = mOldValue;
		}

		bool Unchanged() const
		{
			return (mOldValue == mNewValue);
		}

	private:
		T* mpTarget;
		T mOldValue;
		T mNewValue;
	};
}

#endif //_COMMAND_LIST_H_
#endif //EDITOR
