#pragma once

#include <ostream>

template <class T, int MAXLEVEL>
class SkipList;

template <class T, int MAXLEVEL>
class SkipListNode {
	friend class SkipList<T, MAXLEVEL>;
public:
	
protected:

	T element;
	SkipListNode<T, MAXLEVEL>* forwards[MAXLEVEL + 1];
	
	SkipListNode() {
		for (int i = 1; i <= MAXLEVEL; i++) {
			forwards[i] = nullptr;
		}
	}

	SkipListNode(T element) : element(element) {
		for (int i = 1; i <= MAXLEVEL; i++) {
			forwards[i] = nullptr;
		}
	}

private:

};

template <class T, int MAXLEVEL = 16>
class SkipList {
public:
	typedef SkipListNode<T, MAXLEVEL> NodeType;

	SkipList(T min, T max) : header(nullptr), tail(nullptr),
	max_curr_level(1), min(min), max(max) {
		header = new NodeType(min);
		tail = new NodeType(max);
		for (int i = 1; i <= MAXLEVEL; i++) {
			header->forwards[i] = tail;
		}
	}

	virtual ~SkipList() {
		NodeType* currNode = header->forwards[1];
		while (currNode != tail) {
			NodeType* tempNode = currNode;
			currNode = currNode->forwards[1];
			delete tempNode;
		}
		delete header;
		delete tail;
	}

	void insert(T element) {
		SkipListNode<T, MAXLEVEL>* update[MAXLEVEL];
		NodeType* currNode = header;
		for (int level = max_curr_level; level >= 1; level--) {
			while (currNode->forwards[level]->element < element) {
				currNode = currNode->forwards[level];
			}
			update[level] = currNode;
		}
		currNode = currNode->forwards[1];
		if (currNode->element == element) {
			return false;
		} else {
			int newlevel = randomLevel();
			if (newlevel > max_curr_level) {
				for (int level = max_curr_level + 1; level <= newlevel; level++) {
					update[level] = header;
				}
				max_curr_level = newlevel;
			}
			currNode = new NodeType(element);
			for (int lv = 1; lv <= max_curr_level; lv++) {
				currNode->forwards[lv] = update[lv]->forwards[lv];
				update[lv]->forwards[lv] = currNode;
			}
		}
		return true;
	}

	void erase(T element) {
		SkipListNode<T, MAXLEVEL>* update[MAXLEVEL];
		NodeType* currNode = header;
		for (int level = max_curr_level; level >= 1; level--) {
			while (currNode->forwards[level]->element < element) {
				currNode = currNode->forwards[level];
			}
			update[level] = currNode;
		}
		currNode = currNode->forwards[1];
		if (currNode->element == element) {
			for (int lv = 1; lv <= max_curr_level; lv++) {
				if (update[lv]->forwards[lv] != currNode) {
					break;
				}
				update[lv]->forwards[lv] = currNode->forwards[lv];
			}
			delete currNode;
			// update the max level
			while (max_curr_level > 1 && header->forwards[max_curr_level] == nullptr) {
				max_curr_level--;
			}
		}
	}

	const NodeType* find(T element) {
		NodeType* currNode = header;
		for (int level = max_curr_level; level >= 1; level--) {
			while (currNode->forwards[level]->element < element) {
				currNode = currNode->forwards[level];
			}
		}
		currNode = currNode->forwards[1];
		if (currNode->element == element) {
			return currNode;
		} else {
			return nullptr;
		}
	}

	bool empty() const {
		return ( header->forwards[1] == tail);
	}
	
	void print(std::ostream stream) {
		NodeType* currNode = header->forwards[1];
		while (currNode != tail) {
			stream << "(" << currNode->element << ")" << std::endl;
			currNode = currNode->forwards[1];
		}
	}

protected:

	double uniformRandom() {
		return rand() / double(RAND_MAX);
	}

	int randomLevel() {
		int level = 1;
		double p = 0.5;
		while (uniformRandom() < p && level < MAXLEVEL) {
			level++;
		}
		return level;
	}
	
	T min;
	T max;
	int max_curr_level;
	SkipListNode<T, MAXLEVEL>* header;
	SkipListNode<T, MAXLEVEL>* tail;

private:
		
};
