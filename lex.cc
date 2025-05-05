//STL
#include <vector>
#include <string>
#include <fstream>

//local
#include "common.hh"
#include "lex.hh"


//TODO DEBUG
#include <iostream>


void lex::lex_state::add_token() {

    auto typetab = lex::typetab();
    tokens.emplace_back(lex::token(this->cur_kind, this->cur_lexeme));

    std::cout << "added token: " << this->cur_lexeme << " | type: "
              << typetab[this->cur_kind] << std::endl;
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

void lex::set_kind_type(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::TYPE);
}

void lex::set_kind_var(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::VAR);
}

void lex::set_kind_imm(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::IMM);
}

void lex::set_kind_arith(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::ARITH);
}

void lex::set_kind_op_brack(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::OP_BRACK);
}

void lex::set_kind_cl_brack(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::CL_BRACK);
}

void lex::set_kind_if(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::IF);
}

void lex::set_kind_else(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::ELSE);
}

void lex::set_kind_goto(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::GOTO);
}

void lex::set_kind_label(char ch, void * ctx) {
    CAST_CTX
    ls->set_kind(lex::LABEL);
}


const std::vector<std::function<void(char, void *)>> lex::lex_actions = {
    lex::save_token,
    lex::append_char,
    lex::set_kind_type,
    lex::set_kind_var,
    lex::set_kind_imm,
    lex::set_kind_arith,
    lex::set_kind_op_brack,
    lex::set_kind_cl_brack,
    lex::set_kind_if,
    lex::set_kind_else,
    lex::set_kind_goto,
    lex::set_kind_label
};
