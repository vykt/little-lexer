#pragma once

//STL
#include <vector>
#include <string>

//local
#include "common.hh"
#include "dfa.hh"


namespace lex {

enum type {
    FOO = 1,
    BAR = 2,
    BAZ = 3,
    QUX = 4
};


std::map<enum type, std::string> typetab() {
    return {
        {FOO, "FOO"},
        {BAR, "BAR"},
        {BAZ, "BAZ"},
        {QUX, "QUX"}
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
    dfa::automata dfa;

    //methods
    void read_input(const std::string input_path);

  public:
    //methods
    lexer(const std::string transition_table_path,
          const std::string input_path)
    : dfa(dfa::automata(transition_table_path, input_path, lex_actions)) {}
    void lex();
    std::vector<struct token> & get_tokens() { return ls.get_tokens(); }
};


//state transition functions
void save_token(char ch, void * ctx);
void append_char(char ch, void * ctx);

void set_kind_foo(char ch, void * ctx);
void set_kind_bar(char ch, void * ctx);
void set_kind_baz(char ch, void * ctx);
void set_kind_qux(char ch, void * ctx);


} //namespace `lex`
