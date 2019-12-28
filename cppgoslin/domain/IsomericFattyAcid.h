#ifndef ISOMERIC_FATTY_ACID_H
#define ISOMERIC_FATTY_ACID_H

#include "cppgoslin/domain/StructuralFattyAcid.h"
#include <string>
#include "cppgoslin/domain/LipidEnums.h"
#include <map>

using namespace std;

class IsomericFattyAcid : public StructuralFattyAcid {
public:
    map<int, string>* double_bond_positions;
    
    IsomericFattyAcid(string name, int num_carbon, int num_double_bonds, int num_hydroxyl, LipidFaBondType lipid_FA_bond_type, bool lcb, map<int, string> *_double_bond_positions, int position);
    
    ~IsomericFattyAcid();
};

#endif /* ISOMERIC_FATTY_ACID_H */