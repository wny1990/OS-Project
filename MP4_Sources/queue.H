#include "utils.H"

template<typename T>
class node {
public:
        node()
	{
		val = 0;
		next = NULL;
	}
	node( T element)
	{
		val = element;
		next = NULL;
	}
	T val;
        node<T>* next;
};

template<typename T>
class Queue {
public:
    Queue();
    int  get_size();
    bool isEmpty();
    void enqueue(T a);    
    T dequeue();        
    T front();
    void remove(T target);
private:
    int size;
    node<T> *head;
    node<T> *tail;
};

template<typename T>
Queue<T>::Queue()
{
	size = 0;
	/* dummy head */
	head = new node<T>(0);
	tail = head;
}


template<typename T>
int Queue<T>::get_size()
{
	return size;
}

template<typename T>
bool Queue<T>::isEmpty()
{
	if (size)
		return false;
	return true;
}

template<typename T>
void Queue<T>::enqueue(T element)
{
        node<T> *neo_node = new node<T>(element);
	tail->next = neo_node;
	tail = neo_node;
	size++;
	return;
}

template<typename T>
T Queue<T>::dequeue()
{
        if (size == 0)
		abort();
	T element = head->next->val;
	head = head->next;
	size--;
	return element;
}

template<typename T>
T Queue<T>::front()
{
        if (size == 0)
		abort();
        return head->next->val;
}
 
template<typename T>
void Queue<T>::remove(T target)
{

	node<T> *pt = head;
        while( pt->next != NULL )
	{
        	if ( pt->next->val == target)
			break;
		pt = pt->next;
	}

	node<T> *temp = pt->next;
	if ( temp == NULL)
		return;
	else
		delete pt;
// delete the entry
        pt->next = pt->next->next;
	if ( pt->next == NULL)
		tail = pt;
        size--;
        return;
}
