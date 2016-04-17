Z1_ROOT=.
Z1_BUILD=$(Z1_ROOT)/build
Z1_SRC=$(Z1_ROOT)/src
Z1_INCLUDE=$(Z1_ROOT)/include

VPATH=$(Z1_ROOT):$(Z1_BUILD):$(Z1_SRC):

LIB_INCLUDE=-I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
ALL_INCLUDES=$(LIB_INCLUDE) -I$(Z1_INCLUDE)
CFLAGS=$(ALL_INCLUDES) -O3 -std=gnu99
LDFLAGS= 
LDLIBS=-lglib-2.0 

# the project target exe
Z1_TARGET_EXE=z1.exe

# all test executables
Z1_TESTS_EXE=$(patsubst $(Z1_SRC)/%.c,%.exe,$(wildcard $(Z1_SRC)/*_test.c))

# all object files for the test exe (to remove them from the non-test object files list below)
Z1_TESTS_OBJ=$(patsubst $(Z1_SRC)/%.c,$(Z1_BUILD)/%.o,$(wildcard $(Z1_SRC)/*_test.c))

# all object files
Z1_OBJ=$(patsubst $(Z1_SRC)/%.c,$(Z1_BUILD)/%.o,$(wildcard $(Z1_SRC)/*.c))

# the list of non-test obj files, i.e. all the files that have to be build before any exe
# also excluded is z1.o, the final target object file (of course)
Z1_OBJ_NOTEST=$(filter-out $(Z1_TESTS_OBJ) $(Z1_BUILD)/z1.o, $(Z1_OBJ))

# all include files
Z1_H=$(wildcard $(Z1_INCLUDE)/*.h)

all: $(Z1_TARGET_EXE) tests 
	@echo "Z1_OBJ        :" $(Z1_OBJ)
	@echo "Z1_TESTS_OBJ  :" $(Z1_TESTS_OBJ)
	@echo "Z1_OBJ_NOTEST :" $(Z1_OBJ_NOTEST)

tests: $(Z1_TESTS_EXE)

$(Z1_BUILD)/%.o: $(Z1_SRC)/%.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

# each exe is dependant on its own obj file but also on all non-test object files
%.exe: $(Z1_BUILD)/%.o $(Z1_OBJ_NOTEST)
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@
    
$(Z1_OBJ): $(Z1_H) | $(Z1_BUILD)
    
$(Z1_BUILD):
	mkdir $(Z1_BUILD)
