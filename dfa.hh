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


struct key {

    //attributes
    const std::vector<std::variant<char, std::pair<char, char>>> ranges;

    //methods
    key(const std::vector<std::variant<char, std::pair<char, char>>> ranges)
     : ranges(ranges) {}
};


struct state {

    //attributes
    const std::map<key, struct transition> transitions;

    //methods
    state(const std::map<key, struct transition> transitions)
     : transitions(transitions) {}
};


class dfa {

  _PRIVATE:
    //attributes
    const std::vector<struct state> table;
    const std::vector<std::function<void(char, void *)>> actions;
    std::string input;
    
    //methods
    std::vector<struct state> read_table(const std::string path);

  public:
    //methods
    dfa(const std::string transition_table_path, std::string input,
        std::vector<std::function<void(char, void *)>> actions)
    : table(read_table(transition_table_path)),
      actions(actions),
      input(input) {}

    void evaluate();
};

} //namespace `dfa`
