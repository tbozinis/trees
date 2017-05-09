#pragma once

#include "tree.hpp"


#include <utility>
#include <algorithm>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <array>
#include <queue>
#include <exception>
#include <iosfwd>
#include <utility>

// fwd declarations
template<typename T>
class Tree234;
template<typename K>
class Node234;

template<typename K>
class Tree234 {

  public:

   class Node234 { // public nested node class Tree<K>::Node234
     private:
       friend class Tree234<K>;
       static const int MAX_KEYS;

       enum class NodeMaxItems : int { two_node=1, three_node=2, four_node=3 };

       int to_int(const NodeMaxItems x) const { return static_cast<int>(x); }


       Node234 *parent; /* parent is only used for navigation of the tree. It does not own the memory
                           it points to. */

       int totalItems; /* If 1, two node; if 2, three node; if 3, four node. */

       std::array<K, 3> keys; // in static storage not the heap.

       /*
        * For 2-nodes, children[0] is left pointer, children[1] is right pointer.
        * For 3-nodes, children[0] is left pointer, children[1] the middle pointer, and children[2] the right pointer.
        * And so on for 4-nodes.
        */

       std::array<std::unique_ptr<Node234>, 4> children;

       /*
        * Returns true if key is found in node and sets index so pNode->keys[index] == key
        * Returns false if key is if not found, and sets next to the next in-order child.
        */
       bool NodeDescentSearch(K key, int& index, int& child_index, Node234 *&next);

       int insertKey(K key);

       void connectChild(int childNum, std::unique_ptr<Node234>& child);

       // Remove key, if found, from node, shifting remaining keys to fill its gap.
       K removeKey(int index);

       /*
        * Removes child node (implictly using move ctor) and shifts its children to fill the gap. Returns child pointer.
        */
       std::unique_ptr<Node234> disconnectChild(int child_index);

       void insertChild(int childNum, std::unique_ptr<Node234> &pChild);

       /*
        * Called during remove(K keym, Node234 *).
        * Merges the 2-node children of a parent 2-node into the parent, making the parent a 4-node. The parent adopts the "grand children". The children
        * after having been merged into the parent are deleted.
        */
       Node234 *fuseWithChildren();

        public:

           Node234();
           explicit Node234(K small);
           explicit Node234(K small, K large);
           explicit Node234(K small, K middle, K large);
           constexpr const Node234 *getParent() const;

           constexpr int getTotalItems() const;
           constexpr int getChildCount() const;

           // method to help in debugging
           void printKeys(std::ostream&);

           bool findKey(K key, int& index) const;
           constexpr K getKey(int i) const;
           constexpr bool isLeaf() const;
           constexpr bool isTwoNode() const;
           constexpr bool isThreeNode() const;
           constexpr bool isFourNode() const;
           constexpr bool isEmpty() const;
  }; // end class Tree<K>::Node234

 private:

    // converts from class enum to int.
    int to_int(const typename Tree234<K>::Node234::NodeMaxItems x) const { return static_cast<int>(x); }

    /*
      Node234 *NodeDescentSearchNew(Node234 *current, K value, int& index);
     */

    std::unique_ptr<Node234>  root;

    int  tree_size;

    // implementations of the public depth-frist traversal methods
    bool DoSearch(K key, Node234 *&location, int& index);

    template<typename Functor> void DoInorderTraverse(Functor f, const std::unique_ptr<Node234>& root) const;

    template<typename Functor> void DoPostOrderTraverse(Functor f,  const std::unique_ptr<Node234>& root) const;

    template<typename Functor> void DoPreOrderTraverse(Functor f, const std::unique_ptr<Node234>& root) const;

    template<typename Functor> void DoPostOrder4Debug(Functor f, const std::unique_ptr<Node234>& root);

    void DestroyTree(std::unique_ptr<Node234> &root);

    void CloneTree(const std::unique_ptr<Node234>& pNode2Copy, std::unique_ptr<Node234> &pNodeCopy); // called by copy ctor

    void split(Node234 *node);  // called during insert(K key) to split 4-nodes encountered.

    // Called during remove(K key)
    bool remove(K key, Node234 *location);

    // Called during remove(K key, Node234 *) to convert two-node to three- or four-node during descent of tree.
    Node234 *convertTwoNode(Node234 *node);

    // These methods are called by convertTwoNode()
    Node234 *fuseSiblings(Node234 *parent, int node2_id, int sibling_id);

    Node234 *leftRotation(Node234 *p2node, Node234 *psibling, Node234 *parent, int parent_key_index);

    Node234 *rightRotation(Node234 *p2node, Node234 *psibling, Node234 *parent, int parent_key_index);

  public:

     explicit Tree234() : root{} { }

     Tree234(const Tree234& lhs);
     Tree234(Tree234&& lhs);     // move constructor

     Tree234& operator=(const Tree234& lhs);
     Tree234& operator=(Tree234&& lhs);    // move assignment

     Tree234(std::initializer_list<K> list);
     Tree234(const std::vector<K>& vec);

     constexpr int size() const;
     int getDepth() const; // get depth of tree from root to leaf.

    ~Tree234();

    // Breadth-first traversal
    template<typename Functor> void levelOrderTraverse(Functor f) const;

    // Depth-first traversals
    template<typename Functor> void inOrderTraverse(Functor f) const;
    template<typename Functor> void postOrderTraverse(Functor f) const;
    template<typename Functor> void preOrderTraverse(Functor f) const;

    // Used during development and testing
    template<typename Functor> void debug_dump(Functor f);

    bool search(K key);

    void insert(K key);

    bool remove(K key);
    void test(K key);
};

template <typename T, int A, int B>
class AbTree : public AbstractTree {
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

}

template<typename K> const int  Tree234<K>::Node234::MAX_KEYS = 3;
/*
 * Node234 constructors. Note: While all children are initialized to nullptr, this is not really necessary.
 * Instead your can simply set children[0] = nullptr, since a Node234 is a leaf if and only if children[0] == 0'
 */
