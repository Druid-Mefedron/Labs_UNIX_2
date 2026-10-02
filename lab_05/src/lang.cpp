#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <iomanip>

std::mutex outMutex;
std::chrono::steady_clock::time_point startTime;

double elapsedMs() {
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - startTime).count();
}

void say(const std::string& text) {
    std::lock_guard<std::mutex> lock(outMutex);
    std::cout << text << std::endl;
}

struct Context {
    int lineNo;
    std::map<std::string, double> vars;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual double eval(Context& ctx) const = 0;
};

class Number : public Expr {
public:
    Number(double v) : value(v) {}
    double eval(Context&) const override { return value; }
private:
    double value;
};

class Variable : public Expr {
public:
    Variable(const std::string& n) : name(n) {}
    double eval(Context& ctx) const override {
        if (ctx.vars.count(name) == 0) throw std::runtime_error("unknown variable " + name);
        return ctx.vars[name];
    }
private:
    std::string name;
};

class Operation : public Expr {
public:
    Operation(char o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
    double eval(Context& ctx) const override {
        double a = left->eval(ctx);
        double b = right->eval(ctx);
        if (op == '+') return a + b;
        if (op == '-') return a - b;
        if (op == '*') return a * b;
        if (b == 0) throw std::runtime_error("division by zero");
        return a / b;
    }
private:
    char op;
    std::unique_ptr<Expr> left, right;
};

class Command {
public:
    virtual ~Command() = default;
    virtual void execute(Context& ctx) const = 0;
};

using CommandList = std::vector<std::unique_ptr<Command>>;

class Assign : public Command {
public:
    Assign(const std::string& n, std::unique_ptr<Expr> e) : name(n), expr(std::move(e)) {}
    void execute(Context& ctx) const override { ctx.vars[name] = expr->eval(ctx); }
private:
    std::string name;
    std::unique_ptr<Expr> expr;
};

class Print : public Command {
public:
    Print(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
    void execute(Context& ctx) const override {
        std::ostringstream out;
        out << "    line " << ctx.lineNo << " > " << expr->eval(ctx);
        say(out.str());
    }
private:
    std::unique_ptr<Expr> expr;
};

class Append : public Command {
public:
    Append(const std::string& f, std::unique_ptr<Expr> e) : file(f), expr(std::move(e)) {}
    void execute(Context& ctx) const override {
        double value = expr->eval(ctx);
        std::lock_guard<std::mutex> lock(outMutex);
        std::ofstream out(file, std::ios::app);
        out << value << "\n";
    }
private:
    std::string file;
    std::unique_ptr<Expr> expr;
};

class Sleep : public Command {
public:
    Sleep(std::unique_ptr<Expr> e) : ms(std::move(e)) {}
    void execute(Context& ctx) const override {
        std::this_thread::sleep_for(std::chrono::milliseconds(std::llround(ms->eval(ctx))));
    }
private:
    std::unique_ptr<Expr> ms;
};

class Loop : public Command {
public:
    Loop(const std::string& v, std::unique_ptr<Expr> f, std::unique_ptr<Expr> t, CommandList b)
        : var(v), from(std::move(f)), to(std::move(t)), body(std::move(b)) {}
    void execute(Context& ctx) const override {
        long long a = std::llround(from->eval(ctx));
        long long b = std::llround(to->eval(ctx));
        for (long long i = a; i <= b; i++) {
            ctx.vars[var] = i;
            for (const auto& cmd : body) cmd->execute(ctx);
        }
    }
private:
    std::string var;
    std::unique_ptr<Expr> from, to;
    CommandList body;
};

class Parser {
public:
    Parser(const std::string& line) {
        std::string spaced;
        for (char c : line) {
            if (std::string("+-*/(){};=").find(c) != std::string::npos) {
                spaced += ' ';
                spaced += c;
                spaced += ' ';
            } else {
                spaced += c;
            }
        }
        std::istringstream in(spaced);
        std::string word;
        while (in >> word) tokens.push_back(word);
    }

