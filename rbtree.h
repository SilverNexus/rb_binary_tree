/**
 * Define the red-black tree and its collective operations.
 */

#ifndef RBTREE_H
#define RBTREE_H
#include <ostream>
#include "node.h"
#include <string>
#include <cassert>

template <typename T>
class RBTree{
    private:
	Node<T> *root;
	/**
	 * Recursive function to delete all tree nodes.
	 *
	 * @param at
	 * The node we are at in cleanup
	 */
	void tree_cleanup(Node<T> *at);
	// Private, since we don't want other classes fuddling with these methods
	void rebalance_part1(Node<T> *n);
	void rebalance_part2(Node<T> *n);
	void rebalance_part3(Node<T> *n);
	void rebalance_part4(Node<T> *n);
	void remove_prepare(Node<T> *n);
	void remove_balance_part1(Node<T> *n);
	void remove_balance_part2(Node<T> *n);
	void remove_balance_part3(Node<T> *n);
	void remove_balance_part4(Node<T> *n);
	void remove_balance_part5(Node<T> *n);
	// Find the first node in the tree with the specified value
	Node<T> *find(T *data);
    public:
	RBTree();
	~RBTree();
	/**
	 * Insert a node containing specified data into the tree
	 * Calls any necessary rebalancing after inserting the node.
	 *
	 * @param data
	 * The data to insert into a node in the tree.
	 *
	 * @param only_use
	 * Indicator for whether the data has any other uses.
	 */
	void insert(T *data, bool only_use = false);
	
	/**
	 * Delete a node from the tree.
	 * Calls any necessary functions to rebalance the tree.
	 *
	 * @param node
	 * The data value whose node we remove.
	 *
	 * @return
	 * The removed node in an isolated state.
	 */
	Node<T> *remove(T *node);
	
	inline Node<T> *get_root() const{ return root; }
	
	/**
	 * Displays the contents of the binary tree.
	 *
	 * @param output
	 * The output stream to print the tree to.
	 *
	 * @param tree
	 * The tree to output.
	 *
	 * @return
	 * The ostream, so << can be chained.
	 */
	template<T> friend std::ostream &operator<<(std::ostream &output, const RBTree<T> &tree);
	/**
	 * private output traversal function to print the tree.
	 *
	 * @param output
	 * The stream to write to.
	 *
	 * @param at
	 * The node we are working with.
	 */
	void output_traversal(std::ostream &output, Node<T> *at) const;
	/**
	 * Rotates the current node with its parent,
	 * creating a right rotation to balance the tree.
	 *
	 * @param n
	 * The node for which the rotation occurs
	 */
	void rotate_right(Node<T> *n);
	/**
	 * Rotates the current node with its parent,
	 * creating a left rotation to balance the tree.
	 *
	 * @param n
	 * The node for which the rotation occurs
	 */
	void rotate_left(Node<T> *n);
};

template <typename T>
RBTree<T>::RBTree(){
    root = new Node<T>();
}

template <typename T>
RBTree<T>::~RBTree(){
    // Since we have no way we should feasibly get a null root node, don't check.
    tree_cleanup(root);
}

// Tree cleanup is easier with recursion.
template <typename T>
void RBTree<T>::tree_cleanup(Node<T> *at){
    // Depth-first traversal of the tree. We need to take out the leaves first.
    if (at->left){
	tree_cleanup(at->left);
    }
    if (at->right){
	tree_cleanup(at->right);
    }
    // Prevent us from leaking our memory from deep copies and other situations
    // where this is the only reference we have.
    if (at->is_lone_ref())
	// This should work -- we fetch the pointer and then free at that address.
	delete at->get_data();
    delete at;
}

// Refactored to assume we have an empty black node at every leaf.
template <typename T>
void RBTree<T>::insert(T *indata, bool only_use){
    // Then we can find our place in the tree.
    Node<T> *check = root;
    T *dat;
    // Yes, the assignment in the loop is intentional. We save a function call by storing it here.
    while ( (dat = check->get_data()) ){
	// For this to work, it is assumed you have oveloaded comparison operators
	if (*indata < *dat){
	    check = check->left;
	}
	else{
	    check = check->right;
	}
    }
    // I insert the data into the existing leaf instead to prevent segfaults from orphaned root.
    check->set_data(indata);
    // Change this to red, since it is now an inserted node and not a leaf node.
    check->set_color(RED);
    if (only_use)
	check->set_yes_local_dynamic();
    check->left = new Node<T>();
    check->right = new Node<T>();
    // Link back to ensure we can rebalance.
    check->left->parent = check;
    check->right->parent = check;
    rebalance_part1(check);
    // Ensure root is pointing to the right place.
    while (root->parent)
	root = root->parent;
}

