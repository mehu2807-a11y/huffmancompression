#include "Decompressor.h"
#include "HuffmanTree.h"
#include "FileHandler.h"
using namespace std;

namespace {
    uint64_t readUint64(ifstream& in) {
        uint64_t value = 0;
        in.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }
    uint16_t readUint16(ifstream& in) {
        uint16_t value = 0;
        in.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }
}

void Decompressor::decompress(const string& inputPath, const string& outputPath) {
    ifstream in(inputPath, ios::binary);
    if (!in) {
        throw runtime_error("Could not open input file: " + inputPath);
    }

    char magic[4];
    in.read(magic, 4);
    if (in.gcount() != 4 || strncmp(magic, "HUFF", 4) != 0) {
        throw runtime_error("Not a valid HUFF compressed file: " + inputPath);
    }

    uint64_t originalSize = readUint64(in);
    uint16_t distinctCount = readUint16(in);
    array<long long, 256> freq{};
    freq.fill(0);
    for (int i = 0; i < distinctCount; ++i) {
        unsigned char byteVal = 0;
        long long count = 0;
        in.read(reinterpret_cast<char*>(&byteVal), 1);
        in.read(reinterpret_cast<char*>(&count), sizeof(count));
        freq[byteVal] = count;
    }

    unsigned char paddingBits = 0;
    in.read(reinterpret_cast<char*>(&paddingBits), 1);

    vector<unsigned char> packedData((istreambuf_iterator<char>(in)),
                                       istreambuf_iterator<char>());

    vector<unsigned char> output;
    output.reserve(static_cast<size_t>(originalSize));

    if (originalSize == 0) {
        FileHandler::writeFile(outputPath, output);
        return;
    }
    HuffmanTree tree;
    tree.build(freq);
    Node* root = tree.getRoot();
    if (!root) {
        throw runtime_error("Corrupt compressed file: missing Huffman tree data");
    }

    Node* current = root;
    size_t totalBits = packedData.size() * 8 >= paddingBits
                            ? packedData.size() * 8 - paddingBits
                            : 0;

    for (size_t i = 0; i < totalBits && output.size() < originalSize; ++i) {
        unsigned char byte = packedData[i / 8];
        int bitIndex = 7 - static_cast<int>(i % 8);
        int bit = (byte >> bitIndex) & 1;

        current = (bit == 0) ? current->left : current->right;
        if (!current) {
            throw runtime_error("Corrupt compressed file: invalid bit stream");
        }

        if (current->isLeaf()) {
            output.push_back(current->ch);
            current = root;
        }
    }

    if (output.size() != originalSize) {
        throw runtime_error("Decompression produced unexpected size (file may be corrupt)");
    }

    FileHandler::writeFile(outputPath, output);
}