template<typename K> inline  Tree234<K>::Node234::Node234()  : totalItems(0), parent(nullptr), children()
{
}

template<typename K> inline  Tree234<K>::Node234::Node234(K small)  : totalItems(1), parent(nullptr), children()
{
   keys[0] = small;
}

template<typename K> inline  Tree234<K>::Node234::Node234(K small, K middle)  : totalItems(2), parent(nullptr), children()
{
   keys[0] = small;
   keys[1] = middle;
}

template<typename K> inline  Tree234<K>::Node234::Node234(K small, K middle, K large)  : totalItems(3), parent(nullptr), children()
{
   keys[0] = small;
   keys[1] = middle;
   keys[2] = large;
}

template<typename K> inline Tree234<K>::Tree234(const Tree234<K>& lhs) : tree_size{lhs.tree_size}
{
   CloneTree(lhs.root, root);
}

// move constructor
template<typename K> inline Tree234<K>::Tree234(Tree234&& lhs) : root{std::move(lhs.root)}, tree_size{lhs.tree_size}
{
    root->parent = nullptr;
    lhs.tree_size = 0;
}

template<typename K> inline Tree234<K>::Tree234(std::initializer_list<K> il) : root(nullptr), tree_size{0}
{
    for (K& x: il) { // simply call insert(x)
          insert(x);
    }
}

template<typename K> inline Tree234<K>::Tree234(const std::vector<K>& vec) : root(nullptr), tree_size{0}
{
    for (const K& x: vec) { // simply call insert(x)
          insert(x);
    }
}

// copy assignment
template<typename K> inline Tree234<K>& Tree234<K>::operator=(const Tree234& lhs)
{
  if (root == lhs.root) { // are they the same?

       return *this;
  }

  DestroyTree(root); // free all nodes and then clone lhs.

  tree_size = lhs.tree_size;

  CloneTree(lhs.root, root);

  return *this;
}


template<typename K> inline void Tree234<K>::Node234::printKeys(std::ostream& ostr)
{
  ostr << "[";

  for(auto i = 0; i < getTotalItems(); ++i) {

      ostr << getKey(i);

      if (i < getTotalItems() - 1)       {

         ostr << ", ";
      }
  }

  ostr << "]";
}

template<typename K> inline constexpr int Tree234<K>::Node234::getTotalItems() const
{
   return totalItems;
}

template<typename K> inline constexpr K Tree234<K>::Node234::getKey(int i) const
{
    if (0 <= i && i < getTotalItems()) {

        return keys[i];
    }

    throw std::range_error{"key of Node234 not in range"};
}

template<typename K> inline bool Tree234<K>::Node234::findKey(K key, int& index) const
{
   for(index = 0; index < totalItems; ++index) {

       if (keys[index] == key) {

           return true;
       }
   }

   return false;
}

template<typename K> inline constexpr int Tree234<K>::Node234::getChildCount() const
{
   return totalItems + 1;
}

template<typename K> inline constexpr bool Tree234<K>::Node234::isTwoNode() const
{
   return (totalItems == to_int(NodeMaxItems::two_node)) ? true : false;
}

template<typename K> inline constexpr bool Tree234<K>::Node234::isThreeNode() const
{
   return (totalItems == to_int(NodeMaxItems::three_node)) ? true : false;
}

template<typename K> inline constexpr bool Tree234<K>::Node234::isFourNode() const
{
   return (totalItems == to_int(NodeMaxItems::four_node)) ? true : false;
}

template<typename K> inline constexpr bool Tree234<K>::Node234::isEmpty() const
{
   return (totalItems == 0) ? true : false;
}

template<typename K> inline constexpr int Tree234<K>::size() const
{
  return tree_size;
}

template<typename K> inline int Tree234<K>::getDepth() const
{
  int depth = 0;

  for (auto current = root.get(); current != nullptr; current = current->children[0].get()) {

       ++depth;
  }

  return depth;
}
// move assignment
template<typename K> inline Tree234<K>& Tree234<K>::operator=(Tree234&& lhs)
{
    tree_size = lhs.tree_size;

    lhs.tree_size = 0;

    root = std::move(lhs.root);

    root->parent = nullptr;
}
/*
 * F is a functor whose function call operator takes two parameters: a Node234 * and an int indicating the depth of the node from the root, which has depth 1.
 */
template<typename K> template<typename Functor> inline void Tree234<K>::levelOrderTraverse(Functor f) const
{
   if (root.get() == nullptr) return;

   // pair of: 1. Node234 pointer and 2. level of tree.
   std::queue<std::pair<const Node234*, int>> q;

   auto level = 1;

   q.push(std::make_pair(root.get(), level));

   while (!q.empty()) {

        std::pair<const Node234 *, int> pair_ = q.front();

        const Node234 *current = pair_.first;

        int tree_level = pair_.second;

        f(current, tree_level); // For example: print out all the keys in current.

        if (!current->isLeaf()) {

            for(auto i = 0; i < current->getChildCount(); ++i) {

               q.push(std::make_pair(current->children[i].get(), tree_level + 1));
            }
        }
        q.pop();
   }
}

template<typename K> template<typename Functor> inline void Tree234<K>::inOrderTraverse(Functor f) const
{
   DoInorderTraverse(f, root);
}

template<typename K> template<typename Functor> inline void Tree234<K>::postOrderTraverse(Functor f) const
{
   DoPostOrderTraverse(f, root);
}

template<typename K> template<typename Functor> inline void Tree234<K>::preOrderTraverse(Functor f) const
{
   DoPreOrderTraverse(f, root);
}

template<typename K> template<typename Functor> inline void Tree234<K>::debug_dump(Functor f)
{
   DoPostOrder4Debug(f, root);
}
/*
 * post order traversal
 */
