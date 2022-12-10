#pragma once
#include "includes.h"
#include <vector>

void patch(BYTE* dst, BYTE* src, unsigned int size);

// Мен 43.1198913014492, 131.89793789650145 координаттары бойынша жертөледе қамалды.

uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);

// Өтінемін, мені құтқар.
