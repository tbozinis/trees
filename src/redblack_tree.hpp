
#include "tree.hpp"


template <class T>
class RedBlackTree;

template <class T>
class RedBlackNode {
	friend class RedBlackTree<T>;
public:

protected:

	T element;
	RedBlackNode<T> *left;
	RedBlackNode<T> *right;
	RedBlackNode<T> *parent;
	bool isRed;

	RedBlackNode(T element) : element(element), left(nullptr), right(nullptr), parent(nullptr), isRed(true) {
	}

	void recolor() {
		this->isRed = !this->isRed;
	}

	void set_left(RedBlackNode* left) {
		this->left = left;
		if (left != nullptr) left->parent = this;
	}

	void set_right(RedBlackNode* right) {
		this->right = right;
		if (right != nullptr) right->parent = this;
	}


private:

};

template <class T>
class RedBlackTree : public AbstractTree<T> {
public:

	RedBlackTree() {
		this->root = nullptr;
	}

	bool empty() const override {
		return root == nullptr;
	}

	Optional<T> find(const T & element) override {
		if (this->empty()) return Optional<T>();

		RedBlackNode<T>* node = this->find(root, element);
		if (node == nullptr) return Optional<T>();

		return Optional<T>(node->element);
	}

	void insert(const T & element) override {
		if (this->empty()) {
			this->root = new RedBlackNode<T>(element);
			this->root->recolor();
		} else {
			this->insert(this->root, element);
		}
	}

	void remove(const T & element) override {
		if (!this->empty()) this->remove(root, element);
	}

protected:

	RedBlackNode<T>* root;


	RedBlackNode<T>* find(RedBlackNode<T> *root, T element) const {
		if (root->element == element) {
			return root;
		} else if (root->element > element) {
			if (root->left == nullptr) {
				return nullptr;
			} else {
				return this->find(root->left, element);
			}
		} else {
			if (root->right == nullptr) {
				return nullptr;
			} else {
				return this->find(root->right, element);
			}
		}
	}

	void insert(RedBlackNode<T> *root, T element) {
		RedBlackNode<T>* insertedNode = nullptr;
		if (root->element == element) {
		} else if (root->element > element) {
			if (root->left == nullptr) {
				insertedNode = new RedBlackNode<T>(element);
				root->set_left(insertedNode);
			} else {
				this->insert(root->left, element);
			}
		} else {
			if (root->right == nullptr) {
				insertedNode = new RedBlackNode<T>(element);
				root->set_right(insertedNode);
			} else {
				this->insert(root->right, element);
			}
		}
		if (insertedNode == nullptr) return;
		this->balance(root);
		insertedNode = nullptr;
	}

	void remove(RedBlackNode<T> *root, T element) {
		if (root->element == element) {
			RedBlackNode<T> *leftmostFromRight;
			if (root->left == nullptr) leftmostFromRight = root->right;
			else if (root->right == nullptr) leftmostFromRight = root->left;
			else if (root->left->right == nullptr) leftmostFromRight = root->left;
			else if (root->right->left == nullptr) leftmostFromRight = root->right;
			else {
				leftmostFromRight = root->right->left;
				while (leftmostFromRight->left != nullptr) {
					leftmostFromRight = leftmostFromRight->left;
				}
			}
			if (leftmostFromRight == nullptr) {
				if (root->left != nullptr) root->left->recolor();
				else if (root->right != nullptr) root->right->recolor();
				else if (!root->isRed) reheight(root);
			} else {
				if (leftmostFromRight->left != nullptr) leftmostFromRight->left->recolor();
				else if (leftmostFromRight->right != nullptr) leftmostFromRight->right->recolor();
				else if (!leftmostFromRight->isRed) reheight(leftmostFromRight);
				if (leftmostFromRight != root->left && leftmostFromRight != root->right) leftmostFromRight->parent->set_left(leftmostFromRight->right);

				if (leftmostFromRight != root->left && root->left != nullptr) leftmostFromRight->set_left(root->left);
				if (leftmostFromRight != root->right && root->right != nullptr) leftmostFromRight->set_right(root->right);
				if (root->isRed && !leftmostFromRight->isRed) leftmostFromRight->recolor();
				if (!root->isRed && leftmostFromRight->isRed) leftmostFromRight->recolor();
			}
			if (root->parent != nullptr) {
				if (root->parent->left == root) root->parent->set_left(leftmostFromRight);
				else root->parent->set_right(leftmostFromRight);
			} else {
				this->root = leftmostFromRight;
				if (this->root != nullptr) this->root->parent = nullptr;
			}
			delete root;
		} else if (root->element > element) {
			if (root->left == nullptr) {
			} else {
				this->remove(root->left, element);
			}
		} else {
			if (root->right == nullptr) {
			} else {
				this->remove(root->right, element);
			}
		}
	}


private:


