CXX=g++
CC=gcc
CXXFLAGS=-Iheaders
CFLAGS=
LDFLAGS=-lglfw -LGL -lvulkan -lGL -lGLU
SRCDIR=.
BINDIR=bin
OBJDIR=$(BINDIR)/obj
DEPDIR=$(BINDIR)/dependencies
SOURCES=main.cpp menu.cpp legend.cpp CSVReader.cpp mercator.c
OBJECTS=$(patsubst %.c,$(OBJDIR)/%.o,$(filter %.c,$(SOURCES))) $(patsubst %.cpp,$(OBJDIR)/%.o,$(filter %.cpp,$(SOURCES)))
DEPENDENCIES=$(patsubst %.c,$(DEPDIR)/%.d,$(filter %.c,$(SOURCES))) $(patsubst %.cpp,$(DEPDIR)/%.d,$(filter %.cpp,$(SOURCES)))
LIBSOBJ=$(patsubst %.cpp,$(OBJDIR)/%.o,$(shell find headers -name '*.cpp')) $(patsubst %.c,$(OBJDIR)/%.o,$(shell find headers -name '*.c'))
EXECUTABLE=$(BINDIR)/exec.out

all: shaders executable

# NOTE: dependencies are not recomputed when a header file is modified. Consequently, if a new header is included within another header,
#       it's necessary to execute the "make clean" command before compiling in order to ensure accurate dependency tracking.
# NOTE: 'clean' do not delete libraries object files. Use 'clean_all' to delete all the BINDIR directory. 

dependencies: $(DEPENDENCIES)

$(DEPDIR)/%.d: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MM -MP -MT $(OBJDIR)/$(notdir $*).o $< -MF $@

$(DEPDIR)/%.d: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MM -MP -MT $(OBJDIR)/$(notdir $*).o $< -MF $@

executable: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(LIBSOBJ) | dependencies
	mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(LDFLAGS)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clean_all)
ifneq ($(MAKECMDGOALS),shaders)
-include $(DEPENDENCIES)
endif
endif
endif

$(OBJDIR)/%.o: %.cpp | dependencies
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c | dependencies
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


SHADERSDIR=shaders
SHADERS=$(wildcard $(SHADERSDIR)/*.vert) $(wildcard $(SHADERSDIR)/*.frag)
SHADEROUT=$(patsubst $(SHADERSDIR)/%.vert, $(SHADERSDIR)/%Vert.spv, $(SHADERS)) $(patsubst $(SHADERSDIR)/%.frag, $(SHADERSDIR)/%Frag.spv, $(SHADERS))
SHADERSPV=$(wildcard $(SHADERSDIR)/*.spv)

shaders: $(SHADEROUT)

$(SHADERSDIR)/%Vert.spv: $(SHADERSDIR)/%.vert
	glslc $< -o $@

$(SHADERSDIR)/%Frag.spv: $(SHADERSDIR)/%.frag
	glslc $< -o $@

clean: # do not clean libsobj
	rm -f $(OBJECTS) $(EXECUTABLE) $(SHADERSPV) $(DEPENDENCIES)

clean_all:
	rm -rf $(BINDIR)

.PHONY: executable dependencies shaders clean clean_all
