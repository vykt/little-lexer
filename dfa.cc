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

//TODO DEBUG
#include <iostream>


//debug
_STATIC void cout_key(std::variant<char, std::pair<char, char>> key) {

    if (std::holds_alternative<std::pair<char,char>>(key)) {
        auto pair = std::get<std::pair<char, char>>(key);
        std::cout << "['" << pair.first << "','"
                  << pair.second << "']" << std::endl;
    } else if (std::holds_alternative<char>(key)) {
        std::cout << "'" << std::get<char>(key) << "'" << std::endl;
    } else {
        std::cout << "<empty>" << std::endl;
    }
}


/*
 *  --- [TRANSITION] ---
 */

const struct dfa::transition & dfa::dfa::get_transition(
    const ::dfa::state_t & node, char ch) const {

    //for every key
    for (auto key_it = node.cbegin(); key_it != node.cend(); ++key_it) {

        auto key = key_it->first;

        //for every variant in this key
        for (auto var_it = key.cbegin(); var_it != key.cend(); ++var_it) {

            std::cout << "cmp: '" << ch << "' vs '";
            cout_key(*var_it);

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

            _continue:
            continue;
            
        } //end for every variant in key
        
    } //end for every key

    throw std::runtime_error("Input does not meet any state transition.");
}



/*
 *  --- [DFA] ---
 */

//control characters
#define _EOF '@'
#define _SEC_DELIM '$'
#define _TRN_DELIM '~'


_STATIC _INLINE bool allow_char(char ch) {

    if ((ch == _SEC_DELIM) || (ch == _TRN_DELIM)) return false;
    if ((ch >= 0x20) && (ch <= 0x7e)) return true;
    if ((ch >= 0x9) && (ch <= 0xd)) return true; 
    return false;
}


std::string dfa::dfa::read_input(const std::string & path) {

    const constexpr int page_size = 4096;
    char buf[page_size];
    std::string input;

    std::ifstream ifs(path);
    if (ifs.is_open() == false) {
        throw std::runtime_error("Failed to open input path.");
    }

    while (ifs.read(buf, page_size) || ifs.gcount() > 0) {
        input.append(buf, ifs.gcount());
    }

    ifs.close();
    return input;
}


std::vector<std::string> dfa::dfa::read_table(const std::string & path) {

    std::vector<std::string> lines;
    std::string line;

    std::ifstream ifs(path);
    if (ifs.is_open() == false) {
        throw std::runtime_error("Failed to open table path.");
    }

    while(std::getline(ifs, line) || ifs.gcount() > 0) {
        if (line.empty() || line[0] == '#') continue;
        else lines.emplace_back(line);
    }

    return lines;
}


#define END_THROW(cur, end) if(cur == end) \
                        throw std::runtime_error("Unexpected end of line.");
#define BAD_THROW(part) throw std::runtime_error( \
                            std::string("Bad table format in ") + part);


//get next character of a `.ll` file line, accommodating for escape backslashes
char dfa::dfa::next_char(std::string::const_iterator & cur,
                         std::string::const_iterator & end) {

    if (cur == end) return _EOF;
    char ch = *cur;
    bool escape = (*cur == '\\') ? true : false;
    
    ++cur;
    if (escape == false) return ch;

    END_THROW(cur, end)
    ch = *cur;
    cur++;
    

    switch(ch) {
        case 'n': return '\n';
        case 't': return '\t';
        case 'v': return '\v';
        case 'r': return '\r';
        case 'f': return '\f';
        case '\\': return '\\';
        default: throw std::runtime_error("Improper escape character.");
    }
}


std::string dfa::dfa::next_symbol(std::string::const_iterator & cur,
                                  std::string::const_iterator & end,
                                  char delim) {
    END_THROW(cur, end)
    auto sym_end = std::find(cur, end, delim);

    //if reached end of line before the delimeter
    if (sym_end == end) {
        std::string sym(cur, sym_end);
        if (sym == "end") return sym;
        BAD_THROW("symbol EOL")

    //else reached the delimeter
    } else {
        std::string sym(cur, sym_end);
        cur = sym_end;
        cur++;
        return sym;
    }
}


dfa::key_t dfa::dfa::next_key(std::string::const_iterator & cur,
                              std::string::const_iterator & end) {

    using one_key = std::variant<char, std::pair<char, char>>;
    using range = std::pair<char, char>;

    #define HAS(T, k) std::holds_alternative<T>(k)

    enum last {
        CHAR = 0,
        RANGE = 1,
        COMMA = 2,
        START = 3,
    };
    enum last last = START;

    key_t key;
    one_key _key;


    while (true) {

        END_THROW(cur, end);
        char ch = this->next_char(cur, end);

        switch (ch) {
            case _SEC_DELIM:
                if (last == CHAR) {
                    key.emplace_back(_key);
                    goto _next_key_done;
                } else {
                    BAD_THROW("key");
                }
                goto _next_key_done;
            
            case '-':
                if (last != CHAR) BAD_THROW("key - invalid format")
                last = RANGE;
                break;

            case ',':
                if (last != CHAR) BAD_THROW("key - invalid format")
                key.emplace_back(_key);
                _key = '\0';
                last=COMMA;
                break;

            default:
                if (!allow_char(ch) || (HAS(range, _key)))
                    BAD_THROW("key - invalid character")

                if (HAS(char, _key) && std::get<char>(_key) != 0)
                    _key = range(std::get<char>(_key), ch);
                else _key = ch;
                last=CHAR;
                break;
                
        } //end switch
    
    } //end while

    _next_key_done:
    return key;
}


