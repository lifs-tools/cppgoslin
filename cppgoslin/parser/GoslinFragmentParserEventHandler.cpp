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


#include "cppgoslin/parser/GoslinFragmentParserEventHandler.h"


#define reg(x, y) BaseParserEventHandler<LipidAdduct*>::registered_events->insert({x, bind(&GoslinFragmentParserEventHandler::y, this, placeholders::_1)})
    

GoslinFragmentParserEventHandler::GoslinFragmentParserEventHandler() : GoslinParserEventHandler() {
    fa_list = new vector<FattyAcid*>();
    
    registered_events->at("lipid_pre_event") = bind(&GoslinFragmentParserEventHandler::reset_lipid_fragment, this, placeholders::_1);
    registered_events->at("lipid_post_event") = bind(&GoslinFragmentParserEventHandler::build_lipid_fragment, this, placeholders::_1);
    reg("fragment_name_pre_event", add_fragment_name);
}


void GoslinFragmentParserEventHandler::reset_lipid_fragment(TreeNode *node) {
    reset_lipid(node);
    fragment = NULL;
}


void GoslinFragmentParserEventHandler::add_fragment_name(TreeNode *node) {
    fragment = new Fragment(node->get_text());
}


void GoslinFragmentParserEventHandler::build_lipid_fragment(TreeNode *node) {
    build_lipid(node);
    lipid->fragment = fragment;
    
}
    