template <typename T>
Node<T> *RBTree<T>::remove(T *data){
    // Find the node with that data
    Node<T> *node = find(data), *rem;
    // Verify we found our node
    if (!node)
	return 0;
    // We must check to see how many non-leaf nodes it has
    unsigned num_non_leaf = 0;
    // Check for the presence of data, since that denotes a non-leaf node
    if (node->left->get_data()){
	++num_non_leaf;
    }
    if (node->right->get_data()){
	++num_non_leaf;
    }
    // Since we have only whole numbers, this is an inexpensive comparison.
    switch (num_non_leaf){
	case 0:
	case 1:
	    // Remove the node from the tree.
	    remove_prepare(node);
	    break;
	case 2:
	    // Find either the highest value to the left or the lowest value to the right.
	    // Since our duplicate values are added to the right, it should be better in most cases to go to the left.
	    rem = node->left;
	    while (rem->get_data())
		rem = rem->right;
	    // We went one too far, back it back up to the one with data
	    rem = rem->parent;
	    // We should know that rem cannot be node -- node->left and node->right have data, so it cannot be closer than node->left
	    assert(rem != node);
	    
	    // Since we can assert that the right node has a leaf to the right, we can rebalance on this node and put this value where the other one was.
	    remove_prepare(rem);
	    
	    // Now we put this node where the removed one was.
	    rem->parent = node->parent;
	    // Check to see if we are at the root.
	    if (node->parent){
		// We are not at the root, so assign the parent to point to its new child.
		if (node->parent->left == node)
		    rem->parent->left = rem;
		else
		    rem->parent->right = rem;
	    }
	    else
		// We are at the root, so set the moved node as the entry point.
		root = rem;
	    rem->left = node->left;
	    rem->left->parent = rem;
	    rem->right = node->right;
	    rem->right->parent = rem;
	    // The moved node inherits the color of the removed node.
	    rem->set_color(node->get_color());
	    break;
    }
    // Isolate the removed node
    node->parent = 0;
    node->left = 0;
    node->right = 0;
    return node;
}

template <typename T>
Node<T> *RBTree<T>::find(T *data){
    // Only search if we have nodes.
    if (root->get_data()){
	Node<T> *at = root;
	// We can assert the the first check of at->data will be true
	do{
	    // If our data is larger the the current node, search the right side of the tree.
	    if (*at->get_data() < *data)
		at = at->right;
	    // If it is smaller, go down the left path.
	    else if (*at->get_data() > *data)
		at = at->left;
	    // If they are equal, we found our node.
	    else
		return at;
	} while (at->get_data());
    }
    // If we fall out here, the data value was not in the tree.
    return 0;
}

template <typename T>
std::ostream &operator<<(std::ostream &output, const RBTree<T> &tree){
    tree.output_traversal(output, tree.get_root());
    // And make sure we get off the previous line
    output << std::endl;
    return output;
}

// This is an in-order traversal
template <typename T>
void RBTree<T>::output_traversal(std::ostream &output, Node<T> *at) const{
    assert(at && ((at->left && at->right) || (!at->left && !at->right)));
    // Due to the design of the RB binary tree, both nodes or neither will exist.
    if (at->left){
	output_traversal(output, at->left);
    }
    
    // Output the node.
    output << at;
    
    if (at->right){
	output_traversal(output, at->right);
    }
}

/**
 * These rebalance methods have been adapted from the C code on Wikipedia,
 * which is released under the Creative Commons Attribution-ShareAlike 3.0 Unported License
 */
template <typename T>
void RBTree<T>::rebalance_part1(Node<T> *n){
    // If no parent, then the else statement will not execute.
    if (!n->parent){
	n->set_color(BLACK);
    }
    else if (n->parent->get_color() != BLACK){
	rebalance_part2(n);
    }
}

template <typename T>
void RBTree<T>::rebalance_part2(Node<T> *n){
    Node<T> *u = n->uncle();
    if (u && u->get_color() == RED){
	Node<T> *g;
	n->parent->set_color(BLACK);
	u->set_color(BLACK);
	// For uncle to exist, grandparent must exist.
	g = n->grandparent();
	g->set_color(RED);
	rebalance_part1(g);
    }
    else{
	rebalance_part3(n);
    }
}

template <typename T>
void RBTree<T>::rebalance_part3(Node<T> *n){
    Node<T> *g = n->grandparent();
    if (n == n->parent->right && n->parent == g->left){
	rotate_left(n->parent);
	n = n->left;
    }
    else if (n == n->parent->left && n->parent == g->right){
	rotate_right(n->parent);
	n = n->right;
    }
    rebalance_part4(n);
}

template <typename T>
void RBTree<T>::rebalance_part4(Node<T> *n){
    Node<T> *g = n->grandparent();
    n->parent->set_color(BLACK);
    g->set_color(RED);
    if (n == n->parent->left){
	rotate_right(g);
    }
    else{
	rotate_left(g);
    }
}

