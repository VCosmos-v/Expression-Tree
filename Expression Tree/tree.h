#pragma once

#include <string>
#include <map>

enum class NodeType { OPERATOR, VARIABLE, NUMBER };

class ExpressionTree {
private:
    struct ExpressionNode {
        NodeType type;
        std::string value;
        ExpressionNode* left;
        ExpressionNode* right;

        ExpressionNode(NodeType t, std::string val)
            : type(t), value(std::move(val)), left(nullptr), right(nullptr) {
        }
        ~ExpressionNode() {
            delete left;
            delete right;
        }
    };

    ExpressionNode* root;

    void printInfix(ExpressionNode* node) const;
    int getPriority(char op) const;
    bool isOperator(char c) const;

    double evaluateNode(ExpressionNode* node, const std::map<char, double>& variables) const;
    ExpressionNode* clone(ExpressionNode* node) const;
    ExpressionNode* simplifyNode(ExpressionNode* node);
    bool nodesAreEqual(ExpressionNode* a, ExpressionNode* b) const;

public:
    ExpressionTree();
    ~ExpressionTree();

    void buildFromInfix(const std::string& formula);
    void print() const;
    void simplify();
    double evaluate(const std::map<char, double>& variables) const;
    bool hasTree() const { return root != nullptr; }
    void clear();
};
