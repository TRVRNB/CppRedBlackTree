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
  string version = "1.8";
  Node* root = new Node(0);
}
using namespace red_black_tree;

Node* rotate(Node* to_rotate, short dir){
  // rotate this subtree; dir should be -1 (left) or 1 (right)
  // based on the --C++ version on en.wikipedia.org/wiki/Red-black_tree
  Node* old_parent = to_rotate->parent;
  Node* new_parent = to_rotate->get_child(-dir); // -dir is the opposite direction
  Node* new_child = new_parent->get_child(dir);
  to_rotate->set_child(-dir, new_child);
  new_child->parent = to_rotate;
  new_parent->set_child(dir, to_rotate);
  to_rotate->parent = new_parent;
  new_parent->parent = old_parent;
  if (old_parent != nullptr){
    if (to_rotate == old_parent->right) old_parent->right = new_parent;
    else old_parent->left = new_parent;
  } else {
    root = new_parent;
  }
  return new_parent;
}

void fix_delete(Node* child, Node* parent){
  // sadly i can't completely 'cross-reference' the wikipedia version for deletion, since it uses goto statements which are probably not allowed (i don't want to use them either way honestly)
  // this function is similar to fix_insert, it's a recursive component but the individual cases are handled by main()
  // rebalance the tree, starting from the deleted node's parent, which is not a leaf no matter what! (this does not get called in root case, except when it has two children)
  // I WILL IMPLEMENT THIS IN 1.9!

}

