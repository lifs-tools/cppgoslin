ifeq ($(OS),Windows_NT)
    bin_grammar = ".\writeGrammarsHeader.exe"
	bin_enums = ".\writeLipidEnums.exe"
	bin = libcppGoslin.dll
	SHARED_FLAG = -shared
	BSTATIC_FLAG =
	RPATH_FLAG =
	LIBPATH_ENV =
else
    bin_grammar = "./writeGrammarsHeader"
	bin_enums = "./writeLipidEnums"
	UNAME_S := $(shell uname -s)
	UNAME_M := $(shell uname -m)
	ifeq ($(UNAME_S),Darwin)
		bin = libcppGoslin.dylib
		SHARED_FLAG = -dynamiclib
		BSTATIC_FLAG =
		RPATH_FLAG = -Wl,-rpath,.
		LIBPATH_ENV = DYLD_LIBRARY_PATH=.:$(DYLD_LIBRARY_PATH)
	else
		bin = libcppGoslin.so
		SHARED_FLAG = -shared
		BSTATIC_FLAG = -Bstatic
		RPATH_FLAG = -Wl,-rpath,.
		LIBPATH_ENV = LD_LIBRARY_PATH=.:$(LD_LIBRARY_PATH)
	endif
endif
install_dir = /usr
ifeq ($(origin CC),default)
CC = g++
endif
#CC = g++ -std=c++11
#CC = clang++-10
AR = ar
MARCH = -march=native
abin = libcppGoslin.a
domain = src/domain/Adduct.o src/domain/LipidMolecularSpecies.o src/domain/LipidSnPosition.o src/domain/LipidStructureDefined.o src/domain/FattyAcid.o src/domain/LipidAdduct.o src/domain/LipidSpecies.o src/domain/LipidFullStructure.o src/domain/LipidCompleteStructure.o src/domain/LipidSpeciesInfo.o src/domain/StringFunctions.o src/domain/LipidClasses.o src/domain/DoubleBonds.o src/domain/FunctionalGroup.o src/domain/Headgroup.o src/domain/Cycle.o src/domain/GenericDatastructures.o

parser = src/parser/ParserClasses.o src/parser/KnownParsers.o src/parser/GoslinParserEventHandler.o src/parser/LipidMapsParserEventHandler.o src/parser/SwissLipidsParserEventHandler.o src/parser/HmdbParserEventHandler.o src/parser/SumFormulaParserEventHandler.o src/parser/SumFormulaParser.o src/parser/ShorthandParserEventHandler.o src/parser/FattyAcidParserEventHandler.o src/parser/LipidBaseParserEventHandler.o

obj = ${domain} ${parser}

ifeq '' '${findstring clang++,${CC}}'
  # not clang
  flags = -fstack-protector-strong -fopenmp
else
  # clang
  flags = -fstack-protector-strong	
endif

ifeq ($(OS),Windows_NT)
  flags = -fopenmp
endif

# On macOS, g++ is an alias for Apple Clang which does not ship with OpenMP.
# Override any -fopenmp that the GCC branch may have set above.
ifeq ($(UNAME_S),Darwin)
  flags = -fstack-protector-strong
endif

opt = -std=c++17 -O3 ${flags} -D_FORTIFY_SOURCE=2
# -Wvla -Wall ${MARCH}

main: ${bin}

${bin}:	cppgoslin/parser/KnownGrammars.h src/domain/LipidClasses.cpp cppgoslin/domain/ClassesEnum.h ${obj}
	${CC} ${SHARED_FLAG} ${obj} -o ${bin}
	
	
static: cppgoslin/parser/KnownGrammars.h src/domain/LipidClasses.cpp ${obj}
	${AR} rcs ${abin} ${obj}

	
cppgoslin/parser/KnownGrammars.h: data/goslin/Goslin.g4 data/goslin/LipidMaps.g4 data/goslin/LipidMaps.g4 data/goslin/SwissLipids.g4 data/goslin/HMDB.g4
	${CC} ${opt} -I . -o writeGrammarsHeader writeGrammarsHeader.cpp && ${bin_grammar} "cppgoslin/parser/KnownGrammars.h"
	

cppgoslin/domain/ClassesEnum.h: src/domain/LipidClasses.cpp


src/domain/LipidClasses.cpp: data/goslin/lipid-list.csv cppgoslin/parser/KnownGrammars.h
	${CC} ${opt} -I . -o writeLipidEnums writeLipidEnums.cpp src/domain/StringFunctions.cpp src/parser/SumFormulaParserEventHandler.cpp src/parser/ParserClasses.cpp && ${bin_enums} "src/domain/LipidClasses.cpp"
	

	
