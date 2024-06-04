CXX = g++
CXXFLAGS = -O2 -Wall -Werror -fopenmp -flto -fmerge-all-constants -fdata-sections -ffunction-sections
LDFLAGS = -fopenmp -flto -ffunction-sections -fdata-sections -Wl,--gc-sections

# Use the number of available processors from nproc
NUM_PROCESSORS := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu)

# Set the default number of jobs to the number of available processors
MAKEFLAGS = -j$(NUM_PROCESSORS)

SRC_DIR = $(CURDIR)/src
OBJ_DIR = $(CURDIR)/obj
INSTALL_DIR = $(HOME)/.local/bin

SRC_FILES = bulk_rename++.cpp case_modes.cpp
OBJ_FILES = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

TARGET = bulk_rename++

strip: $(TARGET)
	strip $(TARGET) -o bulk_rename++

all: bulk_rename++

bulk_rename++: $(OBJ_FILES)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) bulk_rename++

.PHONY: clean

install: bulk_rename++
	install -m 755 bulk_rename++ $(INSTALL_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/bulk_rename++
