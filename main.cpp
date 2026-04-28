#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "terminal_format.cpp"
#include "node.h"
#include "node.cpp"
using namespace std;
// C++ red/black tree project - started 4/11/26
// https://github.com/TRVRNB/CppRedBlackTree.git
// this is a fork of my previous project, https://github.com/TRVRNB/CppBinaryTree.git
// also, nobody uses light mode terminal, black is white, deal with it
// red/black is a binary tree type, designed to balance itself and minimize the worst-case time complexity
// all null nodes (empty root, leaf nodes) are black
// red nodes can only have black children
// every path from a node to any of its leaves must have the same number of black nodes as all other paths
// this last rule means that the tree is balanced around black nodes, which usually means red/black will just alternate, and extra nodes added at the end will always be red (not null leaf nodes!)
// these rules guarantee that the binary tree is somewhat balanced, but a special rebalancing function needs to be added

namespace red_black_tree{
  string version = "1.5";
  Node* root = new Node(0);
}
using namespace red_black_tree;

void rotate_left(Node* to_rotate){
  // swap this node with its right child, which is guaranteed to be greater, both will keep their right children
  Node* node2 = to_rotate->right;
  to_rotate->right = node2->left;
  if (node2->left != nullptr && node2->left->value != 0){ // through most compilers, the != nullptr check should prevent the next from running if it's false
    node2->left->parent = to_rotate; // update parent
  }
  node2->parent = to_rotate->parent;
  if (to_rotate == root){ // check for root case
    root = node2; // change root
  } else if (to_rotate == to_rotate->parent->left){ // is left child
    to_rotate->parent->left = node2;
  } else { // is right child
    to_rotate->parent->right = node2;
  }
  node2->left = to_rotate;
  to_rotate->parent = node2;
}

void rotate_right(Node* to_rotate){
  // similar to rotate_left()
  Node* node2 = to_rotate->left;
  to_rotate->left = node2->right;
  if (node2->right != nullptr && node2->right->value != 0){
    node2->right->parent = to_rotate; // update parent
  }
  node2->parent = to_rotate->parent;
  if (to_rotate == root){
    root = node2;
  } else if (to_rotate == to_rotate->parent->right){ // is right child
    to_rotate->parent->right = node2;
  } else { // is left child
    to_rotate->parent->left = node2;
  }
  node2->right = to_rotate;
  to_rotate->parent = node2;
}

void fix_insert(Node* to_fix){
  // i didn't notice, but sometimes this function creates R/R pairs, so i added a check to the end, but i don't know if it violates the black path rule?
  // rebalance this branch of the tree
  while (to_fix != root && to_fix->parent->color == Color::red){ // this should be a black node or the tree should be rotated
    Node* parent1 = to_fix->parent;
    Node* parent2 = parent1->parent; // this is guaranteed to not throw an error as long as to_fix isn't root
    if (parent2 == nullptr){ // parent is root
      break;
    }
    if (parent1 == parent2->left){ // is left grandchild
      Node* aunt = parent2->right;
      if (aunt->color == Color::red){ // if the aunt is currently red, try swapping the colors
        parent1->color = Color::black;
        aunt->color = Color::black;
        parent2->color = Color::red;
        to_fix = parent2; // work with grandparent now
      } else { // this is more complicated because of the black-balancing requirement
        if (to_fix == parent1->right){ // is right child
          to_fix = parent1;
          rotate_left(to_fix); // can barely wrap my mind around how this works, but this is how the R/B trees online work, so okay...
          // to_fix should still be the grandchild of parent2, right?
        }
        parent1->color = Color::black;
        parent2->color = Color::red;
        rotate_right(parent2); // now, swap parent2 with the aunt node? is that right?
      }
    } else { // is right grandchild
      Node* uncle = parent2->left; // assigning genders to left/right nodes was a weird idea in hindsight (especially since each child only has 1 parent, and this means the root node is genderless?)
      if (uncle->color == Color::red){ // if uncle is currently red, try swapping the colors
        parent1->color = Color::black;
        uncle->color = Color::black;
        parent2->color = Color::red;
        to_fix = parent2; // work with grandparent now
      } else { // same story as with the aunt
        if (to_fix == parent1->left){
          to_fix = parent1;
          rotate_right(to_fix);
        }
        parent1->color = Color::black;
        parent2->color = Color::red;
        rotate_left(parent2);
      }
    }
  }
  if (to_fix->parent != nullptr && to_fix->parent->color == Color::red && to_fix->color == Color::red){ // make parent black if both parent and self are red
    to_fix->parent->color = Color::black;
  }
  root->color = Color::black;
}

