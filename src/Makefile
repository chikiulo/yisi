# @file Makefile
# @brief Makefile for building YiSi
#
# @author Jackie Lo and Darlene Stewart
#
# Multilingual Text Processing / Traitement multilingue de textes
# Digital Technologies Research Centre / Centre de recherche en technologies numériques
# National Research Council Canada / Conseil national de recherches Canada
# Copyright 2018, Her Majesty in Right of Canada /
# Copyright 2018, Sa Majeste la Reine du Chef du Canada

# Override the value of MATEPLUS_HOME with a command line definition, or
# consider defining MATEPLUS_HOME in your .profile, for example:
#   export MATEPLUS_HOME=~/u/sandboxes/mateplus
MATEPLUS_HOME ?= ~/u/tools/MATE/mateplus-master/src

JAVA_HOME ?= /space/group/nrc_ict/pkgs/centos6/gcc-4.9.3/jdk1.8.0_131

MATEPLUS_PATH ?= $(MATEPLUS_HOME)/mateplus.jar

ifneq ("$(wildcard $(MATEPLUS_PATH))","")
   WITH_SRLMATE ?= True
else
   $(info *** mateplus.jar not found)
endif

ifneq (clean, $(MAKECMDGOALS))
ifneq (cleaner, $(MAKECMDGOALS))
ifdef WITH_SRLMATE
   $(info Building with SRLMATE...)
else
   $(info Building without SRLMATE...)
endif
endif
endif

CXXFLAGS += -Wall -pedantic -std=c++11 -g -O3
JFLAGS += -cp ${MATEPLUS_PATH}

PROG_NAMES := yisi
TEST_NAMES := cmdlp_test srlgraph_test maxmatching_test lexsim_test w2v_test biw2v_test \
	      lexweight_test phrasesim_test srl_test srlutil_test util_test \
	      yisiscorer_test testbin

ifdef WITH_SRLMATE
   TEST_NAMES += srlmate_test
endif

# List of binaries that need to be built
BIN_NAMES := $(PROG_NAMES) $(TEST_NAMES)

# List of all possible binaries (programs), including those that won't be built.
ALL_BIN_NAMES := $(BIN_NAMES) srlmate_test

SRC_OBJS := $(patsubst %.cpp,../obj/%.o,$(wildcard *.cpp))
CMDLP_OBJS := $(patsubst cmdlp/%.cpp,../obj/%.o,$(wildcard cmdlp/*.cpp))

# We compile/link SRLMATE objects a bit differently.
SRLMATE_OBJS := $(addprefix ../obj/,srlmate.o)
SRLMATE_BIN_OBJS := $(addprefix ../obj/,srlmate_test.o)
SRLMATE_BINS := $(addprefix ../bin/,srlmate_test)


ifdef WITH_SRLMATE
   SRLMATE_OBJS += $(addprefix ../obj/,srl.o)
   SRLMATE_BINS += $(addprefix ../bin/,srl_test yisiscorer_test yisi)
endif

# Object files are c++ sources that do not result in stand alone binaries
ALL_OBJECTS := $(filter-out $(addprefix ../obj/,$(ALL_BIN_NAMES:%=%.o)),$(SRC_OBJS) $(CMDLP_OBJS))

OBJECTS := $(filter-out $(SRLMATE_OBJS),$(ALL_OBJECTS))

#
# Targets
#

# Clear default suffix rules
.SUFFIXES:
# Keep dependencies between calls
.PRECIOUS: ../dep/%.d ../obj/%.o

.PHONY: all binaries scripts
all: binaries scripts
ifdef WITH_SRLMATE
all: ../obj/srlmate.jar
all: en.mplsconfig de.mplsconfig es.mplsconfig zh.mplsconfig
endif

binaries: $(BIN_NAMES:%=../bin/%)

YISIBIN_SUB := "s~^YISIBIN=/path/to/your/yisi/bin$$~YISIBIN=$(dir $(CURDIR))bin~"

.PHONY: scripts
scripts: | ../bin
	cp -p ../src/scripts/resolve_yisicmd.sh ../bin
	sed -e $(YISIBIN_SUB) scripts/run_yisi.sh > ../bin/run_yisi.sh
	chmod a+x ../bin/*.sh

../obj/srlmate.jar: *.java
	mkdir -pv ../obj/java
	${JAVA_HOME}/bin/javac $(JFLAGS) -d ../obj/java $^
	cd ../obj/java && jar -cvf ../srlmate.jar *

en.mplsconfig de.mplsconfig es.mplsconfig zh.mplsconfig: %: %.template
	sed -e "s#<YISI_HOME>/#$(dir $(CURDIR))#g; s#<MATEPLUS_HOME>#$(MATEPLUS_HOME)#g;" < $< > $@

$(SRLMATE_BINS): LDFLAGS += -L${JAVA_HOME}/jre/lib/amd64/server
$(SRLMATE_BINS): LIBRARIES += -ljvm
$(SRLMATE_OBJS) $(SRLMATE_BIN_OBJS): CXXFLAGS += -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -DWITH_SRLMATE

$(SRLMATE_BINS): $(SRLMATE_OBJS)
$(SRLMATE_BINS): OBJECTS += $(SRLMATE_OBJS)

../bin/%: ../obj/%.o $(OBJECTS) | ../bin
	$(CXX) $(LDFLAGS) $< $(OBJECTS) $(LIBRARIES) -o $@

$(SRC_OBJS): ../obj/%.o: %.cpp | ../obj ../dep

$(CMDLP_OBJS): ../obj/%.o: cmdlp/%.cpp | ../obj ../dep

ifdef WITH_SRLMATE
../obj/srl.o: ../obj/.STAMP.WITH_SRLMATE
else
../obj/srl.o: ../obj/.STAMP.WITHOUT_SRLMATE
endif

../obj/%.o:
	$(CXX) $(CXXFLAGS) -MM -MT '$@' $< > $(@:../obj/%.o=../dep/%.d)
	$(CXX) $(CXXFLAGS) -c $< -o $@

../obj/.STAMP.WITH_SRLMATE ../obj/.STAMP.WITHOUT_SRLMATE: | ../obj
	rm -rf ../obj/.STAMP.WITH*_SRLMATE
	touch $@

.PHONY: test
test:
	$(MAKE) -C ../test MATEPLUS_PATH=$(MATEPLUS_PATH)

../dep ../obj ../bin:
	mkdir -p $@

.PHONY: clean cleaner clean.mplsconfig
clean: clean.mplsconfig
	$(RM) -r ../bin
	$(RM) *~

clean.mplsconfig:
	$(RM) -f en.mplsconfig de.mplsconfig

clean.test:
	$(MAKE) -C ../test clean

cleaner: clean clean.test
	$(RM) -r ../dep ../obj

-include $(wildcard ../dep/*.d)
