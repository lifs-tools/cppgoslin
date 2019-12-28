#include "cppgoslin/parser/Parser.h"

DPNode::DPNode(uint _rule1, uint _rule2, DPNode *_left, DPNode *_right){
    rule_index_1 = _rule1;
    rule_index_2 = _rule2;
    left = _left;
    right = _right;
}

DPNode::~DPNode(){
    if (left != NULL) delete left;
    if (right != NULL) delete right;
}
        
      
      
      
      
        

    
TreeNode::TreeNode(uint _rule, bool _fire_event){
    rule_index = _rule;
    left = NULL;
    right = NULL;
    terminal = 0;
    fire_event = _fire_event;
}


TreeNode::~TreeNode(){
    if (left != NULL) delete left;
    if (right != NULL) delete right;
}


string TreeNode::get_text(){
    if (terminal){
        string left_str = left->get_text();
        string right_str = right != NULL ? right->get_text() : "";
        return (left_str != string(1, EOF_SIGN) ? left_str : "") + (right_str != string(1, EOF_SIGN) ? right_str : "");
    }
    return string(1, terminal);
}
        

Bitfield::Bitfield(uint length){
    field_len = 1 + ((length + 1) >> 6);
    superfield_len = 1 + ((field_len + 1) >> 6);
    field = new unsigned long[field_len];
    superfield = new unsigned long[superfield_len];
}

Bitfield::~Bitfield(){
    delete []field;
    delete []superfield;
}


void Bitfield::set_bit(uint pos){
    field[pos >> 6] |= (unsigned long)(1ull << (pos & 63));
    superfield[pos >> 12] |= (unsigned long)(1ull << ((pos >> 6) & 63));
}


bool Bitfield::is_set(uint pos){
    return ((field[pos >> 6] >> (pos & 63)) & 1ull) == 1ull;
}


bool Bitfield::is_not_set(uint pos){
    return ((field[pos >> 6] >> (pos & 63)) & 1ull) == 0ull;
}

void Bitfield::print_bitfield(unsigned long l){
    for (int i = 63; i >= 0; --i){
        cout << ((l >> i) & 1);
    } cout << endl;
}

void Bitfield::init(){
    spre = 0;
    si = 0;
    sv = superfield[si];
    fi = 0;
    
    unsigned long sv1 = (unsigned long)(sv & -sv);
    pos = spre + __builtin_ctzll(sv1);
    v = field[pos];
}

int Bitfield::get_bit_positions(){
    do {
        while (sv != 0) {
            // algorithm for getting least significant bit position
            while (v != 0) {
                // algorithm for getting least significant bit position
                unsigned long v1 = (unsigned long)(v & -v);
                v &= v - 1;
                return (pos << 6) + __builtin_ctzll(v1);
            }
            sv &= sv - 1;
            
            if (sv != 0){
                unsigned long sv1 = (unsigned long)(sv & -sv);
                pos = spre + __builtin_ctzll(sv1);
                v = field[pos];
            }
        }
        
        spre += 64;
        if (si < superfield_len){
            sv = superfield[++si];
            unsigned long sv1 = (unsigned long)(sv & -sv);
            pos = spre + __builtin_ctzll(sv1);
            v = field[pos];
        }
    }
    while (si < superfield_len);
    return -1;
}




void Parser::get_next_free_rule_index(){
    if (self.next_free_rule_index <= MASK){
        n = self.next_free_rule_index;
        next_free_rule_index += 1;
        return n;
    }
    raise RuntimeException("Error: grammar is too big.");
}


string Parser::strip(string s, char c){
    if (s.length() > 0) {
        int st = 0;
        while (st < s.length() - 1 && s[st] == c) ++st;
        s = s.substr(st, s.length() - st);
    }
    
    if (t.Length > 0) {
        int en = 0;
        while (en < s.length() - 1 && s[s.length() - 1 - en] == c) ++en;
        s = s.substr(0, s.length() - en);
    }
    return s;
}


