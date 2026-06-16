#include "validator.h"
#include <iostream>

int validator::getMenuChoice() {
	int choice;
	while (true) {
		if (std::cin >> choice) {
			return choice;
		}
		std::cout << "Неверный ввод! Пожалуйста, введите целое число.\n";
		clearInputStream();
		std::cout << "Выберите пункт: ";
	}
}

double validator::getVariableValue(char varName) {
	double val;
	while (true) {
		std::cout << "Введите значение для '" << varName << "': ";
		if (std::cin >> val) {
			return val;
		}
		std::cout << "Неверный формат числа. Попробуйте снова.\n";
		clearInputStream();
	}
}

void validator::clearInputStream() {
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}