template<typename K> template<typename Functor> void Tree234<K>::DoPostOrderTraverse(Functor f, const std::unique_ptr<Node234>& current) const
{
   if (current == nullptr) {

        return;
   }

   switch (current->totalItems) {

      case 1: // two node
            DoPostOrderTraverse(f, current->children[0]);

            DoPostOrderTraverse(f, current->children[1]);

            f(current->keys[0]);
            break;

      case 2: // three node
            DoPostOrderTraverse(f, current->children[0]);

            DoPostOrderTraverse(f, current->children[1]);

            f(current->keys[0]);

            DoPostOrderTraverse(f, current->children[2]);

            f(current->keys[1]);
            break;

      case 3: // four node
            DoPostOrderTraverse(f, current->children[0]);

            DoPostOrderTraverse(f, current->children[1]);

            f(current->keys[0]);

            DoPostOrderTraverse(f, current->children[2]);

            f(current->keys[1]);

            DoPostOrderTraverse(f, current->children[3]);

            f(current->keys[2]);

            break;
   }
}
/*
 * pre order traversal
 */
template<typename K> template<typename Functor> void Tree234<K>::DoPreOrderTraverse(Functor f, const std::unique_ptr<Node234>& current) const
{

  if (current == nullptr) {

        return;
   }

   switch (current->totalItems) {

      case 1: // two node
            f(current->keys[0]);

            DoPreOrderTraverse(f, current->children[0]);

            DoPreOrderTraverse(f, current->children[1]);

            break;

      case 2: // three node
            f(current->keys[0]);

            DoPreOrderTraverse(f, current->children[0]);

            DoPreOrderTraverse(f, current->children[1]);

            f(current->keys[1]);

            DoPreOrderTraverse(f, current->children[2]);

            break;

      case 3: // four node
            f(current->keys[0]);

            DoPreOrderTraverse(f, current->children[0]);

            DoPreOrderTraverse(f, current->children[1]);

            f(current->keys[1]);

            DoPreOrderTraverse(f, current->children[2]);

            f(current->keys[2]);

            DoPreOrderTraverse(f, current->children[3]);

            break;
   }
}

/*
 * post order traversal for debugging purposes
 */
template<typename K> template<typename Functor> void Tree234<K>::DoPostOrder4Debug(Functor f, const std::unique_ptr<Node234>& current)
{

   if (current == nullptr) {

 return;
   }

   switch (current->totalItems) {

      case 1: // two node
            DoPostOrder4Debug(f, current->children[0]);

            DoPostOrder4Debug(f, current->children[1]);

            f(current->keys[0], 0, current, root);
            break;

      case 2: // three node
            DoPostOrder4Debug(f, current->children[0]);

            DoPostOrder4Debug(f, current->children[1]);

            f(current->keys[0], 0, current, root);

            DoPostOrder4Debug(f, current->children[2]);

            f(current->keys[1], 1, current, root);
            break;

      case 3: // four node
            DoPostOrder4Debug(f, current->children[0]);

            DoPostOrder4Debug(f, current->children[1]);

            f(current->keys[0], 0, current, root);

            DoPostOrder4Debug(f, current->children[2]);

            f(current->keys[1], 1, current, root);

            DoPostOrder4Debug(f, current->children[3]);

            f(current->keys[2], 2, current, root);

            break;
   }
}
/*
 * pre-order traversal
 */
template<typename K>  void Tree234<K>::CloneTree(const std::unique_ptr<Node234>& pNode2Copy, std::unique_ptr<Node234> &pNodeCopy)
{
 if (pNode2Copy != nullptr) {

   // copy node
   switch (pNode2Copy->totalItems) {

      case 1: // two node
      {
            std::unique_ptr<Node234> tmp = std::make_unique<Node234>(pNode2Copy->keys[0]);

            pNodeCopy = std::move(tmp);

            pNodeCopy->parent = pNode2Copy->parent;

            CloneTree(pNode2Copy->children[0], pNodeCopy->children[0]);

            CloneTree(pNode2Copy->children[1], pNodeCopy->children[1]);

            break;

      }   // end case
      case 2: // three node
      {
            std::unique_ptr<Node234> tmp = std::make_unique<Node234>( pNode2Copy->keys[0], pNode2Copy->keys[1]);

            pNodeCopy = std::move(tmp);

            pNodeCopy->parent = pNode2Copy->parent;

            CloneTree(pNode2Copy->children[0], pNodeCopy->children[0]);

            CloneTree(pNode2Copy->children[1], pNodeCopy->children[1]);

            CloneTree(pNode2Copy->children[2], pNodeCopy->children[2]);

            break;
      } // end case
      case 3: // four node
      {
            std::unique_ptr<Node234> tmp = std::make_unique<Node234>( pNode2Copy->keys[0], pNode2Copy->keys[1], pNode2Copy->keys[2]);

            pNodeCopy = std::move(tmp);

            pNodeCopy->parent = pNode2Copy->parent;

            CloneTree(pNode2Copy->children[0], pNodeCopy->children[0]);

            CloneTree(pNode2Copy->children[1], pNodeCopy->children[1]);

            CloneTree(pNode2Copy->children[2], pNodeCopy->children[2]);

            CloneTree(pNode2Copy->children[3], pNodeCopy->children[3]);

            break;
      } // end case
   }
 } else {

    pNodeCopy = nullptr;
 }
}

/*
 * In order traversal
 */
