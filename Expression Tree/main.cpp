#include <iostream>
#include <string>
#include <map>
#include <limits>
#include <clocale>
#include "tree.h"
#include "exceptions.h"
#include "validator.h"



void showMenu() {
    std::cout << "\nМеню\n";
    std::cout << "1. Ввести формулу и построить дерево\n";
    std::cout << "2. Печать текущей формулы (из дерева)\n";
    std::cout << "3. Упростить формулу (вынесение за скобки)\n";
    std::cout << "4. Вычислить значение выражения\n";
    std::cout << "5. Выйти\n";
    std::cout << "Выберите пункт: ";
}

int main() {
    std::setlocale(LC_ALL, "Russian");

    ExpressionTree tree;
    std::map<char, double> variables;
    std::string formula;

    while (true) {
        showMenu();

        int choice = validator::getMenuChoice();

        try {
            switch (choice) {
            case 1: {
                std::cout << "Введите формулу (инфиксная запись, макс. 10 скобок):\n";
                std::cout << "Пример: (a*c+b*c)\nФормула: ";
                validator::clearInputStream();
                std::getline(std::cin, formula);

                tree.buildFromInfix(formula);
                std::cout << "Дерево успешно построено!\n";
                break;
            }
            case 2: {
                std::cout << "Текущее выражение: ";
                tree.print();
                break;
            }
            case 3: {
                if (!tree.hasTree()) {
                    std::cout << "Сначала постройте дерево (Пункт 1)!\n";
                    break;
                }
                tree.simplify();
                std::cout << "Формула упрощена! Результат:\n";
                tree.print();
                break;
            }
            case 4: {
                if (!tree.hasTree()) {
                    std::cout << "Сначала постройте дерево (Пункт 1)!\n";
                    break;
                }

                variables.clear();
                std::cout << "Ввод переменных для вычисления:\n";
                for (char c : formula) {
                    if (std::isalpha(c) && variables.find(c) == variables.end()) {
                        variables[c] = validator::getVariableValue(c);
                    }
                }

                double result = tree.evaluate(variables);
                std::cout << "Результат вычисления: " << result << "\n";
                break;
            }
            case 5: {
                std::cout << "Завершение работы программы.\n";
                return 0;
            }
            default: {
                std::cout << "Неверный пункт меню! Выберите от 1 до 5.\n";
                break;
            }
            }
        }
        catch (const MathException& e) {
            std::cerr << "Произошла ошибка: " << e.what() << "\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Исключение: " << e.what() << "\n";
        }
    }

    return 0;
}