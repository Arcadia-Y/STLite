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
		Node* child = nullptr;
		Node* bros = nullptr;
		Node(const T& v)
		{
			value = v;
			child = nullptr;
			bros = nullptr;
		}
	};
	Node* head = nullptr;
	size_t Size = 0;
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
		y->bros = x->child;
		x->child = y;
		return x;
	}
	// merge all bros of a Node
	Node* merges(Node* x)
	{
		if (x == nullptr) return x;
		if (x->bros == nullptr) return x;
		Node* Nodes[(int)1e6];
		int count = 0;
		for(; x != nullptr; ++count)
		{
			Nodes[count] = x;
			Node* tmp = x->bros;
			x->bros = nullptr;
			x = tmp;
		}
		Nodes[count] = nullptr;
		for (int i = 0; i < count; i += 2)
			Nodes[i] = meld(Nodes[i], Nodes[i+1]);
		if (count % 2) ++count;
		count -= 2;
		for (int i = count; i > 0; i -= 2)
			Nodes[i-2] = meld(Nodes[i-2], Nodes[i]);
		return Nodes[0];
	}
	// copy a heap
	Node* copy(Node* x)
	{
		if (x == nullptr) return x;
		Node* res = (Node*) operator new(sizeof(Node));
		new(&(res->value)) T(x->value); 
		res->child = copy(x->child);
		res->bros = copy(x->bros);
		return res;
	}
	// delete a heap
	void clear(Node* x)
	{
		if (x == nullptr) return;
		clear(x->child);
		clear(x->bros);
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
		Node* tmp = (Node*) operator new(sizeof(Node));
		new(&(tmp->value)) T(e);
		tmp->child = tmp->bros = nullptr;
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
			tmp = merges(head->child);
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