	void balance(RedBlackNode<T> *root) {
		if (root->isRed == false) return;
		if (root->parent == nullptr) return;
		if (root == root->parent->left) {
			if (root->parent->right == nullptr || !root->parent->right->isRed) this->rotate_r(root);
			else {
				root->recolor();
				root->parent->right->recolor();
				root->parent->recolor();
				if (root->parent->parent == nullptr) {
					root->parent->recolor();
					return;
				}
				balance(root->parent->parent);
			}
		}//left rotate
		else {
			if (root->parent->left == nullptr || !root->parent->left->isRed) this->rotate_l(root);
			else {
				root->recolor();
				root->parent->left->recolor();
				root->parent->recolor();
				if (root->parent->parent == nullptr) {
					root->parent->recolor();
					return;
				}
				balance(root->parent->parent);
			}
		}
	}

	void rotate_l(RedBlackNode<T> *root) {
		RedBlackNode<T> *parent = root->parent;
		if (root->left != nullptr && root->left->isRed) {
			RedBlackNode<T> *badChild = root->left;
			root->set_left(badChild->right);
			badChild->set_right(root);
			parent->set_right(badChild);
			root = badChild;
		}
		root->recolor();
		parent->recolor();
		parent->set_right(root->left);
		if (parent->parent != nullptr) {
			if (parent->parent->left == parent) parent->parent->set_left(root);
			else parent->parent->set_right(root);
		} else {
			this->root = root;
			this->root->parent = nullptr;
			root = this->root;
		}
		root->set_left(parent);
	}

	void rotate_r(RedBlackNode<T> *root) {
		RedBlackNode<T> *parent = root->parent;
		if (root->right != nullptr && root->right->isRed) {
			RedBlackNode<T> *badChild = root->right;
			root->set_right(badChild->left);
			badChild->set_left(root);
			parent->set_left(badChild);
			root = badChild;
		}
		root->recolor();
		parent->recolor();
		parent->set_left(root->right);
		if (parent->parent != nullptr) {
			if (parent->parent->left == parent) parent->parent->set_left(root);
			else parent->parent->set_right(root);
		} else {
			this->root = root;
			this->root->parent = nullptr;
			root = this->root;
		}
		root->set_right(parent);
	}

	void reheight(RedBlackNode<T> *root) {
		RedBlackNode<T>* parent = root->parent;
		if (parent == nullptr) return;
		if (root == parent->left) {
			if (parent->isRed) {
				if ((parent->right->left != nullptr && parent->right->left->isRed)
						|| (parent->right->right != nullptr && parent->right->right->isRed)) {
					rotate_l1(parent->right);
				} else {
					parent->right->recolor();
					parent->recolor();
				}
			} else {
				if (parent->right->isRed) {
					if ((parent->right->left->right != nullptr && parent->right->left->right->isRed)
							|| (parent->right->left->left != nullptr && parent->right->left->left->isRed)) {
						rotate_l2(parent->right);
					} else {
						rotate_l3(parent->right);
					}
				} else {
					if ((parent->right->left != nullptr && parent->right->left->isRed)
							|| (parent->right->right != nullptr && parent->right->right->isRed)) {
						rotate_l4(parent->right);
					} else {
						parent->right->recolor();
						if (parent->parent != nullptr) reheight(parent);
					}
				}
			}
		} else {
			if (parent->isRed) {
				if ((parent->left->left != nullptr && parent->left->left->isRed)
						|| (parent->left->right != nullptr && parent->left->right->isRed)) {
					rotate_r1(parent->left);
				} else {
					parent->left->recolor();
					parent->recolor();
				}
			} else {
				if (parent->left->isRed) {
					if ((parent->left->right->right != nullptr && parent->left->right->right->isRed)
							|| (parent->left->right->left != nullptr && parent->left->right->left->isRed)) {
						rotate_r2(parent->left);
					} else {
						rotate_r3(parent->left);
					}
				} else {
					if ((parent->left->left != nullptr && parent->left->left->isRed)
							|| (parent->left->right != nullptr && parent->left->right->isRed)) {
						rotate_r4(parent->left);
					} else {
						parent->left->recolor();
						if (parent->parent != nullptr) reheight(parent);
					}
				}
			}
		}
	}

	void rotate_l1(RedBlackNode<T> *root) {
		RedBlackNode<T> *parent = root->parent;
		if (root->left != nullptr && root->left->isRed) {
			RedBlackNode<T> *badChild = root->left;
			root->set_left(badChild->right);
			badChild->set_right(root);
			parent->set_right(badChild);
			root = badChild;
			parent->recolor();
		} else {
			root->right->recolor();
			root->recolor();
			parent->recolor();
		}
		parent->set_right(root->left);
		if (parent->parent != nullptr) {
			if (parent->parent->left == parent) parent->parent->set_left(root);
			else parent->parent->set_right(root);
		} else {
			this->root = root;
			this->root->parent = nullptr;
			root = this->root;
		}
		root->set_left(parent);
	}

