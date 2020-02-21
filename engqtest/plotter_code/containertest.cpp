//#define CONTAINERTEST
#ifdef CONTAINERTEST
#define DOEXCEPTIONS
template<typename T>
class SetGet {
	T val;
public:
	void set(T t)
	{
		val = t;
	}
	const T get()
	{
		return val;
	}
};

class ContainerOverflow : public std::exception {
	const char* message;
public:
	ContainerOverflow(char* mess) : message(mess) {}
	const char* getMessage() {return message;}
};

class ContainerUnderflow : public std::exception {
	const char* message;
public:
	ContainerUnderflow(char* mess) : message(mess) {}
	const char* getMessage() {return message;}
};

const int defaultsize = 200;
// a virtual base class for Queue implementations
template <typename T>
class Stack {
public:
	virtual ~Stack(){}; // very important
	virtual void push(T t) = 0;
	virtual T pop() = 0;
	virtual T top() = 0;
	virtual bool isEmpty() = 0;
	virtual bool isFull() = 0;
};

template <typename T>
class ArrayStack : public Stack<T>{
	int cursize,maxsize;
	T* data;
public:
	ArrayStack(int ms=defaultsize) : cursize(0),maxsize(ms) // change default max size if needed from defaultsize to ?, or make global
	{
		data = new T[maxsize];
	}
	~ArrayStack()
	{
		delete[] data;
	}
	void push(T t)		// don't forget to add time complexity comments on each function... (mucho points) 
						// low leve functions are O(1), higher level functions are O(n), right?
	{
		if (cursize < maxsize) {
			data[cursize++] = t;
		}
#ifdef DOEXCEPTIONS
		else {
			throw ContainerOverflow("ArrayStack Container overflow");
		}
#endif
	}
	T pop()
	{
		if (cursize > 0)
			return data[--cursize];
		else
#ifdef DOEXCEPTIONS
			throw ContainerUnderflow("ArrayStack Container underflow");
#else
			return T(); // primitive data types default contstructor yields a 0 like result
#endif
	}
	T top()
	{
		if (cursize > 0)
			return data[cursize-1];
		else
			return T();
	}
	bool isEmpty()
	{
		return cursize == 0;
	}
	bool isFull()
	{
		return cursize == maxsize;
	}
};

template <typename T>
class LinkedStack : public Stack<T>{
	int cursize,maxsize;
	struct node {
		T val;
		node* next; // next item down in the stack
	};
	node* head;
public:
	LinkedStack(int ms=defaultsize) : cursize(0),maxsize(ms),head(0) // change default max size if needed from defaultsize to ?
															// or make global
	{
	}
	~LinkedStack()
	{
		while(!isEmpty()) {
			pop();
		}
	}
	void push(T t)
	{
		if (cursize < maxsize) {
			node* newhead = new node;
			newhead->val = t;
			newhead->next = head;
			head = newhead;
			++cursize;
		}
#ifdef DOEXCEPTIONS
		else {
			throw ContainerOverflow("LinkedStack Container overflow");
		}
#endif
	}
	T pop()
	{
		if (cursize > 0) {
			T ret = head->val;
			node* newhead = head->next;
			delete head;
			head = newhead;
			--cursize;
			return ret;
		} else {
#ifdef DOEXCEPTIONS
			throw ContainerOverflow("LinkedStack Container underflow");
#else
			return T(); // primitive data types default contstructor yields a 0 like result
#endif
		}
	}
	T top()
	{
		if (cursize > 0)
			return head->val;
		else
			return T();
	}
	bool isEmpty()
	{
		return cursize == 0;
	}
	bool isFull()
	{
		return cursize == maxsize;
	}
};

// a virtual base class for Queue implementations
template <typename T>
class Queue {
public:
	virtual ~Queue(){}; // very important
	virtual void enq(T t) = 0;
	virtual T deq() = 0;
	virtual T front() = 0;
	virtual bool isEmpty() = 0;
	virtual bool isFull() = 0;
};

