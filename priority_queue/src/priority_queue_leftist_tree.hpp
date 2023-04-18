#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu 
{

/**
 * a container like std::priority_queue which is a heap internal.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue 
{
	struct Node 
	{
		T value;
		Node* lchild = nullptr;
		Node* rchild = nullptr;
		int dist = 0;
	};
	Node* head = nullptr;
	size_t Size = 0;
	// return dist of a Node
	int dist(Node* x)
	{
		if (x == nullptr) return -1;
		return x->dist;
	}
	// meld two heaps
	Node* meld(Node* x, Node* y)
	{
		if (x == nullptr) return y;
		if (y == nullptr) return x;
		try
		{
			Compare cmp;
			if (cmp(x->value, y->value))
				std::swap(x, y);
		}
		catch(...)
		{
			throw runtime_error();
		}
		x->rchild = meld(x->rchild, y);
		if (dist(x->rchild) > dist(x->lchild))
			std::swap(x->lchild, x->rchild);
		x->dist = dist(x->rchild) + 1;
		return x;
	}
	// copy a heap
	Node* copy(Node* x)
	{
		if (x == nullptr) return x;
		Node* res = (Node*) ::operator new(sizeof(Node));
		new (&(res->value)) T(x->value);
		res->lchild = copy(x->lchild);
		res->rchild = copy(x->rchild);
		res->dist = x->dist;
		return res;
	}
	// delete a heap
	void clear(Node* x)
	{
		if (x == nullptr) return;
		clear(x->lchild);
		clear(x->rchild);
		delete x;
	}
public:
	/**
	 * constructors
	 */
	priority_queue() {}
	priority_queue(const priority_queue &other) 
	{
		head = copy(other.head);
		Size = other.Size;
	}
	/**
	 * deconstructor
	 */
	~priority_queue() 
	{
		clear(head);
	}
	/**
	 * Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) 
	{
		if (this == &other) return *this;
		clear(head);
		head = copy(other.head);
		Size = other.Size;
		return *this;
	}
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const 
	{
		if (!Size) throw container_is_empty();
		return head->value;
	}
	/**
	 * push new element to the priority queue.
	 */
	void push(const T &e) 
	{
		Node* tmp = (Node*) ::operator new(sizeof(Node));
		new (&(tmp->value)) T(e);
		tmp->lchild = tmp->rchild = nullptr;
		tmp->dist = 0;
		try
		{
			head = meld(head, tmp);
		}
		catch(...)
		{
			delete tmp;
			throw runtime_error();
		}
		Size++;
	}
	/**
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() 
	{
		if (!Size) throw container_is_empty();
		Node* tmp;
		try
		{
			tmp = meld(head->lchild, head->rchild);
		}
		catch(...)
		{
			throw runtime_error();
		}
		delete head;
		head = tmp;
		tmp = nullptr;
		Size--;
	}
	/**
	 * return the number of the elements.
	 */
	size_t size() const 
	{
		return Size;
	}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const 
	{
		return Size == 0;
	}
	/**
	 * merge two priority_queues with at least O(logn) complexity.
	 * clear the other priority_queue.
	 */
	void merge(priority_queue &other) 
	{
		try 
		{
			head = meld(head, other.head);
		}
		catch(...)
		{
			throw runtime_error();
		}
		Size += other.Size;
		other.head = nullptr;
		other.Size = 0;
	}
};

}

#endif
