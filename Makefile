CXX=g++
CC=gcc
CXXFLAGS=-Iheaders
CFLAGS=$(CXXFLAGS)
LDFLAGS=-lglfw -lvulkan
SRCDIR=.
OBJDIR=bin/obj
BINDIR=bin
SOURCES=BarChart.cpp CSVReader.cpp mercator.c
OBJECTS=$(patsubst %.c,$(OBJDIR)/%.o,$(filter %.c,$(SOURCES))) $(patsubst %.cpp,$(OBJDIR)/%.o,$(filter %.cpp,$(SOURCES)))
EXECUTABLE=$(BINDIR)/exec.out

all:
	make shaders & make executable

executable: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c
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

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(SHADERSPV)
