#include "HuffmanTree.h"
using namespace std;
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
    if (minHeap.empty()) {
        root = nullptr;
        return;
    }

    if (minHeap.size() == 1) {
        Node* only = minHeap.top();
        minHeap.pop();
        root = new Node(only->freq, only, nullptr);
        return;
    }

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
