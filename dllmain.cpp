// dllmain.cpp : Defines the entry point for the DLL application.
#include "includes.h"
//
//DWORD jmpBackAddy;
//DWORD jmpOnehitBackAddy;
//int id;
//void __declspec(naked) hFunction() {
//	__asm {
//		mov edx, id
//		mov [esi+0x08],edx
//		jmp [jmpBackAddy]
//	}
//}
//
//
//void __declspec(naked) oneHitFunction() {
//	_asm {
//		cmp [esp+0x08], 03
//		jne originalcode
//		mov ecx, 0
//		mov[eax + 0x08], ecx
//		jmp exit
//
//		originalcode :
//		mov ecx, [esp + 0x18]
//		mov[eax + 0x08], ecx
//
//		exit:
//		jmp [jmpOnehitBackAddy]
//	}
//}


DWORD WINAPI HackThread(HMODULE hModule) {
	//console
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();

	uintptr_t modulebase = (uintptr_t)GetModuleHandle(L"I Wanna Be The Boshy.exe");
	bool bGod = false, bAutofire = false, bOnehit = false;
	uintptr_t fps_addr = FindDMAAddy(modulebase + 0x59A94, { 0x78 }); //считывание памяти, получение адреса где хранится фпс
	uintptr_t character_addr = FindDMAAddy(modulebase + 0x59A9C, { 0x98,0x500,0x234,0x8,0x8,0x878,0x58 });//получение адреса где хранится айди текущего персонажа
	DWORD OFFSET_GODMOD = 0x48195; //offset в памяти на инструкцию (которую нужно перезаписать чтобы получить бессмертие)
	int id = *(int*)character_addr;

	INPUT ip;//для имитирования нажатия на кнопку заполняются поля(Я не помню че они значат, кроме wVK - код кнопки))))
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	ip.ki.wVk = 0x58;

	int curFPS = *(int*)fps_addr;
	//hooking things
	//DWORD hookAddr = modulebase + 0x4911;
	//int hooklen = 6;
	//jmpBackAddy = hookAddr + hooklen;
	//DWORD oneHitHookAddr = modulebase + 0x125F2;
	//int oneHithooklen = 7;
	//jmpOnehitBackAddy = oneHitHookAddr + hooklen;
	//Hook((BYTE*)(modulebase + 0x4911), hFunction, hooklen);
	//Loop
	while (true) {
		if (bAutofire) {
			ip.ki.dwFlags = 0;
			SendInput(1, &ip, sizeof(INPUT));
			Sleep(10);
			ip.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &ip, sizeof(INPUT));
			Sleep(10); //нажатие клавиши
		}
	
		if (GetAsyncKeyState(0x43) )//"C" 
		{
			bAutofire = !bAutofire;
			display(bAutofire, bGod, curFPS);
			Sleep(100);
		}
		if (GetAsyncKeyState(0x41) )//"A"
		{
			bGod = !bGod;
			display(bAutofire, bGod, curFPS);
			if (bGod) { //0xE9
				patch((BYTE*)(modulebase + OFFSET_GODMOD), (BYTE*)"\x39\xE4\x89\x88\x1C\x01\x00\x00", 8); //изменение байтов ассемблера
			}
			else {
				patch((BYTE*)(modulebase + OFFSET_GODMOD), (BYTE*)"\x39\xDD\x89\x88\x1C\x01\x00\x00", 8);//возвращение исходных
			}
			Sleep(100);
		}
		if (GetAsyncKeyState(VK_OEM_COMMA) ) {
			if ((curFPS - 5) >= 10) {
				curFPS -= 5;
				display(bAutofire, bGod, curFPS);
				Sleep(100);
			}
		}
		if (GetAsyncKeyState(VK_OEM_PERIOD) ) {
			if ((curFPS + 5) < 130) {
				curFPS += 5;
				display(bAutofire, bGod, curFPS);
				Sleep(100);
			}
		}

		if (GetAsyncKeyState(0x44)) //"D"
		{
			//вообщем сначала оно заменяет пустыми инструкциями место, где перезаписывается айди перса
			//если после этого было введено что-то, кроме числа, то оно вернет байты обратно
			//если нет, то заменит перса 
			std::cout << "\nEnter character number): ";
			unsigned character_id;
			std::cin >> character_id;
			if (std::cin.fail()) {
				std::cin.clear();
				std::cin.ignore(99999, '\n');
			}
			else {
				if (character_id >= 1 && character_id <= 36) {
					id = character_id*4+8;
					std::cin.clear();
					std::cin.ignore(99999, '\n');
				}

			}
			Sleep(200);
		}

		//if (GetAsyncKeyState(0x42))//"B"
		//{
		//	if(bOnehit)
		//		patch((BYTE*)(modulebase + 0x4911), (BYTE*)"\x8B\x4C\x24\x18\x89\x48\x08", 7);//возвращение исходных
		//	else
		//		Hook((BYTE*)(modulebase + 0x125F2), oneHitFunction, oneHithooklen);

		//	bOnehit = !bOnehit;
		//	Sleep(200);
		//}

		if (GetAsyncKeyState(VK_END))
		{

			patch((BYTE*)(modulebase + OFFSET_GODMOD), (BYTE*)"\x39\xDD\x89\x88\x1C\x01\x00\x00", 8);//возвращение исходных
			patch((BYTE*)(modulebase + 0x4911), (BYTE*)"\x8B\x51\x08\x89\x56\x08", 6);//возвращение исходных
			break;
		}

		*(int*)fps_addr = curFPS;
		character_addr = FindDMAAddy(modulebase + 0x59A9C, { 0x98,0x500,0x234,0x8,0x8,0x878,0x58 });
		Sleep(5);

	}
    //exit
	fclose(fDummy);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;

}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

