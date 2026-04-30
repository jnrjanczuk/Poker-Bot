# Derived from EECS 281 Makefile

SHELL = /bin/bash

EXECUTABLE  = poker

PROJECTFILE = $(or $(wildcard project*.cpp $(EXECUTABLE).cpp), main.cpp)

# disable built-in rules
.SUFFIXES:

# designate which compiler to use
CXX         = g++

# rule for creating objects
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp

# list of test drivers (with main()) for development
TESTSOURCES = $(wildcard test*.cpp)
TESTSOURCES := $(filter-out $(PROJECTFILE),$(TESTSOURCES))

# list of sources used in project
SOURCES     = $(wildcard *.cpp)
SOURCES     := $(filter-out $(TESTSOURCES), $(SOURCES))
# list of objects used in project
OBJECTS     = $(SOURCES:%.cpp=%.o)

# Default Flags
CXXFLAGS = -std=c++17 -Wconversion -Wall -Werror -Wextra -pedantic

# make debug - will compile sources with $(CXXFLAGS) -g3 and -fsanitize
#              flags also defines DEBUG and _GLIBCXX_DEBUG
debug: CXXFLAGS += -g3 -DDEBUG -fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG
debug:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)_debug
.PHONY: debug

# make release - will compile sources with $(CXXFLAGS) and the -O3 flag also
#                defines NDEBUG so that asserts will not check
release: CXXFLAGS += -O3 -DNDEBUG
release: $(EXECUTABLE)
.PHONY: release

# make valgrind - will compile sources with $(CXXFLAGS) -g3 suitable for
#                 CAEN or WSL (DOES NOT WORK ON MACOS).
valgrind: CXXFLAGS += -g3
valgrind:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)_valgrind
.PHONY: valgrind

# make profile - will compile "all" with $(CXXFLAGS) and the -g3 flag
profile: CXXFLAGS += -g3
profile:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)_profile
.PHONY: profile

# make static - will perform static analysis in the matter currently used
#               on the autograder
static:
	cppcheck --enable=all --suppress=missingIncludeSystem \
      $(SOURCES) *.h *.hpp
.PHONY: static

# name of the tarballs created for submission
FULL_SUBMITFILE = fullsubmit.tar.gz
PARTIAL_SUBMITFILE = partialsubmit.tar.gz
UNGRADED_SUBMITFILE = ungraded.tar.gz

# make identifier - will check to ensure that all source code and header files
#                   include the project identifier, skip subdirectories
#                   also removes old submit tarballs, they are outdated
identifier: $(foreach tsrc,$(wildcard test*.cpp),$(eval NO_IDENTIFIER := $(NO_IDENTIFIER),$(tsrc)))
identifier:
	@if [ $$(grep --include=*.{h,hpp,c,cpp} --exclude={$(NO_IDENTIFIER)} --directories=skip -L $(IDENTIFIER) * | wc -l) -ne 0 ]; then \
		printf "Missing project identifier in file(s): "; \
		echo `grep --include=*.{h,hpp,c,cpp} --exclude={$(NO_IDENTIFIER)} --directories=skip -L $(IDENTIFIER) *`; \
		exit 1; \
	else \
		rm -f $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE) $(UNGRADED_SUBMITFILE); \
	fi
.PHONY: identifier

# Build all executables
all: debug release
all: profile valgrind
.PHONY: all

$(EXECUTABLE): $(OBJECTS)
ifeq ($(EXECUTABLE), $(MAKECMDGOALS))
	@echo $(EXECUTABLE) is an invalid target, use \'make\' or \'make help\' to find a valid target
	@exit 1
endif
ifneq ($(EXECUTABLE), executable)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXECUTABLE)
else
	@echo Edit EXECUTABLE variable in Makefile.
	@echo Using default a.out.
	$(CXX) $(CXXFLAGS) $(OBJECTS)
endif

# names of test executables
TESTS       = $(TESTSOURCES:%.cpp=%)
# Automatically generate any build rules for test*.cpp files
define make_tests
    ifeq ($$(PROJECTFILE),)
	    @echo Edit PROJECTFILE variable to .cpp file with main\(\)
	    @exit 1
    endif
    SRCS = $$(filter-out $$(PROJECTFILE), $$(SOURCES))
    OBJS = $$(SRCS:%.cpp=%.o)
    HDRS = $$(wildcard *.h *.hpp)
    $(1): CXXFLAGS += -g3 -DDEBUG
    $(1): $$(OBJS) $$(HDRS) $(1).cpp
	$$(CXX) $$(CXXFLAGS) $$(OBJS) $(1).cpp -o $(1)
