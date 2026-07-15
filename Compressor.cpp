#include "Compressor.h"
#include "HuffmanTree.h"
#include "FileHandler.h"
using namespace std;

/*
 * Compressed file layout ("HUFF" format)
 * ---------------------------------------
 *  4 bytes   magic number         "HUFF"
 *  8 bytes   original file size   (uint64_t, number of bytes before compression)
 *  2 bytes   distinct byte count  (uint16_t, how many unique byte values appear)
 *  N * 9     frequency table      for each distinct byte: 1 byte value + 8 byte count
 *  1 byte    padding bit count    (how many 0-bits were added to pad the last byte)
 *  ...       packed bit stream    the actual Huffman-encoded data, 8 bits per byte
 */

namespace {
    void writeUint64(ofstream& out, uint64_t value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    void writeUint16(ofstream& out, uint16_t value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
}

void Compressor::compress(const string& inputPath, const string& outputPath) {
    vector<unsigned char> data = FileHandler::readFile(inputPath);

    // Step 1: count frequency of every byte value.
    array<long long, 256> freq{};
    freq.fill(0);
    for (unsigned char c : data) {
        freq[c]++;
    }

    // Step 2: build the Huffman tree (min-heap + greedy merging) and
    // step 3: generate the binary code for every byte via DFS.
    HuffmanTree tree;
    tree.build(freq);
    tree.generateCodes();
    const auto& codes = tree.getCodes();

    // Step 4: replace every input byte with its Huffman code.
    string bitString;
    bitString.reserve(data.size() * 2);
    for (unsigned char c : data) {
        bitString += codes.at(c);
    }

    // Pad with zero bits so the total length is a multiple of 8.
    int paddingBits = bitString.empty() ? 0 : static_cast<int>((8 - bitString.size() % 8) % 8);
    bitString.append(static_cast<size_t>(paddingBits), '0');

    // Pack the bit string into real bytes.
    vector<unsigned char> packedData;
    packedData.reserve(bitString.size() / 8);
    for (size_t i = 0; i < bitString.size(); i += 8) {
        unsigned char byte = 0;
        for (int b = 0; b < 8; ++b) {
            byte = static_cast<unsigned char>((byte << 1) | (bitString[i + b] - '0'));
        }
        packedData.push_back(byte);
    }

    // Step 5: write the header + packed bit stream to disk.
    ofstream out(outputPath, ios::binary);
    if (!out) {
        throw runtime_error("Could not open output file for writing: " + outputPath);
    }

    out.write("HUFF", 4);
    writeUint64(out, static_cast<uint64_t>(data.size()));

    uint16_t distinctCount = 0;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) distinctCount++;
    }
    writeUint16(out, distinctCount);

    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            unsigned char byteVal = static_cast<unsigned char>(i);
            out.write(reinterpret_cast<const char*>(&byteVal), 1);
            long long f = freq[i];
            out.write(reinterpret_cast<const char*>(&f), sizeof(f));
        }
    }

    unsigned char pad = static_cast<unsigned char>(paddingBits);
    out.write(reinterpret_cast<const char*>(&pad), 1);

    if (!packedData.empty()) {
        out.write(reinterpret_cast<const char*>(packedData.data()),
                   static_cast<streamsize>(packedData.size()));
    }
}
