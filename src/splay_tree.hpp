#pragma once

#include <functional>

#include "tree.hpp"

template<typename T, typename Comp>
class SplayTree;

template<typename T, typename Comp>
class SplayTreeNode {
	friend class SplayTree<T, Comp>;
public:

protected:

	T element;
	SplayTreeNode *left;
	SplayTreeNode *right;
	SplayTreeNode *parent;

	SplayTreeNode(const T& init) : element(init), left(nullptr), right(nullptr), parent(nullptr) {
	}

private:
};

template<typename T, typename Comp = std::less<T>>
class SplayTree : public AbstractTree<T> {
public:
	typedef SplayTreeNode<T, Comp> NodeType;

private:
	Comp comp;

	NodeType *root;
	unsigned long p_size;

	void left_rotate(NodeType *x) {
		NodeType *y = x->right;
		if (y) {
			x->right = y->left;
			if (y->left) y->left->parent = x;
			y->parent = x->parent;
		}

		if (!x->parent) root = y;
		else if (x == x->parent->left) x->parent->left = y;
		else x->parent->right = y;
		if (y) y->left = x;
		x->parent = y;
	}

	void right_rotate(NodeType *x) {
		NodeType *y = x->left;
		if (y) {
			x->left = y->right;
			if (y->right) y->right->parent = x;
			y->parent = x->parent;
		}
		if (!x->parent) root = y;
		else if (x == x->parent->left) x->parent->left = y;
		else x->parent->right = y;
		if (y) y->right = x;
		x->parent = y;
	}

	void splay(NodeType *x) {
		while (x->parent) {
			if (!x->parent->parent) {
				if (x->parent->left == x) right_rotate(x->parent);
				else left_rotate(x->parent);
			} else if (x->parent->left == x && x->parent->parent->left == x->parent) {
				right_rotate(x->parent->parent);
				right_rotate(x->parent);
			} else if (x->parent->right == x && x->parent->parent->right == x->parent) {
				left_rotate(x->parent->parent);
				left_rotate(x->parent);
			} else if (x->parent->left == x && x->parent->parent->right == x->parent) {
				right_rotate(x->parent);
				left_rotate(x->parent);
			} else {
				left_rotate(x->parent);
				right_rotate(x->parent);
			}
		}
	}

	void replace(NodeType *u, NodeType *v) {
		if (!u->parent) root = v;
		else if (u == u->parent->left) u->parent->left = v;
		else u->parent->right = v;
		if (v) v->parent = u->parent;
	}

	NodeType* subtree_minimum(NodeType *u) {
		while (u->left) u = u->left;
		return u;
	}

	NodeType* subtree_maximum(NodeType *u) {
		while (u->right) u = u->right;
		return u;
	}
public:

	SplayTree() : root(nullptr), p_size(0L) {
	}

	void insert(const T &key) override {
		NodeType *z = root;
		NodeType *p = nullptr;

		while (z) {
			p = z;
			if (comp(z->element, key)) z = z->right;
			else z = z->left;
		}

		z = new NodeType(key);
		z->parent = p;

		if (!p) root = z;
		else if (comp(p->element, z->element)) p->right = z;
		else p->left = z;

		splay(z);
		p_size++;
	}

	Optional<T> find(const T &key) const override {
		NodeType *z = findNode(key);

		if (z == nullptr) return Optional<T>();

		return Optional<T>(z->element);
	}

	void remove(const T &key) override {
		NodeType *z = findNode(key);
		if (!z) return;

		splay(z);

		if (!z->left) replace(z, z->right);
		else if (!z->right) replace(z, z->left);
		else {
			NodeType *y = subtree_minimum(z->right);
			if (y->parent != z) {
				replace(y, y->right);
				y->right = z->right;
				y->right->parent = y;
			}
			replace(z, y);
			y->left = z->left;
			y->left->parent = y;
		}

		delete z;
		p_size--;
	}

	const T& minimum() {
		return subtree_minimum(root)->element;
	}

	const T& maximum() {
		return subtree_maximum(root)->element;
	}

	bool empty() const override {
		return root == nullptr;
	}

	unsigned long size() const {
		return p_size;
	}

protected:

	NodeType* findNode(const T &key) const {
		NodeType *z = root;
		while (z) {
			if (comp(z->element, key)) z = z->right;
			else if (comp(key, z->element)) z = z->left;
			else return z;
		}
		return nullptr;
	}
};