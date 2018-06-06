/* HEADER *********************************************************************************/
/*!
\file	Serialiser.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Base class for all serialisers. 
	*** Please friend the SerialiserBase. ***

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SERIALISER_BASE_H_
#define _SERIALISER_BASE_H_

#include <string>
#include <fstream>

namespace Dystopia
{
	class Serialiser
	{
	public:
		static constexpr int MODE_READ = std::ios::in;
		static constexpr int MODE_WRITE = std::ios::out;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		template <class T>
		inline static T OpenFile(const std::string&, int = MODE_READ);

		virtual ~Serialiser(void) {};

	protected:

		Serialiser(void) = default;
	};

	template <class Serialiser_t>
	class SerialiserBase : public Serialiser
	{
	public:
		SerialiserBase(SerialiserBase&&) = default;


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void InsertEndBlock  (const std::string& = "");
		void InsertStartBlock(const std::string& = "");

		void ConsumeEndBlock  (void);
		void ConsumeStartBlock(void);

		bool EndOfInput(void) const;

		template <typename T>
		Serialiser_t& Read(T&);

		template <typename T>
		Serialiser_t& Write(const T&);

	protected:

		SerialiserBase(void) : mbBlockRead{ false } {};

		void SetBlockingFlag(bool _bBlockRead);


		// ========================== DERIVED CLASSES SHOULD OVERRIDE THESE ========================== // 

		void ReadEndBlock   (void);
		bool ReadStartBlock (void);
		void WriteEndBlock  (const std::string&);
		void WriteStartBlock(const std::string&);

		template <typename T>
		void ApplyRead(T&);
		template <typename T>
		void ApplyWrite(T&);

		bool Validate(void);

		// The above functions in this section all default to doing nothing
		// If doing nothing is the desired action, it is ok to not override

	private:

		bool mbBlockRead;

		Serialiser_t& GetType(void);
	};
}

template <class Serialiser_t, typename T>
Serialiser_t& operator << (Dystopia::SerialiserBase<Serialiser_t>& _file, const T& _rhs);

template <class Serialiser_t, typename T>
Serialiser_t& operator >> (Dystopia::SerialiserBase<Serialiser_t>& _file, const T& _rhs);






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <class T>
inline T Dystopia::Serialiser::OpenFile(const std::string& _strFile, int _nFlags)
{
	return T::OpenFile(_strFile, _nFlags);
}

template<class Serialiser_t>
inline void Dystopia::SerialiserBase<Serialiser_t>::InsertEndBlock(const std::string& _strBlockName)
{
	GetType().WriteEndBlock(_strBlockName);
}

template<class Serialiser_t>
inline void Dystopia::SerialiserBase<Serialiser_t>::InsertStartBlock(const std::string& _strBlockName)
{
	GetType().WriteStartBlock(_strBlockName);
}

template<class Serialiser_t>
inline void Dystopia::SerialiserBase<Serialiser_t>::ConsumeEndBlock(void)
{
	GetType().ReadEndBlock();
}

template<class Serialiser_t>
inline void Dystopia::SerialiserBase<Serialiser_t>::ConsumeStartBlock(void)
{
	mbBlockRead = !GetType().ReadStartBlock();
}

template<class Serialiser_t>
inline bool Dystopia::SerialiserBase<Serialiser_t>::EndOfInput(void) const
{
	return mbBlockRead;
}

template<class Serialiser_t>
inline void Dystopia::SerialiserBase<Serialiser_t>::SetBlockingFlag(bool _bBlockRead)
{
	mbBlockRead = _bBlockRead;
}

template <class Serialiser_t> template <typename T>
Serialiser_t& Dystopia::SerialiserBase<Serialiser_t>::Write(const T& _rhs)
{
	GetType().ApplyWrite(_rhs);

	return GetType();
}

template <class Serialiser_t> template <typename T>
Serialiser_t& Dystopia::SerialiserBase<Serialiser_t>::Read(T& _rhs)
{
	if (!mbBlockRead)
		GetType().ApplyRead(_rhs);
	else
		_rhs = T{};

	mbBlockRead = !GetType().Validate();
	return GetType();
}

template<class Serialiser_t>
inline void Dystopia::SerialiserBase<Serialiser_t>::ReadEndBlock(void)
{
}

template<class Serialiser_t>
inline bool Dystopia::SerialiserBase<Serialiser_t>::ReadStartBlock(void)
{
	return false;
}

template<class Serialiser_t>
inline void Dystopia::SerialiserBase<Serialiser_t>::WriteEndBlock(const std::string &)
{
}

template<class Serialiser_t>
inline void Dystopia::SerialiserBase<Serialiser_t>::WriteStartBlock(const std::string &)
{
}

template<class Serialiser_t> template<typename T>
inline void Dystopia::SerialiserBase<Serialiser_t>::ApplyRead(T&)
{
}

template<class Serialiser_t> template<typename T>
inline void Dystopia::SerialiserBase<Serialiser_t>::ApplyWrite(T&)
{
}

template<class Serialiser_t>
inline bool Dystopia::SerialiserBase<Serialiser_t>::Validate(void)
{
	return true;
}

template<class Serialiser_t>
inline Serialiser_t & Dystopia::SerialiserBase<Serialiser_t>::GetType(void)
{
	return static_cast<Serialiser_t&>(*this);
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <class Serialiser_t, typename T>
Serialiser_t& operator << (Dystopia::SerialiserBase<Serialiser_t>& _file, const T& _rhs)
{
	return _file.Write(_rhs);
}

template <class Serialiser_t, typename T>
Serialiser_t& operator >> (Dystopia::SerialiserBase<Serialiser_t>& _file, T& _rhs)
{
	return _file.Read(_rhs);
}



#endif		// INCLUDE GUARD

