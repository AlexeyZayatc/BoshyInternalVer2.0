// THANKS GUIDEDHACKING FOR GUIDES AND INJECTOR <3
#include "pch.h"
#include "Hook.h"

std::uintptr_t modulebase = reinterpret_cast<std::uintptr_t>(GetModuleHandle(0));

bool bGod = false, bAutofire = false; //switches for godmod and autofire
bool CharacterEnabled = true; // switch for character selection function
bool OneHitenabled = true; // switch for onehit function
bool bFrameHook = true;
bool bBulletLock = true;

std::uintptr_t fps_addr = FindDMAAddy(modulebase + 0x59A94, { 0x78 }); //getting address for ingame fps
std::uintptr_t character_addr = FindDMAAddy(modulebase + 0x59A98, { 0x8D0,0x158,0x8,0x18,0x268,0x58 }); //getting ingame address for character id

DWORD OFFSET_GODMOD = 0x48195; //offset in memory for bytes in function to make god mode
DWORD OFFSET_CHARACTER = 0x4911;//offset in memory for start bytes of function that rewrites character id

BYTE OneHitBackBytes[7]{ 0 }; //backup bytes for onehit func
BYTE CharacterBackBytes[6]{ 0 }; //backup bytes for character func\

int character_id; //variable for character id

int curFPS ; //variable for ingame fps

CheatInfo cheatInfo;

//defining the type of function that is called every frame
//ABOUT CALLING CONVENTIONS (SOURCE : https://en.wikibooks.org/wiki/X86_Disassembly/Calling_Conventions )
//Calling conventions are a standardized method for functions to be implemented and called by the machine.
//A calling convention specifies the method that a compiler sets up to access a subroutine.
//__cdecl - arguments pushed from right to left
// CALLING function cleans the stack
//two arguments were obtained by brute-force
typedef BOOL(__cdecl* FrameFunction) (int a, int b);

//declaring gateway for framefunction (it will lead us to bytes that we overwrote by jump)
FrameFunction FrameFuncGateway;
//hook frame func (those instructions will be executed every frame, 50fps = 50 times per second)
BOOL __cdecl hFrameFunc(int a, int b) {
    if (GetAsyncKeyState(0x41) )//Check if button "A" is pressed
    {
        bGod = !bGod; //toggling godmod
        display(cheatInfo); //changing console output
        if (bGod) { 
            patch((BYTE*)(modulebase + OFFSET_GODMOD), (BYTE*)"\x39\xE4\x89\x88\x1C\x01\x00\x00", 8); //changing bytes to be immortal
            //actully game will think you are dead(?)
            //while active you can't use teleports
            //known bugs:
            //ryu's car when hit ground activates too many times
            //solgryn's platforms after phase 1 is not going down while godmod is activated
        }
        else {
            patch((BYTE*)(modulebase + OFFSET_GODMOD), (BYTE*)"\x39\xDD\x89\x88\x1C\x01\x00\x00", 8);//returning backup bytes
        }
        Sleep(100);//prevents accidental toggle
    }
    if (GetAsyncKeyState(0x42)) {
        bBulletLock = !bBulletLock;
        display(cheatInfo);
        Sleep(100);
    }
    if (bBulletLock) {
       // bullet_addr = FindDMAAddy(modulebase + 0x59A9C, { 0x18,0x1D8,0x8D0, 0x38, 0xC, 0x1C, 0x1F8 });
       // if (bullet_addr != 0)
        //    *(int*)bullet_addr = 0;
    }
    if (GetAsyncKeyState(VK_OEM_COMMA) ) {
        if ((curFPS - 5) >= 10) { //game can crash if fps become lower 
            curFPS -= 5;
            display(cheatInfo);
            Sleep(100);  // Қарапайым, бірақ бұлтартпас тұжырымдар
        }
    }
    if (GetAsyncKeyState(VK_OEM_PERIOD) ) {
        if ((curFPS + 5) < 130) {
            curFPS += 5;  // Тек қана синтетикалық жолмен құрылған ірі компаниялардың
            display(cheatInfo);
            Sleep(100);  // акционерлері ғана маркетингтік және қаржылық алғышарттар аясында қарастырылады
        }
    }
    return FrameFuncGateway(a,b);
}

