CXX := g++
CXXFLAGS := -std=c++23 -O2 -Wall -Wextra -Wpedantic
LDLIBS :=

TARGET := koiinit
PREFIX ?= /usr/local

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(SOURCES:.cpp=.o)
DEPS := $(OBJECTS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(OBJECTS) $(DEPS) $(TARGET)

.PHONY: all install uninstall clean

-include $(DEPS)
