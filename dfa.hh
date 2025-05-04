#pragma once

//STL
#include <optional>
#include <variant>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <functional>

//local
#include "common.hh"


namespace dfa {

struct transition {

    //attributes
    const int from_idx, to_idx;
    const std::vector<int> actions;

    //methods
    transition(const int from_idx,
               const int to_idx,
               const std::vector<int> & actions)
     : from_idx(from_idx), to_idx(to_idx), actions(actions) {}
};


using key_t = std::vector<std::variant<char, std::pair<char, char>>>;
using state_t = std::map<key_t, transition>;
using symtab_t = std::map<std::string, int>;
using table_t = std::map<int, state_t>;


class dfa {

  _PRIVATE:
    //methods
    std::string read_input(const std::string & path);
    std::vector<std::string> read_table(const std::string & path);

    // -- parsing table (`.ll` file)
    char next_char(std::string::const_iterator & cur,
                   std::string::const_iterator & end);
    std::string next_symbol(std::string::const_iterator & cur,
                            std::string::const_iterator & end,
                            char delim);
    key_t next_key(std::string::const_iterator & cur,
                 std::string::const_iterator & end);
    std::vector<int> next_actions(std::string::const_iterator & cur,
                                  std::string::const_iterator & end);

    symtab_t build_symtab(const std::string & path);
    table_t build_table(const std::string & path);

    const transition & get_transition(const state_t & node, char ch) const;

  public:
    //attributes
    const symtab_t symtab;
    const std::vector<std::function<void(char, void *)>> actions;
    const std::map<int, state_t> table;
    const std::string input;

    //methods
    dfa(const std::string table_path,
        const std::string input_path,
        const std::vector<std::function<void(char, void *)>> & actions)
    : symtab(build_symtab(table_path)),
      actions(actions),
      table(build_table(table_path)),
      input(read_input(input_path)) {}
    
    void evaluate(void * ctx);
};

} //namespace `dfa`