// @note -- only works when no more than 1 node is non-leaf.
// Also, we don't need to return n. The changes to n will be made
template <typename T>
void RBTree<T>::remove_prepare(Node<T> *n){
    Node<T> *child = n->right->get_data() ? n->right : n->left;
    
    // Put child in the place of n
    child->parent = n->parent;
    // Make sure that we keep the root accurate.
    if (!n->parent)
	root = child;
    else if (n == n->parent->left){
	n->parent->left = child;
    }
    else{
	n->parent->right = child;
    }
    if (child->get_data()){
	// We get to reassign only if we have a data element in child
	if (child == n->left){
	    // Ensure we aren't trampling data
	    assert(!child->right->get_data());
	    // Delete the old and assign to the new.
	    delete child->right;
	    child->right = n->right;
	    n->right->parent = child;
	}
	else{
	    // Ensure we aren't trampling data
	    assert(!child->left->get_data());
	    // Delete the old and assign to the new.
	    delete child->left;
	    child->left = n->left;
	    n->left->parent = child;
	}
    }
    else{
	// Otherwise, we clear out the other child
	if (child == n->left){
	    // We should have had right be child if it had data
	    assert(!n->right->get_data());
	    delete n->right;
	}
	else{
	    // Since established the precondition of one leaf node, this should work as well.
	    assert(!n->left->get_data());
	    delete n->left;
	}
    }
    n->left = 0;
    n->right = 0;
    n->parent = 0;
    
    if (n->get_color() == BLACK){
	if (child->get_color() == RED)
	    child->set_color(BLACK);
	else
	    remove_balance_part1(child);
    }
}

template <typename T>
void RBTree<T>::remove_balance_part1(Node<T> *n){
    if (n->parent){
	Node<T> *s = n->sibling();
	if (s->get_color() == RED){
	    n->parent->set_color(RED);
	    s->set_color(BLACK);
	    if (n == n->parent->left){
		rotate_left(n->parent);
	    }
	    else{
		rotate_right(n->parent);
	    }
	}
	if (s->left && s->right) // We crash if these aren't defined, so skip if they aren't.
	    remove_balance_part2(n);
    }
}

template <typename T>
void RBTree<T>::remove_balance_part2(Node<T> *n){
    Node<T> *s = n->sibling();
    if (n->parent->get_color() == BLACK && s->get_color() == BLACK && s->left->get_color() == BLACK && s->right->get_color() == BLACK){
	s->set_color(RED);
	remove_balance_part1(n->parent);
    }
    else
	remove_balance_part3(n);
}

template <typename T>
void RBTree<T>::remove_balance_part3(Node<T> *n){
    Node<T> *s = n->sibling();
    if (n->parent->get_color() == RED && s->get_color() == BLACK && s->left->get_color() == BLACK && s->right->get_color() == BLACK){
	s->set_color(RED);
	n->parent->set_color(BLACK);
    }
    else{
	remove_balance_part4(n);
    }
}

template <typename T>
void RBTree<T>::remove_balance_part4(Node<T> *n){
    Node<T> *s = n->sibling();
    if (s->get_color() == BLACK){
	if (n == n->parent->left && s->right->get_color() == BLACK && s->left->get_color() == RED){
	    s->set_color(RED);
	    s->left->set_color(BLACK);
	    rotate_right(s);
	}
	else if (n == n->parent->right && s->left->get_color() == BLACK && s->right->get_color() == RED){
	    s->set_color(RED);
	    s->right->set_color(BLACK);
	    rotate_left(s);
	}
    }
    remove_balance_part5(n);
}

template <typename T>
void RBTree<T>::remove_balance_part5(Node<T> *n){
    Node<T> *s = n->sibling();
    s->set_color(n->parent->get_color());
    n->parent->set_color(BLACK);
    if (n == n->parent->left){
	s->right->set_color(BLACK);
	rotate_left(n->parent);
    }
    else{
	s->left->set_color(BLACK);
	rotate_right(n->parent);
    }
}

template <typename T>
void RBTree<T>::rotate_left(Node<T> *n){
    Node<T> *saved_q = n->right, *saved_parent = n->parent;
    n->right = saved_q->left;
    saved_q->left->parent = n;
    saved_q->left = n;
    n->parent = saved_q;
    // Make sure we keep connections outside correct.
    saved_q->parent = saved_parent;
    // Make sure the tree goes to the right spot.
    if (saved_parent){
	if (saved_parent->left == n)
	    saved_parent->left = saved_q;
	else
	    saved_parent->right = saved_q;
    }
    else{
	// Adjust the root node if we rotated the top of the tree
	root = saved_q;
    }
}

template <typename T>
void RBTree<T>::rotate_right(Node<T> *n){
    Node<T> *saved_p = n->left, *saved_parent = n->parent;
    n->left = saved_p->right;
    saved_p->right->parent = n;
    saved_p->right = n;
    n->parent = saved_p;
    // Make sure we keep connections outside correct.
    saved_p->parent = saved_parent;
    // Make sure the tree goes to the right spot.
    if (saved_parent){
	if (saved_parent->left == n)
	    saved_parent->left = saved_p;
	else
	    saved_parent->right = saved_p;
    }
    else{
	// Adjust the root node if we rotated the top of the tree
	root = saved_p;
    }
}

#endif
