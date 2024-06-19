# None of these are files related to the build, treat them as targets
.PHONY: all clean vcpkg

# Build folder
BUILD_DIR = build

# Targets

all:
	cmake --preset=default

vcpkg:
	cmake --preset=vcpkg

clean:
	rm -rf $(BUILD_DIR)/*