//template<typename K> template<typename Functor> void Tree234<K>::DoInorderTraverse(Functor f, const Node234 *current) const
template<typename K> template<typename Functor> void Tree234<K>::DoInorderTraverse(Functor f, const std::unique_ptr<Node234>& current) const
{
   if (current == nullptr) {

 return;
   }

   switch (current->getTotalItems()) {

      case 1: // two node
            DoInorderTraverse(f, current->children[0]);

            f(current->keys[0]);

            DoInorderTraverse(f, current->children[1]);
            break;

      case 2: // three node
            DoInorderTraverse(f, current->children[0]);

            f(current->keys[0]);

            DoInorderTraverse(f, current->children[1]);

            f(current->keys[1]);

            DoInorderTraverse(f, current->children[2]);
            break;

      case 3: // four node
            DoInorderTraverse(f, current->children[0]);

            f(current->keys[0]);

            DoInorderTraverse(f, current->children[1]);

            f(current->keys[1]);

            DoInorderTraverse(f, current->children[2]);

            f(current->keys[2]);

            DoInorderTraverse(f, current->children[3]);

            break;
   }
}
/*
 * Requires: childIndex is within the range for the type of node.
 * child is not nullptr.
 * connectChild() is equivalent to doing:
 *
 * newRight->children[0] = std::move(node->children[2]);
 * newRight->children[0]->parent = newRight;
 * newRight->children[1] = std::move(node->children[3]);
 * newRight->children[1]->parent = newRight;
 *
 */
template<typename K> inline void  Tree234<K>::Node234::connectChild(int childIndex, std::unique_ptr<Node234>& child)
{
  children[childIndex] = std::move( child ); // Note: If children[childIndex] currently holds a managed pointer , it will be freed.

  if (children[childIndex] != nullptr) {

       children[childIndex]->parent = this;
  }
}

/*
 * Returns true if key is found in node, and it set index so that this->keys[index] == key.
 * Returns false if key is if not found, and it sets next to point to next child with which to continue the descent search downward (toward a leaf node), and
 * it sets child_index such that next->parent->children[child_index] == next.
 */
template<typename K> inline bool Tree234<K>::Node234::NodeDescentSearch(K value, int& index, int& child_index, Node234 *&next)
{
  for(auto i = 0; i < totalItems; ++i) {

     if (value < keys[i]) {

         next = children[i].get();
         child_index = i;  // new code. index is such that: this->children[index] == next
         return false;

     } else if (keys[i] == value) {

         index = i;
         return true;
     }
  }

  // It must be greater than the last key (because it is not less than or equal to it).
  child_index = totalItems; // new: see 'new code' comment just above.
  next = children[totalItems].get();

  return false;
}

template<typename K> inline void Tree234<K>::Node234::insertChild(int childNum, std::unique_ptr<Node234> &pChild)
{
  // shift children right in order to insert pChild

  /*
   * When insertChild() is called, totalItems will reflect the number of keys after a new key was added by insertKey(K key),
   * but before a new child was inserted using insertChild(). Therefore, the index of the last child would be totalItems - 1.
   *    For example, if the prior totalIems was 1, and we made the 2-node a 3-node by calling insertKey(key), then totalItmes
   * would be 2, but the last child index--before calling insertChild()--would still be 1, or "the new  totalItems" - 1.
   */
  for(auto i = totalItems - 1; i >= childNum ; i--) {

        children[i + 1] = std::move(children[i]); // shift child right. Calls operator=(Node234&&)
   }

  children[childNum] = std::move(pChild);

  if (!isLeaf()) {

     children[childNum]->parent = this; // reset the child's parent pointer, too.
  }

  return;
}


/*
 * Require: childIndex is within the range for the type of node.
 * Returns: child pointer.
 * Note: Always call disconnectChild() before removeItem(), or it will not work correctly because totalItems
 * will have been altered.
 */

template<typename K> inline std::unique_ptr<typename Tree234<K>::Node234> Tree234<K>::Node234::disconnectChild(int childIndex)
{
  std::unique_ptr<Node234> node = std::move(children[childIndex] ); // invokes unique_ptr<Node234> move assignment.

  // shift children (whose last 0-based index is totalItems) left to overwrite removed child i.
  for(auto i = childIndex; i < totalItems; ++i) {

       children[i] = std::move(children[i + 1]); // shift remaining children to the left. Calls unique_ptr<Node234>::operator=(unique_ptr<Node234>&&)
  }

  return node; // invokes unique_ptr<Node234> move constructor since node is an rvalue.
}
/*
 * Preconditions: node is not full, not a four node, and key is not present in node, which may or may not
 * be a leaf node.  Shifts keys in node as needed so that key will be inserted in sorted position. Returns index
 * of inserted key.
 */

template<typename K> inline int  Tree234<K>::Node234::insertKey(K key)
{
  // start on right, examine items
  for(auto i = totalItems - 1; i >= 0 ; --i) {

      if (key < keys[i]) { // if key[i] is bigger

          keys[i + 1] = keys[i]; // shift it right

      } else {

          keys[i + 1] = key; // insert new item
        ++totalItems;        // increase the total item count
          return i + 1;      // return index of inserted key.
      }
    }

    // key is smaller than all keys, so insert it at position 0
    keys[0] = key;
  ++totalItems; // increase the total item count
    return 0;
}

template<typename K> inline K Tree234<K>::Node234::removeKey(int index)
{
  K key = keys[index];

  // shift to the left all keys to the right of index to the left
  for(auto i = index; i < totalItems - 1; ++i) {

      keys[i] = keys[i + 1];
  }

  --totalItems;

  return key;
}

template<typename K> inline constexpr const typename Tree234<K>::Node234 *Tree234<K>::Node234::getParent() const
{
   return parent;
}

template<typename K> inline constexpr  bool Tree234<K>::Node234::isLeaf() const
{
   return !children[0] ? true : false;
}

template<typename K> inline Tree234<K>::~Tree234()
{
  DestroyTree(root);
}

/*
 * Post order traversal, deleting nodes
 */
template<typename K> void Tree234<K>::DestroyTree(std::unique_ptr<Node234> &current)
{
  // For Debug purposes
  Node234 *p = current.get();
  if (current == nullptr) {

      return;
  }

  for(auto i = 0; i < current->totalItems; ++i) {

        DestroyTree(current->children[i]);
   }

   current.reset(); // deletes the pointer owned by unique_ptr<Node234>.
}

template<typename K> inline bool Tree234<K>::search(K key)
{
    // make sure tree has at least one element
    if (root == nullptr) {

       return false;

    } else {
        int index;
        Node234 *location;
        return DoSearch(key, location, index);
    }
}

