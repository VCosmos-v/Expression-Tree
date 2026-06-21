#include "tree.h"
#include "stack.h"
#include "exceptions.h"
#include <iostream>
#include <cctype>

ExpressionTree::ExpressionTree() : root(nullptr) {}
ExpressionTree::~ExpressionTree() { clear(); }

void ExpressionTree::clear() {
    delete root;
    root = nullptr;
}

bool ExpressionTree::isOperator(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int ExpressionTree::getPriority(char op) const {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '~') return 3;
    return 0;
}

// функция синтаксического разбора инфиксной строки и построения бинарного дерева.
void ExpressionTree::buildFromInfix(const std::string& formula) {
    clear();
    // стек для промежуточного хранения готовых поддеревьев.
    Stack<ExpressionNode*> nodes;
    // стек для хранения символов операторов и скобок.
    Stack<std::string> operators;

    int depth = 0;
    bool maybeUnary = true;

    // посимвольный разбор строки.
    for (size_t i = 0; i < formula.length(); ++i) {
        char c = formula[i];
        if (std::isspace(c)) continue;

        // далее обработка каждого символа
        // открывающая скобка
        if (c == '(') {
            depth++;
            if (depth > 10) throw InvalidExpressionException("Превышена глубина вложенности скобок (макс. 10)");
            operators.push("(");
            maybeUnary = true;
        }
        // закрывающая скобка
        else if (c == ')') {
            depth--;
            if (depth < 0) throw InvalidExpressionException("Нарушен баланс скобок");

            while (!operators.isEmpty() && operators.top() != "(") {
                std::string op = operators.top(); operators.pop();
                ExpressionNode* node = new ExpressionNode(NodeType::OPERATOR, op);
                if (op == "~") {
                    node->left = nodes.top(); nodes.pop();
                }
                else {
                    node->right = nodes.top(); nodes.pop();
                    node->left = nodes.top(); nodes.pop();
                }
                nodes.push(node);
            }
            if (!operators.isEmpty()) operators.pop();
            maybeUnary = false;
        }
        // арифметические операции
        else if (isOperator(c)) {
            if (maybeUnary && c == '-') {
                operators.push("~");
            }
            else {
                while (!operators.isEmpty() && getPriority(operators.top()[0]) >= getPriority(c)) {
                    std::string op = operators.top(); operators.pop();
                    ExpressionNode* node = new ExpressionNode(NodeType::OPERATOR, op);
                    if (op == "~") {
                        node->left = nodes.top(); nodes.pop();
                    }
                    else {
                        node->right = nodes.top(); nodes.pop();
                        node->left = nodes.top(); nodes.pop();
                    }
                    nodes.push(node);
                }
                std::string opStr(1, c);
                operators.push(opStr);
            }
            maybeUnary = true;
        }
        // буквенные переменные
        else if (std::isalpha(c)) {
            std::string var(1, c);
            nodes.push(new ExpressionNode(NodeType::VARIABLE, var));
            maybeUnary = false;
        }
        // числа
        else if (std::isdigit(c)) {
            std::string num = "";
            while (i < formula.length() && std::isdigit(formula[i])) {
                num += formula[i];
                i++;
            }
            i--;
            nodes.push(new ExpressionNode(NodeType::NUMBER, num));
            maybeUnary = false;
        }
        else {
            throw InvalidExpressionException("Недопустимый символ в выражении");
        }
    }

    // все оставшиеся операции 
    while (!operators.isEmpty()) {
        std::string op = operators.top(); operators.pop();
        if (op == "(") throw InvalidExpressionException("Нарушен баланс скобок");
        ExpressionNode* node = new ExpressionNode(NodeType::OPERATOR, op);
        if (op == "~") {
            node->left = nodes.top(); nodes.pop();
        }
        else {
            node->right = nodes.top(); nodes.pop();
            node->left = nodes.top(); nodes.pop();
        }
        nodes.push(node);
    }

    if (!nodes.isEmpty()) {
        root = nodes.top();
    }
}

bool ExpressionTree::nodesAreEqual(ExpressionNode* a, ExpressionNode* b) const {
    if (!a && !b) return true;
    if (!a || !b) return false;
    return (a->type == b->type && a->value == b->value &&
        nodesAreEqual(a->left, b->left) && nodesAreEqual(a->right, b->right));
}

ExpressionTree::ExpressionNode* ExpressionTree::clone(ExpressionNode* node) const {
    if (!node) return nullptr;
    ExpressionNode* newNode = new ExpressionNode(node->type, node->value);
    newNode->left = clone(node->left);
    newNode->right = clone(node->right);
    return newNode;
}

