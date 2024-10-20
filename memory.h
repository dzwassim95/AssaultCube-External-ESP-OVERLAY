#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>
#include <TlHelp32.h>
#include <string>

class memory
{
private:
	DWORD procID{};
	HANDLE procH{ nullptr };
public:

	memory(const WCHAR* procN);

	HANDLE GetProcH() const
	{
		return procH;
	}

	std::uintptr_t GetModuleBaseAddr(const WCHAR* moduleN) const;
	std::string readName(std::uintptr_t addr) const;

	template<typename T>
	T read(const std::uintptr_t addr) const
	{
		T value{};
		ReadProcessMemory(procH, (LPCVOID)addr, &value, sizeof(T), NULL);
		return value;
	}

	template<typename T>
	void write(std::uintptr_t addr, const T& value) const
	{
		WriteProcessMemory(procH, (LPVOID)addr, &value, sizeof(T), NULL);
	}
};