template<typename K>  bool Tree234<K>::DoSearch(K key, Node234 *&location, int& index)
{
  Node234 *current = root.get();
  Node234 *next;
  int child_index;

  if (!root) { // <--> if (root.get() == nullptr)

     return false;
  }

  while(true) {

      if (current->NodeDescentSearch(key, index, child_index, next)) {

          location = current;
          return true;

      } else if (current->isLeaf()) {

          return false; // wasn't found

      } else {

          current = next;
      }
  }
}

/*
 * Rather than search down the tree and then possibly promote and break up 4-nodes on the way back up, we split 4 nodes as we call NodeDescentSearch()
 * on the way down.
 * Insertion based on pseudo code at:
 * http://www.unf.edu/~broggio/cop3540/Chapter%2010%20-%202-3-4%20Trees%20-%20Part%201.ppt
 */
template<typename K> void Tree234<K>::insert(K key)
{
   if (root == nullptr) {

      root = std::make_unique<Node234>(key);
      ++tree_size;
      return;
   }

   Node234 *current = root.get();

   // Descend until a leaf node is found, splitting four nodes as they are encountered
   int child_index;

   while(true) {

       if(current->isFourNode()) {// if four node encountered, split it, moving a value up to parent.

            split(current);

            // resume search with parent.
            current = current->getParent();

       }  else if( current->isLeaf() )  {

            // done descending.
            break;

       } else { // internal node encountered

            Node234 *next;
            int index;

            if (current->NodeDescentSearch(key, index, child_index, next) ) {// return if key is already in tree

                return;
            }

            // set current to next
            current = next;
       }
    }

    // Make sure key is not in a leaf node that is 2- or 3-node.
    if ((!current->isFourNode() && current->keys[0] == key) || (current->isThreeNode() && current->keys[1] == key)) {

        return;
    }

    // current node is now a leaf and it is not full (because we split all four nodes while descending).
    current->insertKey(key);
    ++tree_size;
}
/*
 *  Split pseudocode:
 *
 *  Upon encountering a four node: split it into a 2-node
 *
 *  1. We move the largest key into a newly allocated 2-node
 *  2. We convert pnode into a 2-node, holding its smallest key, by setting totalItems to 1.
 *  3. We move the middle key up to the parent( which we know is not a 4-node; else it too would have been split)
 *  4. The two left-most children of the former 4-node become the left and right children of the 2-node holding the smallest key.
 *  5. The two right-most children of the former 4-node are moved to become the left and right children of the 2-node holding the largest key.
 *  6. Insert new data item into the original leaf node.
 *
 */
template<typename K> void Tree234<K>::split(Node234 *pnode)
{
    // remove two largest (of three total) keys...

    K itemC = pnode->keys[2];
    K itemB = pnode->keys[1];

    pnode->totalItems = 1; // This effectively removes all but the smallest key from node.

    std::unique_ptr<Node234> newRight{std::make_unique<Node234>(itemC) }; // Move largest key to what will be the new right child of split node.

    /* Note: The "bool operator()" of unique_ptr tests whether a pointer is being managed, whether get() == nullptr. */
    if (pnode->children[2] && pnode->children[3]) { // If neither are nullptr

        newRight->connectChild(0, pnode->children[2]); // set its left child to the 3rd child of node

        newRight->connectChild(1, pnode->children[3]); // set its right child to the 4th child of node

        pnode->children[2] = nullptr;
        pnode->children[3] = nullptr;
    }

    /* node's left and right children will be the two left most children of the node being split.
     * but first set node's two rightmost children to nullptr */

    // if this is the root, then root was the pnode in all the code above. It will now become the first child a new root 2-node.
    if(pnode == root.get()) {

       /* We will now create a new root unique_ptr<Node234> that is a two node using the about-to-be-former roots's middle value itemB from above. */

        root.release(); // This sets the stored pointer in unique_ptr<Node234> to nullptr. Again: pnode below is the about-to-be-former raw root pointer.

       /*
        * Since the move version of operator=(unique_ptr<t>&&) deletes the managed pointer, we first had to call release() above;
        * otherwise, pnode, the soon-to-be prior root, would have been deleted.
        */
        root = std::move(std::make_unique<Node234>(itemB));

        /* make former root, whose raw pointer is pnode, the left-most child */
        root->children[0] = std::move(std::unique_ptr<Node234>{pnode});

        root->children[0]->parent = root.get();

        root->children[1] = std::move(newRight);
        root->children[1]->parent = root.get();

    }  else { // pnode was not root, so we insert itemB into parent.

        Node234 *parent = pnode->getParent();

        int insert_index = parent->insertKey(itemB); // insert itemB into parent, and using its inserted index...

        int last_index = parent->totalItems - 1;

        // ...move its parent's connections right, starting from its last child index and stopping just before insert_index.
        for(auto i = last_index; i > insert_index; i--)  {

            parent->connectChild(i + 1, parent->children[i]);
        }

        /*
         * ...and add the newRight child
         */

        parent->connectChild(insert_index + 1,  newRight);
    }

    return;
}
/*
 * Deletion based on pseudo code from pages 50-53 of:
 *
 * www.serc.iisc.ernet.in/~viren/Courses/2009/SE286/2-3Trees-Mod.ppt
 *
 * We reduce deletion of an internal node's key to deletion of a leaf node's key by swapping the deleted key
 * with its in-order successor.
 */

