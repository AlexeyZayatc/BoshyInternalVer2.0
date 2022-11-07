#pragma once
#include "includes.h"
#include <vector>

void patch(BYTE* dst, BYTE* src, unsigned int size);

uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
