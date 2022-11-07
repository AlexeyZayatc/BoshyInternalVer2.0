// pch.cpp: source file corresponding to the pre-compiled header

#include "pch.h"

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.
void patch(BYTE* dst, BYTE* src, unsigned int size) {
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

std::uintptr_t FindDMAAddy(std::uintptr_t ptr, std::vector<unsigned int> offsets) {
	std::uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++) {
		try {
			addr = *(uintptr_t*)addr;
			addr += offsets[i];
		}
		catch(const char* exception) {
			std::cout << "\n EXCEPTION WHILE FINDING MULTILEVEL POINTER : " << exception << std::endl;
			return 0;
		}
	}
	return addr;


}


const void display(const bool& baf, const bool& bgm, int fps) noexcept {
	system("cls");
	std::string status[2]{ "DISABLED","ENABLED" };
	std::cout << "Auto-fire is: " << status[baf]
		<< "\nGod-mode is: " << status[bgm]
		<< "\nCurrent max_fps: " << fps;
}