template<typename K> bool Tree234<K>::remove(K key)
{
   if (root == nullptr) {

       return false;

   } else if (root->isLeaf()) { // TODO: Does the code ever pass this if-test?

         int index = 0;

         for (; index < root->getTotalItems(); ++index) {

             if (root->keys[index] == key ) {

               // * Remove key from root, if root is a leaf. This also shifts the in-order successor into
               // * its location.
                root->removeKey(index);

                if (root->isEmpty()) {

                     root.reset(); // delete root
                }

                --tree_size;
                return true;
             }
         }

         return false;

   } else {

       return remove(key, root.get());
  }
}
/*
 * Pseudo code is from slides 50-53 of: www.serc.iisc.ernet.in/~viren/Courses/2009/SE286/2-3Trees-Mod.ppt
 *
 * Deletion is similar to 2-3 trees: We "swap" the item to be deleted with its in-order successor, which
 * is always in a leaf node. "Swap" means we overwrite the item to be deleted with its in-order successor and then
 * remove the in-order successor from the leaf node.
 *
 * There is a problem, however: if the in-order successor is a 2-node leaf, this leaves an empty leaf node, resulting in an
 * unbalanced tree. To prevent this, as we descend the tree we turn 2-nodes (other than the root) into 3-nodes or
 * 4-nodes.
 *
 * There are two cases to consider:
 *
 * Case 1: If an adjacent sibling has 2 or 3 items (and the parent is a 3- or 4-node), we "steal" an item from sibling by
 * rotating items and moving subtree. See slide #51 at www.serc.iisc.ernet.in/~viren/Courses/2009/SE286/2-3Trees-Mod.ppt
 *
 * Case 2: If each adjacent sibling (there are at most two) has only one item (and parent is a 3- or 4-node),
 * we fuse together the two siblings, plus an item we bring down from parent, forming a 4-node and shifting all children effected appropriately.
 *
 * See slide 52 at:
 *     www.serc.iisc.ernet.in/~viren/Courses/2009/SE286/2-3Trees-Mod.ppt
 *
 * An even more thorough explanationed illustrated with a several working examples is at pages 64-66 of
 *    http://www2.thu.edu.tw/~emtools/Adv.%20Data%20Structure/2-3,2-3-4%26red-blackTree_952.pdf
 * http://www.cs.ubc.ca/~liorma/cpsc320/files/B-trees.pdf

 New untested prospective code for remove(K key, Node234 *). This is the remove code for the case when the root is not a leaf node.
 */
template<typename K> bool Tree234<K>::remove(K key, Node234 *current)
{
   Node234 *next = nullptr;
   Node234 *pfound_node = nullptr;
   int key_index;
   int child_index;

   // Search, looking for key, converting 2-nodes encountered into 3- or 4-nodes. After the conversion, the node is searched for the key and, if not found
   //   the  cursor is advanced.
   while(true) {

       if (current == nullptr) {

            return false;

       } else if (current != root.get() && current->isTwoNode()) { // got rid of: current != root.get() && current->isTwoNode()

            // If not the root, convert 2-nodes encountered while descending into 3- or 4-nodes... We special case the root inside of convertTwoNode().
            current = convertTwoNode(current); // ..and resume the key search with the now converted node.

           continue;

       } else if (current->NodeDescentSearch(key, key_index, child_index, next)) { // ...search for item in current node.

            pfound_node = current;
            break; // We found it.

       } else {
          // ... If not found, continue to descend.
           current = next;
           continue;
       }
  }

  if (current == nullptr) {

       return false; // key not found.
  }

  if (!pfound_node->isLeaf()) {// The key is in an internal node, search for its in order successor, converting any 2-nodes encountered.

      // The in-order successor(the next largest item in the tee) wil be the smallest item in the subtree rooted at
      // found_node->children[found_index + 1], which will be the first key in left-most leaf node of the subtree.
      current = pfound_node->children[key_index + 1].get();

      while (true) {

        if (current->isTwoNode()) {

             current = convertTwoNode(current);

             // Check if key move as a result of conversion?
             // Comments:
             // pfound_node is never a 2-node since remove( K key, Node234 *) first converts any 2-nodes to 3- or 4-nodes before calling
             // NodeDescentSearch()--except in the case when the root is a 2-node. The root does not get immediately converted from a 2-node
             // But this code handles that by detecting that the key has moved and recursively calling "remove(key, pfound_node)".
             // pfound_node is not deleted if pfound_node is the root (and the root is a 2-node), and no nodes get deleted when either a
             // rightRotation or leftRotation occurs. So pfound_node is safe then. Finally, pfound_node is notr deleted during fuseSiblings().
             // fuseSiblings() deletes a 2-node sibling but not pfound_node.
             if (pfound_node->getTotalItems() - 1 < key_index || pfound_node->keys[key_index] != key) { // then key moved

                 // ...simply recurse, starting with a new initial starting point of pfound_node.
                 return remove(key, pfound_node);
             }
        }

        if (current->isLeaf()) {

            break;
        }

        child_index = 0; // This must be set inside this loop, as it is used below.
        current = current->children[child_index].get(); // set current to left most child of the node,
     }

  }  else { // pfound_node is a leaf that has already been converted, if it was a 2-node. The node therefore does not need to be freed.
            // So simply call removeKey()

      pfound_node->removeKey(key_index); // We don't need to free pfound_node because it is not a 2-node.

      --tree_size;
      return true;
 }

  // We have the item found in pfound_node->keys[key_index], which is an internal node. We have current->keys[0] as in order successor leaf node, and we know
  // current it is not a leaf node.  So we "swap" the in order successor and the key at pfound_node->keys[key_index].
  // Note: We don't actually save temporarily save the key to be deleted tmp and then overwrite the former in-order successor with it. Instead we simply delete
  // the former in-order successor key.
  // K tmp = pfound_node->keys[key_index]; See Note above

  pfound_node->keys[key_index] = current->keys[0];

  // current->keys[0] = tmp; See Note above.

  current->removeKey(0); // Since current is not a 2-node, it does not need to be freed.
  --tree_size;

  return true;
}
/*
 * Requires: node is 2-node.
 * Promises: node is converted into either a 3- or a 4-node.
 *
 * Code follows pages 51-53 of: www.serc.iisc.ernet.in/~viren/Courses/2009/SE286/2-3Trees-Mod.ppt
 * and pages 64-66 of http://www2.thu.edu.tw/~emtools/Adv.%20Data%20Structure/2-3,2-3-4%26red-blackTree_952.pdf
 *
 * Case 1: If an adjacent sibling--there are at most two--has 2 or 3 items, "steal" an item from the sibling by
 * rotating items and shifting children. See slide 51 of www.serc.iisc.ernet.in/~viren/Courses/2009/SE286/2-3Trees-Mod.ppt
 *
 * Case 2: If each adjacent sibling has only one item (and parent is a 3- or 4-node), we take its sole item together with an item from
 * parent and fuse them into the 2-node, making a 4-node. If the parent is also a 2-node (this only happens in the case of the root),
 * we fuse the three together into a 4-node. In either case, we shift the children as required.
 *
 */
