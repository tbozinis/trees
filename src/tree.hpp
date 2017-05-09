#pragma once

#include <ostream>


template <typename T>
class Optional {
public:

	Optional() : has_value(false) { }
	Optional(const T & value) : has_value(true), value(value) { }

	bool has() { return has_value; }
	T & get() { return value; }

protected:

	bool has_value;
	T value;

private:
};

template <typename T>
class AbstractTree {
public:

	virtual ~AbstractTree() { }

	virtual Optional<T> find(const T & element) = 0;

	virtual void insert(const T & element) = 0;

	virtual void remove(const T & element) = 0;

	virtual void clear() {
		throw "Not implemented";
	}

	virtual bool empty() const = 0;

	virtual void print(std::ostream & stream) const {
		throw "Not implemented";
	}

protected:

private:

};