template <typename T>
class ArrayQueue : public Queue<T>{
	int cursize,maxsize;
	int begin,end;
	T* data;
public:
	ArrayQueue(int ms=defaultsize) : cursize(0),maxsize(ms),begin(0),end(0) // change default max size if needed from defaultsize to ?, or make global
	{
		data = new T[maxsize];
	}
	~ArrayQueue()
	{
		delete[] data;
	}
	void enq(T t)
	{
		if (cursize < maxsize) {
			data[end++] = t; // put items at the end
			if (end == maxsize)
				end = 0; // wrap around
			++cursize;
		}
#ifdef DOEXCEPTIONS
		else {
			throw ContainerOverflow("ArrayQueue Container overflow");
		}
#endif
	}
	T deq()
	{
		if (cursize > 0) {
			T ret = data[begin++]; // get items from the front
			if (begin == maxsize)
				begin = 0;
			--cursize;
			return ret;
		} else {
#ifdef DOEXCEPTIONS
			throw ContainerOverflow("ArrayQueue Container underflow");
#else
			return T();
#endif
		}
	}
	T front()
	{
		if (cursize > 0)
			return data[begin];
		else
			return T();
	}
	bool isEmpty()
	{
		return cursize == 0;
	}
	bool isFull()
	{
		return cursize == maxsize;
	}
};

template <typename T>
class LinkedQueue : public Queue<T>{
	int cursize,maxsize;
	struct node {
		T val;
		node* next; // the next guy in line
	};
	node* begin,*end;
public:
	LinkedQueue(int ms=defaultsize) : cursize(0),maxsize(ms),begin(0),end(0) // change default max size if needed from defaultsize to ?
															// or make global
	{
	}
	~LinkedQueue() // O(n)
	{
		while(!isEmpty()) {
			deq();
		}
	}
	void enq(T t) // O(1) etc.
	{
		if (cursize < maxsize) {
			if (cursize == 0) { // the first guy show up to an empty line, setup all pointers
				begin = new node;
				begin->next = 0;
				begin->val = t;
				end = begin;
			} else {
				end->next = new node; // this is the newest guy to get in line, old end points to next guy, the new end guy
				end = end->next; // let's look at this guy, the new end of line
				end->val = t; // set his personality
				end->next = 0; // no one after this guy
			}
			++cursize;
		}
#ifdef DOEXCEPTIONS
		else {
			throw ContainerOverflow("LinkedQueue Container overflow");
		}
#endif
	}
	T deq()
	{
		if (cursize > 0) {
			T ret = begin->val;
			if (cursize == 1) { // the last node is being removed, clear out all pointers
				delete begin;
				begin = 0;
				end = 0;
			} else {
				node* newbegin = begin->next; // point to the next guy in line
				delete begin; // good, now delete the guy that is leaving the line
				begin = newbegin; // this is the new guy in front of line
			}
			--cursize;
			return ret; 
		} else {
#ifdef DOEXCEPTIONS
			throw ContainerOverflow("LinkedQueue Container overflow");
#else
			return T(); // primitive data types default contstructor yields a 0 like result
#endif
		}
	}
	T front()
	{
		if (cursize > 0)
			return begin->val;
		else
			return T();
	}
	bool isEmpty()
	{
		return cursize == 0;
	}
	bool isFull()
	{
		return cursize == maxsize;
	}
};

//#define logger printf // make logger printf for people who don't have the engine called engq
char* randomtext[] = {
	"this","is","some","random","text",0
};

template<typename T>
void fillAllStack(Stack<T>& stk)
{
	char** p = randomtext;
	while(*p)
		stk.push(*p++);
}

template<typename T>
void emptyAllStack(Stack<T>& stk)
{
	while(!stk.isEmpty())
		logger("pop Stack : %s\n",stk.pop());
}

template<typename T>
void fillAllQueue(Queue<T>& que)
{
	char** p = randomtext;
	while(*p)
		que.enq(*p++);
}

template<typename T>
void emptyAllQueue(Queue<T>& que)
{
	while(!que.isEmpty())
		logger("deq Queue : %s\n",que.deq());
}

