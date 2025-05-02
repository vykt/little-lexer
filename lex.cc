//STL
#include <vector>
#include <string>
#include <fstream>

//local
#include "common.hh"
#include "lex.hh"



void lex::lex_state::add_token() {

    tokens.emplace_back(lex::token(this->cur_kind, this->cur_lexeme));
    this->cur_lexeme.clear();

    return;
}


void lex::lexer::lex() {

    this->dfa.evaluate(&this->ls);

    return;
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
