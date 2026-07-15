#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <bits/stdc++.h>
using namespace std;

class Decompressor {
public:
    // Reads a "HUFF" compressed file from inputPath, rebuilds the Huffman
    // tree from its stored frequency table, and writes the original bytes
    // back out to outputPath.
    static void decompress(const string& inputPath, const string& outputPath);
};

#endif // DECOMPRESSOR_H
