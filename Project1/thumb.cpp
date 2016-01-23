#include <iostream>

int execute(__int16 opcode){
	__int16 type = (opcode & 0xE000) >> 13;
	std::cout << type;
	return 0;
}

int main(){
	int a;
	dissect(0xE000);
	std::cin >> a;
	return 0;
}