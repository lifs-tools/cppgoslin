#ifndef LIPID_ADDUCT_Y
#define LIPID_ADDUCT_Y

#include "MolecularFattyAcid.h"
#include <string>
#include "LipidExceptions.h"
#include "LipidEnums.h"
#include <sstream>

using namespace std;

class LipidMolecularSubspecies : LipidSpecies {
public:
    map<string, FattyAcid*> fa;
    vector<FattyAcid*> fa_list;

    LipidMolecularSubspecies (string head_group, vector<FattyAcid*> *_fa = NULL);
    ~LipidMolecularSubspecies();
    string build_lipid_subspecies_name(string fa_separator);
    string get_lipid_string(LipidLevel level = UNDEFINED_LEVEL);
};

#endif /* LIPID_ADDUCT_Y */