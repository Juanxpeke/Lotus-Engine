# Build folder
BUILD_DIR = build

# Targets

default:
	cmake --preset=default

vcpkg:
	cmake --preset=vcpkg

clean:
	rm -rf $(BUILD_DIR)/*
