#include <iostream>
#include <cctype>
#include "rbtree.h"

int main(){
    using namespace std;
    int *input;
    RBTree<int> tree;
    char yes_no;
    cout << tree;
    do{
	input = new int;
	cout << "Enter the number: ";
	cin >> (*input);
	tree.insert(input, true);
	cout << tree;
	// Not a memory leak -- the old value will be cleaned up with the tree.
	cout << "Enter another? (Y/N) ";
	cin >> yes_no;
    } while (toupper(yes_no) == 'Y');
    // Now we go through removal
    
    // We don't need to continually allocate new integers on deletion.
    input = new int;
    Node<int> *tmp;
    cout << tree;
    do{
	cout << "Enter the number to remove: ";
	cin >> (*input);
	tmp = tree.remove(input);
	if (tmp)
	    cout << "Removed node with value " << *tmp->get_data() << endl;
	else
	    cout << "Node not found.\n";
	cout << tree;
	cout << "Enter another? (Y/N) ";
	cin >> yes_no;
    } while (toupper(yes_no) == 'Y');
    return 0;
}
