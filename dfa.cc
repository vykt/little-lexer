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



/*
 *  --- [TRANSITION] ---
 */

const struct dfa::transition & dfa::dfa::get_transition(
    const ::dfa::state & node, char ch) const {

    //for every key
    for (auto key_it = node.cbegin(); key_it != node.cend(); ++key_it) {

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



/*
 *  --- [DFA] ---
 */


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

    if (cur == end) return '\0';
    char ch = *cur;
    bool escape = (*cur == '\\') ? true : false;
    
    ++cur;
    if (escape == false) return ch;

    if (cur == end) return '\0';
    ch = *cur;
    cur++;
    

    switch(ch) {
        case 'n': return '\n';
        case 't': return '\t';
        case 'v': return '\v';
        case 'r': return '\r';
        case 'f': return '\f';
        case 'b': return '\b';
        case 'a': return '\a';
        case '\\': return '\\';
        case '*': return '*'; 
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
        sym_end--;
        std::string sym(cur, sym_end);
        if (sym == "end") return sym;
        BAD_THROW("symbol EOL")

    //else reached the delimeter
    } else {
        std::string sym(cur, sym_end--);
        cur = sym_end;
        cur++;
        cur++;
        return sym;
    }
}


dfa::key dfa::dfa::next_key(std::string::const_iterator & cur,
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

    key key;
    one_key _key;


    while (true) {

        END_THROW(cur, end);
        char ch = this->next_char(cur, end);

        switch (ch) {
            case ':':
                if (last == CHAR) {
                    key.emplace_back(_key);
                    goto _next_key_done;
                } else {
                    BAD_THROW("key");
                }
                goto _next_key_done;
            
            case '-':
                if (last != CHAR) BAD_THROW("key")
                last = RANGE;
                break;

            case ',':
                if (last != CHAR) BAD_THROW("key")
                key.emplace_back(_key);
                _key = '\0';
                last=COMMA;
                break;

            default:
                if ((ch < 0x30) || (ch > 0x7d) || (HAS(range, _key)))
                    BAD_THROW("key")

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

        //if reached EOL
        if (ch == '\0') {
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


dfa::symtab dfa::dfa::build_symtab(const std::string & path) {

    auto lines = this->read_table(path);
    ::dfa::symtab symtab;

    int idx = 0;
    for (auto line_it = lines.cbegin(); line_it != lines.cend(); ++line_it) {

        auto cur = line_it->cbegin();
        auto end = line_it->cend();
        std::string sym = this->next_symbol(cur, end, ' ');
    
        if (symtab.find(sym) == this->symtab.end()) {
            symtab.emplace(sym, idx);
            ++idx;
        } else {
            throw std::runtime_error("Duplicate state definition.");
        }
    } //end for

    return symtab;
}


dfa::table dfa::dfa::build_table(const std::string & path) {

    /*
     *  Behold, the inline parser.
     */

    auto lines = this->read_table(path);

    #define NEXT_CHAR str_it++; END_THROW(str_it)

    std::map<int, ::dfa::state> state_table;
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


        dfa::state state;

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
                        NEXT_CHAR;
                        if (*str_it == ':')
                            run_loop = false;
                        if (*str_it != ',')
                            throw std::runtime_error("Invalid range");
                        NEXT_CHAR;
                        break;

                    case ',':
                        key.emplace_back(range);
                        NEXT_CHAR;
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
            str_it = sym_end;
            NEXT_CHAR


            //actions
            std::vector<int> action_idxs;
            std::string idx;
            run_loop = true;

            //skip if end-of-line & no actions
            str_it++;
            if (str_it != line_it->cend()) {

                while(run_loop) {

                    idx = "";
                    if (*str_it == ' ') { NEXT_CHAR; break; }
                    idx += *str_it;

                    do {
                        str_it++;
                        if (str_it == line_it->cend()) { run_loop = false; break; }
                        else if (*str_it == ' ') { NEXT_CHAR; run_loop = false; break; }
                        else if (*str_it >= 0x30 && *str_it <= 0x39) idx += *str_it;
                        else if (*str_it == ',') { NEXT_CHAR; break; }
                    } while (true);

                    action_idxs.push_back(std::stoi(idx));
            
                } //end while
            } //end if

            //construct the transition
            struct transition transition(from_idx_it->second,
                                         to_idx_it->second,
                                         action_idxs);
            state.emplace(key, transition);

        } //end while (every transition)


        //add state to state table
        state_table[state_idx] = state;
        ++state_idx;

    } //end for (every line)

    return state_table;
}



void dfa::automata::evaluate(void * ctx) {

    #define STATE_THROW if(current == this->table.cend()) \
                          throw std::runtime_error("Invalid state reached.");

    auto str_it = this->input.cbegin();
    auto current = this->table.find(0);
    STATE_THROW

    //while there is input available
    while (str_it != this->input.cend()) {

        //get appropriate transition
        auto & transition = this->get_transition(current->second, *str_it);

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
