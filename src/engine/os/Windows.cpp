#include "engine/os/Utils.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace tdrp::os
{

std::string CalculateComputerName()
{
	TCHAR nameBuffer[MAX_COMPUTERNAME_LENGTH];
	DWORD nameSize = MAX_COMPUTERNAME_LENGTH;

	if (::GetComputerName(nameBuffer, &nameSize) == TRUE)
	{
		std::string result;

#if defined(UNICODE) || defined(_UNICODE)
		// Translate to UTF-8.
		int len = ::WideCharToMultiByte(CP_UTF8, 0, nameBuffer, nameSize, 0, 0, 0, 0);
		if (len > 0)
		{
			// Pre-allocate the string and re-run the conversion.
			result.append(len, '\0');
			len = ::WideCharToMultiByte(CP_UTF8, 0, nameBuffer, nameSize, &result[0], len, 0, 0);
		}
#else
		result.assign(nameBuffer, nameSize);
#endif

		return result;
	}

	return std::string("DEFAULT");
}

} // end namespace tdrp::os
