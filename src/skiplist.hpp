#pragma once

#include <stdlib.h>

#include <ostream>
#include <iostream>

template <typename T, int ML>
class SkipList;

template <typename T, int ML>
class SkipListNode {
	friend class SkipList<T, ML>;
public:
	
protected:

	T element;
	SkipListNode<T, ML>* forwards[ML + 1];
	
	SkipListNode() {std::cout << "boom" << std::endl;
		for (int i = 1; i <= ML; i++) {
			forwards[i] = nullptr;
		}
	}

	SkipListNode(T element) : element(element) {
		for (int i = 1; i <= ML; i++) {
			forwards[i] = nullptr;
		}
	}

private:

};

template <typename T, int ML = 16>
class SkipList : public AbstractTree<T> {
public:
	typedef SkipListNode<T, ML> NodeType;

	SkipList(T min, T max) : min(min), max(max), max_curr_level(1) {
		header = new NodeType(min);
		tail = new NodeType(max);
		for (int i = 1; i <= ML; i++) {
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

	void insert(const T & element) override {
		SkipListNode<T, ML>* update[ML];
		NodeType* currNode = header;
		for (int level = max_curr_level; level >= 1; level--) {
			while (currNode->forwards[level]->element < element) {
				currNode = currNode->forwards[level];
			}
			update[level] = currNode;
		}
		currNode = currNode->forwards[1];
		if (currNode->element == element) {
			//skip
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
	}

	void remove(const T & element) override {
		SkipListNode<T, ML>* update[ML];
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

	Optional<T> find(const T & element) const override {
		NodeType* currNode = header;
		for (int level = max_curr_level; level >= 1; level--) {
			while (currNode->forwards[level]->element < element) {
				currNode = currNode->forwards[level];
			}
		}
		currNode = currNode->forwards[1];
		if (currNode->element == element) {
			return Optional<T>(currNode->element);
		} else {
			return Optional<T>();
		}
	}

	bool empty() const override {
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
		while (uniformRandom() < p && level < ML) {
			level++;
		}
		return level;
	}
	
	T min;
	T max;
	int max_curr_level;
	SkipListNode<T, ML>* header;
	SkipListNode<T, ML>* tail;

private:
		
};