std::vector<int> dfa::dfa::next_actions(std::string::const_iterator & cur,
                                        std::string::const_iterator & end) {


    //actions
    std::vector<int> actions;
    std::string idx;

    enum last {
        DIGIT = 0,
        COMMA = 1,
        START = 2,
    };
    enum last last = START;


    while(true) {

        char ch = this->next_char(cur, end);

        //if reached next transaction or EOL
        if (ch == _TRN_DELIM || ch == _EOF) {
            switch (last) {
                case START:
                    goto _next_actions_done;
                case COMMA:
                    BAD_THROW("actions - format")
                case DIGIT:
                    actions.push_back(std::stoi(idx));
                    goto _next_actions_done;
            }
        } //end if

        if (last == COMMA || last == START) {
            if ((ch < 0x30) || (ch > 0x39)) BAD_THROW("actions - not a digit")
            idx = std::string() + ch;
            last = DIGIT;
        } else if (last == DIGIT) {
            if (ch == ',') {
                actions.push_back(std::stoi(idx));
                last = COMMA;
            } else if ((ch >= 0x30) && (ch <= 0x39)) {
                idx += ch;
                last = DIGIT;
            } else BAD_THROW("actions - invalid character")
        } //end if
        
    } //end while

    _next_actions_done:
    return actions;
}


dfa::symtab_t dfa::dfa::build_symtab(const std::string & path) {

    auto lines = this->read_table(path);
    ::dfa::symtab_t symtab;

    int idx = 0;
    for (auto line_it = lines.cbegin(); line_it != lines.cend(); ++line_it) {

        auto cur = line_it->cbegin();
        auto end = line_it->cend();
        std::string sym = this->next_symbol(cur, end, _TRN_DELIM);
    
        if (symtab.find(sym) == this->symtab.end()) {
            symtab.emplace(sym, idx);
            ++idx;
        } else {
            throw std::runtime_error("Duplicate state definition.");
        }
    } //end for

    return symtab;
}


dfa::table_t dfa::dfa::build_table(const std::string & path) {

    /*
     *  Behold, the (formerly) inline parser.
     */

    auto lines = this->read_table(path);
    std::map<int, ::dfa::state_t> state_table;
    int state_idx = 0;
    
    for (auto line_it = lines.cbegin(); line_it != lines.cend(); ++line_it) {

        state_t state;
        auto cur = line_it->cbegin();
        auto end = line_it->cend();

        auto from_sym = this->next_symbol(cur, end, _TRN_DELIM);
        auto from_idx_it = this->symtab.find(from_sym);
        if (from_idx_it == this->symtab.end())
            throw std::runtime_error("Symbol not found.");
        if (from_sym == "end") goto _build_table_done;

        //every transition
        while (cur != end) {

            key_t key = this->next_key(cur, end);

            auto to_sym = this->next_symbol(cur, end, _SEC_DELIM);
            auto to_idx_it = this->symtab.find(to_sym);
            if (to_idx_it == this->symtab.end())
                throw std::runtime_error("Symbol not found.");

            auto actions = next_actions(cur, end);

            transition transition(
                           from_idx_it->second, to_idx_it->second, actions);
            state.emplace(key, transition);

        } //end while

        _build_table_done:
        //add state to state table
        state_table[state_idx] = state;
        ++state_idx;

    } //end for

    return state_table;
}


void dfa::dfa::evaluate(void * ctx) {

    #define STATE_THROW if(state == this->table.cend()) \
                          throw std::runtime_error("Invalid state reached.");

    auto cur = this->input.cbegin();
    auto state = this->table.find(0);
    STATE_THROW

    //while there is input available
    while (cur != this->input.cend()) {

        //get appropriate transition
        auto & transition = this->get_transition(state->second, *cur);
        std::cout << "From " << transition.from_idx << " to "
                  << transition.to_idx << std::endl;

        //run each action for this transition
        for (auto act = transition.actions.cbegin();
             act != transition.actions.cend(); ++act) {

            std::cout << "Running action " << *act << std::endl;
            this->actions[*act](*cur, ctx);
        }

        //change state
        state = this->table.find(transition.to_idx);
        cur++;
        STATE_THROW
        
    } //end while

    return;
}
