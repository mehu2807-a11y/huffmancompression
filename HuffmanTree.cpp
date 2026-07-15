#include "HuffmanTree.h"
using namespace std;

// Comparator for the min-heap: smallest frequency first.
// The tie-breaker on 'ch' just makes tree construction deterministic
// when two nodes share the same frequency.
struct Compare {
    bool operator()(const Node* a, const Node* b) const {
        if (a->freq != b->freq) return a->freq > b->freq;
        return a->ch > b->ch;
    }
};

HuffmanTree::HuffmanTree() : root(nullptr) {}

HuffmanTree::~HuffmanTree() {
    destroy(root);
}

void HuffmanTree::destroy(Node* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

void HuffmanTree::build(const array<long long, 256>& freq) {
    priority_queue<Node*, vector<Node*>, Compare> minHeap;

    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            minHeap.push(new Node(static_cast<unsigned char>(i), freq[i]));
        }
    }

    // Edge case: empty input -> no tree at all.
    if (minHeap.empty()) {
        root = nullptr;
        return;
    }

    // Edge case: exactly one distinct character in the whole file.
    // Wrap it in a single internal node so it still gets a valid code ("0")
    // and decompression can still walk left/right correctly.
    if (minHeap.size() == 1) {
        Node* only = minHeap.top();
        minHeap.pop();
        root = new Node(only->freq, only, nullptr);
        return;
    }

    // Classic greedy Huffman merge: repeatedly take the two least frequent
    // nodes and merge them into a new parent node, until one node remains.
    while (minHeap.size() > 1) {
        Node* left = minHeap.top();  minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();

        Node* merged = new Node(left->freq + right->freq, left, right);
        minHeap.push(merged);
    }

    root = minHeap.top();
}

void HuffmanTree::generateCodes() {
    codes.clear();
    if (!root) return;
    generateCodesHelper(root, "");
}

void HuffmanTree::generateCodesHelper(Node* node, const string& code) {
    if (!node) return;

    if (node->isLeaf()) {
        // A file with a single distinct character never recurses past the
        // internal wrapper node, so 'code' could still be empty here.
        codes[node->ch] = code.empty() ? "0" : code;
        return;
    }

    generateCodesHelper(node->left, code + "0");
    generateCodesHelper(node->right, code + "1");
}

const unordered_map<unsigned char, string>& HuffmanTree::getCodes() const {
    return codes;
}

Node* HuffmanTree::getRoot() const {
    return root;
}
