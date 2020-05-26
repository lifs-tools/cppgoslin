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


#ifndef STRINGFUNCTIONS_H
#define STRINGFUNCTIONS_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "cppgoslin/domain/LipidExceptions.h"


using namespace std;

static const char RULE_ASSIGNMENT = ':';
static const char RULE_SEPARATOR = '|';
static const char RULE_TERMINAL = ';';
static const char DEFAULT_QUOTE = '\'';
static const string IMPORT_TERM = "import";



enum Content {NoContext, InLineComment, InLongComment, InQuote};
enum MatchWords {NoMatch, LineCommentStart, LineCommentEnd, LongCommentStart, LongCommentEnd, Quote};


string replace_all(std::string str, const std::string& from, const std::string& to);
string strip(string s, char c);
vector<string>* split_string(string text, char separator, char _quote = DEFAULT_QUOTE);
vector<string>* extract_text_based_rules(string grammar, char _quote = DEFAULT_QUOTE);

#endif /* STRINGFUNCTIONS_H */
