CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -fopenmp -flto -fmerge-all-constants -fdata-sections -ffunction-sections -fno-plt -fno-rtti
LDFLAGS = -fopenmp -flto -ffunction-sections -fdata-sections -fno-plt -Wl,--gc-sections -Wl,--strip-all -Wl,--as-needed -Wl,-z,relro -Wl,-z,now

# Use the number of available processors from nproc
NUM_PROCESSORS := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu)

# Set the default number of jobs to the number of available processors
MAKEFLAGS = -j$(NUM_PROCESSORS)

SRC_DIR = $(CURDIR)/src
OBJ_DIR = $(CURDIR)/obj
INSTALL_DIR = $(HOME)/.local/bin

SRC_FILES = bulk_rename++.cpp case_modes.cpp
OBJ_FILES = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

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