	void rotate_l2(RedBlackNode<T> *root) {
		RedBlackNode<T>* parent = root->parent;
		RedBlackNode<T>* grandParent = parent->parent;
		if (root->left->left != nullptr && root->left->left->isRed) {
			RedBlackNode<T> *badChild = root->left->left;
			root->left->set_left(badChild->right);
			badChild->set_right(root->left);
			root->set_left(badChild);
			badChild->right->recolor();
			badChild->recolor();
		}
		parent->set_right(root->left->left);
		root->left->set_left(parent);
		root->set_left(root->left->right);
		root->left->recolor();
		parent->parent->set_right(root);
		if (grandParent != nullptr) {
			if (grandParent->left == parent) grandParent->set_left(parent->parent);
			else grandParent->set_right(parent->parent);
		} else {
			this->root = parent->parent;
			this->root->parent = nullptr;
		}
	}

	void rotate_l3(RedBlackNode<T> *root) {
		RedBlackNode<T>* parent = root->parent;
		root->left->recolor();
		parent->set_right(root->left);
		root->recolor();
		if (parent->parent != nullptr) {
			if (parent->parent->left == parent) parent->parent->set_left(root);
			else parent->parent->set_right(root);
		} else {
			this->root = root;
			this->root->parent = nullptr;
		}
		root->set_left(parent);
	}

	void rotate_l4(RedBlackNode<T> *root) {
		RedBlackNode<T> *parent = root->parent;
		if (root->left != nullptr && root->left->isRed) {
			RedBlackNode<T> *badChild = root->left;
			root->set_left(badChild->right);
			badChild->set_right(root);
			parent->set_right(badChild);
			root = badChild;
			root->recolor();
		} else {
			root->right->recolor();
		}
		parent->set_right(root->left);
		if (parent->parent != nullptr) {
			if (parent->parent->left == parent) parent->parent->set_left(root);
			else parent->parent->set_right(root);
		} else {
			this->root = root;
			this->root->parent = nullptr;
			root = this->root;
		}
		root->set_left(parent);
	}

	void rotate_r1(RedBlackNode<T> *root) {
		RedBlackNode<T> *parent = root->parent;
		if (root->right != nullptr && root->right->isRed) {
			RedBlackNode<T> *badChild = root->right;
			root->set_right(badChild->left);
			badChild->set_left(root);
			parent->set_left(badChild);
			root = badChild;
			parent->recolor();
		} else {
			root->left->recolor();
			root->recolor();
			parent->recolor();
		}
		parent->set_left(root->right);
		if (parent->parent != nullptr) {
			if (parent->parent->left == parent) parent->parent->set_left(root);
			else parent->parent->set_right(root);
		} else {
			this->root = root;
			this->root->parent = nullptr;
			root = this->root;
		}
		root->set_right(parent);
	}

	void rotate_r2(RedBlackNode<T> *root) {
		RedBlackNode<T>* parent = root->parent;
		RedBlackNode<T>* grandParent = parent->parent;
		if (root->right->right != nullptr && root->right->right->isRed) {
			RedBlackNode<T> *badChild = root->right->right;
			root->right->set_right(badChild->left);
			badChild->set_left(root->right);
			root->set_right(badChild);
			badChild->left->recolor();
			badChild->recolor();
		}
		parent->set_left(root->right->right);
		root->right->set_right(parent);
		root->set_right(root->right->left);
		root->right->recolor();
		parent->parent->set_left(root);
		if (grandParent != nullptr) {
			if (grandParent->left == parent) grandParent->set_left(parent->parent);
			else grandParent->set_right(parent->parent);
		} else {
			this->root = parent->parent;
			this->root->parent = nullptr;
		}
	}

	void rotate_r3(RedBlackNode<T> *root) {
		RedBlackNode<T>* parent = root->parent;
		root->right->recolor();
		parent->set_left(root->right);
		root->recolor();
		if (parent->parent != nullptr) {
			if (parent->parent->left == parent) parent->parent->set_left(root);
			else parent->parent->set_right(root);
		} else {
			this->root = root;
			this->root->parent = nullptr;
		}
		root->set_right(parent);
	}

	void rotate_r4(RedBlackNode<T> *root) {
		RedBlackNode<T> *parent = root->parent;
		if (root->right != nullptr && root->right->isRed) {
			RedBlackNode<T> *badChild = root->right;
			root->set_right(badChild->left);
			badChild->set_left(root);
			parent->set_left(badChild);
			root = badChild;
			root->recolor();
		} else {
			root->left->recolor();
		}
		parent->set_left(root->right);
		if (parent->parent != nullptr) {
			if (parent->parent->left == parent) parent->parent->set_left(root);
			else parent->parent->set_right(root);
		} else {
			this->root = root;
			this->root->parent = nullptr;
			root = this->root;
		}
		root->set_right(parent);
	}

};
