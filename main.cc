//STL
#include <iostream>
#include <stdexcept>

//local
#include "common.hh"
#include "lex.hh"


int main(int argc, char ** argv) {

    if (argc != 3) {
        throw std::runtime_error("Use: little-lexer <states file> <input file>");
    }

    //run lexer
    lex::lexer lexer(argv[1], argv[2]);
    lexer.lex();

    //view output
    auto & tokens = lexer.get_tokens();


    //for every token
    auto typetab = lex::typetab();
    for (auto it = tokens.cbegin(); it != tokens.cend(); ++it) {
        std::cout << "OPCODE: " << typetab[it->kind]
                  << " LEXEME: " << it->lexeme << std::endl; 
    }

    return 0;
}
