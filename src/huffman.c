#include "huffman.h"

static const uint32_t FREQUENCY_TABLE[256 + 1] = {
	1 << 30, 4545, 2657, 431, 1950, 919, 444, 482, 2244, 617, 838, 542, 715, 1814, 304, 240, 754, 212, 647, 186,
	283, 131, 146, 166, 543, 164, 167, 136, 179, 859, 363, 113, 157, 154, 204, 108, 137, 180, 202, 176,
	872, 404, 168, 134, 151, 111, 113, 109, 120, 126, 129, 100, 41, 20, 16, 22, 18, 18, 17, 19,
	16, 37, 13, 21, 362, 166, 99, 78, 95, 88, 81, 70, 83, 284, 91, 187, 77, 68, 52, 68,
	59, 66, 61, 638, 71, 157, 50, 46, 69, 43, 11, 24, 13, 19, 10, 12, 12, 20, 14, 9,
	20, 20, 10, 10, 15, 15, 12, 12, 7, 19, 15, 14, 13, 18, 35, 19, 17, 14, 8, 5,
	15, 17, 9, 15, 14, 18, 8, 10, 2173, 134, 157, 68, 188, 60, 170, 60, 194, 62, 175, 71,
	148, 67, 167, 78, 211, 67, 156, 69, 1674, 90, 174, 53, 147, 89, 181, 51, 174, 63, 163, 80,
	167, 94, 128, 122, 223, 153, 218, 77, 200, 110, 190, 73, 174, 69, 145, 66, 277, 143, 141, 60,
	136, 53, 180, 57, 142, 57, 158, 61, 166, 112, 152, 92, 26, 22, 21, 28, 20, 26, 30, 21,
	32, 27, 20, 17, 23, 21, 30, 22, 22, 21, 27, 25, 17, 27, 23, 18, 39, 26, 15, 21,
	12, 18, 18, 27, 20, 18, 15, 19, 11, 17, 33, 12, 18, 15, 19, 18, 16, 26, 17, 18,
	9, 10, 25, 22, 22, 17, 20, 16, 6, 16, 15, 20, 14, 18, 24, 335, 1517};

typedef struct HuffmanConstructNode {
	uint16_t node_id;
	uint32_t frequency;
} HuffmanConstructNode;

typedef struct Node {
	// symbol
	uint32_t bits;
	uint32_t num_bits;

	// don't use pointers for this. shorts are smaller so we can fit more data into the cache
	uint16_t leafs[2];

	// what the symbol represents
	uint8_t symbol;
} Node;

enum {
	HUFFMAN_EOF_SYMBOL = 256,

	HUFFMAN_MAX_SYMBOLS = HUFFMAN_EOF_SYMBOL + 1,
	HUFFMAN_MAX_NODES = (HUFFMAN_MAX_SYMBOLS * 2) - 1,

	HUFFMAN_LUTBITS = 10,
	HUFFMAN_LUTSIZE = (1 << HUFFMAN_LUTBITS),
	HUFFMAN_LUTMASK = (HUFFMAN_LUTSIZE - 1)
};

static Node nodes[HUFFMAN_MAX_NODES];
static Node *decode_luts[HUFFMAN_LUTSIZE];
static Node *start_node;
static int32_t num_nodes;
bool huffman_initialized = false;

static void bubble_sort_nodes(HuffmanConstructNode **list, int32_t size) {
	uint8_t changed = 1;
	HuffmanConstructNode *temp;

	while(changed) {
		changed = 0;
		for(int32_t i = 0; i < size - 1; i++) {
			if(list[i]->frequency < list[i + 1]->frequency) {
				temp = list[i];
				list[i] = list[i + 1];
				list[i + 1] = temp;
				changed = 1;
			}
		}
		size--;
	}
}

static void setbits_r(Node *node, int32_t bits, uint32_t depth) {
	if(node->leafs[1] != 0xffff) {
		setbits_r(&nodes[node->leafs[1]], bits | (1 << depth), depth + 1);
	}
	if(node->leafs[0] != 0xffff) {
		setbits_r(&nodes[node->leafs[0]], bits, depth + 1);
	}

	if(node->num_bits) {
		node->bits = bits;
		node->num_bits = depth;
	}
}

static void construct_tree(const uint32_t *frequencies) {
	HuffmanConstructNode aNodesLeftStorage[HUFFMAN_MAX_SYMBOLS];
	HuffmanConstructNode *apNodesLeft[HUFFMAN_MAX_SYMBOLS];
	int32_t num_nodes_left = HUFFMAN_MAX_SYMBOLS;

	// add the symbols
	for(int32_t i = 0; i < HUFFMAN_MAX_SYMBOLS; i++) {
		nodes[i].num_bits = 0xffffffff;
		nodes[i].symbol = i;
		nodes[i].leafs[0] = 0xffff;
		nodes[i].leafs[1] = 0xffff;

		if(i == HUFFMAN_EOF_SYMBOL) {
			aNodesLeftStorage[i].frequency = 1;
		} else {
			aNodesLeftStorage[i].frequency = frequencies[i];
		}
		aNodesLeftStorage[i].node_id = i;
		apNodesLeft[i] = &aNodesLeftStorage[i];
	}

	num_nodes = HUFFMAN_MAX_SYMBOLS;

	// construct the table
	while(num_nodes_left > 1) {
		// we can't rely on stdlib's qsort for this, it can generate different results on different implementations
		bubble_sort_nodes(apNodesLeft, num_nodes_left);

		nodes[num_nodes].num_bits = 0;
		nodes[num_nodes].leafs[0] = apNodesLeft[num_nodes_left - 1]->node_id;
		nodes[num_nodes].leafs[1] = apNodesLeft[num_nodes_left - 2]->node_id;
		apNodesLeft[num_nodes_left - 2]->node_id = num_nodes;

		apNodesLeft[num_nodes_left - 2]->frequency = apNodesLeft[num_nodes_left - 1]->frequency + apNodesLeft[num_nodes_left - 2]->frequency;
		num_nodes++;
		num_nodes_left--;
	}

	start_node = &nodes[num_nodes - 1];

	// build symbol bits
	setbits_r(start_node, 0, 0);
}

