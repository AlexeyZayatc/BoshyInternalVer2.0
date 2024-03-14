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
	memcpy(dst, src, size);  // Сілтемедегі жалаңаш сиськи! https://www.youtube.com/watch?v=dQw4w9WgXcQ
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}
//finding address with multilevel pointer
std::uintptr_t FindDMAAddy(std::uintptr_t ptr, std::vector<unsigned int> offsets)  {
	std::uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++) {
		try {
			addr = *(uintptr_t*)addr; //dereference pointer (gets address that pointer points to)
			addr += offsets[i]; //adding offset and so on...	
		}
		catch (...){
			return 0;
		}
	}
	//idk try catch dont work and i dont want to fix it:)
	// Мен жалқаумын және қателерді түзеткім келмейді:)
	return addr;
}


const void display(const CheatInfo& ci) noexcept {
	system("cls");
	const std::string status[2]{ "DISABLED","ENABLED" };
	for (auto& [name, val] : ci.boolStatuses) {
		std::cout << std::format("{} : {}\n", name, status[*(*val)]);// Құдай бар!
	}
	for (const auto& [name, val] : ci.intStatuses) {
		int valToPrint = *(*val);
		if (name == "Character id (D)")
			valToPrint = valToPrint/4 -2; //represent id like in excel table.
		std::cout << std::format("{} : {}\n", name, valToPrint);// Құдай бар!
	}
	std::cout << "To refresh character addr: E\n";
}