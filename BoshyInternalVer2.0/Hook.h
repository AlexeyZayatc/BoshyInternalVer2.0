#pragma once
#include <Windows.h>
#include "memory.h"

// Не болып жатқанын түсінбейсіз бе? Түсіндіруді қараңыз: https://www.youtube.com/watch?v=wZIaj0FDIuI

bool Detour32(BYTE* src, BYTE* dst, const int len);

BYTE* TrampHook32(BYTE* src, BYTE* dst, const int len);

class Hook
{
public:
	Hook(BYTE* src, BYTE* dst, BYTE* PtrToGatewayPtr, const int len);
	void Enable();
	void Disable();
	void Toggle();
private:
	bool bStatus = false;
	BYTE* src = nullptr;
	BYTE* dst = nullptr;
	BYTE* PtrToGatewayPtr = nullptr;
	int len = 0;
	BYTE originalBytes[10]{0};

};