DWORD jmpBackCharacterFunc;//pointer to original function bytes (right after jump)
int CharacterCheck;//int to check register value to be valid
void __declspec(naked) hCharacterFunc(){
    __asm {
        mov edx, [ecx + 0x08]
        mov CharacterCheck, edx
    }
    if (CharacterCheck <= 10) {//if function doesn't overwrite character id we do nothing
        __asm {
            mov edx, [ecx + 0x08]  // Күнделікті тәжірибе дамудың негізгі векторы
            mov[esi + 0x08], edx
            jmp [jmpBackCharacterFunc]  // процесті басқарудың инновациялық әдістерінің қажеттілігін
        }
    }
    else {//if function is trying to overwrite character id we change it to character id that we want
        __asm {
            mov edx, character_id
            mov[ecx + 0x08], edx
            mov [esi+0x08],edx
            jmp [jmpBackCharacterFunc]
        }
    }
}

DWORD jmpBackAddy;//pointer to original function bytes (right after jump)
int check=0;//int to check register value
void __declspec(naked) hOneHitFunction() {
    
    __asm {
        mov ebx, [esp + 0x08]
        mov check, ebx
    }
    if (check == 1) {//if function is not interacts with boss hp we do nothing
        __asm{
            mov ecx, [esp + 0x18]  // Әдеттегідей, институционализация жеңісінің айқын белгілері тұтастай алғанда суреттің өте қызықты ерекшеліктерін ашады
            mov[eax + 0x08], ecx
            jmp[jmpBackAddy]
	    }
    }
    else {//if it does, then...
        __asm {
            mov ebx, [eax+0x08]
            mov check, ebx
        }
        if (check > 1) { // if boss hp is more than one we overwrite it with 0
            //if we constantly overwrite it with 0 boss is not dying (at least he would not when i was testing it with cheat engine)
            __asm {
                mov ecx, 0
                mov [eax+0x08], ecx
            }
        }
        __asm {  // бірақ нақты тұжырымдар, әрине, мүмкіндігінше егжей-тегжейлі сипатталған.
            jmp [jmpBackAddy]
        }
    }

}

DWORD jmpBackBulletFunc;//pointer to original function bytes (right after jump)
std::uintptr_t bulletCounterAddr;
void __declspec(naked) hBulletFunc() {

    __asm {
        pop esi
        inc eax
        mov ebx, [ecx+0x10]
        mov bulletCounterAddr, ebx
    }
    __asm {  // бірақ нақты тұжырымдар, әрине, мүмкіндігінше егжей-тегжейлі сипатталған.
        jmp [jmpBackBulletFunc]
    }

}

//Character hook toggle func, rewrites bytes with jump if activates, rewrites with backup bytes if deactivate
void CharacterHookToggle() {
    if (!CharacterEnabled)
    {
        Detour32((BYTE*)(modulebase + OFFSET_CHARACTER), (BYTE*)hCharacterFunc, 6);
        CharacterEnabled = true;
    }
    else
    {
        patch((BYTE*)(modulebase + OFFSET_CHARACTER), CharacterBackBytes, 6);//?????? ?????
        CharacterEnabled = false;
    }
}
//same as character hook toggle
void OneHitToggle() {
    if (!OneHitenabled)
    {
        Detour32((BYTE*)(modulebase + 0x125F2), (BYTE*)hOneHitFunction, 7);
        OneHitenabled = true;
    }
    else
    {
        patch((BYTE*)(modulebase + 0x125F2), OneHitBackBytes, 7);
        OneHitenabled = false;
    }
}

