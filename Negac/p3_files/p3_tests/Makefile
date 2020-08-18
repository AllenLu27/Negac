TESTFILES := $(wildcard *.nega)
TESTS := $(TESTFILES:.nega=.test)

.PHONY: all

all: $(TESTS)

%.test:
	@rm -f $*.unparse $*.err
	@touch $*.unparse $*.err
	@echo "TEST $*"
	@../negac $*.nega -c 2> $*.err ;\
	PROG_EXIT_CODE=$$?;\
	if [ $$PROG_EXIT_CODE != 0 ]; then \
		echo "negac error:"; \
		cat $*.err; \
		exit 1; \
	fi; \
	diff -B --ignore-all-space $*.err $*.err.expected; \
	STDERR_DIFF_EXIT=$$?;\
	exit $$STDERR_DIFF_EXIT

clean:
	rm -f *.err *.unparse
