#include <bits/stdc++.h>
#include <filesystem>
#include "Compressor.h"
#include "Decompressor.h"

using namespace std;
namespace fs = filesystem;

void printUsage(const char* progName) {
    cout << "Huffman File Compression\n"
         << "-------------------------\n"
         << "Usage:\n"
         << "  " << progName << " compress <input_file> <output_file>\n"
         << "  " << progName << " decompress <input_file> <output_file>\n"
         << "  " << progName << " demo\n"
         << "      Runs compress + decompress on input.txt -> compressed.bin -> output.txt\n"
         << "      and prints a compression report.\n";
}

void printReport(const string& originalPath, const string& compressedPath) {
    auto origSize = fs::file_size(originalPath);
    auto compSize = fs::file_size(compressedPath);

    cout << "\n--- Compression Report ---\n";
    cout << "Original size:   " << origSize << " bytes\n";
    cout << "Compressed size: " << compSize << " bytes\n";

    if (origSize > 0) {
        double ratio = (1.0 - static_cast<double>(compSize) / static_cast<double>(origSize)) * 100.0;
        cout << "Space saved:     " << fixed << setprecision(2) << ratio << "%\n";
    }
}

void runDemo() {
    const string in = "input.txt";
    const string compressed = "compressed.bin";
    const string out = "output.txt";

    if (!fs::exists(in)) {
        cerr << "Error: '" << in << "' not found in the current directory.\n";
        return;
    }

    cout << "== Huffman Coding Demo ==\n";
    cout << "Compressing '" << in << "' -> '" << compressed << "'...\n";
    Compressor::compress(in, compressed);

    cout << "Decompressing '" << compressed << "' -> '" << out << "'...\n";
    Decompressor::decompress(compressed, out);

    printReport(in, compressed);

    // Verify round-trip correctness.
    ifstream f1(in, ios::binary), f2(out, ios::binary);
    bool identical = equal(istreambuf_iterator<char>(f1),
                            istreambuf_iterator<char>(),
                            istreambuf_iterator<char>(f2));
    cout << (identical
                 ? "\nVerification: output.txt matches input.txt exactly. Lossless!\n"
                 : "\nVerification FAILED: output.txt does not match input.txt.\n");
}

int main(int argc, char* argv[]) {
    try {
        if (argc == 1) {
            runDemo();
            return 0;
        }

        string mode = argv[1];

        if (mode == "compress" && argc == 4) {
            Compressor::compress(argv[2], argv[3]);
            cout << "Compressed '" << argv[2] << "' -> '" << argv[3] << "'\n";
            printReport(argv[2], argv[3]);
        } else if (mode == "decompress" && argc == 4) {
            Decompressor::decompress(argv[2], argv[3]);
            cout << "Decompressed '" << argv[2] << "' -> '" << argv[3] << "'\n";
        } else if (mode == "demo") {
            runDemo();
        } else {
            printUsage(argv[0]);
            return 1;
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