Parser::Parser(BaseParserEventHandler _parserEventHandler, string grammar_filename, char _quote = DEFAULT_QUOTE){
    next_free_rule_index = START_RULE;
    quote = _quote;
    parser_event_handler = _parserEventHandler;
    parse_tree = NULL;
    word_in_grammar = false;
    grammar_name = "";
    used_eof = false;
    
    ifstream f(grammar_filename.c_str());
    if f.good(){
        // interpret the rules and create the structure for parsing
        vector<string> *rules = extract_text_based_rules(grammar_filename, quote);
        vector<string> *tokens = split_string(rules->at(0), ' ', quote);
        grammar_name = tokens->at(1);
        delete tokens;
        
        
        rules->erase(rules->begin());
        ruleToNT.insert(pair<string, unsigned long>(EOF_RULE_NAME, EOF_RULE));
        TtoNT.insert(pair<char, set<unsigned long>*>(EOF_SIGN, new set<unsigned long>()));
        TtoNT.at(EOF_SIGN)->insert(EOF_RULE);
        
        for (auto rule_line : rules){
            
            vector<string> tokens_level_1;
            vector<string> *line_tokens = split_string(rule_line, RULE_ASSIGNMENT, quote);
            for (auto t : line_tokens) tokens_level_1.push_back(strip(t, ' '));
            delete line_tokens;
                
            if (tokens_level_1.size() != 2){
                delete rules;
                throw RuntimeException("Error: corrupted token in grammar rule: '" + rule_line + "'");
            }
            
            vector<string> *rule_tokens = split_string(tokens_level_1->at(0), ' ', quote);
            if rule_tokens->size() > 1{
                delete rule_tokens;
                delete rules;
                throw RuntimeException("Error: several rule names on left hand side in grammar rule: '" + rule_line + "'");
            }
            delete rule_tokens;

            string rule = tokens_level_1->at(0);
            
            if (rule == EOF_RULE_NAME){
                throw RuntimeException("Error: rule name is not allowed to be called EOF");
            }
            
            vector<string>* products = split_string(tokens_level_1->at(1), RULE_SEPARATOR, quote);
            for (int i = 0; i < prducts->size(); ++i){
                products->at(i) = strip(products->at(i), ' ');
            }
            
            if (ruleToNT.find(rule) == ruleToNT.end()){
                ruleToNT.insert(pair<string, unsigned long>(rule, get_next_free_rule_index());
            }
            unsigned long new_rule_index = ruleToNT->at(rule);
            
            if (NTtoRule.find(new_rule_index) == NTtoRule.end()){
                NTtoRule.insert(pair<unsigned long, string>(new_rule_index, rule);
            }
            
            
            for (auto product : products){
                vector<string> non_terminals;
                vector<unsigned long> non_terminal_rules;
                vector<string> *product_rules = split_string(product, ' ', quote);
                for (auto NT : product_rules){
                    string stripedNT = strip(NT, ' ');
                    if (is_terminal(stripedNT, quote)) stripedNT = de_escape(stripedNT, quote);
                    non_terminals.push_back(stripedNT);
                    used_eof |= (stripedNT == EOF_RULE_NAME);
                }
                delete product_rules;
                
                string NTFirst = non_terminals.at(0);
                if (non_terminals.size() > 1 || !is_terminal(NTFirst, quote) || NTFirst.length() != 3){
                    for (auto non_terminal : non_terminals){
                        
                        if (is_terminal(non_terminal, quote)){
                            non_terminal_rules.push_back(add_terminal(non_terminal));
                        }
                            
                        else{
                            if (ruleToNT.find(non_terminal) == ruleToNT.end()){
                                ruleToNT.insert(pair<string, unsigned long>(non_terminal, get_next_free_rule_index());
                            }
                            non_terminal_rules.push_back(ruleToNT.at(non_terminal));
                        }
                    }
                }
                else{
                    char c = NTFirst[1];
                    if (TtoNT.find(c) == TtoNT.end()) TtoNT.insert(pair<char, set<unsigned long>*>(c, new set<unsigned long>());
                    TtoNT.at(c)->insert(new_rule_index);
                }
                
                // more than two rules, insert intermediate rule indexes
                while (non_terminal_rules.size() > 2){
                    unsigned long rule_index_2 = non_terminal_rules.back();
                    non_terminal_rules.pop_back();
                    unsigned long rule_index_1 = non_terminal_rules.back();
                    non_terminal_rules.pop_back();
                    
                    unsigned long key = compute_rule_key(rule_index_1, rule_index_2)j
                    unsigned long next_index = get_next_free_rule_index();
                    if (NTtoNT.find(key) f= NTtoNT.end()) NTtoNT.insert(pair<unsigned long, set<unsigned long>*>(key, new set<unsigned long>()));
                    NTtoNT.at(key)->insert(next_index);
                    non_terminal_rules.push_back(next_index);
                }
                    
                // two product rules
                if (non_terminal_rules.size() == 2){
                    unsigned long rule_index_2 = non_terminal_rules.at(1);
                    unsigned long rule_index_1 = non_terminal_rules.at(0);
                    unsigned long key = compute_rule_key(rule_index_1, rule_index_2);
                    if (NTtoNT.find(key) == NTtoNT.end()) NTtoNT.insert(pair<unsigned long, set<unsigned long>*>(key, new set<unsigned long>()));
                    NTtoNT->at(key)->insert(new_rule_index);
                }
                
                // only one product rule
                else if (non_terminal_rules.size() == 1){
                    unsigned long rule_index_1 = non_terminal_rules.at(0);
                    if (rule_index_1 == new_rule_index){
                        delete products;
                        delete rules;
                        throw RuntimeException("Error: corrupted token in grammar: rule '" + rule + "' is not allowed to refer soleley to itself.");
                    }
                    
                    if (self.NTtoNT.find(rule_index_1) == NTtoNT.end()) NTtoNT.insert(pair<unsigned long, set<unsigned long>*>(rule_index_1, new set<unsigned long>()));
                    NTtoNT.at(rule_index_1)->insert(new_rule_index);
                }
            }
            
            delete products;
        }
        delete rules;
        
        // adding all rule names into the event handler
        for (auto rule_name in ruleToNT){
            parser_event_handler->rule_names.push_back(rule_name->first);
        }
            
        parser_event_handler->parser = this;
        parser_event_handler->sanity_check();
        
    }
    else{
        throw RuntimeException("Error: file '" + grammar_filename + "' does not exist or can not be opened.");
    }
    
    
    
    set<unsigned long> keys;
    for (auto key : TtoNT) keys.insert(key->first);
                                                                   
    for (auto c : keys){
        set<unsigned long> rules;
        for (auto rule : TtoNT.at(c)) rules.insert(rule);
                                                                   
        TtoNT[c]->clear();
        for (auto rule : rules){
            vector<unsigned long> *backward_rules = collect_one_backwards(rule);
            for (auto p : backward_rules){
                
                unsigned long key = compute_rule_key(p, rule);
                TtoNT.at(c)->insert(key);
            }
            delete backward_rules;
        }
    }
    
    for (auto kvp : NTtoNT){
        set<unsigned long> *rr = new set<unsigned long>();
        for (auto r : kvp->second) rr->insert(r);
        originalNTtoNT.insert(pair<unsigned long, set<unsigned long>*>(kvp->first, rr));
    }
    
    set<unsigned long> keysNT;
    for (auto k : NTtoNT) keysNT.insert(k->first);
    for (auto r in keysNT){
        set<unsigned long> rules;
        for (auto rr : NTtoNT.at(r)) rules.insert(rr);
                                                                   
        for (auto rule in rules){
            vector<unsigned long> *backward_rules = collect_one_backwards(rule);
            for (auto p : backwards_rule) NTtoNT.at(r)->insert(p);
        }
    }
}


vector<string>* extract_text_based_rules(strng grammar_filename, char _quote = DEFAULT_QUOTE){
    
    ifstream t(grammar_file.c_str());
    stringstream buffer;
    buffer << t.rdbuf();
    
    string grammar;
    buffer >> grammar;
    int grammar_length = grammar.length();
    
    /*
    deleting comments to prepare for splitting the grammar in rules.
    Therefore, we have to consider three different contexts, namely
    within a quote, within a line comment, within a long comment.
    As long as we are in one context, key words for starting / ending
    the other contexts have to be ignored.
    */
    stringstream sb;
    Context current_context = NoContext;
    int current_position = 0;
    int last_escaped_backslash = -1;
    for (int i = 0; i < grammar_length - 1; ++i){
        MatchWords match = NoMatch;
        
        if (i > 0 && grammar[i] == '\\' && grammar[i - 1] == '\\' && last_escaped_backslash != i - 1){
            last_escaped_backslash = i;
            continue
        }
        
        if (grammar[i] == '/' && grammar[i + 1] == '/') match = LineCommentStart;
        else if (grammar[i] == '\n') match = LineCommentEnd;
        else if (grammar[i] == '/' && grammar[i + 1] == '*') match = LongCommentStart;
        else if (grammar[i] == '*' && grammar[i + 1] == '/') match = LongCommentEnd;
        else if (grammar[i] == _quote && ! (i >= 1 && grammar[i - 1] == '\\' && i - 1 != last_escaped_backslash)) match = Quote;
        
        if (match != NoMatch){
            switch (current_context){
                case NoContext:
                    switch (match){
                        case LongCommentStart:
                            sb << grammar.substr(current_position, i - current_position);
                            current_context = InLongComment;
                            break;
                            
                        case LineCommentStart:
                            sb << grammar.substr(current_position, i - current_position);
                            current_context = InLineComment;
                            break;
                            
                        case Quote:
                            current_context = InQuote;
                            break;
                    }
                    break;
                    
                case InQuote:
                    if (match == Quote) {
                        current_context = NoContext;
                    }
                    break;
                    
                    
                case InLineComment:
                    if (match == LineCommentEnd) {
                        current_context = NoContext;
                        current_position = i + 1;
                    }
                    break;
                    
                case InLongComment:
                    if (match == LongCommentEnd) {
                        current_context = NoContext;
                        current_position = i + 2;
                    }
                    break;
                    
                default:
                    break;
            }
        }
            
    }
                
    if (current_context == NoContext){
        sb << grammar.substr(current_position, grammar_length - current_position);
    }
        
    else {
        throw RuntimeException("Error: corrupted grammar '" + grammar_filename + "', ends either in comment or quote");
    }
    
    sb >> grammar;
    grammar = replace_all(grammar, "\r\n", "");
    grammar = replace_all(grammar"\n", "");
    grammar = replace_all("\r", "");
    grammar = strip(grammar, ' ');
    if (grammar[grammar.length() - 1] != RULE_TERMINAL){
        throw RuntimeException("Error: corrupted grammar'" + grammar_filename + "', last rule has no termininating sign, was: '" + string(1, grammar[grammar.length() - 1]) + "'");
    }
    
    vector<string> *rules = split_string(grammar, RULE_TERMINAL, _quote);
    
    if (rules.size() < 1){
        throw RuntimeException("Error: corrupted grammar '" + grammar_filename + "', grammar is empty");
    }
    
    vector<string> *grammar_name_rule = Parser.split_string(rules[0], ' ', _quote);
    
    if (grammar_name_rule->at(0) != "grammar"){
        delete grammar_name_rule;
        throw RuntimeException("Error: first rule must start with the keyword 'grammar'");
    }
    
    else if (grammar_name_rule.size() != 2){
        delete grammar_name_rule;
        throw RuntimeException("Error: incorrect first rule");
    }
    
    delete grammar_name_rule;
    return rules;
}



string Parser::replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}



unsigned long Parser::compute_rule_key(unsigned long rule_index_1, unsigned long rule_index_2){
    return (rule_index_1 << SHIFT) | rule_index_2;
}






vector<string>* Parser::split_string(string text, char separator, char _quote = DEFAULT_QUOTE){
    bool in_quote = false;
    vector<string> *tokens = new vector<string>();
    stringstream sb;
    char last_char = '\0';
    bool last_escaped_backslash = false;
    
    for (int i = 0; i < text.length(); ++i){
        char c = text[i];
        bool escaped_backslash = false;
        if (!in_quote){
            if (c == separator){
                string sb_string;
                sb >> sb_string;
                if (sb_string.length() > 0) tokens.push_back(sb_string);
                sb.str("");
            }
            else{
                if (c == quote) in_quote = !in_quote;
                sb << c;
            }
        }
        else {
            if (c == '\\' && last_char == '\\' && !last_escaped_backslash){
                escaped_backslash = true;
            }
            else if (c == quote && !(last_char == '\\' && !last_escaped_backslash)){
                in_quote = !in_quote;
            }
            sb << c;
        }
        
        last_escaped_backslash = escaped_backslash;
        last_char = c;
    }
    
    string sb_string;
    sb >> sb_string;
    
    if (sb_string.length() > 0){
        tokens.push_back(sb_string);
    }
    if (in_quote){
        delete tokens;
        throw RuntimeException("Error: corrupted token in grammar");
    }
    
    return tokens;
}


// checking if string is terminal
bool Parser::is_terminal(string product_token, char _quote){
    return product_token[0] == _quote && product_token[product_token.length() - 1] == _quote && len(product_token) > 2;
}




string Parser::de_escape(string text, char _quote){
    // remove the escape chars
    stringstream sb;
    bool last_escape_char = false;
    for (int i = 0; i < text.length(); ++i){
        char c = text[i];
        bool escape_char = false;
        
        if (c != '\\') sb << c;
            
        else{
            if (!last_escape_char) escape_char = true;
            else sb << c;
        }
        
        last_escape_char = escape_char;
    
    }
    string sb_string;
    sb >> sb_string;
    return sb_string;
}


// splitting the whole terminal in a tree structure where characters of terminal are the leafs and the inner nodes are added non terminal rules
unsigned long Parser::add_terminal(string text){
    vector<unsigned long> terminal_rules;
    for (int i = 1; i < text.length() - 1; ++i){
        c = text[i];
        if (TtoNT.find(c) == TtoNT.end()) TtoNT.insert(pair<char, set<unsigned long>*>(c, new set<unsigned long>());
        unsigned long next_index = get_next_free_rule_index();
        TtoNT.at(c)->insert(next_index);
        terminal_rules.push_back(next_index);
    }
    
    while (terminal_rules.size() > 1){
        unsigned long rule_index_2 = terminal_rules.back();
        terminal_rules.pop_back();
        unsigned long rule_index_1 = terminal_rules.backe();
        terminal_rules.pop_back();
        
        unsigned long next_index = get_next_free_rule_index();
        
        unsigned long key = compute_rule_key(rule_index_1, rule_index_2);
        if (NTtoNT.find(key) == NTtoNT.end()) NTtoNT.insert(pair<unsigned long, set<unsigned long>*>(key, new set<unsigned long>());
        NTtoNT.at(key)->insert(next_index);
        terminal_rules.push_back(next_index);
    }
    return terminal_rules.at(0);
}



// expanding singleton rules, e.g. S -> A, A -> B, B -> C
vector<unsigned long> Parser::collect_one_backwards(unsigned long rule_index){
    vector<unsigned long> *collection = new vector<unsigned long>();
    collection->push_back(rule_index);
    int i = 0;
    while (i < collection.length()){
        unsigned long current_index = collection->at(i);
        if (NTtoNT.find(current_index) != NTtoNT.end()){
            for (auto previous_index : NTtoNT.at(current_index) collection.push_back(previous_index);
        }
        i += 1;
    }
    
    return collection;
}



vector<unsigned long>* collect_backwards(unsigned long child_rule_index, unsigned parent_rule_index){
    if (originalNTtoNT.find(child_rule_index) == originalNTtoNT.end()) return NULL;
    
    for (auto previous_index : self.originalNTtoNT.at(child_rule_index){
        if (previous_index == parent_rule_index) return []
        
        else if (originalNTtoNT.find(previous_index) != originalNTtoNT.end()){
            vector<set>* collection = collect_backwards(previous_index, parent_rule_index);
            if (collection != NULL){
                collection.push_back(previous_index);
                return collection;
            }
        }
    }
    return NULL;
}
    
    
    
    

void Parser::raise_events(TreeNode *node){
    if (node != NULL){
        node_rule_name = node->fire_event ?  NTtoRule.at(node.rule_index) : ""
        if (node->fire_event) parser_event_handler->handle_event(node_rule_name + "_pre_event", node);
        
        if (node->left != NULL) { // node.terminal is != None when node is leaf
            raise_events(node->left);
            if (node->right != NULL) raise_events(node->right);
        }
            
        if (node->fire_event) parser_event_handler->handle_event(node_rule_name + "_post_event", node);
    }
}





// filling the syntax tree including events
void fill_tree(TreeNode *node, DPNode *dp_node){
    // checking and extending nodes for single rule chains
    unsigned long key = dp_node->left != NULL ? compute_rule_key(dp_node->rule_index_1, dp_node->rule_index_2) : dp_node->rule_index_2;
    
    vector<unsigned long> *merged_rules = collect_backwards(key, node->rule_index);
    if (merged_rules != NULL){
        for (auto rule_index : merged_rules){
            node->left = new TreeNode(rule_index, NTtoRule.find(rule_index) != NTtoRule.end());
            node = node->left;
        }
    }
    delete merged_rules;
    
    if (dp_node->left != NULL) { // None => leaf
        node->left = TreeNode(dp_node->rule_index_1, NTtoRule.find(dp_node->rule_index_1) != NTtoRule.end());
        node.right = TreeNode(dp_node->rule_index_2, NTtoRule.find(dp_node->rule_index_2) != NTtoRule.end());
        fill_tree(node->left, dp_node->left);
        fill_tree(node->right, dp_node->right);
    }
    else:
        // I know, it is not 100% clean to store the character in an integer
        // especially when it is not the dedicated attribute for, but the heck with it!
        node->terminal = dp_node->rule_index_1;
}



// re-implementation of Cocke-Younger-Kasami algorithm
void Parser::parse(string text_to_parse){
    if (used_eof) text_to_parse += string(1, EOF_SIGN);
    
    parse_regular(text_to_parse);
}
    
    
    
    
void Parser::parse_regular(string text_to_parse){
    word_in_grammar = false;
    if (parse_tree != NULL) delete parse_tree;
    parse_tree = NULL;
    int n = text_to_parse.length();
    // dp stands for dynamic programming, nothing else
    dp_table = [None for x in range(n)]
    # Ks is a lookup, which fields in the dp_table are filled
    Ks = [None for x in range(n)]
    
    
    
    
    
    for i in range(n):
        dp_table[i] = [None for x in range(n - i)]
        Ks[i] = Bitfield(n - 1)
        for j in range(n - i): dp_table[i][j] = {}
    
    for i in range(n):
        c = text_to_parse[i]
        if c not in self.TtoNT: return
        
        for rule_index in sorted(self.TtoNT[c]):
            new_key = rule_index >> Parser.SHIFT
            old_key = rule_index & Parser.MASK
            dp_node = DPNode(c, old_key, None, None)
            dp_table[i][0][new_key] = dp_node
            Ks[i].set(0)
    
    for i in range (1, n):
        im1 = i - 1
        for j in range(n - i):
            D = dp_table[j]
            Di = D[i]
            jp1 = j + 1
            
            for k in Ks[j].get_bit_positions():
                if k >= i: break
                if Ks[jp1 + k].is_not_set(im1 - k): continue
            
            
            
                for index_pair_1 in D[k].items():
                    for index_pair_2 in dp_table[jp1 + k][im1 - k].items():
                        key = Parser.compute_rule_key(index_pair_1[0], index_pair_2[0])
                        
                        if key not in self.NTtoNT: continue
                        
                        content = DPNode(index_pair_1[0], index_pair_2[0], index_pair_1[1], index_pair_2[1])
                        Ks[j].set(i)
                        for rule_index in self.NTtoNT[key]:
                            Di[rule_index] = content
    
    for i in range(n - 1, 0, -1):
        if Parser.START_RULE in dp_table[0][i]:
            self.word_in_grammar = True
            self.parse_tree = TreeNode(Parser.START_RULE, Parser.START_RULE in self.NTtoRule)
            self.fill_tree(self.parse_tree, dp_table[0][i][Parser.START_RULE])
            self.raise_events(self.parse_tree)
            break
}