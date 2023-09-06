BUILD := ./build

CXX := clang++
CXXFLAGS := $(CXXFLAGS) -std=c++20 -g -pedantic-errors -Wall -Wextra
CXXSRCS := $(wildcard src/*.cpp)
CXXOBJS := $(CXXSRCS:%.cpp=$(BUILD)/%.o)

all: lox++

ilox: $(CXXOBJS)
	$(CXX) $(LDFLAGS) $(LDLIBS) -o $@ $^

lox++: $(CXXOBJS)
	$(CXX) $(LDFLAGS) $(LDLIBS) -o $@ $^

$(BUILD)/%.o: %.cpp $(DEPS) | $(BUILD)
	@mkdir -p $(@D)
	$(CXX) -MMD $(CXXFLAGS) $(INCLUDE) -c $< -o $@

-include $(shell find . -path $(BUILD)/*/*.d)

$(BUILD):
	@mkdir -p $@

clean:
	rm -f ./lox++
	rm -rf ./build

.PHONY: all clean