// (f1*f2 +- f3*f4) преобразуется в умножение со скобками, если найден общий множитель.
ExpressionTree::ExpressionNode* ExpressionTree::simplifyNode(ExpressionNode* node) {
    if (!node) return nullptr;

    node->left = simplifyNode(node->left);
    node->right = simplifyNode(node->right);

    // Проверка применимости правила. Упрощение возможно только на узлах сложения или вычитания.
    if (node->type == NodeType::OPERATOR && (node->value == "+" || node->value == "-")) {
        ExpressionNode* L = node->left;
        ExpressionNode* R = node->right;

        // Проверка, что оба поддерева являются операциями умножения *.
        if (L && R && L->type == NodeType::OPERATOR && L->value == "*" &&
            R->type == NodeType::OPERATOR && R->value == "*") {

            // указатели на множители
            ExpressionNode* f1 = L->left;
            ExpressionNode* f2 = L->right;
            ExpressionNode* f3 = R->left;
            ExpressionNode* f4 = R->right;

            // Общий множитель справа в обоих выражениях (f2 == f4) -> (f1 +- f3) * f2
            if (nodesAreEqual(f2, f4)) {
                ExpressionNode* opNew = new ExpressionNode(NodeType::OPERATOR, node->value);
                opNew->left = clone(f1);
                opNew->right = clone(f3);

                ExpressionNode* mulNew = new ExpressionNode(NodeType::OPERATOR, "*");
                mulNew->left = opNew;
                mulNew->right = clone(f2);

                delete node;
                return mulNew;
            }

            // Общий множитель слева в обоих выражениях (f1 == f3) -> f1 * (f2 +- f4)
            if (nodesAreEqual(f1, f3)) {
                ExpressionNode* opNew = new ExpressionNode(NodeType::OPERATOR, node->value);
                opNew->left = clone(f2);
                opNew->right = clone(f4);

                ExpressionNode* mulNew = new ExpressionNode(NodeType::OPERATOR, "*");
                mulNew->left = clone(f1);
                mulNew->right = opNew;

                delete node;
                return mulNew;
            }

            // f1 == f4 -> общий множитель f1. Результат: f1 * (f2 +- f3)
            if (nodesAreEqual(f1, f4)) {
                ExpressionNode* opNew = new ExpressionNode(NodeType::OPERATOR, node->value);
                opNew->left = clone(f2);
                opNew->right = clone(f3);

                ExpressionNode* mulNew = new ExpressionNode(NodeType::OPERATOR, "*");
                mulNew->left = clone(f1);
                mulNew->right = opNew;

                delete node;
                return mulNew;
            }

            // f2 == f3 -> общий множитель f2. Результат: (f1 +- f4) * f2
            if (nodesAreEqual(f2, f3)) {
                ExpressionNode* opNew = new ExpressionNode(NodeType::OPERATOR, node->value);
                opNew->left = clone(f1);
                opNew->right = clone(f4);

                ExpressionNode* mulNew = new ExpressionNode(NodeType::OPERATOR, "*");
                mulNew->left = opNew;
                mulNew->right = clone(f2);

                delete node;
                return mulNew;
            }
        }
    }
    return node;
}

void ExpressionTree::simplify() {
    root = simplifyNode(root);
}

double ExpressionTree::evaluateNode(ExpressionNode* node, const std::map<char, double>& variables) const {
    if (!node) return 0;

    if (node->type == NodeType::NUMBER) {
        return std::stod(node->value);
    }
    if (node->type == NodeType::VARIABLE) {
        char varName = node->value[0];
        if (variables.find(varName) == variables.end()) {
            throw MathException(std::string("Переменная ") + varName + " не задана!");
        }
        return variables.at(varName);
    }

    if (node->value == "~") {
        return -evaluateNode(node->left, variables);
    }

    double leftVal = evaluateNode(node->left, variables);
    double rightVal = evaluateNode(node->right, variables);

    if (node->value == "+") return leftVal + rightVal;
    if (node->value == "-") return leftVal - rightVal;
    if (node->value == "*") return leftVal * rightVal;
    if (node->value == "/") {
        if (rightVal == 0.0) throw DivisionByZeroException();
        return leftVal / rightVal;
    }

    return 0;
}

double ExpressionTree::evaluate(const std::map<char, double>& variables) const {
    if (!root) throw MathException("Дерево пустое!");
    return evaluateNode(root, variables);
}

void ExpressionTree::printInfix(ExpressionNode* node) const {
    if (!node) return;
    if (node->type == NodeType::OPERATOR) std::cout << "(";
    printInfix(node->left);
    if (node->value == "~") std::cout << "-";
    else std::cout << node->value;
    printInfix(node->right);
    if (node->type == NodeType::OPERATOR) std::cout << ")";
}

void ExpressionTree::print() const {
    if (!root) {
        std::cout << "Дерево пусто.\n";
        return;
    }
    printInfix(root);
    std::cout << "\n";
}