    CommandList parseAll() {
        CommandList list = parseBlock();
        if (pos < tokens.size()) throw std::runtime_error("unexpected " + peek());
        return list;
    }

private:
    std::vector<std::string> tokens;
    size_t pos = 0;

    std::string peek() { return pos < tokens.size() ? tokens[pos] : ""; }
    std::string next() { return pos < tokens.size() ? tokens[pos++] : ""; }

    void expect(const std::string& s) {
        if (next() != s) throw std::runtime_error("expected " + s);
    }

    CommandList parseBlock() {
        CommandList list;
        while (pos < tokens.size() && peek() != "}") {
            if (peek() == ";") { next(); continue; }
            list.push_back(parseCommand());
        }
        return list;
    }

    std::unique_ptr<Command> parseCommand() {
        std::string word = next();
        if (word == "print") return std::make_unique<Print>(parseExpr());
        if (word == "sleep") return std::make_unique<Sleep>(parseExpr());
        if (word == "append") {
            std::string file = next();
            return std::make_unique<Append>(file, parseExpr());
        }
        if (word == "for") {
            std::string var = next();
            expect("=");
            auto from = parseExpr();
            expect("to");
            auto to = parseExpr();
            expect("{");
            CommandList body = parseBlock();
            expect("}");
            return std::make_unique<Loop>(var, std::move(from), std::move(to), std::move(body));
        }
        expect("=");
        return std::make_unique<Assign>(word, parseExpr());
    }

    std::unique_ptr<Expr> parseExpr() {
        auto left = parseTerm();
        while (peek() == "+" || peek() == "-") {
            char op = next()[0];
            left = std::make_unique<Operation>(op, std::move(left), parseTerm());
        }
        return left;
    }

    std::unique_ptr<Expr> parseTerm() {
        auto left = parseFactor();
        while (peek() == "*" || peek() == "/") {
            char op = next()[0];
            left = std::make_unique<Operation>(op, std::move(left), parseFactor());
        }
        return left;
    }

    std::unique_ptr<Expr> parseFactor() {
        std::string word = next();
        if (word == "(") {
            auto e = parseExpr();
            expect(")");
            return e;
        }
        if (!word.empty() && std::isdigit(static_cast<unsigned char>(word[0]))) return std::make_unique<Number>(std::stod(word));
        if (!word.empty() && std::isalpha(static_cast<unsigned char>(word[0]))) return std::make_unique<Variable>(word);
        throw std::runtime_error("expected value, got '" + word + "'");
    }
};

void sayTime(int lineNo, const std::string& event) {
    std::lock_guard<std::mutex> lock(outMutex);
    std::cout << "[" << std::fixed << std::setprecision(3) << elapsedMs() << " ms] line "
              << lineNo << ": " << event << std::defaultfloat << std::endl;
}

void runLine(int lineNo, const CommandList& program) {
    sayTime(lineNo, "START");
    Context ctx{lineNo, {}};
    try {
        for (const auto& cmd : program) cmd->execute(ctx);
    } catch (const std::exception& e) {
        say("    line " + std::to_string(lineNo) + " error: " + e.what());
    }
    sayTime(lineNo, "FINISH");
}

int main() {
    std::cout << "Enter commands, one set per line. Type run to execute." << std::endl;

    std::vector<CommandList> programs;
    std::vector<int> numbers;
    std::string line;
    int lineNo = 0;

    while (std::getline(std::cin, line) && line != "run") {
        if (line.empty()) continue;
        lineNo++;
        try {
            Parser parser(line);
            programs.push_back(parser.parseAll());
            numbers.push_back(lineNo);
        } catch (const std::exception& e) {
            std::cout << "line " << lineNo << " syntax error: " << e.what() << std::endl;
        }
    }

    startTime = std::chrono::steady_clock::now();
    std::vector<std::thread> threads;
    for (size_t i = 0; i < programs.size(); i++) {
        threads.emplace_back(runLine, numbers[i], std::cref(programs[i]));
    }
    for (auto& t : threads) t.join();

    std::cout << "All lines finished." << std::endl;
    return 0;
}
