#pragma once

//STL
#include <vector>
#include <string>

//local
#include "common.hh"
#include "dfa.hh"


namespace lex {

enum type {
    TYPE = 0,
    VAR = 1,
    IMM = 2,
    ARITH = 3,
    OP_BRACK = 4,
    CL_BRACK = 5,
    IF = 6,
    ELSE = 7,
    GOTO = 8,
    LABEL = 9
};


inline std::map<enum type, std::string> typetab() {
    return {
        {TYPE,     "Type          "},
        {VAR,      "Variable      "},
        {IMM,      "Immmediate    "},
        {ARITH,    "Arithmetic op."},
        {OP_BRACK, "Open bracket  "},
        {CL_BRACK, "Close bracket "},
        {IF,       "If            "},
        {ELSE,     "Else          "},
        {GOTO,     "Go-to         "},
        {LABEL,    "Label         "}
    };
}


struct token {

    const enum type kind;
    const std::string lexeme;

    token(const enum type kind, const std::string lexeme)
    : kind(kind), lexeme(lexeme) {}
};


class lex_state {

  _PRIVATE:
    //attributes
    std::vector<token> tokens;
    enum type cur_kind;
    std::string cur_lexeme;

  public:
    //methods
    std::vector<struct token> & get_tokens() { return tokens; };

    // -- produce new token
    void add_token();

    // -- modify current token
    void set_kind(const enum type kind) { this->cur_kind = kind; };
    void append_char(const char ch) { this->cur_lexeme += ch; }
};


extern const std::vector<std::function<void(char, void *)>> lex_actions;


class lexer {

  _PRIVATE:
    //attributes
    lex_state ls;
    dfa::dfa dfa;

    //methods
    void read_input(const std::string input_path);

  public:
    //methods
    lexer(const std::string transition_table_path,
          const std::string input_path)
    : dfa(dfa::dfa(transition_table_path, input_path, lex_actions)) {}
    void lex();
    std::vector<struct token> & get_tokens() { return ls.get_tokens(); }
};


//state transition functions
void save_token(char ch, void * ctx);
void append_char(char ch, void * ctx);

void set_kind_type(char ch, void * ctx);
void set_kind_var(char ch, void * ctx);
void set_kind_imm(char ch, void * ctx);
void set_kind_arith(char ch, void * ctx);
void set_kind_op_brack(char ch, void * ctx);
void set_kind_cl_brack(char ch, void * ctx);
void set_kind_if(char ch, void * ctx);
void set_kind_else(char ch, void * ctx);
void set_kind_goto(char ch, void * ctx);
void set_kind_label(char ch, void * ctx);

} //namespace `lex`