void fix_insert(Node* to_fix){
  // recursive function
  // rebalance the tree, starting from this leaf node
  // i think to_fix is guaranteed to be red at the start of this function
  if (to_fix->parent->color == Color::black) return; // simplest case
  Node* parent = to_fix->parent;
  Node* grandparent = parent->parent;
  if (grandparent == nullptr){
    // this means there haven't been enough elements added to need serious rebalancing, can just change parent's color and the red insertion will not affect the black-depth rule (if parent is root it should be black anyway)
    parent->color = Color::black;
    return;
  }
  short dir = -1;
  if (parent == grandparent->right) dir = 1;
  Node* uncle = grandparent->get_child(-dir);
  if (uncle->color == Color::black){
    // fix black depth
    if (to_fix == parent->get_child(-dir)){
      // case 5 (inner grandchild)
      rotate(parent, dir);
      to_fix = parent;
      parent = grandparent->get_child(dir);
    }
    rotate(grandparent, -dir);
    parent->color = Color::black;
    grandparent->color = Color::red;
    return;
  }
  parent->color = Color::black;
  uncle->color = Color::black;
  grandparent->color = Color::red;
  if (grandparent->parent != nullptr) fix_insert(grandparent);
  else {
    root = grandparent;
    grandparent->color = Color::black;
  }
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
    } // is not empty, keep searching through left
    return add_to_tree(to_add, add_to->left);
  } else { // greater than or equal to
    if (add_to->right->value == 0){
      Node* child = add_to_tree(to_add, add_to->right);
      delete add_to->right;
      add_to->right = child;
      child->parent = add_to;
      return child;
    } // is not empty, keep searching through right
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

void find_black_depth(vector<unsigned short>* black_depths, Node* current_node, unsigned short depth){
  // if this is the end of a left path, push_back the current depth
  // this is why null nodes need to be actual nodes
  if (current_node->color == Color::black) depth++;
  if (current_node->left != nullptr){ // if left exists
    find_black_depth(black_depths, current_node->left, depth);
  } else {
    black_depths->push_back(depth);
  }
  if (current_node->right != nullptr) find_black_depth(black_depths, current_node->right, depth);
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
      cout << WHITE << "DELETE: delete a number from the tree (if it exists!)" << endl;
      cout << WHITE << "CHECK: check the black-depth condition" << endl;
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
      cout << GREEN << "Enter the filename (max 80 chars): " << RESET << flush;
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
      if (root == nullptr || root->value == 0){
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
    } else if (input == "DELETE"){ // DELETE
      // deletion is extremely nontrivial in R/B tree due to the whole balancing thing
      // it was difficult already, but this is going to be much harder
      // one very hacky solution i can think of that i would definitely get a bad grade for is just unwrapping the tree into a big vector and then repeatedly calling add_to_tree() with the numbers in the vector, excluding the one DELETE was called on (you could even make it faster by sorting the vector and starting in the middle!)
      // this would work but i know for a fact it's not what i'm supposed to do, so i'll only do it if i am genuinely out of ideas
      // it'd also probably be O(n^2) which is bad time complexity for a simple data structure, even if it is for deletion (which is probably more acceptable to have bad complexity for compared to searching in a binary tree)
      // (a slightly better version of this: i could just recursively add all the children of to_delete)
      if (root->value == 0){
	cout << RED << "Add some numbers first!" << endl;
	continue;
      }
      string input; // this isn't technically in the same scope, so the original input still exists but is inaccessible here
      cout << RESET << "What number do you want to delete?: " << flush;
      cin >> input;
      unsigned short to_find = stoi(input);
      Node* current_node = root;
      // no need to keep track of previous_node due to having a parent pointer
      while (current_node->value != to_find){
	if (current_node->value > to_find){ // too high
	  current_node = current_node->left;
	} else { // too low
	  current_node = current_node->right;
	}
	if (current_node->value == 0){ // null
	  current_node = current_node->parent;
	  break; // this prevents an infinite loop
	}
      } if (current_node->value != to_find){ // failure
	cout << RED << "No instances of " << to_find << " found." << endl;
	continue;
      }
      // DELETION LOGIC
      // current_node is the node i want to delete
      Node* to_delete = current_node;
      Node* new_child = nullptr; // will be used for fix_delete
      Color color = to_delete->color;
      Node* parent = nullptr;
      
      if (to_delete == root){
	// ROOT
	if (to_delete->left->value == 0 && to_delete->right->value == 0){ // root has no children!
	  root = new Node(0);
	  delete to_delete;
	  cout << WHITE << "Deleted " << input << '.' << endl;
	  continue;
	} else if (to_delete->left->value == 0 || to_delete->right->value == 0){ // root has one child
	  if (to_delete->left->value != 0) root = to_delete->left;
	  else root = to_delete->right;
	  delete to_delete;
	  cout << WHITE << "Deleted " << input << '.' << endl;
	  continue;
	}
	// if it's still running here, treat it like a non-leaf (root but with 2 children)
      }
      // -----------------------------
      if (to_delete->left->value == 0 && to_delete->right->value == 0){
	// NO CHILDREN
	parent = to_delete->parent;
	new_child = new Node(0);
	new_child->color = Color::black; // i think it will do this by default (can't be too safe though!)
	new_child->parent = parent;
	if (to_delete == current_node->left){ // is left child
	  parent->left = new_child;
	} else { // is right child
	  parent->right = new_child;
	}
	delete to_delete;
	if (color == Color::black){ // if it was black, there is a black node missing from this path
	  fix_delete(new_child, parent);
	}
	cout << WHITE << "Deleted " << input << '.' << endl;
	continue;
	// -----------------------------
      } else if (to_delete->left->value == 0 || to_delete->right->value == 0){
	// ONE CHILD
	// this case is also very simple
	parent = to_delete->parent;
	if (to_delete->left->value != 0) child = to_delete->left; // has left child
	else child = to_delete->right; // has right child
	child->parent = parent;
	// now, update the parent's children pointers
	if (to_delete == parent->left) parent->left = child; // is left child
	else parent->right = child; // is right child
	delete to_delete; // this is safe since the only child is taken care of
	if (color == Color::black){ // same as no children
	  fix_delete(child, parent);
	}
	cout << WHITE << "Deleted " << input << '.' << endl;
	continue;
	// -----------------------------
      } else {
	// TWO CHILDREN
	// by far the most non-trivial case!
	// start by replacing to_delete with a distant nephew, use current_node to access the original
	to_delete = to_delete->right;
	while (to_delete->left->value != 0){ // while left child exists
	  to_delete = to_delete->left;
	}
	current_node->value = to_delete->value; // override the original
	parent = to_delete->parent;
	// add the rest of this in 1.9! time to go to sleep (it's thursday)
	
      }
      // -------------------------------
    } else if (input == "CHECK"){ // CHECK
      vector<unsigned short>* black_depths = new vector<unsigned short>();
      // this exists for debugging purposes, mostly
      // i will use a recursive function
      find_black_depth(black_depths, root, 0);
      cout << RESET;
      for (unsigned short depth : *black_depths){
	cout << depth << ' ';
      }
      cout << endl;
    }
  }
  cout << YELLOW << "Goodbye!" << endl;
  return 0;
}
