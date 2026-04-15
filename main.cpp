#include <iostream>
#include <string>
#include <fstream>
#include "terminal_format.cpp"
#include "node.h"
#include "node.cpp"
using namespace std;
// C++ red/black tree project - started 4/11/26
// https://github.com/TRVRNB/CppRedBlackTree.git
// this is a fork of my previous project, https://github.com/TRVRNB/CppBinaryTree.git
// also, nobody uses light mode terminal, black is white, deal with it

namespace red_black_tree{
  string version = "1.0";
  Node* root = new Node(0);
}
using namespace red_black_tree;

Node* add_to_tree(unsigned short to_add, Node* add_to){
  // recursive function, checks this node and its children for empty space that fits the binary search tree requirements
  // yes i know the variable names are terrible
  if (add_to->value == 0){ // empty space is found (requirements are checked by the previous recursion), should also work for root
    if (add_to == root){
      root = new Node(to_add);
      // create leaves, too
      // leaves are blank nodes (0), treated as not existing
      root->left = new Node(0);
      root->left->color = Color::black;
      root->right = new Node(0);
      root->right->color = Color::black;
      return root;
    }
    add_to = new Node(to_add);
    // again, create leaves
    add_to->left = new Node(0);
    add_to->left->color = Color::black;
    add_to->right = new Node(0);
    add_to->right->color = Color::black;
    return add_to;
  }
  if (to_add < add_to->value){ // less than
    Node* child = add_to_tree(to_add, add_to->left);
    if (child != nullptr){
      delete add_to->left;
      child->parent = add_to;
      add_to->left = child;
    }
    return nullptr;
  }
  // greater than or equal to
  Node* child = add_to_tree(to_add, add_to->right);
  if (child != nullptr){
    delete add_to->right;
    child->parent = add_to;
    add_to->right = child;
  }
  return nullptr;
}

void print_tree(Node* to_print, unsigned short recursion){
  // recursive function, prints this node's children
  if (to_print == nullptr){ // if this node doesn't exist
    return;
  }
  recursion++;
  print_tree(to_print->right, recursion); // print right first
  cout << RESET;
  for (int i = 2; i < recursion; i++){ // spaces before self
    cout << ' ';
    if (i < recursion-1){
      cout << '|';
    } else {
      cout << ' ';
    }
  }
  // add color
  if (to_print->color == Color::red){
    cout << RED;
  } else if (to_print->color == Color::black){
    cout << WHITE;
  }
  cout << to_print->value << endl; // cout self
  print_tree(to_print->left, recursion); // print left
}

