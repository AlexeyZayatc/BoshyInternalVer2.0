// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Hook.h"
std::uintptr_t modulebase = reinterpret_cast<std::uintptr_t>(GetModuleHandle(0));
bool bGod = false, bAutofire = false, bOnehit = false, patched = false;

std::uintptr_t fps_addr = FindDMAAddy(modulebase + 0x59A94, { 0x78 }); //считывание памяти, получение адреса где хранится фпс
std::uintptr_t character_addr = FindDMAAddy(modulebase + 0x59A98, { 0x8D0,0x158,0x8,0x18,0x268,0x58 });//получение адреса где хранится айди текущего персонажа

DWORD OFFSET_GODMOD = 0x48195; //offset в памяти на инструкцию (которую нужно перезаписать чтобы получить бессмертие)
DWORD OFFSET_CHARACTER = 0x4911;

int character_id;

int curFPS ;

typedef BOOL(__cdecl* FrameFunction) (int a, int b);

FrameFunction FrameFuncGateway;
FrameFunction oFrameFunc;

BOOL __cdecl hFrameFunc(int a, int b) {

    
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
    if (patched) *(int*)character_addr = character_id;
    return FrameFuncGateway(a,b);
}


typedef BOOL(__cdecl* CharacterFunction) ();

//найти перепись персонажа получше)))))))) (check naked)

CharacterFunction CharacterFuncGateway;

BOOL __cdecl hCharacterFunc() {

    return CharacterFuncGateway();
}

typedef BOOL(__cdecl* OneHitFunction)();

OneHitFunction OneHitFunctionGateway;
//cmp dword ptr[eax + 0x08], 1
//jle exit
//mov ecx, 0
//mov[eax + 0x08], ecx
DWORD jmpBackAddy;
        //cmp dword ptr [esp + 0x08], 1
        //je originalcode

        //jmp exit

        //originalcode:
          //      exit:
        //jmp [jmpBackAddy]
int check=0;
void __declspec(naked) hOneHitFunction() {
    
    __asm {
        mov ebx, [esp + 0x08]
        mov check, ebx
    }
    if (check == 1) {
        __asm{
            mov ecx, [esp + 0x18]
            mov[eax + 0x08], ecx
            jmp[jmpBackAddy]
	    }
    }
    else {
        __asm {
            mov ebx, [eax+0x08]
            mov check, ebx
        }
        if (check > 1) {
            __asm {
                mov ecx, 0
                mov [eax+0x08], ecx
            }
        }
        __asm {
            jmp [jmpBackAddy]
        }
    }

}


DWORD WINAPI HackThread(HMODULE hModule) {
    if (!AllocConsole())
        MessageBox(NULL, L"The console window was not created", NULL, MB_ICONEXCLAMATION);

    FILE* fp;

    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);

    std::cout.clear();



     character_id = *(int*)character_addr;

     curFPS = *(int*)fps_addr;
   
  //  std::cout << std::hex << "modulebase: " << modulebase;
   // oFrameFunc = (FrameFunction)(modulebase + 0x48110);
   // oFrameFunc = (FrameFunction)TrampHook32((BYTE*)oFrameFunc, (BYTE*)hFrameFunc, 5);

    Hook FrameHook((BYTE*)(modulebase + 0x48110), (BYTE*)hFrameFunc, (BYTE*)&FrameFuncGateway, 5);
    FrameHook.Enable();
    //Hook CharacterHook((BYTE*)(modulebase + OFFSET_CHARACTER), (BYTE*)hCharacterFunc, (BYTE*)&CharacterFuncGateway, 6);
    //CharacterHook.Enable();
    // memset(CharacterFuncGateway, '\x90', 3);z
    BYTE OneHitBackBytes[7]{ 0 };
    memcpy(OneHitBackBytes, (BYTE*)(modulebase + 0x125F2), 7);
   Detour32((BYTE*)(modulebase+0x125F2), (BYTE*)hOneHitFunction, 7);
    jmpBackAddy = modulebase + 0x125F2 + 7;
    INPUT ip;//для имитирования нажатия на кнопку заполняются поля(Я не помню че они значат, кроме wVK - код кнопки))))
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.wVk = 0x58;
    display(bAutofire, bGod, curFPS);
    while (true)
    {
        if (bAutofire) {
            //нажатие клавиши
            ip.ki.dwFlags = 0;
            SendInput(1, &ip, sizeof(INPUT));
            Sleep(10);
            ip.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &ip, sizeof(INPUT));
            Sleep(10);
        }
        if (GetAsyncKeyState(0x43))//"C" 
        {
            bAutofire = !bAutofire;
             display(bAutofire, bGod, curFPS);
            Sleep(100);
        }
        if (GetAsyncKeyState(VK_END))
        {
            FrameHook.Disable();
            patch((BYTE*)(modulebase + OFFSET_GODMOD), (BYTE*)"\x39\xDD\x89\x88\x1C\x01\x00\x00", 8);//возвращение исходных
            patch((BYTE*)(modulebase + 0x125F2), OneHitBackBytes, 7);
           // patch((BYTE*)(modulebase + 0x4911), (BYTE*)"\x8B\x51\x08\x89\x56\x08", 6);//возвращение исходных
            break;
        }
        if (GetAsyncKeyState(0x46)) //"F"
        {
            FrameHook.Toggle();
            patch((BYTE*)(modulebase + OFFSET_GODMOD), (BYTE*)"\x39\xDD\x89\x88\x1C\x01\x00\x00", 8);//возвращение исходных
            Sleep(100);
        }
        if (GetAsyncKeyState(0x44)) //"D"
        {
            if (!patched) {
                patch((BYTE*)(modulebase + OFFSET_CHARACTER), (BYTE*)"\x90\x90\x90\x90\x90\x90", 6);
                patched = true;
            }
            std::cout << "\nEnter character number: ";
            int temp_char_id;
            std::cin >> temp_char_id;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            else
            {
                if (temp_char_id >= 1 && temp_char_id <= 36) {
                    character_id = temp_char_id * 4 + 8;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    Sleep(100);
                }
            }
        }
        if (GetAsyncKeyState(0x45)) //"E"
        {
            character_addr = FindDMAAddy( modulebase + 0x59AB8, { 0x0,0x1C4,0x420,0x30,0x84,0x470,0x58 });
            Sleep(100);
        }
        if (GetAsyncKeyState(0x57))//"W" 
        {
            if (patched) {
                patch((BYTE*)(modulebase + OFFSET_CHARACTER), (BYTE*)"\x8B\x51\x08\x89\x56\x08", 6);
                patched = false;
            }
            Sleep(100);
        }
        *(int*)fps_addr = curFPS;
        Sleep(10);
    }
    fclose(fp);
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
        CloseHandle(CreateThread(nullptr,0,(LPTHREAD_START_ROUTINE)HackThread,hModule,0,nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

