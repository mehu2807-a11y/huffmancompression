#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <bits/stdc++.h>
using namespace std;

class Compressor {
public:
    // Reads inputPath, builds a Huffman tree from its byte-frequencies,
    // and writes a compressed binary file to outputPath.
    static void compress(const string& inputPath, const string& outputPath);
};

#endif // COMPRESSOR_H
