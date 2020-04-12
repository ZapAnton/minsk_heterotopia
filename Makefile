BUILD_DIRECTORY = bin
EXECUTABLE_NAME = minsk_heterotopia
BUILD_PATH = $(BUILD_DIRECTORY)/$(EXECUTABLE_NAME)

.PHONY: release
release:
	g++ -Wall -Werror -Wpedantic *.h *.cpp -O3 -o $(BUILD_PATH)

.PHONY: run
run:
	$(BUILD_PATH)

.PHONY: format
format:
	clang-format -i *.cpp *.h

.PHONY: clean
clean:
	rm $(BUILD_DIRECTORY)/* *.ppm
