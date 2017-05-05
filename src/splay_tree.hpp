#pragma once

#include <functional>

#include "tree.hpp"

template<typename T>
class SplayTree;

template<typename T>
class SplayTreeNode {
	friend class SplayTree<T>;
public:

protected:

	T element;
	SplayTreeNode *left;
	SplayTreeNode *right;

	SplayTreeNode(const T& init) : element(init), left(nullptr), right(nullptr) {
	}

private:
};

template <typename T>
class SplayTree : public AbstractTree<T>
{
    public:
		typedef SplayTreeNode<T> splay;

		splay *root = nullptr;

        SplayTree()
        {
        }

        // RR(Y rotates to the right)
        splay* RR_Rotate(splay* k2)
        {
            splay* k1 = k2->left;
            k2->left = k1->right;
            k1->right = k2;
            return k1;
        }

        // LL(Y rotates to the left)
        splay* LL_Rotate(splay* k2)
        {
            splay* k1 = k2->right;
            k2->right = k1->left;
            k1->left = k2;
            return k1;
        }

        // An implementation of top-down splay tree
        splay* Splay(T element, splay* root)
        {
            if (!root)
                return nullptr;
            splay header(element);
            /* header.right points to L tree;
            header.left points to R Tree */
            header.left = header.right = nullptr;
            splay* LeftTreeMax = &header;
            splay* RightTreeMin = &header;
            while (1)
            {
                if (element < root->element)
                {
                    if (!root->left)
                        break;
                    if (element < root->left->element)
                    {
                        root = RR_Rotate(root);
                        // only zig-zig mode need to rotate once,
                        if (!root->left)
                            break;
                    }
                    /* Link to R Tree */
                    RightTreeMin->left = root;
                    RightTreeMin = RightTreeMin->left;
                    root = root->left;
                    RightTreeMin->left = nullptr;
                }
                else if (element > root->element)
                {
                    if (!root->right)
                        break;
                    if (element > root->right->element)
                    {
                        root = LL_Rotate(root);
                        // only zag-zag mode need to rotate once,
                        if (!root->right)
                            break;
                    }
                    /* Link to L Tree */
                    LeftTreeMax->right = root;
                    LeftTreeMax = LeftTreeMax->right;
                    root = root->right;
                    LeftTreeMax->right = nullptr;
                }
                else
                    break;
            }
            /* assemble L Tree, Middle Tree and R tree */
            LeftTreeMax->right = root->left;
            RightTreeMin->left = root->right;
            root->left = header.right;
            root->right = header.left;
            return root;
        }

        splay* New_Node(T element)
        {
            splay* p_node = new splay(element);
            return p_node;
        }

        void insert(const T & element) override
        {
            static splay* p_node = nullptr;
            if (!p_node)
                p_node = New_Node(element);
            else
                p_node->element = element;
            if (!root)
            {
                root = p_node;
                p_node = nullptr;
                return;
            }
            root = Splay(element, root);
            /* This is BST that, all elements <= root->element is in root->left, all elements >
            root->element is in root->right. */
            if (element < root->element)
            {
                p_node->left = root->left;
                p_node->right = root;
                root->left = nullptr;
                root = p_node;
            }
            else if (element > root->element)
            {
                p_node->right = root->right;
                p_node->left = root;
                root->right = nullptr;
                root = p_node;
            }
            else
                return;
            p_node = nullptr;
        }

        void remove(const T & element)
        {
            splay* temp;
            if (!root)
                return;
            root = Splay(element, root);
            if (element != root->element)
                return;
            else
            {
                if (!root->left)
                {
                    temp = root;
                    root = root->right;
                }
                else
                {
                    temp = root;
                    /*Note: Since element == root->element,
                    so after Splay(element, root->left),
                    the tree we get will have no right child tree.*/
                    root = Splay(element, root->left);
                    root->right = temp->right;
                }
                delete temp;
                return;
            }
        }

        splay* Search(T element, splay* root)
        {
            return Splay(element, root);
        }

		Optional<T> find(const T & element) override {
			splay *node = Search(element, root);
			if (node) {
				return Optional<T>(node->element);
			} else {
				return Optional<T>();
			}
		}

		virtual bool empty() const override {
			return root == nullptr;
		}
};
