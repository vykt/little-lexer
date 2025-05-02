
//STL
#include <vector>
#include <variant>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>
#include <stdexcept>

//local
#include "common.hh"
#include "dfa.hh"



const struct dfa::transition & dfa::state::get_transition(char ch) const {

    //for every key
    for (auto key_it = this->transitions.cbegin();
         key_it != this->transitions.cend(); ++key_it) {

        auto key = key_it->first;

        //for every variant in this key
        for (auto var_it = key.cbegin(); var_it != key.cend(); ++var_it) {

            if (std::holds_alternative<char>(*var_it)) {
                if (std::get<char>(*var_it) == ch)
                    return key_it->second;
                else goto _continue;
            } else {
                auto ch_pair = std::get<std::pair<char, char>>(*var_it);
                if ((ch >= ch_pair.first) && (ch <= ch_pair.second))
                    return key_it->second;
                else goto _continue;
            }
            
        } //end for every variant in key

        _continue:
        continue;
        
    } //end for every key

    throw std::runtime_error("Input does not meet any state transition.");
}



#define END_THROW(it) if(it == line_it->cend()) \
                        throw std::runtime_error("Unexpected end of line.");


void dfa::automata::build_state_symtab(const std::vector<std::string> & lines) {

    for (auto line_it = lines.cbegin(); line_it != lines.cend(); ++line_it) {

        auto sym_end = std::find(line_it->cbegin(), line_it->cend(), ' ');
        END_THROW(sym_end);
        std::string sym(line_it->cbegin(), sym_end--);
    
        if (this->symtab.find(*line_it) == this->symtab.end()) {
            this->symtab.emplace(*line_it, 0);
        } else {
            throw std::runtime_error("Duplicate state definition.");
        }
    } //end for

    return;
}


std::map<int, struct dfa::state>
    dfa::automata::build_table(const std::vector<std::string> & lines) {

    /*
     *  Behold, the inline parser.
     */

    #define NEXT_CHAR str_it++; END_THROW(str_it)

    std::map<int, struct dfa::state> state_table;
    int state_idx = 0;
    
    for (auto line_it = lines.cbegin(); line_it != lines.cend(); ++line_it) {

        //own symbol
        auto str_it = line_it->cbegin();
        auto sym_end = std::find(str_it, line_it->cend(), ' ');
        END_THROW(sym_end);
        
        std::string from_sym(line_it->cbegin(), sym_end--);
        auto from_idx_it = this->symtab.find(from_sym);
        if (from_idx_it == this->symtab.end())
            throw std::runtime_error("Symbol not found.");

        str_it = sym_end;
        NEXT_CHAR
        NEXT_CHAR


        std::map<key, struct transition> transitions;

        //every state transition
        while (str_it != line_it->cend()) {

            //state transition keys
            key key;
            std::variant<char, std::pair<char, char>> range;
            bool run_loop = true;
        
            while (run_loop) {

                range = *str_it;
                NEXT_CHAR

                switch (*str_it) {
                    case ':':
                        key.emplace_back(range);
                        run_loop = false;
                        break;
                    
                    case '-':
                        NEXT_CHAR
                        range = std::pair<char, char>
                                    (std::get<char>(range), *str_it);
                        key.emplace_back(range);
                        break;

                    case ',':
                        break;

                    default:
                        throw std::runtime_error("Invalid range.");    
                } //end switch
            
            } //end while

            NEXT_CHAR


            //next state
            sym_end = std::find(str_it, line_it->cend(), ':');
            END_THROW(sym_end);
            std::string to_sym(str_it, sym_end--);
            auto to_idx_it = this->symtab.find(to_sym);
            if (to_idx_it == this->symtab.end())
                throw std::runtime_error("Symbol not found.");
            NEXT_CHAR
            NEXT_CHAR


            //actions
            std::vector<int> action_idxs;
            std::string idx;
            run_loop = true;

            while(run_loop) {

                idx = "";
                idx += *str_it;

                do {
                    str_it++;
                    if (str_it == line_it->cend()) { run_loop = false; break; }
                    else if (*str_it == ' ') { run_loop = false; break; }
                    else if (*str_it >= 0x30 && *str_it <= 0x39) idx += *str_it;
                    else if (*str_it == ',') break;
                } while (true);

                action_idxs.push_back(std::stoi(idx));
            
            } //end while


            //construct the transition
            struct transition transition(from_idx_it->second,
                                         to_idx_it->second,
                                         action_idxs);
            transitions.emplace(key, transition);

        } //end while (every transition)


        //add state to state table
        struct state state(transitions);
        state_table[state_idx] = state;
        ++state_idx;

    } //end for (every line)

    return state_table;
}


_STATIC const constexpr int page_size = 4096;


void dfa::automata::read_input(const std::string path) {

    char buf[page_size];

    std::ifstream ifs(path);
    if (ifs.is_open() == false) {
        throw std::runtime_error("Failed to open input path.");
    }

    while (ifs.read(buf, page_size) || ifs.gcount() > 0) {
        this->input.append(buf, ifs.gcount());
    }

    ifs.close();
    return;
}



std::map<int, struct dfa::state>
    dfa::automata::read_table(const std::string path) {

    std::vector<std::string> lines;
    std::string line;

    std::ifstream ifs(path);
    if (ifs.is_open() == false) {
        throw std::runtime_error("Failed to open table path.");
    }

    while(std::getline(ifs, line) || ifs.gcount() > 0) {
        if (line[0] == '#') continue;
        else lines.emplace_back(line);
    }

    build_state_symtab(lines);

    return build_table(lines); //tail call
}


void dfa::automata::evaluate(void * ctx) {

    #define STATE_THROW if(current == this->table.cend()) \
                          throw std::runtime_error("Invalid state reached.");

    auto str_it = this->input.cbegin();
    auto current = this->table.find(0);
    STATE_THROW

    //while there is input available
    while (str_it != this->input.cend()) {

        auto & transition = current->second.get_transition(*str_it);

        //run each action for this transition
        for (auto act_it = transition.action_idxs.cbegin();
             act_it != transition.action_idxs.cend(); ++act_it) {
            this->actions[*act_it](*str_it, ctx);
        }

        //change state
        current = this->table.find(transition.to_idx);
        STATE_THROW
        
    } //end while

    return;
}
