/**
 * Author: Daniel Hawkins
 * Date: 2016-11-17
 * File: main.cpp
 * Description: File to test functionality of the binary tree.
 *
 * Distributed under Creative Commons BY-SA 3.0 License
 */

#include <iostream>
#include <cctype>
#include "rbtree.h"

char do_menu();

int main(){
    using namespace std;
    int *input;
    RBTree<int> tree;
    char yes_no;
    do{
	yes_no = do_menu();
	cout << tree;
	switch (yes_no){
	    case '1':
		input = new int;
		cout << "Enter the number: ";
		cin >> (*input);
		tree.insert(input, true);
		cout << tree;
		// Not a memory leak -- the old value will be cleaned up with the tree.
		break;
	    case '2':
		// We don't need to continually allocate new integers on deletion.
		input = new int;
		Node<int> *tmp;
		do{
		    cout << "Enter the number to remove: ";
		    cin >> (*input);
		    tmp = tree.remove(input);
		    if (tmp)
			cout << "Removed node with value " << *tmp->get_data() << endl;
		    else
			cout << "Node not found.\n";
		    cout << tree;
		} while (!tmp);
		delete tmp;
		delete input;
		break;
	    default:
		;
	}
	cin.ignore(1, '\n');
    } while (yes_no != '3');
    // Tree is cleaned up on program termination.
    return 0;
}

char do_menu(){
    using namespace std;
    char input;
    do{
	cout << "Menu\n"
	     << "-------------------\n"
	     << "1. Add a node\n"
	     << "2. Remove a node\n"
	     << "3. Quit\n"
	     << "-------------------\n"
	     << "\nEnter your selection: ";
	cin.get(input);
    } while (input < '1' || input > '3');
    return input;
}