bool testbrackets(const char* s)
{
	LinkedStack<char> stk;
	char left2right[256];
	left2right['('] = ')';
	left2right['{'] = '}';
	left2right['['] = ']';
	//int bal = 0;
	char left;
	while(*s) {
		switch(*s) {
		case '(': // *s is a left
		case '{':
		case '[':
			//logger("open '%c'\n",*s);
			stk.push(*s); // only push lefts
			//++bal;
			break;
		case ')': // *s is a right, pop and see if the popped left matches this right
		case '}':
		case ']':
			if (stk.isEmpty()) {
				logger("missing some openings, too many closings\n");
				return false;
			}
			left = stk.pop(); // pop a left
			// does this match the right?
			if (left2right[left] != *s) {
				logger("left '%c' doesn't match right '%c'\n",left,*s);
				return false;
			}
			break;
		default: // skip other chars
			break;
		}
		++s;
	}
/*	if (bal > 0) {
		logger("too many openings, missing some closings\n");
		return false;
	} else if (bal < 0) {
		logger("missing some openings, too many closings\n");
		return false;
	} else {
		logger("bracket matching all okay!\n");
		return true;
	} */
	if (!stk.isEmpty()) {
		logger("too many openings, missing some closings\n");
		return false;
	} else {
		logger("bracket matching all okay!\n");
		return true;
	}
}
#if 1
#include <iostream>
using namespace std;
#include "assert.h"

template <class Object>
class ChainedHashTable
{
	struct ListNode 
    {
        string key;
		Object info;
		ListNode * next;
        
        ListNode(string k, Object ob, ListNode *n)
        {
            key = k; info = ob; next = n;
        }
        
        //
        static ListNode * make(string k, Object ob, ListNode * l)
        {
			return new ListNode(k,ob,l); // Rick, Thats it!
        }
        
        static ListNode * find(string k, ListNode * l)
        {
            while (l != NULL)
            {
                if (l->key == k)
                {
                    return l;
                }
                l = l->next;
            }
            return NULL;
        }
        
        // Rick added
        static ListNode * remove(string k, ListNode * curnode)
        {
			ListNode * prevnode = NULL;
			ListNode * retnode = curnode;

            while (curnode != NULL)
            {
                if (curnode->key == k)
                { 
					// found a match
					ListNode * nextnode = curnode->next;
					// time for pointer surgery
					delete curnode;
					if (!prevnode) { // removing from the front
						retnode = nextnode; // return a different pointer (the second object in the chain)
					} else {
						prevnode->next = nextnode; // make prev pointer skip over curpointer, That's it! 
					}
					break;
                }
				prevnode = curnode;
                curnode = curnode->next;
            }
			return retnode;
		}
        
        // Rick
        static void freeList(ListNode * l)
        {
            while (l != NULL)
            {
                ListNode* ln = l->next; // Rick added, get next before l becomes invalid
				delete l; // Rick added, can't do l->next anymore, that's why we copied l->next to ln
                l = ln;
            }
        }
	};
	ListNode* head;
    
    int hash(string key, int N)
    {
        const int shift = 6;
        const int mask = ~0 >> (32-shift);
        int result = 0;
        for (unsigned int i = 0; i < key.length(); i++)
            result = (result << shift) | (key[i] & mask);
        return abs(result) % N;
    }
    
    ListNode ** T;
    int capacity;
    
public:
    ChainedHashTable(int numberOfChains)
    {
        capacity = numberOfChains;
        T = new ListNode* [capacity];
        for (int i = 0; i < capacity; ++i)
        {
            T[i] = NULL;
        }
    }
    
    void insert(string key, Object info)
    {
        int h = hash(key, capacity);
        T[h] = ListNode::make(key, info, T[h]);
    }
    
    Object lookup(string key)
    {
        int h = hash(key, capacity);
        ListNode *l = ListNode::find(key, T[h]);
        if (!l) 
        {
			//assert(false);
            //error("undefined key: " + key);
            cout << "Error - Undefined Key: " << key << endl;
        }
        return l->info;
    }
    
    void remove(string key)
    {
        int h = hash(key, capacity);
        T[h] = ListNode::remove(key, T[h]);
    }
    
    ~ChainedHashTable()
    {
        for (int i = 0; i < capacity; ++i)
            ListNode::freeList(T[i]);
        delete[] T;
    }
};

