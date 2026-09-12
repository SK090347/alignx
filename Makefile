# Top-level build: C++ (CMake), Java (javac), Python (venv-friendly)
.PHONY: all cpp java python test clean fixtures

BUILD_DIR ?= cpp/build
CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -Icpp/include
JAVA_SRC := $(shell find java/src -name '*.java' 2>/dev/null)
JAVA_OUT := java/out
PY ?= python3

all: cpp java

cpp:
	cmake -S cpp -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) -j

# Plain g++ fallback without CMake
cpp-make:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c cpp/src/smith_waterman.cpp -o $(BUILD_DIR)/smith_waterman.o
	$(CXX) $(CXXFLAGS) cpp/src/main.cpp $(BUILD_DIR)/smith_waterman.o -o $(BUILD_DIR)/alignx

java:
	mkdir -p $(JAVA_OUT)
	javac -d $(JAVA_OUT) $(JAVA_SRC)

python:
	$(PY) -m pip install -e python -q || $(PY) -m pip install -e python --break-system-packages -q

test: all
	$(PY) -m pytest python/tests -v

clean:
	rm -rf $(BUILD_DIR) $(JAVA_OUT) python/**/__pycache__ .pytest_cache
	find . -name '*.pyc' -delete 2>/dev/null || true

# Convenience: run both engines on a fixture pair
demo: all
	@echo '=== C++ ==='
	./$(BUILD_DIR)/alignx --affine fixtures/query.fa fixtures/subject.fa
	@echo '=== Java ==='
	java -cp $(JAVA_OUT) com.alignx.Main --affine fixtures/query.fa fixtures/subject.fa
