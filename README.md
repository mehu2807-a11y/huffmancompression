# Huffman File Compression

A from-scratch C++ implementation of lossless file compression using
**Huffman Coding** — the same core idea historically used in ZIP, and as
one stage inside JPEG and MP3 encoders.

Frequent bytes get short binary codes, rare bytes get longer binary codes,
and the total number of bits needed to represent the file drops.

```
Input:  AAAAABBBCCD
ASCII:  11 chars x 8 bits = 88 bits
Huffman codes:  A=0  B=10  C=110  D=111
Encoded:        0000010101011011011 1  ->  24 bits
```

This repo compresses **any** file (text or binary), not just ASCII text —
it works on raw bytes, so it can handle images, executables, etc. Highly
repetitive files compress well; already-dense/random files may slightly
*grow* (a few bytes of header overhead) since there's no redundancy left
to squeeze out. That's expected, correct Huffman behavior, not a bug.

## Project structure

```
HuffmanCompression/
│
├── main.cpp              # CLI entry point (compress / decompress / demo)
├── HuffmanTree.h/.cpp     # Min-heap tree building + DFS code generation
├── Compressor.h/.cpp      # Encodes a file into the .huff binary format
├── Decompressor.h/.cpp    # Rebuilds the tree and decodes a .huff file
├── FileHandler.h/.cpp     # Thin wrapper over binary file I/O
├── Makefile               # Build script
├── input.txt              # Sample input for the demo
└── README.md
```

## DSA concepts used

| Concept                     | Where                                                        |
|------------------------------|--------------------------------------------------------------|
| Min-Heap / Priority Queue     | `HuffmanTree::build` — repeatedly pops the two least-frequent nodes |
| Binary Tree                   | `Node` struct — the Huffman tree itself                      |
| DFS / Tree Traversal          | `HuffmanTree::generateCodesHelper` — assigns `0`/`1` codes recursively |
| Hash Map (`unordered_map`)    | Character → frequency, and character → code lookups          |
| Greedy Algorithm              | Merging the two least frequent nodes at every step is provably optimal for prefix codes |
| Bit manipulation              | Packing a string of `'0'`/`'1'` characters into real bytes for storage |

## How it works

**Compression pipeline**

```
Read file (bytes)
   -> Count frequency of each byte value        (array/hash map)
   -> Build Huffman tree                         (min-heap, greedy merge)
   -> Generate binary code per byte              (DFS: left = 0, right = 1)
   -> Replace every byte with its code           (bit string)
   -> Pack bits into bytes + write header         -> compressed.bin
```

**Decompression pipeline**

```
Read header (original size + frequency table)
   -> Rebuild the identical Huffman tree
   -> Walk the tree bit by bit (0 = left, 1 = right)
   -> Every time a leaf is reached, output that byte and reset to the root
   -> Stop once the original byte count is reached
```

### Compressed file format (`.huff` / `.bin`)

```
4 bytes    magic "HUFF"
8 bytes    original file size (uint64)
2 bytes    number of distinct byte values (uint16)
N * 9 bytes  frequency table: [1 byte value][8 byte count] per distinct byte
1 byte     number of padding bits added to the final byte
...        packed Huffman-encoded bit stream
```

Storing only the *distinct* byte/frequency pairs (rather than a fixed
256-slot table) keeps the header small for text-like files that only use
a handful of different characters.

## Building

Requires a C++17 compiler.

```bash
make
```

This produces a `huffman` executable.

## Usage

```bash
# Compress a file
./huffman compress input.txt compressed.bin

# Decompress it back
./huffman decompress compressed.bin output.txt

# Or just run the built-in demo (compress -> decompress -> verify -> report)
./huffman demo
```

`make demo` does the build + demo run in one step.

Example demo output:

```
== Huffman Coding Demo ==
Compressing 'input.txt' -> 'compressed.bin'...
Decompressing 'compressed.bin' -> 'output.txt'...

--- Compression Report ---
Original size:   5800 bytes
Compressed size: 3192 bytes
Space saved:     44.97%

Verification: output.txt matches input.txt exactly. Lossless!
```

## Complexity

Let `n` = file size in bytes, `k` = number of distinct byte values (`k <= 256`).

- Frequency counting: `O(n)`
- Building the tree: `O(k log k)` (each heap push/pop is `O(log k)`, done `O(k)` times)
- Code generation (DFS): `O(k)`
- Encoding the file: `O(n)`
- Decoding the file: `O(n)` (each bit moves one step down the tree; tree depth is bounded by `k`)

Overall: **`O(n + k log k)`**, i.e. essentially linear in the file size.

## Possible extensions

- Serialize the actual tree shape into the header (instead of the
  frequency table) to shave a few more bytes off small files.
- Adaptive/dynamic Huffman coding (tree updates as the file is read, no
  header needed at all).
- Combine with LZ77-style dictionary matching for a DEFLATE-like scheme.
- Wrap directories: compress multiple files into a single archive.
