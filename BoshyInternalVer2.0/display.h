#pragma once
#include <iostream>
#include <map>

// Интерпол менің артымда. Мен енді жасыра алмаймын.
struct CheatInfo {
	CheatInfo() {
		boolStatuses = {
			{"Auto-fire (C)",&autoFire},
			{"Frame hook (F) (if false - god-mod/fps is not working)",&frameHook},
			{"God-mode (A)",&godMode},
			{"One-hit (V)",&oneHit},
			{"Character hook (W)",&charHook},
		};
		intStatuses = {
			{"Current fps ( , : .)",&fps},
			{"Character id (D)",&charId}
		};
	}
	void debug() {
		std::cout << "&autoFire : " << &autoFire
			<< "\nboolStatuses[Auto-fire]: "<< boolStatuses["Auto-fire"] << "\n";
	}
	std::map<std::string, bool**> boolStatuses;
	std::map<std::string, int**> intStatuses;
	bool* autoFire, * frameHook, * godMode, * oneHit, * charHook;
	int* fps, * charId;
};

const void display(const CheatInfo& ci) noexcept;