template<typename K> typename Tree234<K>::Node234 *Tree234<K>::convertTwoNode(Node234 *node)
{
   Node234 *convertedNode;
   Node234 *parent = node->getParent();

   int parentKeyTotal = parent->getTotalItems();
   int parentChildrenTotal = parent->getChildCount();

   // First, we find the index of the 2-node such that parent->children[node2_index] == node, by comparing node's key to its
   // parent's keys.
   int node2_index = 0;

   for (; node2_index < parentKeyTotal; ++node2_index) {
       /*
        * If we never break, then node->keys[0] is greater than the last key of its parent, which means
        * node == parent->children[parent->totalItems], the last child.
        */

       if (node->keys[0] < parent->keys[node2_index] ) {
            break;
       }
   }

   // Determine if any adjacent sibling has a 3- or 4-node, giving preference to the right adjacent sibling first.
   bool has3or4NodeSibling = false;
   int sibling_index;

   int left_adjacent = node2_index - 1;
   int right_adjacent = node2_index  + 1;

   if (right_adjacent < parentChildrenTotal && !parent->children[right_adjacent]->isTwoNode()) {

 has3or4NodeSibling = true;
        sibling_index = right_adjacent;

   } else if (left_adjacent >= 0 && !parent->children[left_adjacent]->isTwoNode()) {

 has3or4NodeSibling = true;
        sibling_index = left_adjacent;

   } else if (right_adjacent < parentChildrenTotal) { // There are no 3- or 4-nodes siblings. Therefore the all siblings
                                                      // are 2-node(s).
        sibling_index = right_adjacent;

   } else { // sibling is to the left.

        sibling_index = left_adjacent;
   }

   // Determine, based on whether the parent is a two node, whether to rotate or fuse.
   // Check if its parent 2-node (or 3- or 4-node).

   if (has3or4NodeSibling == false) { // All adjacent siblings are also 2-nodes...

        if (parent->isTwoNode()) { //... as is the parent, which must be root; otherwise, it would have already been converted.

      convertedNode = parent->fuseWithChildren();

        } else { // parent is 3- or 4-node and there a no 3- or 4-node adjacent siblings

             convertedNode = fuseSiblings(parent, node2_index, sibling_index);
        }

   } else { // it has a 3- or 4-node sibling.

      Node234 *psibling = parent->children[sibling_index].get();

      Node234 *p2node = parent->children[node2_index].get();

      /*
       * First we get the index of the parent's key value such that either
       *
       *   parent->children[node2_index]->keys[0]  <  parent->keys[index] <  parent->children[sibling_id]->keys[0]
       *
       *  or
       *
       *    parent->children[sibling_id]->keys[0]  <  parent->keys[index] <  parent->children[node2_index]->keys[0]
       *
       * by taking the minimum of the indecies.
       */

      int parent_key_index = std::min(node2_index, sibling_index);

      if (node2_index > sibling_index) { /* If sibling is to the left, then
                                    *
                                    *  parent->children[sibling_id]->keys[0] < parent->keys[index] < parent->children[node2_index]->keys[0]
                                    *
                                    * and we do a right rotation
                                    */

          convertedNode = rightRotation(p2node, psibling, parent, parent_key_index);

      } else { /* else sibling is to the right and
                *    parent->children[node2_index]->keys[0]  <  parent->keys[index] <  parent->children[sibling_id]->keys[0]
                * therefore do a left rotation
           */

          convertedNode = leftRotation(p2node, psibling, parent, parent_key_index);
      }
   }

   return convertedNode;
}
/*
 * Requirements:
 * 1. Parent node is a 2-node, and its two children are also both 2-nodes. Parent must be the tree's root (this is an inherent property of the
 *    2 3 4 tree insertion algorithm).
 *
 * Promises:
 * 1. 4-node resulting from fusing of the two 2-nodes' keys into the parent.
 * 2. Deletion of the 2-node children from the tree
 * 3. Adoption of the 2-node children's children as children of parent.
 *
 * Pseudo code:
 *
 * 1. Absorbs its children's keys as its own.
 * 2. Makes its grandchildren its children and deletes its former, now orphaned child nodes.
 */
template<typename K> typename Tree234<K>::Node234 *Tree234<K>::Node234::fuseWithChildren()
{
  // move key of 2-node
  keys[1] = keys[0];

  // absorb children's keys
  keys[0] = children[0]->keys[0];
  keys[2] = children[1]->keys[0];

  totalItems = 3;

  std::unique_ptr<Node234> leftOrphan = std::move(children[0]);
  std::unique_ptr<Node234> rightOrphan = std::move(children[1]);

  // make grandchildren the children.
  connectChild(0, leftOrphan->children[0]); // connectChild() will also reset parent pointer of right parameter.
  connectChild(1, leftOrphan->children[1]);
  connectChild(2, rightOrphan->children[0]);
  connectChild(3, rightOrphan->children[1]);

  return this;

}// <-- leftOrphan and rightOrphan are automatically deleted because they are unique_ptr<Node234> pointers

/*
 * Requires: sibling is to the left, therefore: parent->children[sibling_id]->keys[0] < parent->keys[index] < parent->children[node2_index]->keys[0]
 */
