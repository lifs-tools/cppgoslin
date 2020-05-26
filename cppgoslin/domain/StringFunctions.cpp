/*
MIT License

Copyright (c) 2020 Dominik Kopczynski   -   dominik.kopczynski {at} isas.de
                   Nils Hoffmann  -  nils.hoffmann {at} isas.de

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "cppgoslin/domain/StringFunctions.h"

string strip(string s, char c){
    if (s.length() > 0) {
        uint32_t st = 0;
        while (st < s.length() - 1 && s[st] == c) ++st;
        s = s.substr(st, s.length() - st);
    }
    
    if (s.length() > 0) {
        uint32_t en = 0;
        while (en < s.length() - 1 && s[s.length() - 1 - en] == c) ++en;
        s = s.substr(0, s.length() - en);
    }
    return s;
}



vector<string>* split_string(string text, char separator, char _quote){
    bool in_quote = false;
    vector<string> *tokens = new vector<string>();
    stringstream sb;
    char last_char = '\0';
    bool last_escaped_backslash = false;
    
    for (uint32_t i = 0; i < text.length(); ++i){
        char c = text[i];
        bool escaped_backslash = false;
        if (!in_quote){
            if (c == separator){
                string sb_string;
                sb_string = sb.str();
                if (sb_string.length() > 0) tokens->push_back(sb_string);
                sb.str("");
            }
            else{
                if (c == _quote) in_quote = !in_quote;
                sb << c;
            }
        }
        else {
            if (c == '\\' && last_char == '\\' && !last_escaped_backslash){
                escaped_backslash = true;
            }
            else if (c == _quote && !(last_char == '\\' && !last_escaped_backslash)){
                in_quote = !in_quote;
            }
            sb << c;
        }
        
        last_escaped_backslash = escaped_backslash;
        last_char = c;
    }
    
    string sb_string;
    sb_string = sb.str();
    
    if (sb_string.length() > 0){
        tokens->push_back(sb_string);
    }
    if (in_quote){
        delete tokens;
        throw RuntimeException("Error: corrupted token in grammar");
    }
    return tokens;
}


string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}




#include <iostream>
vector<string>* extract_text_based_rules(string grammar, char _quote){
    vector<string> *rules = NULL;
    int grammar_length = grammar.length();
    
    /*
    deleting comments to prepare for splitting the grammar in rules.
    Therefore, we have to consider three different contexts, namely
    within a quote, within a line comment, within a long comment.
    As long as we are in one context, key words for starting / ending
    the other contexts have to be ignored.
    */
    stringstream sb;
    Content current_context = NoContext;
    int current_position = 0;
    int last_escaped_backslash = -1;
    
    for (int i = 0; i < grammar_length - 1; ++i){
        MatchWords match = NoMatch;
        
        if (i > 0 && grammar[i] == '\\' && grammar[i - 1] == '\\' && last_escaped_backslash != i - 1){
            last_escaped_backslash = i;
            continue;
        }
        
        if (grammar[i] == '/' && grammar[i + 1] == '/') match = LineCommentStart;
        else if (grammar[i] == '\n') match = LineCommentEnd;
        else if (grammar[i] == '/' && grammar[i + 1] == '*') match = LongCommentStart;
        else if (grammar[i] == '*' && grammar[i + 1] == '/') match = LongCommentEnd;
        else if (grammar[i] == _quote &&  !(i >= 1 && grammar[i - 1] == '\\' && i - 1 != last_escaped_backslash)) match = Quote;
        
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
                            
                        default:
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
        throw RuntimeException("Error: corrupted grammar, ends either in comment or quote");
    }
    
    grammar = sb.str();
    grammar = replace_all(grammar, "\r\n", "");
    grammar = replace_all(grammar, "\n", "");
    grammar = replace_all(grammar, "\r", "");
    grammar = strip(grammar, ' ');
    
    
    if (grammar[grammar.length() - 1] != RULE_TERMINAL){
        throw RuntimeException("Error: corrupted grammar, last rule has no termininating sign, was: '" + string(1, grammar[grammar.length() - 1]) + "'");
    }
    
    rules = split_string(grammar, RULE_TERMINAL, _quote);
    
    if (rules->size() < 1){
        throw RuntimeException("Error: corrupted grammar, grammar is empty");
    }
    vector<string> *grammar_name_rule = split_string(rules->at(0), ' ', _quote);
    
    if (grammar_name_rule->size() > 0 && grammar_name_rule->at(0) != "grammar"){
        delete grammar_name_rule;
        throw RuntimeException("Error: first rule must start with the keyword 'grammar'");
    }
    
    
    else if (grammar_name_rule->size() != 2){
        delete grammar_name_rule;
        throw RuntimeException("Error: incorrect first rule");
    }
    
    delete grammar_name_rule;
    return rules;
}

