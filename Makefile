BUILD_DIR=build

default:
	cmake --preset=default

vcpkg:
	cmake --preset=vcpkg

clean:
	rm -rf $(BUILD_DIR)/*