int anothermain()
{
	ChainedHashTable<int> amap(32);
	amap.insert("hey",32);
	amap.lookup("hey");
	amap.remove("hey");
	return 0;
}
#endif
void containertest()
{
	logger("---------------------- start containertest\n");

	// test out a simple set get template class
	SetGet<float>* cp = new SetGet<float>;
	cp->set(1.414f);
	float r = cp->get();
	logger("test setget ret val = %f\n",r);
	delete(cp);
	logger("/\\/\\\n");

	Queue<float> *qtf[2] = {
		new ArrayQueue<float>,
		new LinkedQueue<float>
	};
	Queue<char*> *qtcs[2] = {
		new ArrayQueue<char*>,
		new LinkedQueue<char*>
	};
	int i;
	for (i=0;i<2;++i) {
		// try out some of the 4 container classes for the assignment
		// do it with floats
		//LinkedQueue<float>* asf = new LinkedQueue<float>(2); // size of 2
//		Queue<float>* asf = new LinkedQueue<float>; // default size
		Queue<float>* asf = qtf[i];
		Queue<char*>* ascs = qtcs[i];
		asf->enq(3.14f);
		asf->enq(2.718f);
		asf->enq(1.618f);
		logger("top asf = %f\n",asf->front());
		while(!asf->isEmpty())
			logger("pop asf = %f\n",asf->deq());
		logger("asf empty value returned = %f\n",asf->front());
		//delete asf;
		logger("/\\/\\\n");

		// do it with c strings
		//LinkedQueue<char*>* ascs = new LinkedQueue<char*>(2);
		//Queue<char*>* ascs = new ArrayQueue<char*>;
		ascs->enq("fee");
		ascs->enq("fi");
		ascs->enq("fo");
		ascs->enq("fum");
		if (ascs->front())
			logger("top ascs = %s\n",ascs->front());
		else
			logger("top ascs = %s\n","empty");
		while(!ascs->isEmpty()) {
			const char* retcs = ascs->deq();
			retcs = retcs ? retcs : "empty";
			logger("pop ascs = %s\n",retcs);
		}
		logger("ascs pointer to empty value returned = %p\n",ascs->front());
		//delete ascs;
		logger("/\\/\\\n");
	}
	for (i=0;i<2;++i) {
		delete qtf[i];
		delete qtcs[i];
	}
	// now run some tests with random text
	logger("ArrayStack\n");
	Stack<char*>* s = new ArrayStack<char*>;
	fillAllStack<char*>(*s);
	emptyAllStack<char*>(*s);
	delete s;
	logger("LinkedStack\n");
	s = new LinkedStack<char*>;
	fillAllStack<char*>(*s);
	emptyAllStack<char*>(*s);
	delete s;
	logger("ArrayQueue\n");
	Queue<char*>* q = new ArrayQueue<char*>;
	fillAllQueue<char*>(*q);
	emptyAllQueue<char*>(*q);
	delete q;
	logger("LinkedQueue\n");
	q = new LinkedQueue<char*>;
	fillAllQueue<char*>(*q);
	emptyAllQueue<char*>(*q);
	delete q;

	// make a small linked queue and overflow it, causing an exception
	try {
		logger("tiny LinkedQueue, will throw an exception\n");
		q = new LinkedQueue<char*>(2); // override default size
		fillAllQueue<char*>(*q);
		emptyAllQueue<char*>(*q);
	} catch (ContainerOverflow e) {
		logger("exception occured in ContainerOverflow, it's message is %s\n",e.getMessage());
	} catch (ContainerUnderflow e) {
		logger("exception occured in ContainerUnderflow, it's message is %s\n",e.getMessage());
	}
	delete q;
	logger("\n\n");
	// play around with () {} and []
	char * expr[4] = {
		"({(())})((([({})])))(((((()([{()}])(()))))))()", // okay
		"({(())})((([({})])))(((((()([{()}])(())))))", // eof, not enough
		"({(())})((([({})])))(((((()([{()}])(()))))))()])", // eof, too much
// the homework doesn't include this case but it should
		"(([}))" // mismatched brackets
	};
	const int nexpr = sizeof(expr)/sizeof(expr[0]);
	for (i=0;i<nexpr;++i) {
		bool res = testbrackets(expr[i]);
		logger("testbracket %s = %s\n\n",expr[i],res ? "true" : "false");
	}
	logger("---------------------- end containertest\n");
}
#endif
