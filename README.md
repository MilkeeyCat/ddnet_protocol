## DDNet networking library
![Build](https://github.com/MilkeeyCat/ddnet_protocol/actions/workflows/build.yml/badge.svg)

written in C btw

## Installing

```sh
cmake -B build/
cmake --build build/
sudo cmake --install build/
```

```C
// sample.c

#include <stdint.h>
#include <stdio.h>

#include <ddnet_protocol/huffman.h>

int main() {
	uint8_t decompressed[512];
	uint8_t compressed[] = {0x74, 0xde, 0x16, 0xd9, 0xa2, 0x8a, 0x1b};
	DDNetError err = DDNET_ERR_NONE;
	ddnet_huffman_decompress(compressed, sizeof(compressed), decompressed, sizeof(decompressed), &err);
	if(err == DDNET_ERR_NONE) {
		puts((const char *)decompressed); // foo
	}
}
```

```
gcc sample.c -o sample -lddnet_protocol
./sample
```

# Linking the library using CMake

Example's file structure:
```
.
├── libs
│   └── ddnet_protocol
├── main.c
└── CMakeLists.txt
```
```cmake
# CMakeLists.txt

cmake_minimum_required(VERSION 3.16)
project(crocks)

add_executable(crocks
    main.c
)

add_subdirectory(libs/ddnet_protocol)
target_link_libraries(crocks ddnet_protocol)
```
