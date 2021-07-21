/*
MIT License

Copyright (c) the authors (listed in global LICENCE file)

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


#include "DoubleBonds.h"

DoubleBonds::DoubleBonds(int num){
    num_double_bonds = num;
}


DoubleBonds* DoubleBonds::copy(){
    DoubleBonds* db = new DoubleBonds(num_double_bonds);
    for (auto &kv : double_bond_positions){
        db->double_bond_positions.insert({kv.first, kv.second});
    }
    return db;
}


int DoubleBonds::get_num(){
    if (double_bond_positions.size() > 0 && (int)double_bond_positions.size() != num_double_bonds)
        throw ConstraintViolationException("Number of double bonds does not match to number of double bond positions");
        
    return num_double_bonds;
}

