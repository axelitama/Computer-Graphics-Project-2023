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