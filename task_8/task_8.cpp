#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <memory> 

// Базовый класс компонента
class Expression {
public:
    virtual ~Expression() = default; 
    virtual void print() const = 0;
    virtual int calculate(const std::map<std::string, int>& context) const = 0;
};

// Приспособленец: Константа
class Constant : public Expression {
private:
    int value;
public:
    Constant(int v) : value(v) {}

    void print() const override {
        std::cout << value;
    }

    int calculate(const std::map<std::string, int>&) const override {
        return value;
    }
};

// Приспособленец: Переменная
class Variable : public Expression {
private:
    std::string name;
public:
    Variable(const std::string& n) : name(n) {}

    void print() const override {
        std::cout << name;
    }

    int calculate(const std::map<std::string, int>& context) const override {
        auto it = context.find(name);

        if (it != context.end()) 
            return it->second;

        throw std::runtime_error("Переменная '" + name + "' не задана");
    }
};

// Компоновщик: Сложение
class Addition : public Expression {
private:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;
public:
    Addition(std::shared_ptr<Expression> l, std::shared_ptr<Expression> r) : left(l), right(r) {}

    void print() const override {
        std::cout << "(";
        left->print();
        std::cout << " + ";
        right->print();
        std::cout << ")";
    }

    int calculate(const std::map<std::string, int>& context) const override {
        return left->calculate(context) + right->calculate(context);
    }
};

// Фабрика Приспособленцев
class ExpressionFactory {
private:
    std::unordered_map<int, std::shared_ptr<Constant>> constants;
    std::unordered_map<std::string, std::shared_ptr<Variable>> variables;
public:
    ExpressionFactory() {
        for (int i = -5; i <= 256; ++i) {
            constants[i] = std::make_shared<Constant>(i);
        }
    }

    std::shared_ptr<Constant> createConstant(int val) {
        if (constants.find(val) == constants.end()) {
            constants[val] = std::make_shared<Constant>(val);
        }
        return constants[val];
    }

    std::shared_ptr<Variable> createVariable(const std::string& name) {
        if (variables.find(name) == variables.end()) {
            variables[name] = std::make_shared<Variable>(name);
        }
        return variables[name];
    }

    void removeConstant(int val) {
        if (val >= -5 && val <= 256) {
            return;
        }
        constants.erase(val);
    }

    void removeVariable(const std::string& name) {
        variables.erase(name);
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    ExpressionFactory factory;

    auto c = factory.createConstant(2);
    auto v = factory.createVariable("x");

    auto expression = std::make_shared<Addition>(c, v);

    std::map<std::string, int> context;
    context["x"] = 3;

    expression->print();
    std::cout << "\nРезультат: " << expression->calculate(context) << std::endl;

    std::cout << "\nПроверка удаления из фабрики" << std::endl;

    auto bigConst = factory.createConstant(1000);
    std::cout << "Количество ссылок на константу 1000: " << bigConst.use_count() << " (1 у фабрики, 1 у bigConst)\n";

    factory.removeConstant(1000); 
    std::cout << "Константа 1000 удалена из фабрики. Количество ссылок: " << bigConst.use_count() << " (у bigConst)\n";

    return 0;
}
