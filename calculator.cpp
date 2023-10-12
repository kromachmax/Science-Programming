#include<iostream>
#include<vector>
#include<string>
#include<stack>
#include<cctype>
#include<unordered_map>
#include<unordered_set>
#include<map>
#include<set>
#include<algorithm>
#include<cmath>
#include<dlfcn.h>
#include<utility>
#include<filesystem>
#include<regex>
#define PI 3.1459265358979323

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

std::string myPath = "./plugins/";

std::vector<std::string> file_paths;

class MyException:public std::exception{
    public:
        MyException(const char* s) noexcept : str(s){};
        ~MyException() = default;
        const char* what() const noexcept override{
            return str;
        };
    private:
        const char* str;
};

double (*Sin) (double) = nullptr;
double (*Cos) (double) = nullptr;
double (*Degree) (double, double) = nullptr;
double (*Ln) (double) = nullptr;
double (*Exp) (double) = nullptr;

void Init_Library(){

    for(const auto& file : file_paths){

        std::string s = myPath + file; 

        void *hdl = dlopen(s.c_str(), RTLD_LAZY);
        if(hdl){

            if(!Sin) Sin = (double (*) (double))dlsym(hdl, "Sin");
            if(!Cos) Cos = (double (*) (double))dlsym(hdl, "Cos");
            if(!Ln) Ln  = (double (*) (double))dlsym(hdl, "Ln");
            if(!Exp) Exp = (double (*) (double))dlsym(hdl, "Exp");
            if(!Degree) Degree = (double (*) (double, double))dlsym(hdl, "Degree");

        }
    }
}

bool IsOperator(const std::string& s) {
    return s == "+" || s == "-" || s == "*" || s == "/" || s == "^";
}

bool UnaryOperator(const std::string& s){
    return s == "sin" || s == "cos" || s == "ln" || s == "exp";
}

bool HasHigherPrecedence(const std::string& op1, const std::string& op2) {
    std::unordered_map<std::string, int> precedence = {
        {"+", 1}, {"-", 1},
        {"*", 2}, {"/", 2},
        {"sin", 4}, {"^", 3}, 
        {"cos", 4}, {"ln", 4},
        {"exp", 4} 
    };
    return precedence[op1] >= precedence[op2];
}

bool isNumeric(std::string const &str)noexcept
{
    auto it = str.begin();
    while (it != str.end() && std::isdigit(*it)) {
        it++;
    }
    return !str.empty() && it == str.end();
}

int Check(const std::vector<std::string>& brackets) noexcept {
    std::stack<char> bracketStack;
    for (const std::string& bracket : brackets) {
        if (bracket == "(") {
            bracketStack.push('(');
        } else if (bracket == ")") {
            if (!bracketStack.empty()) {
                bracketStack.pop();
            } else {
                return 0;
            }
        }
    }
    return bracketStack.empty();
}

void String_conversion(const std::string& str, std::vector<std::string>& ans) {
    std::string current_token;
    size_t size = str.size();

    for (size_t i = 0; i < size; i++) {
        if (isdigit(str[i])) {
            current_token += str[i];
        } else {
            bool add_token = false;
            if (!current_token.empty()) {
                ans.push_back(current_token);
                current_token.clear();
            }
            if (str[i] != ' ') {
                if (str[i] == 's') {
                    size_t position = str.find("sin", i);
                    if (position == i) {
                        current_token += "sin";
                        i += 2;
                        add_token = true;
                    } 
                }
                else if(str[i] == 'c'){
                    size_t position = str.find("cos", i);
                    if (position == i) {
                        current_token += "cos";
                        i += 2;
                        add_token = true;
                    }
                }
                else if(str[i] == 'l'){
                    size_t position = str.find("ln", i);
                    if (position == i) {
                        current_token += "ln";
                        i += 1;
                        add_token = true;
                    }
                }
                else if(str[i] == 'e'){
                    size_t position = str.find("exp", i);
                    if (position == i) {
                        current_token += "exp";
                        i += 2;
                        add_token = true;
                    }
                }
                if(!add_token){
                    current_token += str[i];
                    ans.push_back(current_token);
                    current_token.clear();
                }
            }
        }
    }

    if (!current_token.empty()) {
        ans.push_back(current_token);
    }
}

