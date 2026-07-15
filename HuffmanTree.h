#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <bits/stdc++.h>
using namespace std;

// A single node of the Huffman Tree.
// Leaf nodes hold an actual character; internal nodes are "merge" nodes
// created while building the tree and only hold a combined frequency.
struct Node {
    unsigned char ch;
    long long freq;
    Node* left;
    Node* right;

    Node(unsigned char c, long long f)
        : ch(c), freq(f), left(nullptr), right(nullptr) {}

    Node(long long f, Node* l, Node* r)
        : ch(0), freq(f), left(l), right(r) {}

    bool isLeaf() const { return left == nullptr && right == nullptr; }
};

class HuffmanTree {
public:
    HuffmanTree();
    ~HuffmanTree();

    // Builds the tree from a 256-entry frequency table (one slot per byte value)
    // using a min-heap (priority queue) + greedy merging.
    void build(const array<long long, 256>& freq);

    // Walks the tree (DFS) and assigns a binary string code to every leaf.
    void generateCodes();

    const unordered_map<unsigned char, string>& getCodes() const;
    Node* getRoot() const;

private:
    Node* root;
    unordered_map<unsigned char, string> codes;

    void generateCodesHelper(Node* node, const string& code);
    void destroy(Node* node);
};

#endif // HUFFMAN_TREE_H
