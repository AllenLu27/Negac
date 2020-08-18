TESTFILES := $(wildcard *.nega)
TESTS := $(TESTFILES:.nega=.test)

.PHONY: all

all: $(TESTS)

%.test:
	@rm -f $*.output
	@touch $*.output
	@echo "TEST $*"
	@../negac $*.nega -a $*.output;\
	PROG_EXIT_CODE=$$?;\
	if [ $$PROG_EXIT_CODE != 0 ]; then \
		echo "negac error:"; \
		cat $*.err; \
		exit 1; \
	fi; \
	diff -B --ignore-all-space $*.output $*.expected;\
	STDERR_DIFF_EXIT=$$?;\
	exit $$STDERR_DIFF_EXIT

clean:
	rm -f *.output



