#pragma once
/*
* C++ Program to Implement ScapeGoat Tree
*/
#include <iostream>
#include <cstdlib>
#include <cmath>
using namespace std;
/*
* Class SGTNode
*/

template<typename T>
class SGTNode
{
public:
	SGTNode<T> *right, *left, *parent;
	T value;
	int size;
	SGTNode()
	{
		value = NULL;
		right = NULL;
		left = NULL;
		parent = NULL;
		size = 0;
	}
	SGTNode(T val)
	{
		value = val;
		right = NULL;
		left = NULL;
		parent = NULL;
	}
};


/*
*   Class ScapeGoatTree
*/
template<typename T>
class ScapeGoatTree
{
private:
	SGTNode<T> *root;
	int n, q;
public:
	ScapeGoatTree()
	{
		root = NULL;
		n = 0;
	}

	/* Function to check if tree is empty */
	bool isEmpty()
	{
		return root == NULL;
	}

	/* Function to clear  tree */
	void makeEmpty()
	{
		root = NULL;
		n = 0;
	}

	/* Function to count number of nodes recursively */
	int size(SGTNode<T> *r)
	{
		if (r == NULL)
			return 0;
		else
		{
			int l = 1;
			l += size(r->left);
			l += size(r->right);
			return l;
		}
	}

	/* Functions to search for an element */
	bool search(T val)
	{
		return search(root, val);
	}

	/* Function to search for an element recursively */
	bool search(SGTNode<T> *r, T val)
	{
		bool found = false;
		while ((r != NULL) && !found)
		{
			T rval = r->value;
			if (val < rval)
				r = r->left;
			else if (val > rval)
				r = r->right;
			else
			{
				found = true;
				break;
			}
			found = search(r, val);
		}
		return found;
	}

	/* Function to return current size of tree */
	int size()
	{
		return n;
	}

	/* Function for inorder traversal */
	void inorder()
	{
		inorder(root);
	}
	void inorder(SGTNode<T> *r)
	{
		if (r != NULL)
		{
			inorder(r->left);
			cout << r->value << "   ";
			inorder(r->right);
		}
		else
			return;
	}

	/* Function for preorder traversal */
	void preorder()
	{
		preorder(root);
	}
	void preorder(SGTNode<T> *r)
	{
		if (r != NULL)
		{
			cout << r->value << "   ";
			preorder(r->left);
			preorder(r->right);
		}
		else
			return;
	}

	/* Function for postorder traversal */
	void postorder()
	{
		postorder(root);
	}
	void postorder(SGTNode<T> *r)
	{
		if (r != NULL)
		{
			postorder(r->left);
			postorder(r->right);
			cout << r->value << "   ";
		}
		else
			return;
	}

	static int const log32(int q)
	{
		double const log23 = 2.4663034623764317;
		return (int)ceil(log23 * log(q));
	}

	/* Function to insert an element */
	bool insert(T x)
	{
		/* first do basic insertion keeping track of depth */
		SGTNode<T> *u = new SGTNode<T>(x);
		int d = addWithDepth(u);
		if (d > log32(q))
		{
			/* depth exceeded, find scapegoat */
			SGTNode<T> *w = u->parent;

			if (w != NULL)
			{
				while (3 * size(w) <= 2 * size(w->parent))
					w = w->parent; 
				rebuild(w->parent);
			}
		}
		return d >= 0;
	}

	bool remove(T x) {
		if (super.remove(x)) {
			if (2 * n < q) {
				rebuild(r);
				q = n;
			}
			return true;
		}
		return false;
	}

	/* Function to rebuild tree from node u */
	void rebuild(SGTNode<T> *u)
	{
		if (u == NULL) return;
		int ns = size(u);
		SGTNode<T> *p = u->parent;
		SGTNode<T> **a = new SGTNode<T>*[ns];
		packIntoArray(u, a, 0);
		if (p == NULL)
		{
			root = buildBalanced(a, 0, ns);
			root->parent = NULL;
		}
		else if (p->right == u)
		{
			p->right = buildBalanced(a, 0, ns);
			p->right->parent = p;
		}
		else
		{
			p->left = buildBalanced(a, 0, ns);
			p->left->parent = p;
		}
	}

	/* Function to packIntoArray */
	int packIntoArray(SGTNode<T> *u, SGTNode<T> *a[], int i)
	{
		if (u == NULL)
		{
			return i;
		}
		i = packIntoArray(u->left, a, i);
		a[i++] = u;
		return packIntoArray(u->right, a, i);
	}

	/* Function to build balanced nodes */
	SGTNode<T> *buildBalanced(SGTNode<T> **a, int i, int ns)
	{
		if (ns == 0)
			return NULL;
		int m = ns / 2;
		a[i + m]->left = buildBalanced(a, i, m);
		if (a[i + m]->left != NULL)
			a[i + m]->left->parent = a[i + m];
		a[i + m]->right = buildBalanced(a, i + m + 1, ns - m - 1); \
			if (a[i + m]->right != NULL)
				a[i + m]->right->parent = a[i + m];
		return a[i + m];
	}

	/* Function add with depth */
	int addWithDepth(SGTNode<T> *u)
	{
		SGTNode<T> *w = root;
		if (w == NULL)
		{
			root = u;
			n++;
			q++;
			return 0;
		}
		bool done = false;
		int d = 0;
		do
		{
			if (u->value < w->value)
			{
				if (w->left == NULL)
				{
					w->left = u;
					u->parent = w;
					done = true;
				}
				else
				{
					w = w->left;
				}
			}
			else if (u->value > w->value)
			{
				if (w->right == NULL)
				{
					w->right = u;
					u->parent = w;
					done = true;
				}
				else
				{
					w = w->right;
				}
			}
			else
				return -1;
			d++;
		} while (!done);
		n++;
		q++;
		return d;
	}
};