#include "pch.h"
#include "Hook.h"
// Қарама-қарсы көзқарас саяси процестің элементтері негізгі факторлардың рөліне үміткер ретінде көрсетілгенін білдіреді. Айта кету керек, жоспарланған көрсеткіштердің орындалуы бар мінез-құлық үлгілерін талдауға тікелей байланысты. Тек сілтеме диаграммалары сала бойынша ассоциативті түрде таратылады. Мақсатты аудитория өкілдерін тартудың жоғары деңгейі қарапайым фактінің айқын дәлелі болып табылады: оқыту шекарасы әрбір қатысушыны жаппай қатысу жүйесіне қатысты өз бетінше шешім қабылдау мүмкіндігін бірегей түрде анықтайды. Біздің әрқайсымыз анық нәрсені түсінеміз: дамудың негізгі векторы қалыптасқан қаржылық және әкімшілік жағдайлардың алғышарттарын жасайды. Кенеттен ішкі саясаттың кейбір ерекшеліктері тек маркетингтік және қаржылық үй-жайлар контекстінде қарастырылады. Ал оппозиция өкілдерінің әрекеті екіұшты болып, өзінің ұтымды шегінде тосқауыл болатыны даусыз. Заманауи стандарттарды спецификациялау шеңберінде дизайн құрылымының негізгі ерекшеліктері өздерінің ұтымды шектеулерінде құлыпталады. Әрине, лауазымды қалыптастыру бойынша күнделікті жұмыстың басталуы, сондай-ақ таныс нәрселерге тың көзқараспен қарау материалдық-техникалық және кадрлық базаны қалыптастырудың жаңа принциптеріне жаңа көкжиектер ашатыны сөзсіз. Мырзалар, позициялық зерттеулердің жоғары сапасы даму моделін енгізу және жаңғырту процесін талап етеді. Сонымен қатар, бүгінгі экономикалық күн тәртібі бізге өзін-өзі қамтамасыз ететін және сыртқы тәуелді тұжырымдамалық шешімдерді анықтаудан басқа таңдау бермейді! Біздің мақсатымыз көрінетіндей қарапайым емес: әлеуметтік-бағдарланған ұлттық жобаға бағыт дамудың прогрессивті бағыттарына кең мүмкіндіктер береді. Ұйымның міндеті, әсіресе синтетикалық тестілеу, бар мінез-құлық үлгілерін талдау қажеттілігін сөзсіз бекітеді. Алдын ала жасалған қорытындылар көңіл көншітпейді: синтетикалық тестілеу бар мінез-құлық үлгілерін талдауды тексеруге арналған қызықты эксперимент болып табылады. Бірақ перспективалық жоспарлау жаппай қатысу жүйесін жүзеге асырудың тәуелсіз жолдарын қамтиды.
//detour func
bool Detour32(BYTE* src, BYTE* dst, const int len) {
	if (len < 5) {//if len is less then 5 we cant do jmp instruction so we cant hook
		return false;
	}

	DWORD curprotection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curprotection);//changing protection to read and write
	memset(src, '\x90', len);//nop'ing bytes (overwrite them with NOP instructions that do NOTHING)
	uintptr_t relativeAddress = dst - src - 5; //calculating relative address(address to jump)
	*src = 0xE9;//jmp instuction bytes

	*(uintptr_t*)(src + 1) = relativeAddress; //set address to jmp

	VirtualProtect(src, len, curprotection, &curprotection);//restoring old protection
	return true;
}

BYTE* TrampHook32(BYTE* src, BYTE* dst, const int len) {
	if (len < 5) return 0; //same as in detour32

	//create gateway
	BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	//write stolen bytes
	memcpy_s(gateway, len, src, len);

	//get the gateway to destination address
	uintptr_t gatewayRelativeAddress = src - gateway - 5;

	//add the jmp opcode to the end	of the gateway
	*(gateway + len) = 0xE9;

	//write the address of the gateway to the jmp
	*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddress;

	//perform the detour
	Detour32(src, dst, len); // Мен кіммін?
	return gateway;
}
Hook::Hook(BYTE* src, BYTE* dst, BYTE* PtrToGatewayPtr, const int len) {
	this->src = src;
	this->dst = dst;
	this->len = len;
	this->PtrToGatewayPtr = PtrToGatewayPtr;
}
void Hook::Enable() {
	memcpy(originalBytes, src, len);
	*(uintptr_t*)PtrToGatewayPtr = (uintptr_t)TrampHook32(src, dst, len);
	bStatus = true;
}
void Hook::Disable() {
	patch(src, originalBytes, len);
	bStatus = false; 
}
void Hook::Toggle() {
	if (!bStatus) Enable();
	else Disable();
}