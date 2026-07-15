#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <bits/stdc++.h>
using namespace std;

// Small wrapper around basic binary file I/O so the rest of the
// codebase never touches ifstream/ofstream directly.
class FileHandler {
public:
    static vector<unsigned char> readFile(const string& path);
    static void writeFile(const string& path, const vector<unsigned char>& data);
};

#endif // FILE_HANDLER_H