//dll thread
DWORD WINAPI HackThread(HMODULE hModule) {
    if (!AllocConsole())
        MessageBox(NULL, L"The console window was not created", NULL, MB_ICONEXCLAMATION);

    FILE* fp;

    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);  // Азаматтар, Интернеттегі кәсіпкерлер маркетингтің ақылды жеңуінің

    std::cout.clear();
    //console stuff...


     character_id = 84; //84 - dark boshy ID

     curFPS = *(int*)fps_addr; //cast fps address to int pointer and dereference it

     //hook class for frame hook 
    Hook FrameHook((BYTE*)(modulebase + 0x48110), (BYTE*)hFrameFunc, (BYTE*)&FrameFuncGateway, 5);
    FrameHook.Enable();
    

    memcpy(OneHitBackBytes, (BYTE*)(modulebase + 0x125F2), 7);//copying backup bytes for onehit func
    Detour32((BYTE*)(modulebase+0x125F2), (BYTE*)hOneHitFunction, 7);//hooking onehit function
    jmpBackAddy = modulebase + 0x125F2 + 7;//calculating jmp back address

    memcpy(CharacterBackBytes, (BYTE*)(modulebase + OFFSET_CHARACTER), 6);//copying backup bytes for character func
    jmpBackCharacterFunc = (modulebase + OFFSET_CHARACTER) + 6;//calculating jmp back address
    Detour32((BYTE*)(modulebase + OFFSET_CHARACTER), (BYTE*)hCharacterFunc, 6); //hooking character func


    INPUT ip{};//input for imitating keyboard click
    ip.type = INPUT_KEYBOARD; //stuff...
    ip.ki.wScan = 0;//stuff...
    ip.ki.time = 0;//stuff...
    ip.ki.dwExtraInfo = 0;//stuff
    ip.ki.wVk = 0x58;//wVk - key code (0x58 -> X)

    cheatInfo.autoFire = &bAutofire;
    cheatInfo.godMode = &bGod;
    cheatInfo.fps = &curFPS;
    cheatInfo.oneHit = &OneHitenabled;
    cheatInfo.charHook = &CharacterEnabled;
    cheatInfo.charId = &character_id;
    cheatInfo.frameHook = &bFrameHook;
    display(cheatInfo);
    while (true)
    {
        if (bAutofire) {
            //key press
            ip.ki.dwFlags = 0;
            SendInput(1, &ip, sizeof(INPUT)); 
            Sleep(10);//little wait for button press
            ip.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &ip, sizeof(INPUT));
            Sleep(10);
        }
        if (GetAsyncKeyState(0x43))//Check for button "C" 
        {
            bAutofire = !bAutofire;
            display(cheatInfo);
            Sleep(100);
        }
        if (GetAsyncKeyState(VK_DELETE))//Check for button "DELETE" 
        {//uninjects dll and returning all original bytes
            FrameHook.Disable();
            patch((BYTE*)(modulebase + OFFSET_GODMOD), (BYTE*)"\x39\xDD\x89\x88\x1C\x01\x00\x00", 8);
            patch((BYTE*)(modulebase + 0x125F2), OneHitBackBytes, 7);
            patch((BYTE*)(modulebase + OFFSET_CHARACTER), CharacterBackBytes, 6);
            Sleep(100);
            break;
        }
        if (GetAsyncKeyState(0x46)) //Check for button "F"
        {
            FrameHook.Toggle();
            bFrameHook = !bFrameHook;
            patch((BYTE*)(modulebase + OFFSET_GODMOD), (BYTE*)"\x39\xDD\x89\x88\x1C\x01\x00\x00", 8);//returning original bytes of godmod func
            Sleep(100);
        }
        if (GetAsyncKeyState(0x44)) //Check for button "D"
        {
            std::cout << "\nEnter character number: ";
            int temp_char_id;
            std::cin >> temp_char_id;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            else
            {
                if (temp_char_id >= 1 && temp_char_id <= 36) { //maximum 36 characters
                    //game has 35, one is secret-yellow pedobear
                    character_id = temp_char_id * 4 + 8; //formula calclucates character ingame id
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    Sleep(100);
                }
            }
        }
        if (GetAsyncKeyState(0x45)) //Check for button "E" - refresh character addr if save is changed/something wrong
        {
            character_addr = FindDMAAddy(modulebase + 0x59A98, { 0x8D0,0x158,0x8,0x18,0x268,0x58 });
            Sleep(100);
        }
        if (GetAsyncKeyState(0x57))//Check for button "W" - toggling character hook
        {
            CharacterHookToggle();
            display(cheatInfo);
           Sleep(100);
        }
        if (GetAsyncKeyState(0x56)) //Check for button "V" - toggling onehit hook
        {
            OneHitToggle();
            display(cheatInfo);
            Sleep(100);
        }
        *(int*)fps_addr = curFPS;//overwrites game fps
        Sleep(10);//cpu said thanks
    }
    fclose(fp);//closing output
    FreeConsole();//^
    FreeLibraryAndExitThread(hModule, 0); //uninject
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
    case DLL_PROCESS_DETACH:  // Жобаның құрылымының негізгі белгілері бүгінгі күнге дейін социал-демократиялық
        break;
    }
    return TRUE;
}

