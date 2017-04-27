#pragma once

#include <ostream>

// Node and forward declaration because g++ does
// not understand nested classes.
template <class T>
class AvlTree;

template <class T>
class AvlNode
{
	T element;
	AvlNode   *left;
	AvlNode   *right;
	int       height;

	AvlNode(const T & theElement, AvlNode *lt, AvlNode *rt, int h = 0)
		: element(theElement), left(lt), right(rt), height(h) { }
	friend class AvlTree<T>;
};

// AvlTree class
//
// CONSTRUCTION: with ITEM_NOT_FOUND object used to signal failed finds
//
// ******************PUBLIC OPERATIONS*********************
// void insert( x )       --> Insert x
// void remove( x )       --> Remove x (unimplemented)
// Comparable find( x )   --> Return item that matches x
// Comparable findMin( )  --> Return smallest item
// Comparable findMax( )  --> Return largest item
// boolean isEmpty( )     --> Return true if empty; else false
// void makeEmpty( )      --> Remove all items
// void printTree( )      --> Print tree in sorted order

template <class T>
class AvlTree
{
public:
	explicit AvlTree(const T & notFound) :
		ITEM_NOT_FOUND(notFound), root(nullptr) {}
	AvlTree(const AvlTree & rhs) :
		ITEM_NOT_FOUND(rhs.ITEM_NOT_FOUND), root(nullptr) {
		*this = rhs;
	}
	~AvlTree()
	{
		makeEmpty();
	}

	const T & findMin()
	{
		return elementAt(findMin(root));
	}
	const T & findMax()
	{
		return elementAt(findMax(root));
	}
	const T & find(const T & x)
	{
		return elementAt(find(x, root));
	}
	bool isEmpty()
	{
		return root == nullptr;
	}
	void printTree(std::ostream & stream)
	{
		if (isEmpty())
			stream << "Empty tree" << std::endl;
		else
			printTree(root, stream);
	}

	void makeEmpty()
	{
		makeEmpty(root);
	}
	void insert(const T & x)
	{
		insert(x, root);
	}
	void remove(const T & x)
	{
		root = remove(x, root);
	}

	const AvlTree & operator=(const AvlTree & rhs)
	{
		if (this != &rhs)
		{
			makeEmpty();
			root = clone(rhs.root);
		}
		return *this;
	}

	bool isBalanced()
	{
		return isBalanced(root);
	}

protected:
	AvlNode<T> *root;
	const T ITEM_NOT_FOUND;

	const T & elementAt(AvlNode<T> *t)
	{
		if (t == nullptr)
			return ITEM_NOT_FOUND;
		else
			return t->element;
	}

