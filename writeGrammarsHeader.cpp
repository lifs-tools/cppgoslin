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


#include <fstream>
#include <set>
#include <iostream>
#include <stdlib.h>
#include "cppgoslin/domain/StringFunctions.h"

using namespace std;
using namespace goslin;

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}



void addingGrammar(ofstream& offile, string grammarName, string grammarFilename){

    
    ifstream infile(grammarFilename.c_str());
    offile << "const string " + grammarName + " = \"";
    if (!infile.good()){
        cout << "Error: file '" + grammarFilename + "' not found." << endl;
        exit(-1);
    }
    
    string grammar((std::istreambuf_iterator<char>(infile)), (std::istreambuf_iterator<char>()));
    infile.close();
    vector<string> *rules = extract_text_based_rules(grammar, DEFAULT_QUOTE);
    set<string> imported;
    int i = 0;
    while (i < (int)rules->size()){
        string rule = rules->at(i);
        
        // checking for import
        vector<string>* tokens = split_string(rule, ' ', DEFAULT_QUOTE);
        if (tokens->size() == 2 && tokens->at(0) == IMPORT_TERM){
            
            string import_grammar = strip(tokens->at(1), ' ');
            if (imported.find(import_grammar) != imported.end()){
                cout << "Error: grammar '" << import_grammar << "' is already imported";
                exit(-1);
            }
            
            char sep = '/';
            #ifdef _WIN32
            sep = '\\';
            #endif
            
            size_t i = grammarFilename.rfind(sep, grammarFilename.length());
            string dir_path = "";
            if (i != string::npos) {
                dir_path = grammarFilename.substr(0, i);
            }
            string import_file = dir_path + sep + import_grammar + ".g4";
            
            ifstream infile_import(import_file);
            if (!infile.good()){
                cout << "Error: import file '" + import_grammar + "' not found in path '" + import_file + "'." << endl;
                exit(-1);
            }
            string grammar_import((std::istreambuf_iterator<char>(infile_import)), (std::istreambuf_iterator<char>()));
            infile_import.close();
            vector<string> *rules_import = extract_text_based_rules(grammar_import, DEFAULT_QUOTE);
            
            int j = 1;
            while (j < (int)rules_import->size()){
                rules->push_back(rules_import->at(j++));
            }
            
            delete rules_import;        
                    
        }
        else {
            replaceAll(rule, "\\", "\\\\");
            replaceAll(rule, "\"", "\\\"");
            rule += "; \\n\\";
            offile << rule << endl;
        }
        delete tokens;
        ++i;
    }
    delete rules;
    offile << "\";" << endl;
}



void writeGrammarHeader(string ofFileName){
    ofstream offile(ofFileName.c_str());
    
    offile << "/*" << endl;
    offile << "MIT License" << endl;
    offile << endl;
    offile << "Copyright (c) 2020 Dominik Kopczynski   -   dominik.kopczynski {at} isas.de" << endl;
    offile << "                   Nils Hoffmann  -  nils.hoffmann {at} isas.de" << endl;
    offile << endl;
    offile << "Permission is hereby granted, free of charge, to any person obtaining a copy" << endl;
    offile << "of this software and associated documentation files (the \"Software\"), to deal" << endl;
    offile << "in the Software without restriction, including without limitation the rights" << endl;
    offile << "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell" << endl;
    offile << "copies of the Software, and to permit persons to whom the Software is" << endl;
    offile << "furnished to do so, subject to the following conditions:" << endl;
    offile << endl;
    offile << "The above copyright notice and this permission notice shall be included in all" << endl;
    offile << "copies or substantial portions of the Software." << endl;
    offile << endl;
    offile << "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR" << endl;
    offile << "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY," << endl;
    offile << "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE" << endl;
    offile << "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER" << endl;
    offile << "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM," << endl;
    offile << "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE" << endl;
    offile << "SOFTWARE." << endl;
    offile << "*/" << endl;
    offile << endl;
    offile << endl;
    offile << endl;
    
    offile << "#ifndef KNOWN_GRAMMARS_H" << endl;
    offile << "#define KNOWN_GRAMMARS_H" << endl << endl;

    offile << "#include <string>" << endl;
    //offile << "#include \"cppgoslin/parser/Parser.h\"" << endl;

    offile << "using namespace std;" << endl;

    
    addingGrammar(offile, "goslin_grammar", "data/goslin/Goslin.g4");
    offile << endl << endl << endl;
    
    addingGrammar(offile, "goslin_fragment_grammar", "data/goslin/GoslinFragments.g4");
    offile << endl << endl << endl;
    
    addingGrammar(offile, "lipid_maps_grammar", "data/goslin/LipidMaps.g4");
    offile << endl << endl << endl;
    
    addingGrammar(offile, "swiss_lipids_grammar", "data/goslin/SwissLipids.g4");
    offile << endl << endl << endl;
    
    addingGrammar(offile, "hmdb_grammar", "data/goslin/HMDB.g4");
    offile << endl << endl << endl;
    
    addingGrammar(offile, "sum_formula_grammar", "data/goslin/SumFormula.g4");
    offile << endl << endl << endl;
    
    offile << "#endif /* KNOWN_GRAMMARS_H */" << endl;
}







int main(int argc, char** argv){
    if (argc < 2){
        cout << "Error, specify grammar output filename." << endl;
        exit(-1);
    }

    writeGrammarHeader(argv[1]);
    return 0;
}
