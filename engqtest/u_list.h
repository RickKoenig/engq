struct header
	{
	struct node *first;
	struct node *last;
	};

struct node
	{
	struct node *next;
	struct node *prev;
	};

//#ifndef __LISTLIST
void list_init(struct header *alist);	// nulls out a header

// insert after key (assumes key is in list)
void list_insert_after_this(struct header *alist,struct node *element,struct node *key);
// insert before key (assumes key is in list)
void list_insert_before_this(struct header *alist,struct node *element,struct node *key);

#define list_insert_head(alist,element) list_insert_after_this((alist),(element),NULL);
#define list_insert_tail(alist,element) list_insert_before_this((alist),(element),NULL);


void list_remove_this(struct header *alist,void *element);

#define list_remove_head(alist) list_remove_this((alist),(alist)->first)
#define list_remove_tail(alist) list_remove_this((alist),(alist)->last)

// list search and size
int list_search(struct header *alist,struct node *node);	
// get node given index
struct node *list_index(struct header *alist,int index); 

//#endif