Node* add_to_tree(unsigned short to_add, Node* add_to){
  if (add_to->value == 0){ // empty space is found (requirements are checked by the previous recursion), should also work for root
    Node* new_node = new Node(to_add);
    // again, create leaves
    new_node->left = new Node(0);
    new_node->left->color = Color::black;
    new_node->left->parent = new_node;
    new_node->right = new Node(0);
    new_node->right->color = Color::black;
    new_node->right->parent = new_node;
    new_node->color = Color::red; // treat red as 'default' for now?
    new_node->parent = nullptr;
    return new_node;
  }
  if (to_add < add_to->value){ // less than
    if (add_to->left->value == 0){ // found leaf node to replace
      Node* child = add_to_tree(to_add, add_to->left);
      delete add_to->left;
      add_to->left = child;
      child->parent = add_to;
      return child;
    }
    return add_to_tree(to_add, add_to->left);
  } else { // greater than or equal to
    if (add_to->right->value == 0){
      Node* child = add_to_tree(to_add, add_to->right);
      delete add_to->right;
      add_to->right = child;
      child->parent = add_to;
      return child;
    }
    return add_to_tree(to_add, add_to->right);
  }
  // end
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
  if (to_print->value != 0){
    cout << to_print->value << endl; // cout self
  } else {
    cout << "null" << endl;
  }
  print_tree(to_print->left, recursion); // print left
}

int main(){
  root->parent = nullptr; // JUST IN CASE!
  root->color = Color::black;
  cout << RED << "Red" << GREEN << '/' << WHITE << "Black" << GREEN << " Tree - Version " << version << endl;
  cout << YELLOW << "Type 'HELP' for a list of commands." << endl;
  string input;
  while (input != "QUIT"){ // QUIT
    input = "";
    cout << GREEN << "Enter a command: " << RESET << flush;
    cin >> input; // GNU-emacs fixed this line on the red-black version?
    if (input == "HELP"){ // HELP
      cout << WHITE << "HELP: prints a list of commands (obviously)" << endl;
      cout << WHITE << "QUIT: stops the program" << endl;
      cout << WHITE << "ADD: adds a number to the tree" << endl;
      cout << WHITE << "LOAD: adds multiple numbers from a file" << endl;
      cout << WHITE << "PRINT: prints the tree" << endl;
      cout << WHITE << "SEARCH: find if a number exists" << endl;
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
      Node* added = add_to_tree(to_add, root);
      if (root->value == 0){
        root = added;
        root->parent = nullptr;
        root->color = Color::black;
        root->left->parent = root;
        root->right->parent = root;
      } else if (added != nullptr){
        fix_insert(added);
      }
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
      Node* added = add_to_tree(to_add, root);
      if (root->value == 0){
        root = added;
        root->parent = nullptr;
        root->color = Color::black;
        root->left->parent = root;
        root->right->parent = root;
      } else if (added != nullptr){
        fix_insert(added);
      }
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
      while (current_node->value != to_find){ // keep looking, print a debugging message first
	// set color
	string c;
	if (current_node->color == Color::red){
	  c = RED;
	} else {
	  c = WHITE;
	}
	// stop searching if it reaches a wall or it finds the right number
	if (current_node->value > to_find){ // too big
	  cout << "Less than " << c << current_node->value << RESET << "..." << endl;
	  current_node = current_node->left;
	} else { // too small
	  cout << "Greater than " << c << current_node->value << RESET << "..." << endl;
	  current_node = current_node->right;
	}
	if (current_node->value == 0){ // end found
	  break;
	}
      }
      // now, find out if it failed
      if (current_node->value == 0){ // failure
	cout << WHITE << "No instances of " << YELLOW << to_find << WHITE << " found." << endl;
      } else { // success
	cout << WHITE << "That number exists!" << endl;
      }
    }
  }
  cout << YELLOW << "Goodbye!" << endl;
  return 0;
}