int main(){
  cout << RED << "Red" << GREEN << '/' << WHITE << "Black" << GREEN << " Tree - Version " << version << endl;
  cout << YELLOW << "Type 'HELP' for a list of commands." << endl;
  string input;

  while (input != "QUIT"){ // QUIT
    input = "";
    cout << GREEN << "Enter a command: " << RESET << flush;
					      cin >> input; // why does GNU-emacs hate this line?
    if (input == "HELP"){ // HELP
      cout << WHITE << "HELP: prints a list of commands (obviously)" << endl;
      cout << WHITE << "QUIT: stops the program" << endl;
      cout << WHITE << "ADD: adds a number to the binary tree" << endl;
      cout << WHITE << "LOAD: adds multiple numbers from a file" << endl;
      cout << WHITE << "PRINT: prints the binary tree" << endl;
      cout << WHITE << "SEARCH: find if a number exists" << endl;
      cout << WHITE << "DELETE: delete a number from the tree" << endl;
    } else if (input == "ADD"){ // ADD
      string input1;
      cout << GREEN << "Enter an integer (1-999): " << RESET << flush;
      cin >> input1;
      unsigned short to_add = stoi(input1); // unsigned short has range 0 to ~65000
      if (to_add == 0 || to_add > 999){ // check for bounds
	cout << RED << to_add << " isn't within bounds." << endl;
	// this will print the wrapped version, technically you could enter -65000 and it would be within range, it's a feature
	continue;
      }
      add_to_tree(to_add, root); // call add to root
      cout << WHITE << "Added " << to_add << " to tree." << endl;

    } else if (input == "PRINT"){ // PRINT
      cout << RESET;
      print_tree(root, 1);
      
    } else if (input == "LOAD"){ // LOAD
      // this is mostly copied from my heap loading code
      cout << GREEN << "Enter the filname (max 80 chars): " << RESET << flush;
      string input;
      cin >> input;
      // make the file
      ifstream file(input);
      char file_text_array[401]; // c-style (array) works better for this
      file.getline(file_text_array, 400);
      string file_text(file_text_array);
      if (file_text.length() == 0){
	cout << YELLOW << input << RED << " not found in current folder." << endl;
      } else {
	// iterate over it
	int string_index = 0;
	string current_num;
	for (int i = 0; i < 100; i++){
	  if (string_index < file_text.length()){
	    current_num = "";
	    int j = 0;
	    // individual numbers
	    while (file_text[string_index] != ' ' && j < 4){
	      current_num.append(1, file_text[string_index]);
	      string_index++;
	      j++;
	    }
	  }
	  string_index++;
	  unsigned short to_add = stoi(current_num);
	  add_to_tree(to_add, root);
	}
      cout << WHITE << "Done!" << endl;
      }
      
    } else if (input == "SEARCH"){ // SEARCH
      if (root == nullptr){
	cout << RED << "Add some numbers first!" << endl;
	continue;
      }
      Node* current_node = root;
      cout << GREEN << "Enter a number to search for: " << RESET << flush;
      string input;
      cin >> input;
      unsigned short to_find = stoi(input); // cast to int
      cout << RESET;
      while (current_node->value != to_find){
	// stop searching if it reaches a wall or it finds the right number
	if (current_node->value > to_find){ // too big
	  cout << "Less than " << current_node->value << "..." << endl;
	  current_node = current_node->left;
	} else { // too small
	  cout << "Greater than " << current_node->value << "..." << endl;
	  current_node = current_node->right;
	}
	if (current_node == nullptr){ // end found
	  break;
	}
      }
      // now, find out if it failed
      if (current_node == nullptr){ // failure
	cout << WHITE << "No instances of " << YELLOW << to_find << WHITE << " found." << endl;
      } else { // success
	cout << WHITE << "That number exists!" << endl;
      }
    } else if (input == "DELETE"){ // DELETE
      if (root == nullptr){
	cout << RED << "Add some numbers first!" << endl;
      }
      Node* current_node = root;
      Node* previous_node = nullptr;
      cout << GREEN << "Enter a number to delete: " << RESET << flush;
      string input;
      cin >> input;
      unsigned short to_find = stoi(input);
      cout << RESET;
      while (current_node->value != to_find){
	// same as SEARCH loop but keeps parent
	previous_node = current_node;
	if (current_node->value > to_find){
	  current_node = current_node->left;
	} else {
	  current_node = current_node->right;
	}
	if (current_node == nullptr){
	  break;
	}
      }
      if (current_node == nullptr){ // failure
	cout << RED << "No instances found." << endl;
      } else {
	// four cases: no children, one child, two children, and root
	// for no children it can just do nothing since there will be no consequence to deleting it
	if (current_node == root){ // check root first since it's simplest
	  if (current_node->right != nullptr){
	    root = current_node->right;
	    current_node->right = nullptr;
	  } else {
	    root = current_node->left;
	    current_node->left = nullptr;
	  }
	}
	if (current_node->right != nullptr){ // has right child
	  if (current_node == previous_node->right){ // donate it to parent
	    previous_node->right = current_node->right;
	  } else {
	    previous_node->left = current_node->right;
	  }
	  if (current_node->left != nullptr){ // has left child
	    // look for room for left child
	    Node* current_node2 = root;
	    Node* left = current_node->left;
	    while (!(current_node2->left == nullptr && current_node2->value > left->value)){
	      if (current_node2->value > left->value){
		current_node2 = current_node2->left;
	      } else {
		current_node2 = current_node2->right;
	      }
	    }
	    // current_node2 is new parent
	    current_node2->left = left;
	    current_node->left = nullptr;
	  }
	} else if (current_node->left != nullptr){
	  if (previous_node != nullptr){ // not root case
	    if (current_node == previous_node->right){
	      previous_node->right = current_node->left;
	    } else {
	      previous_node->left = current_node->left;
	    }
	  } else {
	    // look for room for left child (same as above), in this case there was originally a right child
	    Node* current_node2 = root;
	    Node* left = current_node->left;
	    while (!(current_node2->left == nullptr && current_node2->value > left->value)){
	      if (current_node2->value > left->value){
		current_node2 = current_node2->left;
	      } else {
		current_node2 = current_node2->right;
	      }
	    }
	    // current_node2 is new parent
	    current_node2->left = left;
	    current_node->left = nullptr;
	  }
	}
	if (previous_node != nullptr){ // delete self from parent if it still exists
	  if (previous_node->left == current_node){
	    previous_node->left = nullptr;
	  } else if (previous_node->right == current_node){
	    previous_node->right = nullptr;
	  }
	}
	delete current_node;
	cout << WHITE << "Deleted " << RESET << input << endl;
      }
    }
  }
  cout << YELLOW << "Goodbye!" << endl;
  return 0;
}
