//STL
#include <vector>
#include <string>
#include <fstream>

//local
#include "common.hh"
#include "lex.hh"


std::string lex::lexer::read_input(const std::string input_path) const {

    const constexpr int page_size = 4096;
    char buf[page_size];
    std::string out;

    std::ifstream ifs(input_path);
    if (ifs.is_open() == false) {
        return "";
    }

    while (ifs.read(buf, page_size) || ifs.gcount() > 0) {
        out.append(buf, ifs.gcount());
    }

    ifs.close();
    return out;
}


/*
 *  NOTE: Each state transition function in this file should take
 *        `void * ctx` as its only parameter. The macro below will
 *        define a lexer state pointer `ls`, which all state
 *        transition macros should use.
 */
#define CAST_CTX lex_state * ls = static_cast<lex_state *>(ctx);


/*
 *  NOTE: As a shortcut, the DFA passes the current character to its 
 *        `action()` function. This is quite impure; ideally each  
 *        character would possess its own `append()` function.
 *
 *        The pure approach is taken with the `kind` field instead;
 *        each value of `kind` has its own setter function.
 */
void lex::save_token(char ch, void * ctx) {
    CAST_CTX
    ls->add_token();
}

void lex::append_char(char ch, void * ctx) {
    CAST_CTX
    ls->append_char(ch);
}

void lex::set_kind_foo(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::FOO);
}

void lex::set_kind_bar(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::BAR);
}

void lex::set_kind_baz(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::BAZ);
}

void lex::set_kind_qux(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::QUX);
}


const std::vector<std::function<void(char, void *)>> lex::lex_actions = {
    lex::save_token,
    lex::append_char,
    lex::set_kind_foo,
    lex::set_kind_bar,
    lex::set_kind_baz,
    lex::set_kind_qux
};
