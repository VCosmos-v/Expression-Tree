#pragma once

#include <stdexcept>
#include <string>

class MathException : public std::runtime_error {
public:
    explicit MathException(const std::string& message) : std::runtime_error(message) {}
};

class DivisionByZeroException : public MathException {
public:
    DivisionByZeroException() : MathException("Ошибка: Деление на ноль!") {}
};

class InvalidExpressionException : public MathException {
public:
    explicit InvalidExpressionException(const std::string& msg) : MathException("Ошибка в формуле: " + msg) {}
};
