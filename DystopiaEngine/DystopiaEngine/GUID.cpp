#include "GUID.h"
#include <chrono>
#include <ctime>

namespace Dystopia
{
	ulong GUIDGenerator::GetUniqueID()
	{
		using TimeUnit = std::chrono::nanoseconds;
		/*Current time in milliseconds unit*/
		auto currTime_ms = std::chrono::duration_cast<TimeUnit>(std::chrono::system_clock::now().time_since_epoch());

		return static_cast<long>(currTime_ms.count()) << 19;
	}
}