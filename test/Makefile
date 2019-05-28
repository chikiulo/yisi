#!/usr/bin/make

# @file Makefile
# @brief Makefile for the YiSi unit test suite
#
# @author Darlene Stewart
#
# Multilingual Text Processing / Traitement multilingue de textes
# Digital Technologies Research Centre / Centre de recherche en technologies numériques
# National Research Council Canada / Conseil national de recherches Canada
# Copyright 2018, Her Majesty in Right of Canada /
# Copyright 2018, Sa Majeste la Reine du Chef du Canada

SHELL:=/bin/bash

.NOTPARALLEL:
.SECONDARY:

# Override the value of MATEPLUS_HOME with a command line definition, or
# consider defining MATEPLUS_HOME in your .profile, for example:
#   export MATEPLUS_HOME=~/u/sandboxes/mateplus
MATEPLUS_HOME ?= ~/u/tools/MATE/mateplus-master/src

MATEPLUS_PATH ?= $(MATEPLUS_HOME)/mateplus.jar

ifneq ("$(wildcard $(MATEPLUS_PATH))","")
   WITH_SRLMATE ?= True
else
   $(info *** mateplus.jar not found)
endif

.PHONY: all
# "make all" should create the .gitignore file
all: gitignore
	@echo All tests PASSED.

test: all

CMDLP_TEST_PROGS := cmdlp_test.1 cmdlp_test.2
SIMPLE_TEST_PROGS := util_test
SIMPLE_TEST_PROGS += phrasesim_test
SIMPLE_TEST_PROGS += lexweight_test
SIMPLE_TEST_PROGS += lexsim_test
SIMPLE_TEST_PROGS += biw2v_test
SIMPLE_TEST_PROGS += w2v_test
SIMPLE_TEST_PROGS += maxmatching_test
SIMPLE_TEST_PROGS += srlutil_test
SIMPLE_TEST_PROGS += srlgraph_test
SIMPLE_TEST_PROGS += yisiscorer_test

ifdef WITH_SRLMATE
   SIMPLE2_TEST_PROGS += srl_test
   SIMPLE2_TEST_PROGS += srlmate_test
endif

ALL_SIMPLE_TEST_PROGS := $(CMDLP_TEST_PROGS) $(SIMPLE_TEST_PROGS) $(SIMPLE2_TEST_PROGS)

all: $(ALL_SIMPLE_TEST_PROGS)
all: test_yisi

TMP_FILES += $(addsuffix .out, $(ALL_SIMPLE_TEST_PROGS))
TMP_FILES += test_yisi_[0-2]*.out test_hyp.docyisi[0-2]* test_hyp.sntyisi[0-2]*
TMP_FILES += test_ref.en.srl

compare.%: %
	diff $< ref/$< -q

SUB:="s/^[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*  */00.00.000 /"

compare2.%: %
	diff <(cat $< | sed -e $(SUB)) <(cat ref/$< | sed -e $(SUB)) -q

.PHONY: $(ALL_SIMPLE_TEST_PROGS)
$(CMDLP_TEST_PROGS): %: compare.%.out
$(SIMPLE_TEST_PROGS): %: compare.%.out
$(SIMPLE2_TEST_PROGS): %: compare2.%.out

cmdlp_test.1.out: ARGS = --beta 0 --settings <(echo hello) --refs test_ref.en --help
cmdlp_test.2.out: ARGS = --beta 0 --settings <(echo hello) --refs /etc/timezone
util_test.out: ARGS =
phrasesim_test.out: ARGS = --lexsim-type w2v --outlexsim-path mini.d300.en \
   --reflexweight-type learn --reflexweight-path test_ref.en --phrasesim-type nwpr --ngram-size 3
lexweight_test.out: ARGS =
lexsim_test.out: ARGS = w2v mini.d300.en EU UK
biw2v_test.out: ARGS = mini.d300.de mini.d300.en . .
w2v_test.out: ARGS = mini.d300.en EU UK
maxmatching_test.out: ARGS =
srlutil_test.out: ARGS = test_ref.en.conll09
srlgraph_test.out: ARGS = test_ref.en test_ref.en.assert
yisiscorer_test.out: ARGS = --lexsim-type w2v --outlexsim-path mini.d300.en \
   --reflexweight-type learn --reflexweight-path test_ref.en --phrasesim-type nwpr --ngram-size 3
srl_test.out: ARGS = mate ../src/en.mplsconfig test_ref.en test_ref.en.srl
srlmate_test.out: ARGS = ../src/en.mplsconfig <<<'Hello there'

$(CMDLP_TEST_PROGS): BIN = ../src/cmdlp/build/bin
$(SIMPLE_TEST_PROGS) $(SIMPLE2_TEST_PROGS): BIN = ../bin

compare2.srl_test.out: compare.test_ref.en.srl

test_ref.en.srl: srl_test.out ;

$(addsuffix .out, $(ALL_SIMPLE_TEST_PROGS)): %.out:
	$(BIN)/$(firstword $(subst ., ,$*)) $(ARGS) &> $@

YSFX_NOSRL := 0 1 2
#YSFX_SRL := 1_srl 2_srl
YSFX_SRL := 1_srl

.PHONY: test_yisi
.PHONY: $(foreach n,$(YSFX_NOSRL),test_yisi_$n)
test_yisi: $(foreach n,$(YSFX_NOSRL),test_yisi_$n)

$(foreach n,$(YSFX_NOSRL),test_yisi_$n): test_yisi_%: compare.test_yisi_%.out
$(foreach n,$(YSFX_NOSRL),compare.test_yisi_$n.out): compare.test_yisi_%.out: compare.test_hyp.docyisi% compare.test_hyp.sntyisi%

.PHONY: $(foreach n,$(YSFX_SRL),test_yisi_$n)
ifdef WITH_SRLMATE
test_yisi: $(foreach n,$(YSFX_SRL),test_yisi_$n)
endif

$(foreach n,$(YSFX_SRL),test_yisi_$n): test_yisi_%: compare2.test_yisi_%.out
$(foreach n,$(YSFX_SRL),compare2.test_yisi_$n.out): compare2.test_yisi_%.out: compare.test_hyp.docyisi% compare.test_hyp.sntyisi%

test_yisi_%.out: yisi-%.config
	../bin/yisi --config $< &> $@

test_hyp.docyisi% test_hyp.sntyisi%: test_yisi_%.out
	:

########################################
.PHONY: gitignore
gitignore: 
	echo "$(TMP_FILES) $(TMP_DIRS) core core.* .gitignore !src/* !ref/* !in/* !data/*" | tr ' ' '\n' > .gitignore

.PHONY: clean clean.tmp_files
clean: clean.tmp_files
clean.tmp_files:
	$(RM) $(TMP_FILES) core core.* .gitignore

ifdef TMP_DIRS
.PHONY: clean.tmp_dirs
clean: clean.tmp_dirs
clean.tmp_dirs:
	$(RM) -r $(TMP_DIRS)
endif
