#include <iostream>
#include "node.h"

Node::Node(unsigned short n_value){ // constructor
  color = Color::black; // in case it wasn't set (DELETE)
  value = n_value;
}

Node::~Node(){ // destructor
  // delete children if they are null
  if (value == 0) return;
  if (left->value == 0) delete left;
  if (right->value == 0) delete right;
}
