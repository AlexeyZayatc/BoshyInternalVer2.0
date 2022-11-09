// pch.cpp: source file corresponding to the pre-compiled header
// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.

#include "pch.h"
//patches function bytes
//changing dst bytes protection to read and write
//copies there src bytes
//then restoring protection
void patch(BYTE* dst, BYTE* src, unsigned int size) {
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}
//finding address with multilevel pointer
std::uintptr_t FindDMAAddy(std::uintptr_t ptr, std::vector<unsigned int> offsets) {
	std::uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++) {
		try {
			addr = *(uintptr_t*)addr; //dereference pointer (gets address that pointer points to)
			addr += offsets[i]; //adding offset and so on...
		}
		catch(const char* exception) {
			std::cout << "\n EXCEPTION WHILE FINDING MULTILEVEL POINTER : " << exception << std::endl;
			return 0;
		}
	}
	//idk try catch dont work and i dont want to fix it:)
	return addr;
}


const void display(const bool& baf, const bool& bgm, int fps, const bool& oh, const bool& ch) noexcept {
	system("cls");
	const std::string status[2]{ "DISABLED","ENABLED" };
	std::cout << "Auto-fire is: " << status[baf]
		<< "\nGod-mode is: " << status[bgm]
		<< "\nCurrent max_fps: " << fps
		<< "\nOneHitStatus: " << status[oh]
		<< "\nCharacterStatus: " << status[ch];
}