#include "memory.h"

memory::memory(const WCHAR* procN)
{
	PROCESSENTRY32 entry{};
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(snapshot, &entry))
	{
		if (!wcscmp(procN, entry.szExeFile))
		{
			procID = entry.th32ProcessID;
			procH = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
		}
		else
		{
			while (Process32Next(snapshot, &entry))
			{
				if (!wcscmp(procN, entry.szExeFile))
				{
					procID = entry.th32ProcessID;
					procH = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
					break;
				}
			}
		}

	}
	if (snapshot)
	{
		CloseHandle(snapshot);
	}
}

std::uintptr_t memory::GetModuleBaseAddr(const WCHAR* moduleN) const
{
	MODULEENTRY32 entry{};
	entry.dwSize = sizeof(MODULEENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procID);

	std::uintptr_t result{};

	if (Module32First(snapshot, &entry))
	{
		if (!wcscmp(moduleN, entry.szModule))
		{
			result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
		}
		else
		{
			while (Module32Next(snapshot, &entry))
			{
				if (!wcscmp(moduleN, entry.szModule))
				{
					result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
					break;
				}
			}
		}

	}
	if (snapshot)
	{
		CloseHandle(snapshot);
	}
	return result;
}

std::string memory::readName(std::uintptr_t addr) const {
	std::string name;
	char ch;

	// Loop and read characters until a null terminator is found
	while ((ch = read<char>(addr)) != '\0') {
		name += ch;
		addr += sizeof(char);  // Move to the next character in memory
	}

	return name;
}

