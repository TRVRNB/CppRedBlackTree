#ifndef BINARY_SEARCH_NODE
#define BINARY_SEARCH_NODE
#include <iostream>

// new implementation from my binary tree project; contains color enum as well

enum class Color{
  // scoped enum with my red/black colors
  red, // Color::red
  black // Color::black
};

class Node{
  // node class for binary search tree
 public:
  Node(unsigned short n_value);
  ~Node();
  unsigned short value; // what integer this node represents
  Node* parent = nullptr; // parent
  Node* left = nullptr; // left child, must be smaller
  Node* right = nullptr; // right child, must be bigger
  Color color = Color::red; // is this node red or black
};

#endif