bool String_Validation(std::vector<std::string>& ans) {
    
    if(ans.size() == 1){
        if(isNumeric(ans[0])) return true;
        else return false;
    }

    if (!Check(ans)) {
        return false;
    }
    if (IsOperator(ans[0])) {
        if(ans[0] == "+" || ans[0] == "-")
        ans.insert(ans.begin(), "0");
        else return false;
    }

    if(ans[0] == "^") return false;

    if(UnaryOperator(ans[0]) && (UnaryOperator(ans[1]) || IsOperator(ans[1]))){
        return false;
    }

    size_t size = ans.size();
    for(int i = 0;i < size; i++){
        if(!IsOperator(ans[i]) && !isNumeric(ans[i])
                && ans[i] != "(" && ans[i] != ")" && !UnaryOperator(ans[i])){
            return false;
        }
    }

    for(int i=0; i < size-1; i++){
        if(ans[i] == "(" && ans[i+1] == ")") return false;
    }

    if (IsOperator(ans[size - 1]) || UnaryOperator(ans[size - 1])) { 
        return false;
    }
    for (int i = 1; i < size - 1; i++) {

        if(UnaryOperator(ans[i]) && ans[i+1] != "("){
            return false;
        }

        if (IsOperator(ans[i])) {
            if (IsOperator(ans[i + 1]) || IsOperator(ans[i - 1])) {
                return false;
            }
        }
        if(isNumeric(ans[i])){
            if(isNumeric(ans[i-1]) || isNumeric(ans[i+1])){
                return false;
            }
        }

        if (ans[i] == "(" && IsOperator(ans[i + 1])) {
            if(ans[i+1] == "+" || ans[i+1] == "-"){
                auto it = ans.cbegin();
                ans.emplace(it + i + 1, "0");
                i++;
                continue;
            }
            else{
               return false;
            }
        }

        if(ans[i] == "(" && isNumeric(ans[i-1])){
            return false;
        }

        if (ans[i] == ")" && (IsOperator(ans[i - 1])
                || isNumeric(ans[i + 1]) || UnaryOperator(ans[i + 1]))) {
            return false;
        }
    }
    return true;
}


void ConvertToPostfix(std::vector<std::string>& postfix, const std::vector<std::string>& infix) {
    std::stack<std::string> operatorStack;
    for (const std::string& token : infix) {
        if (isNumeric(token)) {
            postfix.push_back(token);
        } else {
            if (IsOperator(token) || UnaryOperator(token)) {
                while (!operatorStack.empty()
                        && (IsOperator(operatorStack.top()) || UnaryOperator(operatorStack.top()))
                        && HasHigherPrecedence(operatorStack.top(), token)) {
                    postfix.push_back(operatorStack.top());
                    operatorStack.pop();
                }
                operatorStack.push(token);
            } else if (token == "(") {
                operatorStack.push(token);
            } else if (token == ")") {
                while (!operatorStack.empty() && operatorStack.top() != "(") {
                    postfix.push_back(operatorStack.top());
                    operatorStack.pop();
                }
                if (!operatorStack.empty() && operatorStack.top() == "(") {
                    operatorStack.pop();
                }
            }
        }
    }
    while (!operatorStack.empty()) {
        postfix.push_back(operatorStack.top());
        operatorStack.pop();
    }
}

double Convert(const std::string &s) {
    double result = 0;
    double multiplier = 1;
    for (int i = s.size() - 1; i >= 0; i--) {
        result += (s[i] - '0') * multiplier;
        multiplier *= 10;
    }
    return result;
}

void Total_Answer(const std::vector<std::string>& postfix) {
    std::stack<double> stack;
    for (const std::string &token : postfix) {
        if (isNumeric(token)) {
            stack.push(Convert(token));
        } else {
            double operand2 = stack.top();
            stack.pop();
            double operand1;
            if(!UnaryOperator(token)){
                operand1=stack.top();
                stack.pop();
            }
            double result;
            if (token == "+") {
                result = operand1 + operand2;
            } else if (token == "*") {
                result = operand1 * operand2;
            } else if (token == "-") {
                result = operand1 - operand2;
            } else if(token == "/"){
                result = operand1 / operand2;
            } else if(token == "sin"){
                if(Sin){
                    result = Sin(operand2);
                }
                else {
                    throw MyException("Library was not loaded");
                } 

            } else if(token == "^"){

                if(Degree){
                    if(operand1 < 0) throw MyException("Error in pow function");
                    result = Degree(operand1, operand2);
                }
                else {
                    throw MyException("Library was not loaded");
                }

            } else if(token == "cos"){

                if(Cos){
                    result = Cos(operand2);
                }
                else {
                    throw MyException("Library was not loaded");
                }     
                
            } else if(token == "ln"){

                if(Ln){
                    if(operand2 <= 0) throw MyException("Error in the definition of logarithms");
                    result = Ln(operand2);
                }
                else {
                    throw MyException("Library was not loaded");
                } 
 
            } else if(token == "exp"){

                if(Exp){
                    result = Exp(operand2);
                }
                else {
                    throw MyException("Library was not loaded");
                } 
 
            }
            stack.push(result);
        }
    }
    std::cout << stack.top() << std::endl;
}

void GetFilePaths(){
    for (const auto& dirEntry : recursive_directory_iterator(myPath)){
        std::string extension = dirEntry.path().extension();
        if(extension == ".so"){ 
            file_paths.push_back(dirEntry.path().filename().string());
        }
    }
}

int main(void){
    
    std::cout<<"Введите сроку, возможны опреации: sin, cos, ln, exp, ^ :"<<std::endl;

    GetFilePaths();

    Init_Library();

    std::string s;
    std::vector<std::string> tokens;
    std::getline(std::cin, s);
    String_conversion(s, tokens);

    try{
        if(String_Validation(tokens)){ 
            std::vector<std::string>postfix;
            ConvertToPostfix(postfix, tokens);
            Total_Answer(postfix);
        }
        else throw MyException("Invalid string");
    }
    catch(const std::exception& e){
        std::cout<<e.what()<<std::endl;
    } 
    return 0;
}