static void huffman_init() {
	if(huffman_initialized) {
		return;
	}
	huffman_initialized = 1;
	const uint32_t *frequencies = FREQUENCY_TABLE;

	// make sure to cleanout every thing
	memset(nodes, 0, sizeof(nodes));
	memset((void *)decode_luts, 0, sizeof(decode_luts));

	start_node = NULL;
	num_nodes = 0;

	// construct the tree
	construct_tree(frequencies);

	// build decode LUT
	for(int32_t i = 0; i < HUFFMAN_LUTSIZE; i++) {
		uint32_t bits = i;
		int32_t counter;
		Node *node = start_node;
		for(counter = 0; counter < HUFFMAN_LUTBITS; counter++) {
			node = &nodes[node->leafs[bits & 1]];
			bits >>= 1;

			if(!node) {
				break;
			}

			if(node->num_bits) {
				decode_luts[i] = node;
				break;
			}
		}

		if(counter == HUFFMAN_LUTBITS) {
			decode_luts[i] = node;
		}
	}
}

size_t huffman_compress(const uint8_t *input, size_t input_len, uint8_t *output, size_t output_len) {
	huffman_init();
	// this macro loads a symbol for a byte into bits and bitcount
#define HUFFMAN_MACRO_LOADSYMBOL(sym) \
	bits |= nodes[sym].bits << bitcount; \
	bitcount += nodes[sym].num_bits;

	// this macro writes the symbol stored in bits and bitcount to the dst pointer
#define HUFFMAN_MACRO_WRITE() \
	while(bitcount >= 8) { \
		*dst++ = (uint8_t)(bits & 0xff); \
		if(dst == dstend) \
			return -1; \
		bits >>= 8; \
		bitcount -= 8; \
	}

	// setup buffer pointers
	const uint8_t *src = input;
	const uint8_t *srcend = src + input_len;
	uint8_t *dst = output;
	uint8_t *dstend = dst + output_len;

	// symbol variables
	uint32_t bits = 0;
	uint32_t bitcount = 0;

	// make sure that we have data that we want to compress
	if(input_len) {
		// {A} load the first symbol
		int32_t symbol = *src++;

		while(src != srcend) {
			// {B} load the symbol
			HUFFMAN_MACRO_LOADSYMBOL(symbol)

			// {C} fetch next symbol, this is done here because it will reduce dependency in the code
			symbol = *src++;

			// {B} write the symbol loaded at
			HUFFMAN_MACRO_WRITE()
		}

		// write the last symbol loaded from {C} or {A} in the case of only 1 byte input buffer
		HUFFMAN_MACRO_LOADSYMBOL(symbol)
		HUFFMAN_MACRO_WRITE()
	}

	// write EOF symbol
	HUFFMAN_MACRO_LOADSYMBOL(HUFFMAN_EOF_SYMBOL)
	HUFFMAN_MACRO_WRITE()

	// write out the last bits
	*dst++ = bits;

	// return the size of the output
	return (dst - (const uint8_t *)output);

	// remove macros
#undef HUFFMAN_MACRO_LOADSYMBOL
#undef HUFFMAN_MACRO_WRITE
}

size_t huffman_decompress(const uint8_t *input, size_t input_len, uint8_t *output, size_t output_len) {
	huffman_init();
	// setup buffer pointers
	uint8_t *dst = output;
	const uint8_t *src = input;
	uint8_t *dstend = dst + output_len;
	const uint8_t *srcend = src + input_len;

	uint32_t bits = 0;
	uint32_t bitcount = 0;

	Node *eof = &nodes[HUFFMAN_EOF_SYMBOL];
	Node *node = 0;

	while(1) {
		// {a} try to load a node now, this will reduce dependency at location {d}
		node = 0;
		if(bitcount >= HUFFMAN_LUTBITS) {
			node = decode_luts[bits & HUFFMAN_LUTMASK];
		}

		// {b} fill with new bits
		while(bitcount < 24 && src != srcend) {
			bits |= (*src++) << bitcount;
			bitcount += 8;
		}

		// {C} load symbol now if we didn't that earlier at location {A}
		if(!node) {
			node = decode_luts[bits & HUFFMAN_LUTMASK];
		}

		if(!node) {
			return -1;
		}

		// {d} check if we hit a symbol already
		if(node->num_bits) {
			// remove the bits for that symbol
			bits >>= node->num_bits;
			bitcount -= node->num_bits;
		} else {
			// remove the bits that the lut checked up for us
			bits >>= HUFFMAN_LUTBITS;
			bitcount -= HUFFMAN_LUTBITS;

			// walk the tree bit by bit
			while(1) {
				// traverse tree
				node = &nodes[node->leafs[bits & 1]];

				// remove bit
				bitcount--;
				bits >>= 1;

				// check if we hit a symbol
				if(node->num_bits) {
					break;
				}

				// no more bits, decoding error
				if(bitcount == 0) {
					return -1;
				}
			}
		}

		// check for eof
		if(node == eof) {
			break;
		}

		// output character
		if(dst == dstend) {
			return -1;
		}
		*dst++ = node->symbol;
	}

	// return the size of the decompressed buffer
	return (dst - (const uint8_t *)output);
}
