.PHONY: all build-dir m-dyn m-static ms-dyn ms-static

BUILD_DIR=./build

all: m-dyn m-static ms-dyn ms-static

build-dir:
	test -d $(BUILD_DIR) || mkdir $(BUILD_DIR)

m-dyn: build-dir
	gcc -o $(BUILD_DIR)/m-dyn main.c
m-static: build-dir
	gcc -static -o $(BUILD_DIR)/m-static main.c

ms-dyn: build-dir
	gcc -o $(BUILD_DIR)/ms-dyn main_strings.c
ms-static: build-dir
	gcc -static -o $(BUILD_DIR)/ms-static main_strings.c
