#include <iostream>
#include "node.h"

Node::Node(unsigned short n_value){ // constructor
  color = Color::black; // in case it wasn't set (DELETE)
  value = n_value;
}

Node* Node::get_child(short child){
  if (child == -1) return left;
  else return right;
}

void Node::set_child(short child, Node* new_child){
  if (child == -1) left = new_child;
  else right = new_child;
}
