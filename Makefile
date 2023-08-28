CXX=g++
CC=gcc
CXXFLAGS=-Iheaders
CFLAGS=
LDFLAGS=-lglfw -lvulkan
SRCDIR=.
BINDIR=bin
OBJDIR=$(BINDIR)/obj
DEPDIR=$(BINDIR)/dependencies
SOURCES=main.cpp CSVReader.cpp mercator.c
OBJECTS=$(patsubst %.c,$(OBJDIR)/%.o,$(filter %.c,$(SOURCES))) $(patsubst %.cpp,$(OBJDIR)/%.o,$(filter %.cpp,$(SOURCES)))
DEPENDENCIES=$(patsubst %.c,$(DEPDIR)/%.d,$(filter %.c,$(SOURCES))) $(patsubst %.cpp,$(DEPDIR)/%.d,$(filter %.cpp,$(SOURCES)))
EXECUTABLE=$(BINDIR)/exec.out

all: shaders dependencies executable

executable: $(EXECUTABLE)
	rm -f $(DEPENDENCIES)

$(EXECUTABLE): $(OBJECTS) | dependencies
	mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(LDFLAGS)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),shaders)
-include $(DEPENDENCIES)
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

dependencies: $(DEPENDENCIES)

$(DEPDIR)/%.d: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MM -MP -MT $(OBJDIR)/$(notdir $*).o $< -MF $@

$(DEPDIR)/%.d: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MM -MP -MT $(OBJDIR)/$(notdir $*).o $< -MF $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(SHADERSPV) $(DEPENDENCIES)

.PHONY: executable dependencies shaders clean