	void insert(const T & x, AvlNode<T> * & t)
	{
		if (t == nullptr)
			t = new AvlNode<T>(x, nullptr, nullptr);
		else if (x < t->element)
		{
			insert(x, t->left);
			if (height(t->left) - height(t->right) == 2) {
				if (x < t->left->element)
					right_rotation(t);
				else
					doubleWithLeftChild(t);
			}
		}
		else if (t->element < x)
		{
			insert(x, t->right);
			if (height(t->right) - height(t->left) == 2) {
				if (t->right->element < x)
					left_rotation(t);
				else
					doubleWithRightChild(t);
			}
		}
		else
			;  // Duplicate; do nothing
		t->height = max(height(t->left), height(t->right)) + 1;
	}
	AvlNode<T> * remove(const T & x, AvlNode<T> * & t)
	{
		if (t == nullptr)
		{
			return nullptr;
		}
		else if (x < t->element)
			t->left = remove(x, t->left);
		else if (t->element < x)
			t->right = remove(x, t->right);
		else
		{
			AvlNode<T>* l = t->left;
			AvlNode<T>* r = t->right;

			delete(t);

			if (!r) return l;
			AvlNode<T>* min = findMin(r);
			min->right = balance_min(r);
			min->left = l;
			return balance(min);
		}
		return balance(t);
	}
	AvlNode<T> * findMin(AvlNode<T> *t) 
	{
		if (t == nullptr)
			return t;

		while (t->left != nullptr)
			t = t->left;
		return t;
	}
	AvlNode<T> * findMax(AvlNode<T> *t)
	{
		if (t == nullptr)
			return t;

		while (t->right != nullptr)
			t = t->right;
		return t;
	}
	AvlNode<T> * find(const T & x, AvlNode<T> *t) 
	{
		while (t != nullptr)
			if (x < t->element)
				t = t->left;
			else if (t->element < x)
				t = t->right;
			else
				return t;    // Match

		return nullptr;   // No match
	}
	void makeEmpty(AvlNode<T> * & t) 
	{
		if (t != nullptr)
		{
			makeEmpty(t->left);
			makeEmpty(t->right);
			delete t;
		}
		t = nullptr;
	}
	void printTree(AvlNode<T> *t, std::ostream & stream)
	{
		if (t != nullptr)
		{
			printTree(t->left);
			stream << t->element << std::endl;
			printTree(t->right);
		}
	}
	AvlNode<T> * clone(AvlNode<T> *t)
	{
		if (t == nullptr)
			return nullptr;
		else
			return new AvlNode<T>(t->element, clone(t->left),
				clone(t->right), t->height);
	}
	bool isBalanced(AvlNode<T> *n)
	{
		int lh; /* for height of left subtree */
		int rh; /* for height of right subtree */

				/* If tree is empty then return true */
		if (n == nullptr)
			return 1;

		/* Get the height of left and right sub trees */
		lh = height(n->left);
		rh = height(n->right);
		
		int hdif = lh > rh ? lh - rh : rh - lh;
		if (hdif <= 1 &&
			isBalanced(n->left) &&
			isBalanced(n->right))
			return 1;

		/* If we reach here then tree is not height-balanced */
		return 0;
	}

private:
	// Avl manipulations
	int balance_factor(AvlNode<T> *t)
	{
		return height(t->right) - height(t->left);
	}

	int height(AvlNode<T> *t)
	{
		return t == nullptr ? -1 : t->height;
	}
	void fixheight(AvlNode<T> *t)
	{
		int h1 = height(t->left);
		int h2 = height(t->right);
		t->height = (h1 > h2 ? h1 : h2) + 1;
	}
	AvlNode<T>* balance(AvlNode<T> * & n)
	{
		fixheight(n);
		if (balance_factor(n) == 2)
		{
			if (balance_factor(n->right) < 0)
				n->right = left_rotation(n->right);
			return right_rotation(n);
		}
		if (balance_factor(n) == 2)
		{
			if (balance_factor(n->left) > 0)
				n->left = right_rotation(n->left);
			return left_rotation(n);
		}
		return n;
	}
	AvlNode<T>* balance_min(AvlNode<T> * & temp)
	{
		if (temp->left == 0)
			return temp->right;
		temp->left = balance_min(temp->left);
		return balance(temp);
	}
	AvlNode<T>* right_rotation(AvlNode<T> * & node)
	{
		AvlNode<T> *temp = node->left;
		node->left = temp->right;
		temp->right = node;
		fixheight(node);
		fixheight(temp);

		return temp;
	}
	AvlNode<T>* left_rotation(AvlNode<T> * & node)
	{
		AvlNode<T> *temp = node->right;
		node->right = temp->left;
		temp->left = node;
		fixheight(node);
		fixheight(temp);

		return temp;
	}
	AvlNode<T>* doubleWithLeftChild(AvlNode<T> * & k3)
	{
		left_rotation(k3->left);
		return right_rotation(k3);
	}
	AvlNode<T>* doubleWithRightChild(AvlNode<T> * & k1)
	{
		right_rotation(k1->right);
		return left_rotation(k1);
	}

};
