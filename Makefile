CXX=g++
CXXFLAGS=-Iheaders
LDFLAGS=-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
SRCDIR=.
OBJDIR=bin/obj
BINDIR=.
SOURCES=SimpleCube.cpp
OBJECTS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))
EXECUTABLE=$(BINDIR)/bin/VulkanTest

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)