template<typename K> typename Tree234<K>::Node234 *Tree234<K>::rightRotation(Node234 *p2node, Node234 *psibling, Node234 *parent, int parent_key_index)
{
  // Add the parent's key to 2-node, making it a 3-node

  // 1. But first shift the 2-node's sole key right one position
  p2node->keys[1] = p2node->keys[0];

  p2node->keys[0] = parent->keys[parent_key_index];  // 2. Now bring down parent key

  p2node->totalItems = to_int(Tree234<K>::Node234::NodeMaxItems::three_node); // 3. increase total items

  int total_sibling_keys = psibling->totalItems;

  // 4. disconnect right-most child of sibling
  std::unique_ptr<Node234> pchild_of_sibling = psibling->disconnectChild(total_sibling_keys);

  K largest_sibling_key = psibling->removeKey(total_sibling_keys - 1); // remove the largest, the right-most, sibling's key.

  parent->keys[parent_key_index] = largest_sibling_key;  // 5. overwrite parent item with largest sibling key

  p2node->insertChild(0, pchild_of_sibling); // add former right-most child of sibling as its first child

  return p2node;
}
/* Requires: sibling is to the right therefore: parent->children[node2_index]->keys[0]  <  parent->keys[index] <  parent->children[sibling_id]->keys[0]
 * Do a left rotation
 */
template<typename K> typename Tree234<K>::Node234 *Tree234<K>::leftRotation(Node234 *p2node, Node234 *psibling, Node234 *parent, int parent_key_index)
{
  // pnode2->keys[0] doesn't change.
  p2node->keys[1] = parent->keys[parent_key_index];  // 1. insert parent key making 2-node a 3-node

  p2node->totalItems = to_int(Tree234<K>::Node234::NodeMaxItems::three_node);// 3. increase total items

  std::unique_ptr<Node234> pchild_of_sibling = psibling->disconnectChild(0); // disconnect first child of sibling.

  // Remove smallest key in sibling
  K smallest_sibling_key = psibling->removeKey(0);

  parent->keys[parent_key_index] = smallest_sibling_key;  // overwrite parent item with it.

  // add former first child of silbing as right-most child of our 3-node.
  p2node->insertChild(p2node->totalItems, pchild_of_sibling);

  return p2node;
}
/*
 * Requirements:
 *
 * 1. parent is a 3- or 4-node.
 * 2. parent->children[node2_index] and parent->children[sibling_index] are both 2-nodes
 *
 * Promises:
 *
 * 1. The 2-node at parent->children[node2_index] is converted into a 4-node by fusing it with the 2-node at parent->children[sibling_index] along with
 *    a key from the parent located at parent->keys[parent_key_index]
 *
 * 2. The 2-node sibling at parent->children[silbing_index] is then deleted from the tree, and its children are connected to the converted 2-node (into a 4-node)
 *
 * 3. parent->childen[node2_id] is the 2-node being converted (into a 3- or 4-node).
 *
 * 4. The parent becomes either a 2-node, if it was a 3-node, or a 2-node if it was a 4-node?
 *
 */
template<typename K> typename Tree234<K>::Node234 *Tree234<K>::fuseSiblings(Node234 *parent, int node2_index, int sibling_index)
{
  Node234 *psibling;

  Node234 *p2node = parent->children[node2_index].get();

  // First get the index of the parent's key value to be stolen and added into the 2-node
  int parent_key_index = std::min(node2_index, sibling_index);

  if (node2_index > sibling_index) { // sibling is to the left:

      /* Adjust parent:
         1. Remove parent key (and shift its remaining keys and reduce its totalItems)
         2. Reset parent's children pointers after removing sibling.
       * Note: There is a potential insidious bug: disconnectChild depends on totalItems, which removeKey() reduces. Therefore,
       * disconnectChild() must always be called before removeKey().
       */
      std::unique_ptr<Node234> psibling = parent->disconnectChild(sibling_index); // This will do #2.

      K parent_key = parent->removeKey(parent_key_index); //this will do #1

      // Now, add both the sibling's and parent's key to 2-node

      // 1. But first shift the 2-node's sole key right two positions
      p2node->keys[2] = p2node->keys[0];

      p2node->keys[1] = parent_key;  // 2. bring down parent key

      p2node->keys[0] = psibling->keys[0]; // 3. insert adjacent sibling's sole key.

      p2node->totalItems = 3; // 3. increase total items

      // Add sibling's children to the former 2-node, now 4-node...

      p2node->children[3] = std::move(p2node->children[1]);  // ... but first shift its children right two positions
      p2node->children[2] = std::move(p2node->children[0]);

      // Insert sibling's first two child. Note: connectChild() will also reset the parent pointer of these children (to be p2node).
      p2node->connectChild(1, psibling->children[1]);
      p2node->connectChild(0, psibling->children[0]);

   // <-- automatic deletion of psibling in above after } immediately below
  } else { // sibling is to the right:


      /* Next adjust parent:
         1. Remove parent key (and shift its remaining keys and reduce its totalItems)
         2. Reset its children pointers
       * Note: There is a potential insidious bug: disconnectChild depends on totalItems, which removeKey reduces. Therefore,
       * disconnectChild() must always be called before removeKey(), or children will not be shifted correctly.
       */
      std::unique_ptr<Node234> psibling = parent->disconnectChild(sibling_index); // this does #2

      K parent_key = parent->removeKey(parent_key_index); // this will #1

      // p2node->key[0] is already in the correct position
      p2node->keys[1] = parent_key;  // 1. bring down parent key

      p2node->keys[2] = psibling->keys[0];// 2. insert sibling's sole key.

      p2node->totalItems = 3; // 3. make it a 4-node

      // Insert sibling's last two child. Note: connectChild() will also reset the parent pointer of these children (to be p2node).

      p2node->connectChild(3, psibling->children[1]);  // Add sibling's children
      p2node->connectChild(2, psibling->children[0]);

  } // <-- automatic deletion of psibling

  return p2node;
}
