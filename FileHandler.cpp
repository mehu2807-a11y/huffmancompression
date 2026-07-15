#include "FileHandler.h"
using namespace std;

vector<unsigned char> FileHandler::readFile(const string& path) {
    ifstream file(path, ios::binary);
    if (!file) {
        throw runtime_error("Could not open file for reading: " + path);
    }
    file.unsetf(ios::skipws);
    return vector<unsigned char>((istreambuf_iterator<char>(file)),
                                   istreambuf_iterator<char>());
}

void FileHandler::writeFile(const string& path, const vector<unsigned char>& data) {
    ofstream file(path, ios::binary);
    if (!file) {
        throw runtime_error("Could not open file for writing: " + path);
    }
    if (!data.empty()) {
        file.write(reinterpret_cast<const char*>(data.data()), static_cast<streamsize>(data.size()));
    }
}
