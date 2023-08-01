CXX=g++
CXXFLAGS=-Iheaders
LDFLAGS=-lglfw -lvulkan
SRCDIR=.
OBJDIR=bin/obj
BINDIR=bin
SOURCES=SimpleCube.cpp
OBJECTS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))
EXECUTABLE=$(BINDIR)/exec.out

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)


SHADERSDIR=shaders
SHADERS=$(wildcard $(SHADERSDIR)/*.vert) $(wildcard $(SHADERSDIR)/*.frag)
SHADEROUT=$(patsubst $(SHADERSDIR)/%.vert, $(SHADERSDIR)/%Vert.spv, $(SHADERS)) $(patsubst $(SHADERSDIR)/%.frag, $(SHADERSDIR)/%Frag.spv, $(SHADERS))

shaders: $(SHADEROUT)

$(SHADERSDIR)/%Vert.spv: $(SHADERSDIR)/%.vert
	glslc $< -o $@

$(SHADERSDIR)/%Frag.spv: $(SHADERSDIR)/%.frag
	glslc $< -o $@
