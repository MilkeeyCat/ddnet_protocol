## DDNet networking library
![Build](https://github.com/MilkeeyCat/ddnet_protocol/actions/workflows/build.yml/badge.svg)

written in C btw

## installing

```
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
	huffman_decompress(compressed, sizeof(compressed), decompressed, sizeof(decompressed));
	puts((const char *)decompressed); // foo
}
```

```
g++ sample.c -o sample -lddnet_protocol
./sample
```

