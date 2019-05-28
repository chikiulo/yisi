# ========================================================================
# Makefile for the command-line option library, which was cloned from:
# https://github.com/masaers/cmdlp (v0.4.1 tag)
#
# Copyright (c) 2018 Markus S. Saers
# ========================================================================
# Makefile
#
# Authors: Markus Saers <masaers@gmail.com>
# ========================================================================
#
# Settings
#

CXXFLAGS+=-Wall -pedantic -std=c++11 -g -O3 -Ibuild/include
LDFLAGS=

PROG_NAMES=
TEST_NAMES=cmdlp_test


#
# Derived settings
#

# List of binaries that needs to be built
BIN_NAMES=$(PROG_NAMES) $(TEST_NAMES)

# Object files are c++ sources that do not result in stand alone binaries
OBJECTS=$(patsubst %.cpp,build/obj/%.o,$(filter-out $(BIN_NAMES:%=%.cpp),$(wildcard *.cpp)))

GENERATED_HEADERS=build/include/magic_enum.h

#
# Targets
#

# Clear default suffix rules
.SUFFIXES :

.PRECIOUS : build/dep/%.d build/obj/%.o

all : binaries build/lib/libcmdlp.a build/lib/libcmdlp.so

binaries : $(GENERATED_HEADERS) $(BIN_NAMES:%=build/bin/%)

test : $(TEST_NAMES:%=build/test/%.out)
	@if [ -s build/test/.ERROR ]; then \
	     ( cat build/test/.ERROR; rm build/test/.ERROR ) \
	else echo "\n[ALL TESTS PASSED]\n"; \
	fi

build/lib/libcmdlp.a : $(OBJECTS)
	@mkdir -pv ${@D}
	$(AR) ruv $@ $^

build/lib/libcmdlp.so : $(OBJECTS)
	@mkdir -pv ${@D}
	$(CXX) -shared -fPIC $(LDFLAGS) -o $@ $^

build/bin/% : build/obj/%.o $(OBJECTS)
	@mkdir -pv ${@D}
	$(CXX) $(LDFLAGS) $< $(OBJECTS) -o $@

build/obj/%.o : %.cpp
	@mkdir -pv ${@D}
	@mkdir -pv $(dir $(@:build/obj/%.o=build/dep/%.d))
	$(CXX) $(CXXFLAGS) -MM -MT '$@' $< > $(@:build/obj/%.o=build/dep/%.d)
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

build/include/%.h : data/%.xml
	@mkdir -pv ${@D}
	xsltproc easenum.xslt $< > $@

build/test/%.out : build/bin/%
	@mkdir -pv ${@D}
	@if [ -e $@ ]; then \
	( cp $@ $@.old; \
          $< &> $@; \
	  diff $@ $@.old >> build/test/.ERROR \
	  || echo "REGRESSION TEST FAILED: $<" >> build/test/.ERROR \
	  ; \
	  rm $@.old ) \
	else \
	( $< &> $@; \
          echo "WARNING: No regression test: $<" >> build/test/.ERROR ) \
	fi

clean :
	@rm -rf build/dep build/obj build/bin build/lib
	@rm -f *~

cleaner : clean
	@rm -rf build

-include $(wildcard build/dep/*.d)