src/domain/%.o: src/domain/%.cpp cppgoslin/parser/KnownGrammars.h src/domain/LipidClasses.cpp cppgoslin/domain/ClassesEnum.h
	${CC} ${opt} -I. -fPIC -o $@ -c $<
	
	
src/parser/%.o: src/parser/%.cpp cppgoslin/parser/KnownGrammars.h src/domain/LipidClasses.cpp cppgoslin/domain/ClassesEnum.h cppgoslin/parser/Parser_impl.h cppgoslin/parser/BaseParserEventHandler_impl.h
	${CC} ${opt} -I. -fPIC -o $@ -c $<
	
	
src/tests/%.o: src/tests/%.cpp ${bin}
	${CC} ${opt} -I. -fPIC -o $@ -c $<
	
clean-win:
	.\win-clean.bat
	
clean:
	rm -f "cppgoslin/parser/KnownGrammars.h"
	rm -f "src/domain/LipidClasses.cpp"
	rm -f "cppgoslin/domain/ClassesEnum.h"
	rm -f cppgoslin/domain/*.o
	rm -f cppgoslin/parser/*.o
	rm -f cppgoslin/tests/*.o
	rm -f ${bin}
	rm -f *Test
	rm -f writeGrammarsHeader
	rm -f writeLipidEnums
	rm -f ${abin}
	
dist-clean: clean
	rm -f ${install_dir}/lib/${bin}
	rm -rf ${install_dir}/include/cppgoslin


install: ${bin}
	mkdir -p ${install_dir}/lib
	mkdir -p ${install_dir}/include
	cp -r cppgoslin ${install_dir}/include
	cp ${bin} ${install_dir}/lib
	
	
ShorthandTest: src/tests/ShorthandTest.o
	${CC} -I. ${opt} ${BSTATIC_FLAG} ${RPATH_FLAG} -o ShorthandTest src/tests/ShorthandTest.o ${bin}

ParserTest: src/tests/ParserTest.o
	${CC} -I. ${opt} ${BSTATIC_FLAG} ${RPATH_FLAG} -o ParserTest src/tests/ParserTest.o ${bin}

SumFormulaTest: src/tests/SumFormulaTest.o
	${CC} -I. ${opt} ${BSTATIC_FLAG} ${RPATH_FLAG} -o SumFormulaTest src/tests/SumFormulaTest.o ${bin}

MassesTest: src/tests/MassesTest.o
	${CC} -I. ${opt} ${BSTATIC_FLAG} ${RPATH_FLAG} -o MassesTest src/tests/MassesTest.o ${bin}
	
LipidMapsTest: src/tests/LipidMapsTest.o
	${CC} -I. ${opt} ${BSTATIC_FLAG} ${RPATH_FLAG} -o LipidMapsTest src/tests/LipidMapsTest.o ${bin}
	
GoslinTest: src/tests/GoslinTest.o
	${CC} -I. ${opt} ${BSTATIC_FLAG} ${RPATH_FLAG} -o GoslinTest src/tests/GoslinTest.o ${bin}
	
SwissLipidsTest: src/tests/SwissLipidsTest.o
	${CC} -I. ${opt} ${BSTATIC_FLAG} ${RPATH_FLAG} -o SwissLipidsTest src/tests/SwissLipidsTest.o ${bin}
	
HmdbTest: src/tests/HmdbTest.o
	${CC} -I. ${opt} ${BSTATIC_FLAG} ${RPATH_FLAG} -o HmdbTest src/tests/HmdbTest.o ${bin}
	
FattyAcidsTest: src/tests/FattyAcidsTest.o
	${CC} -I. ${opt} ${BSTATIC_FLAG} ${RPATH_FLAG} -o FattyAcidsTest src/tests/FattyAcidsTest.o ${bin}
	
	
test: FattyAcidsTest ShorthandTest ParserTest SumFormulaTest MassesTest GoslinTest LipidMapsTest SwissLipidsTest HmdbTest

	
runtests: FattyAcidsTest ShorthandTest ParserTest SumFormulaTest MassesTest GoslinTest LipidMapsTest SwissLipidsTest HmdbTest
	${LIBPATH_ENV} ./FattyAcidsTest
	${LIBPATH_ENV} ./ShorthandTest
	${LIBPATH_ENV} ./ParserTest
	${LIBPATH_ENV} ./SumFormulaTest
	${LIBPATH_ENV} ./MassesTest
	${LIBPATH_ENV} ./GoslinTest
	${LIBPATH_ENV} ./LipidMapsTest
	${LIBPATH_ENV} ./SwissLipidsTest
	${LIBPATH_ENV} ./HmdbTest
