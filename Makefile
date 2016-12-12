COMPILER			= clang
#COMPILER			= gcc
CC				= $(COMPILER) -std=c11
WARNINGS	= -Wall -pedantic
#COMPILE		= -pipe -O3 -fPIC
DEBUG		 = -g -pg -DDEBUG -fgnu89-inline
CFLAGS  = $(TEMP) $(DEBUG) $(WARNINGS) $(COMPILE) $(PROC_OPT)
LDFLAGS  = -lm

.PHONY: all
all: exe doc utest

.PHONY: exe
exe: dates

.PHONY: doc
doc: dates.doc/dates.pdf

dates: dates.o datesTU.o
	$(CC) $(CFLAGS) -o "$@" $^ $(LDFLAGS)

dates.doc/dates.pdf: dates.c dates.h dates.doxygen
	doxygen dates.doxygen
	(cd dates.doc/latex ; make ; mv refman.pdf ../dates.pdf ; cd -)

utest: LDFLAGS += -lpthread
utest: LDFLAGS += -L/usr/lib/llvm-3.2/lib # -lprofile_rt
utest: dates_tu_check.o dates.o
	$(CC) $(CFLAGS) $(INCLUDES) -o "$@" $^ -lcheck $(LDFLAGS)
	CK_VERBOSITY=verbose ./utest
#	@LD_LIBRARY_PATH=/usr/lib/llvm-3.2/lib:${LD_LIBRARY_PATH} CK_VERBOSITY=verbose valgrind --leak-check=full --track-origins=yes --show-reachable=yes  --error-limit=no --gen-suppressions=all --log-file=utest_valgrind.log "./$@" || rm "./$@"