endef
$(foreach test, $(TESTS), $(eval $(call make_tests, $(test))))

alltests: $(TESTS)
.PHONY: alltests

# make clean - remove .o files, executables, tarball
clean:
	rm -Rf *.dSYM
	rm -f $(OBJECTS) $(EXECUTABLE) $(EXECUTABLE)_debug
	rm -f $(EXECUTABLE)_valgrind $(EXECUTABLE)_profile $(TESTS) perf.data* \
      $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE) $(UNGRADED_SUBMITFILE)
.PHONY: clean

# Files that should not be included in a tarball
EXCLUDE_FILES = getopt.\?

# get a list of all files that might be included in a submit
# different submit types can do additional filtering to remove unwanted files
FULL_SUBMITFILES=$(filter-out $(wildcard test*.cpp), \
                   $(wildcard Makefile *.h *.hpp *.cpp test*.txt))

# make fullsubmit.tar.gz - cleans, creates tarball including test files
$(FULL_SUBMITFILE): $(FULL_SUBMITFILES)
	rm -f $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE) $(UNGRADED_SUBMITFILE)
	COPYFILE_DISABLE=true tar --exclude=$(EXCLUDE_FILES) -vczf $(FULL_SUBMITFILE) $(FULL_SUBMITFILES)
	@echo !!! Final submission prepared, test files included... READY FOR GRADING !!!

# make partialsubmit.tar.gz - cleans, creates tarball omitting test files
PARTIAL_SUBMITFILES=$(filter-out $(wildcard test*.txt), $(FULL_SUBMITFILES))
$(PARTIAL_SUBMITFILE): $(PARTIAL_SUBMITFILES)
	rm -f $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE) $(UNGRADED_SUBMITFILE)
	COPYFILE_DISABLE=true tar --exclude=$(EXCLUDE_FILES) -vczf $(PARTIAL_SUBMITFILE) \
      $(PARTIAL_SUBMITFILES)
	@echo !!! WARNING: No test files included. Use 'make fullsubmit' to include test files. !!!

# make ungraded.tar.gz - cleans, creates tarball omitting test files, Makefile
UNGRADED_SUBMITFILES=$(filter-out Makefile, $(PARTIAL_SUBMITFILES))
$(UNGRADED_SUBMITFILE): $(UNGRADED_SUBMITFILES)
	rm -f $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE) $(UNGRADED_SUBMITFILE)
	@touch __ungraded
	COPYFILE_DISABLE=true tar --exclude=$(EXCLUDE_FILES) -vczf $(UNGRADED_SUBMITFILE) \
      $(UNGRADED_SUBMITFILES) __ungraded
	@rm -f __ungraded
	@echo !!! WARNING: This submission will not be graded. !!!

# shortcut for make submit tarballs
fullsubmit: identifier $(FULL_SUBMITFILE)
partialsubmit: identifier $(PARTIAL_SUBMITFILE)
ungraded: identifier $(UNGRADED_SUBMITFILE)
.PHONY: fullsubmit partialsubmit ungraded

# REMOTE_PATH has default definition above
sync2caen:
ifeq ($(UNIQNAME), youruniqname)
	@echo Edit UNIQNAME variable in Makefile.
	@exit 1;
endif
	# Synchronize local files into target directory on CAEN
	rsync \
      -av \
      --delete \
      --exclude '*.o' \
      --exclude '$(EXECUTABLE)' \
      --exclude '$(EXECUTABLE)_debug' \
      --exclude '$(EXECUTABLE)_valgrind' \
      --exclude '$(EXECUTABLE)_profile' \
      --exclude '.git*' \
      --exclude '.vs*' \
      --exclude '*.code-workspace' \
      --filter=":- .gitignore" \
      "."/ \
      "$(UNIQNAME)@login-course.engin.umich.edu:$(REMOTE_PATH)/"
	echo "Files synced to CAEN at ~/$(REMOTE_PATH)/"
.PHONY: sync2caen