#pragma once

#include "tree.hpp"

// Node and forward declaration because g++ does
// not understand nested classes.
template <class T>
class AvlTree;

template <class T>
class AvlNode {
	friend class AvlTree<T>;
public:

protected:

	T element;
	AvlNode *left;
	AvlNode *right;
	int height;

	AvlNode(const T & theElement, AvlNode *lt = nullptr, AvlNode *rt = nullptr, int h = 0)
		: element(theElement), left(lt), right(rt), height(h) { }

private:

};

// AvlTree class
//
// CONSTRUCTION: with ITEM_NOT_FOUND object used to signal failed finds
//
// ******************PUBLIC OPERATIONS*********************
// void insert( x )       --> Insert x
// void remove( x )       --> Remove x
// Comparable find( x )   --> Return item that matches x
// Comparable findMin( )  --> Return smallest item
// Comparable findMax( )  --> Return largest item
// boolean isEmpty( )     --> Return true if empty; else false
// void clear( )          --> Remove all items
// void printTree( )      --> Print tree in sorted order

template <class T>
class AvlTree : public AbstractTree<T> {
public:

	AvlTree() :
		root(nullptr),
		size(0) { }

	explicit AvlTree(const T & notFound) :
		root(nullptr),
		size(0) { }

	AvlTree(const AvlTree & rhs) :
		root(nullptr),
		size(rhs.size) {
		*this = rhs;
	}

	~AvlTree() {
		clear();
	}

	const T & findMin() const {
		return elementAt(findMin(root));
	}

	const T & findMax() const {
		return elementAt(findMax(root));
	}

	Optional<T> find(const T & element) const override {
		return elementAt(find(element, root));
	}

	bool empty() const override {
		return root == nullptr;
	}

	void print(std::ostream & stream) const override {
		if (empty())
			stream << "Empty tree" << std::endl;
		else
			print(root, stream);
	}

	void clear() override {
		clear(root);
	}

	void insert(const T & x) override {
		root = insert(x, root);
	}

	void remove(const T & x) override {
		root = remove(x, root);
	}

	const AvlTree & operator=(const AvlTree & rhs) {
		if (this != &rhs) {
			clear();
			root = clone(rhs.root);
		}
		return *this;
	}

	bool isBalanced() const {
		return is_balanced(root);
	}

	int getSize() const { return size; }
	
protected:

	AvlNode<T> *root;
	
	int size;

	const Optional<T> elementAt(AvlNode<T> *t) const {
		if (t == nullptr) return Optional<T>();

		return Optional<T>(t->element);
	}

	AvlNode<T>* insert(const T & x, AvlNode<T> * & t) {
		if (t == nullptr) {
			size++;
			return new AvlNode<T>(x, nullptr, nullptr);
		}
		else if (x < t->element) {
			t->left = insert(x, t->left);
		}
		else if (t->element < x) {
			t->right = insert(x, t->right);
		}
		else {
			return nullptr;
		}
		balance(t);
		return t;
	}

	AvlNode<T> * remove(const T & x, AvlNode<T> * & t) {
		if (t == nullptr) return nullptr;

		if (x < t->element) {
			t->left = remove(x, t->left);
		} else if (t->element < x) {
			t->right = remove(x, t->right);
		} else {
			AvlNode<T>* l = t->left;
			AvlNode<T>* r = t->right;

			delete t;
			size--;

			if (!r) return l;

			AvlNode<T>* min = findMin(r);
			min->right = balance_min(r);
			min->left = l;
			AvlNode<T>* old = balance(min);
			return old;
		}
		return balance(t);
	}

	AvlNode<T> * findMin(AvlNode<T> *t) const {
		if (t == nullptr) return nullptr;

		while (t->left) {
			t = t->left;
		}
		return t;
	}

	AvlNode<T> * findMax(AvlNode<T> *t) const {
		if (t == nullptr) return nullptr;

		while (t->right) {
			t = t->right;
		}
		return t;
	}
	
	AvlNode<T> * find(const T & x, AvlNode<T> *t) const {

		while (t != nullptr) {
			if (x < t->element)
				t = t->left;
			else if (t->element < x)
				t = t->right;
			else
				return t; // Match
		}
		return nullptr; // No match
	}

	void clear(AvlNode<T> * & t) {
		if (t == nullptr) return;

		clear(t->left);
		clear(t->right);
		
		delete t;
		size--;
		t = nullptr;
	}

	void print(AvlNode<T> *t, std::ostream & stream) const {
		if (t == nullptr) return;

		print(t->left, stream);
		stream << t->element << std::endl;
		print(t->right, stream);
	}

	AvlNode<T> * clone(AvlNode<T> *t) {
		if (t == nullptr) return nullptr;

		return new AvlNode<T>(t->element, clone(t->left),
			clone(t->right), t->height);
	}

	bool is_balanced(AvlNode<T> *n) const {

		// If tree is empty then return true
		if (n == nullptr) return true;

		// Get the height of left and right sub trees
		int lh = height(n->left);
		int rh = height(n->right);

		int hdif = lh > rh ? lh - rh : rh - lh;
		if (hdif <= 1 &&
			is_balanced(n->left) &&
			is_balanced(n->right))
			return true;

		// If we reach here then tree is not height-balanced
		return false;
	}

private:

	static int max(int a, int b) { return a > b ? a : b; }

	// Avl manipulations
	int balance_factor(AvlNode<T> *t) const {
		return height(t->right) - height(t->left);
	}

	int height(AvlNode<T> *t) const {
		return t == nullptr ? -1 : t->height;
	}

	void fixheight(AvlNode<T> *t) {
		int h1 = height(t->left);
		int h2 = height(t->right);
		t->height = max(h1, h2) + 1;
	}

	AvlNode<T>* balance(AvlNode<T> * & n) {
		fixheight(n);

		//right heavy
		if (balance_factor(n) == 2) {
			//if right subtree is left heavy
			if (balance_factor(n->right) < 0)
				rotate_rl(n);
			else
				rotate_l(n);
			return n;
		}

		//left heavy
		if (balance_factor(n) == -2) {
			//if left subtree is right heavy
			if (balance_factor(n->left) > 0)
				rotate_lr(n);
			else
				rotate_r(n);
			return n;
		}
		return n;
	}

	AvlNode<T>* balance_min(AvlNode<T> * & temp) {
		if (temp->left == 0) return temp->right;
		temp->left = balance_min(temp->left);
		return balance(temp);
	}

	void rotate_r(AvlNode<T> * & node) {
		AvlNode<T> *child = node->left;
		node->left = child->right;
		child->right = node;

		fixheight(node);
		fixheight(child);
		node = child;
	}

	void rotate_l(AvlNode<T> * & node) {
		AvlNode<T> *child = node->right;
		node->right = child->left;
		child->left = node;

		fixheight(node);
		fixheight(child);
		node = child;
	}

	void rotate_lr(AvlNode<T> * & node) {
		rotate_l(node->left);
		rotate_r(node);
	}

	void rotate_rl(AvlNode<T> * & node) {
		rotate_r(node->right);
		rotate_l(node);
	}

};
