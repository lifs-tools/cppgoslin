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


#ifndef BASE_PARSER_EVENT_HANDLER_H
#define BASE_PARSER_EVENT_HANDLER_H

#include <set>
#include <map>
#include <string>
#include <vector>
#include <functional>
#include "cppgoslin/domain/LipidExceptions.h"
#include "cppgoslin/domain/StringFunctions.h"
#include "cppgoslin/parser/Parser.h"

template<class T>
class Parser;

class TreeNode;

using namespace std;

template <class T>
class BaseParserEventHandler {
public:
    Parser<T>* parser;
    map<string, function<void(TreeNode *)>>* registered_events;
    set<string> rule_names;
    T content;
    string parse_string;
    vector<string> domain;
    
    BaseParserEventHandler();
    virtual ~BaseParserEventHandler();
    void sanity_check();
    void handle_event(string event_name, TreeNode *node);
    static bool endswith(const string &main_str, const string &to_match);
};

#include "cppgoslin/parser/BaseParserEventHandler_impl.h"
            
#endif /*  BASE_PARSER_EVENT_HANDLER_H */
            
