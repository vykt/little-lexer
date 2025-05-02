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
    const std::vector<int> action_idxs;

    //methods
    transition(const int from_idx,
               const int to_idx,
               const std::vector<int> & action_idxs)
     : from_idx(from_idx), to_idx(to_idx), action_idxs(action_idxs) {}
};


using key = std::vector<std::variant<char, std::pair<char, char>>>;


struct state {

    //attributes
    std::map<key, struct transition> transitions;

    //methods
    state(const std::map<key, struct transition> transitions)
     : transitions(transitions) {}

    const struct transition & get_transition(char ch) const;
};


class automata {

  _PRIVATE:
    //attributes
    std::map<std::string, int> symtab;
    const std::vector<std::function<void(char, void *)>> actions;
    std::string input;
    
    //methods
    void build_state_symtab(const std::vector<std::string> & lines);
    std::map<int, struct dfa::state>
        build_table(const std::vector<std::string> & lines);

    void read_input(const std::string path);
    std::map<int, struct state> read_table(const std::string path);

    

  public:
    //attributes
    const std::map<int, struct state> table;

    //methods
    automata(const std::string transition_table_path,
             const std::string input_path,
             const std::vector<std::function<void(char, void *)>> & actions);
    void evaluate(void * ctx);
};

} //namespace `dfa`
