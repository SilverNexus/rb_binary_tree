/**
 * Author: Daniel Hawkins
 * Date: 2016-10-04
 * File: node.h
 * Description:
 * Defines the class for a binary tree node.
 *
 * Distributed under Creative Commons BY-SA 3.0 License.
 */

#ifndef NODE_H
#define NODE_H

/**
 * Define the color options for nodes of the tree
 */
enum COLOR{
    BLACK = 0,
    RED
};

/**
 * Define the class each node will be modeled after.
 *
 * @caveat
 * It is assumed that data type T can be compared using
 * standard comparison operators.
 */
template <typename T>
class Node{
    private:
	// Keep track of whether we specifically allocated space for the data.
	bool is_local_dynamic;
	// This will be private. Just to protect it.
	T *data;
	// One bit for color
	COLOR color:1;
    public:
	// These are public because it allows for quick traversal.
	// I don't want to bother with wrapping it in functions either.
	Node<T> *parent;
	Node<T> *left;
	Node<T> *right;
	Node();
	/*
	 * This constructor only does a shallow copy.
	 * To do a deep copy, make an empty node and 
	 * use the copy_data method.
	 */
	Node(T *dat);
	~Node();
	/**
	 * Makes a deep copy of the node data.
	 *
	 * @param dat
	 * The data to make the deep copy of in the node.
	 */
	void copy_data(T *dat);
	/**
	 * Returns the grandparent of this node.
	 */
	Node<T> *grandparent();
	/**
	 * Returns the sibling of the parent of this node.
	 */
	Node<T> *uncle();
	/**
	 * Returns the sibling of the current node
	 */
	Node<T> *sibling();
	// Some inline getters and setters for private members
	inline COLOR get_color(){ return color; }
	inline T *get_data(){ return data; }
	inline bool is_lone_ref(){ return is_local_dynamic; }
	inline void set_lone_ref(bool is_loc){ is_local_dynamic = is_loc; }
	inline void set_data(T *dat){ data = dat; }
	inline void set_color(COLOR col){ color = col; }
	/**
	 * Sets the flag indicating the current node's data should be deleted
	 * on the destruction of this tree.
	 * @warning only use this if the data has no other references that are used!
	 */
	inline void set_yes_local_dynamic(){ is_local_dynamic = true; }
	
	/**
	 * Displays the contents of the node.
	 *
	 * @param output
	 * The output stream to print the tree to.
	 *
	 * @param node
	 * The node to output.
	 *
	 * @return
	 * The ostream, so << can be chained.
	 */
	template<T> friend std::ostream &operator<<(std::ostream &output, const Node<T> &node);
};

template<typename T> std::ostream &operator<<(std::ostream &output, const Node<T> &node){
    // Don't try to dereference a null pointer.
    if (node.data)
	output << *node.data << "/" << (node.color == BLACK ? "B" : "R") << (!node.parent ? "/ROOT" : "") << " ";
    else
	output << "null/" << (node.color == BLACK ? "B" : "R") << (!node.parent ? "/ROOT" : "") << " ";
    return output;
}

template <typename T>
Node<T>::Node(){
    parent = 0;
    // This is a leaf node. All leaf nodes must be black.
    color = BLACK;
    left = 0;
    right = 0;
    data = 0;
    is_local_dynamic = false;
}

// Note that this is a shallow copy.
template <typename T>
Node<T>::Node(T *dat){
    parent = 0;
    // This is an inserted node. Inserted nodes are red initially.
    color = RED;
    left = 0;
    right = 0;
    data = dat;
    is_local_dynamic = false;
}

template <typename T>
Node<T>::~Node(){
}

template <typename T>
void Node<T>::copy_data(T *dat){
    data = new T();
    *data = *dat;
    is_local_dynamic = true;
}

template <typename T>
Node<T> *Node<T>::uncle(){
    Node<T> *g = grandparent();
    if (!g){
	return 0;
    }
    if (parent == g->left){
	return g->right;
    }
    else{
	return g->left;
    }
}

template <typename T>
Node<T> *Node<T>::grandparent(){
    // Since this is a member function, we can assert this node exists.
    if (parent){
	return parent->parent;
    }
    // All code paths in if statement return, so if we get here, parent was null.
    return 0;
}

template <typename T>
Node<T> *Node<T>::sibling(){
    if (parent){
	if (this == parent->left){
	    return parent->right;
	}
	else{
	    return parent->left;
	}
    }
    // All code paths in if statement return, so if we get here, parent was null.
    return 0;
}

#endif
