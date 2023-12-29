#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

// ------------------------ Debug ------------------------
static int DEBUG = 0;

int debug_print(const char *format, ...)
{
    if (DEBUG != 0)
    {
        va_list args;
        va_start(args, format);
        int result = vprintf(format, args);
        va_end(args);
        return result;
    }
    return 0;
}

// ------------------------ Utility ------------------------

#ifndef __UTILITY_H__
#define __UTILITY_H__

typedef struct __generic_pair__
{
    int key;
    void *data;
} GenericPair;

GenericPair *create_generic_pair(const int key, void *data);

#endif // __UTILITY_H__

GenericPair *create_generic_pair(const int key, void *data)
{
    GenericPair *pair = (GenericPair *)malloc(sizeof(GenericPair));
    pair->key = key;
    pair->data = data;
    return pair;
}

// ------------------------ Utility ------------------------

// ------------------------ Hash Table ------------------------

#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

// Define the maximum size of the hash table
#define INIT_HASH_TABLE_SIZE 100
#define RESIZE_FACTOR 2
#define PRIME 31

// Define the hash table entry structure
typedef struct __hash_node__
{
    GenericPair pair;
    struct __hash_node__ *next;
} HashNode;

HashNode *create_hash_node(const GenericPair *p);

typedef struct __f_heap_node__
{
    GenericPair *pair;
    int degree;
    int marked;

    // doubly linked list
    struct __f_heap_node__ *left;
    struct __f_heap_node__ *right;

    // parent children
    struct __f_heap_node__ *parent;

    // we not need `children`
    // because we could use `child->right` to traverse all children
    struct __f_heap_node__ *child;

    // Function pointers
    // add sibling node to the left of the node
    struct __f_heap_node__ *(*add_sibling)(struct __f_heap_node__ *, struct __f_heap_node__ *);
    void (*delete_child)(struct __f_heap_node__ *, struct __f_heap_node__ *);

} FHeapNode;

// Define the hash table structure
typedef struct __hash_table__
{
    int size;
    int bucket_size;
    // Entry* buckets[INIT_SIZE];
    HashNode **buckets;
    /* Function pointers */
    // hash function
    int (*hash_key)(struct __hash_table__ *, const int key);

    void (*insert)(struct __hash_table__ *, const GenericPair *pair);
    GenericPair *(*find)(struct __hash_table__ *, const int key);
    GenericPair *(*find_pair)(struct __hash_table__ *, const GenericPair *pair);
    int (*remove)(struct __hash_table__ *, const int key);
    int (*remove_pair)(struct __hash_table__ *, const GenericPair *pair);

} HashTable;

// Function to insert a key-value pair into the hash table
void hash_table_insert(HashTable *self, const GenericPair *pair);

// Function to retrieve the value associated with a key from the hash table
GenericPair *hash_table_find(HashTable *self, const int key);

GenericPair *hash_table_find_pair(HashTable *self, const GenericPair *pair);

// Function to remove a key-value pair from the hash table
int hash_table_remove(HashTable *self, const int key);

int hash_table_remove_pair(HashTable *self, const GenericPair *pair);

// Function to initialize the hash table
HashTable *create_hash_table(int (*hash_key)(HashTable *, const int));

void destroy_hash_table(HashTable **self);

#endif

HashNode *create_hash_node(const GenericPair *p)
{
    HashNode *node = (HashNode *)malloc(sizeof(HashNode));
    node->pair = *p;
    node->next = NULL;
    return node;
}

void hash_table_insert(HashTable *self, const GenericPair *pair)
{
    int index = self->hash_key(self, pair->key);
    HashNode *node = create_hash_node(pair);

    if (self->buckets[index] == NULL)
    {
        self->buckets[index] = node;
    }
    else
    {
        node->next = self->buckets[index];
        self->buckets[index] = node;
    }
    self->size++;
}

GenericPair *hash_table_find(HashTable *self, const int key)
{
    int index = self->hash_key(self, key);

    // debug_print("index: %d\n", index);
    HashNode *node = self->buckets[index];

    while (node != NULL)
    {
        if (node->pair.key == key)
        {
            break;
        }
        node = node->next;
    }

    if (node == NULL)
    {
        return NULL;
    }

    return &(node->pair);
}

GenericPair* hash_table_find_pair(HashTable *self, const GenericPair *pair)
{
    int key = pair->key;
    int value = *(int *)(pair->data);

    int index = self->hash_key(self, key);

    // debug_print("index: %d\n", index);
    HashNode *node = self->buckets[index];

    while (node != NULL)
    {
        if (node->pair.key == key)
        {
            FHeapNode *f_node = (FHeapNode *)(node->pair.data);
            if (f_node->pair->key == key && *(int *)(f_node->pair->data) == value)
            {
                break;
            }
        }
        node = node->next;
    }

    if (node == NULL)
    {
        return NULL;
    }

    return &(node->pair);
}

int hash_table_remove(HashTable *self, const int key)
{
    // debug_print("hash_table_remove\n");
    int index = self->hash_key(self, key);
    HashNode *node = self->buckets[index];
    HashNode *prev = NULL;

    while (node != NULL)
    {
        if (node->pair.key == key)
        {
            break;
        }
        prev = node;
        node = node->next;
    }

    if (node == NULL)
    {
        return 0;
    }

    if (prev == NULL)
    {
        self->buckets[index] = node->next;
    }
    else
    {
        prev->next = node->next;
    }

    // free(node);
    self->size--;
    return 1;
}

int hash_table_remove_pair(HashTable *self, const GenericPair *pair)
{
    int key = pair->key;
    int value = *(int *)(pair->data);

    int index = self->hash_key(self, key);
    HashNode *node = self->buckets[index];
    HashNode *prev = NULL;

    while (node != NULL)
    {
        if (node->pair.key == key)
        {
            FHeapNode *f_node = (FHeapNode *)(node->pair.data);
            if (f_node->pair->key == key && *(int *)(f_node->pair->data) == value)
            {
                break;
            }
        }
        prev = node;
        node = node->next;
    }

    if (node == NULL)
    {
        return 0;
    }

    if (prev == NULL)
    {
        self->buckets[index] = node->next;
    }
    else
    {
        prev->next = node->next;
    }

    // free(node);
    self->size--;
    return 1;
}

HashTable *create_hash_table(int (*hash_key)(HashTable *, const int))
{
    HashTable *self = (HashTable *)malloc(sizeof(HashTable));
    self->bucket_size = INIT_HASH_TABLE_SIZE;
    self->buckets = (HashNode **)malloc(sizeof(HashNode *) * INIT_HASH_TABLE_SIZE);
    self->size = 0;
    // bind functions
    self->hash_key = hash_key;
    self->insert = hash_table_insert;
    self->find = hash_table_find;
    self->find_pair = hash_table_find_pair;
    self->remove = hash_table_remove;
    self->remove_pair = hash_table_remove_pair;

    return self;
}

void destroy_hash_table(HashTable **self)
{
    if (*self == NULL)
    {
        // debug_print("destroy_hash_table : self == NULL\n");
        return;
    }
    int i;
    for (i = 0; i < (*self)->bucket_size; i++)
    {
        HashNode *node = (*self)->buckets[i];
        while (node != NULL)
        {
            HashNode *temp = node;
            node = node->next;
            free(temp);
        }
    }

    for (int i = 0; i < INIT_HASH_TABLE_SIZE; i++)
    {
        free((*self)->buckets[i]);
    }
    free((*self)->buckets);
    free((*self));

    (*self) = NULL;
}

// hash function for Hash table
int custom_hash_key(HashTable *self, const int key)
{
    return (key * key * PRIME) % self->bucket_size;
}

// ------------------------ Hash Table ------------------------

// ------------------ Implementation of F-heap ------------------   |
// |
// |    reference: https://github.com/mgold/Fibonacci-Heap
// |    reference: https://cihcih.medium.com/%E5%9C%96%E8%AB%96%E6%BC%94%E7%AE%97%E6%B3%95ch19-2-fibonacci-heaps-3abde385b88c
// |
// |
// ------------------ Implementation of F-heap ------------------

// ------------------------ F-heap ------------------------

// typedef struct __f_heap_node__
// {
//     GenericPair *pair;
//     int degree;
//     int marked;

//     // doubly linked list
//     struct __f_heap_node__ *left;
//     struct __f_heap_node__ *right;

//     // parent children
//     struct __f_heap_node__ *parent;

//     // we not need `children`
//     // because we could use `child->right` to traverse all children
//     struct __f_heap_node__ *child;

//     // Function pointers
//     // add sibling node to the left of the node
//     struct __f_heap_node__ *(*add_sibling)(struct __f_heap_node__ *, struct __f_heap_node__ *);
//     void (*delete_child)(struct __f_heap_node__ *, struct __f_heap_node__ *);

// } FHeapNode;

FHeapNode *f_heap_node_add_sibling(FHeapNode *old_node, FHeapNode *new_node)
{
    // after:
    // old_node->left <-> new_node <-> old_node <-> old_node->right

    new_node->left = old_node->left;
    new_node->right = old_node;
    old_node->left->right = new_node;
    old_node->left = new_node;

    return old_node;
}

void f_heap_node_delete_child(FHeapNode *parent, FHeapNode *child)
{
    // remove `child` from `parent`'s children list

    if (child->right == child) // only has one child
    {
        parent->child = NULL;
    }
    else // children : find `child` in `parent`'s children list , and remove it
    {
        FHeapNode *cur = parent->child;
        FHeapNode *prev = NULL;
        while (cur != child)
        {
            prev = cur;
            cur = cur->right;
        }

        // unlink `child` from `parent`'s children list
        prev->right = child->right;
        child->right->left = prev;

        // update `parent`'s child
        if (parent->child == child)
        {
            parent->child = child->right;
        }
    }

    free(child);
}

FHeapNode *create_f_heap_node(int key, int value)
{
    FHeapNode *node = (FHeapNode *)malloc(sizeof(FHeapNode));

    node->pair = (GenericPair *)malloc(sizeof(GenericPair));
    node->pair->key = key;
    node->pair->data = malloc(sizeof(int));
    memcpy(node->pair->data, &value, sizeof(int));

    node->degree = 0;

    // has lost a child before , 1 = true , 0 = false
    // root node is always 0
    node->marked = 0;

    node->left = node;
    node->right = node;

    node->parent = NULL;
    node->child = NULL;

    node->add_sibling = f_heap_node_add_sibling;
    node->delete_child = f_heap_node_delete_child;
    return node;
}

typedef struct __f_heap__
{
    /* ----- Internal Data ----- */
    FHeapNode *min;
    int size; // number of nodes

    // `root` itself is a circular doubly linked list
    // because we could use `min->right` to traverse all roots
    FHeapNode *root;

    // HashTable[ key ].data = pointer of node(key,val)
    // use HashTable to find node(key,val) in O(1) time
    HashTable *hash_table;

    /* ----- Public API ----- */
    GenericPair (*get_min)(struct __f_heap__ *);
    FHeapNode *(*insert)(struct __f_heap__ *, int, int);
    FHeapNode *(*delete)(struct __f_heap__ *, int, int);

    // Return the node with the minimum key (and remove it from the heap)
    // extract = get_min + delete
    FHeapNode *(*extract)(struct __f_heap__ *);

    // decrease Node(x,val) by y :
    // newKey = x - y
    // ( we will use HashTable to find Node(x,val) )
    void (*decrease)(struct __f_heap__ *, int, int, int);
    // void (*decrease)(struct __f_heap__ *, FHeapNode *, int);

    // other public functions
    // int (*get_size)(struct __f_heap__ *);
    // int (*is_empty)(struct __f_heap__ *);

} FHeap;

// Debug Functions Prototype
void print_hash_table(HashTable *hash_table);
void print_f_heap(FHeap *heap);
void print_f_heap_node(FHeapNode *node);
void print_generic_pair_node(GenericPair *pair);
void print_generic_pair_int(GenericPair *pair);

/* ------------------------ F-heap  Internal Functions ------------------------ */

void f_heap_update_min(FHeap *heap)
{
    // debug_print("f_heap_update_min\n");
    print_f_heap(heap);

    if( heap->root == NULL ){
        // debug_print("heap->root == NULL\n");
        heap->min = NULL;
        return;
    }
    
    if (heap->root != NULL)
    {
        // debug_print("heap->root != NULL\n");
        int new_min_key = 2147483647;
        FHeapNode *temp = heap->root;
        do
        {   
            // debug_print("temp: (%d)%d\n", temp->pair->key, *(int *)(temp->pair->data));
            if (temp->pair->key < new_min_key)
            {
                new_min_key = temp->pair->key;
                heap->min = temp;
            }
            temp = temp->right;
        } while (temp != heap->root);

    }

    // debug_print("after update min\n");
    // debug_print("(%d)%d\n", heap->min->pair->key, *(int *)(heap->min->pair->data));
}

FHeapNode *f_heap_insert(FHeap *heap, FHeapNode *new_node)
{
    // debug_print("f_heap_insert\n");
    // update min
    if (heap->root == NULL)
    {
        heap->min = new_node;
        heap->root = new_node;
    }
    else
    {
        if (new_node->pair->key < heap->min->pair->key)
        {
            heap->min = new_node;
        }

        // add new_node to root list
        heap->root = heap->root->add_sibling(heap->root, new_node);
    }
    // debug_print("new_node->pair->key: %d\n", new_node->pair->key);

    // update heap size
    heap->size++;

    // update hash table

    // const GenericPair *result = heap->hash_table->find(heap->hash_table, new_node->pair->key);
    // if (result != NULL)
    // { // already exist
    //     return new_node;
    // }

    // debug_print("after find\n");

    const GenericPair *new_pair = create_generic_pair(new_node->pair->key, new_node);
    heap->hash_table->insert(heap->hash_table, new_pair);

    return new_node;
}

// remove `child` from `parent`'s children list
// add `child` to root list
FHeapNode *f_heap_cut(FHeap *heap, FHeapNode *par, FHeapNode *child)
{
    debug_print("f_heap_cut\n");
    print_f_heap(heap);

    // remove `child` from `parent`'s children list
    if (child->right == child)
    {
        par->child = NULL;
    }
    else
    {
        if (par->child == child)
        {
            par->child = child->right;
        }

        child->right->left = child->left;
        child->left->right = child->right;
    }

    // add `child` to root list
    if (heap->root == NULL)
    {
        heap->root = child;
    }
    else
    {
        heap->root = heap->root->add_sibling(heap->root, child);

        // debug_print("after add_sibling\n");
        // print_f_heap(heap);

        FHeapNode *temp = heap->root;
        // do
        // {
        //     debug_print("(%d)%d ", temp->pair->key, *(int *)(temp->pair->data));
        // } while (temp != heap->root);
        // debug_print("\n");
    }

    // update heap size
    heap->size++;

    // debug_print("after cut\n");
    // print_f_heap(heap);

    return child;
}

FHeapNode *f_heap_cascading_cut(FHeap *heap, FHeapNode *node)
{
    FHeapNode *par = node->parent;
    if (par != NULL)
    {
        if (node->marked == 0)
        {
            node->marked = 1;
        }
        else
        {
            f_heap_cut(heap, par, node);
            f_heap_cascading_cut(heap, par);
        }
    }
    return node;
}

void f_heap_decrease(FHeap *heap, FHeapNode *node, int diff)
{

    // debug_print("original node\n");
    // debug_print("(%d)%d\n", node->pair->key, *(int *)(node->pair->data));
    // remove original from hash table
    heap->hash_table->remove_pair(heap->hash_table, node->pair);

    // debug_print("before decrease\n");
    // print_f_heap(heap);
    // debug_print("\n");

    int newKey = node->pair->key - diff;
    // debug_print("newKey: %d\n", newKey);

    // update node's key
    node->pair->key = newKey;

    // debug_print("after update key\n");
    // print_f_heap(heap);

    // update node's parent
    FHeapNode *par = node->parent;
    if (par != NULL && node->pair->key < par->pair->key)
    {
        // cut if not in root list
        // debug_print("node->pair->key < par->pair->key\n");
        f_heap_cut(heap, par, node);
        f_heap_cascading_cut(heap, par);

        // re insert node to root list
        if (heap->root == NULL)
        {
            heap->root = node;
        }
        else
        {
            heap->root = heap->root->add_sibling(heap->root, node);
            // debug_print("after add_sibling\n");
            // print_f_heap(heap);
        }
    }

    

    // debug_print("after re insert\n");
    // print_f_heap(heap);

    // update min
    if (heap->root != NULL)
    {
        FHeapNode *temp = heap->root;
        do
        {
            if (temp->pair->key < heap->min->pair->key)
            {
                heap->min = temp;
            }
            temp = temp->right;
        } while (temp != heap->root);
    }

    // debug_print("after decrease\n");
    // print_f_heap(heap);

    // re insert node to hash table
    heap->hash_table->insert(heap->hash_table, node);

    return;
}

FHeapNode *f_heap_delete(FHeap *heap, FHeapNode *oldNode)
{
    // update heap size
    heap->size--;

    /* delete all links */
    // a. parent link
    // b. sibling link
    // c. child link

    // a. delete parent link
    /*  delete parent link  */
    // 1. is in root list
    // 2. is not in root list

    // 1. is in root list
    if (oldNode->parent == NULL)
    {

        if (oldNode->right == oldNode) // only one node in root list
        {
            // debug_print("only one node in root list\n");
            heap->root = NULL;
        }
        else // more than one node in root list
        {
            // debug_print("more than one node in root list\n");
            oldNode->right->left = oldNode->left;
            oldNode->left->right = oldNode->right;
            heap->root = oldNode->right;
        }
    }
    else
    { // 2. is not in root list
        oldNode->parent->delete_child(oldNode->parent, oldNode);
    }

    // b. delete sibling link ( is not in root list )
    if (oldNode->parent != NULL && oldNode->right != oldNode)
    {
        oldNode->right->left = oldNode->left;
        oldNode->left->right = oldNode->right;
    }

    // debug_print("after delete root\n");
    print_f_heap(heap);

    // add children to root list
    FHeapNode *child = oldNode->child;
    if (child != NULL)
    {
        child->left->right = NULL; // break the circular doubly linked list
        while (child != NULL)
        {
            FHeapNode *right = child->right;
            child->parent = NULL;
            child->left = NULL;
            child->right = NULL;

            heap->root = heap->root->add_sibling(heap->root, child);

            child = right;
        }
    }
    // debug_print("after add children to root list\n");
    // print_f_heap(heap);

    f_heap_update_min(heap);
    // debug_print("after update min\n");
    // print_f_heap(heap);

    // update hash table
    // debug_print("before remove pair from hash table\n");
    // print_hash_table(heap->hash_table);
    // debug_print("remove pair from hash table\n");
    // debug_print("pair: (%d)%d\n", oldNode->pair->key, *(int *)(oldNode->pair->data));
    heap->hash_table->remove_pair(heap->hash_table, oldNode->pair);
    // print_hash_table(heap->hash_table);

    return oldNode;
}

void f_heap_link(FHeap *heap, FHeapNode *be_parent, FHeapNode *be_child)
{
    // assert be_parent is in root list
    if (be_parent->parent != NULL || be_child->parent != NULL)
    {
        // debug_print("be_parent is not in root list\n");
        // print_f_heap_node(be_child);
        // print_f_heap_node(be_parent);
        return;
    }

    // remove be_child from root list
    if (be_child->right == be_child) // only one node in root list
    {
        heap->root = NULL;
    }
    else // more than one node in root list
    {
        be_child->right->left = be_child->left;
        be_child->left->right = be_child->right;
        heap->root = be_child->right;
    }

    // add `be_child` to be_parent's children list
    if (be_parent->child == NULL)
    {
        be_parent->child = be_child;
        be_child->left = be_child;
        be_child->right = be_child;
    }
    else
    {
        be_parent->child = be_parent->child->add_sibling(be_parent->child, be_child);
    }

    // update x's parent
    be_child->parent = be_parent;

    // update be_parent's marked & degree
    be_parent->marked = 0;
    be_parent->degree++;

    be_child->marked = 0;

    // debug_print("after link\n");
    // print_f_heap(heap);
}

void f_heap_consolidate(FHeap *heap)
{

    // create an array A[0..D(n)] to store roots of trees
    // D(n) = log2(n)
    HashTable *array = create_hash_table(custom_hash_key);

    // traverse all roots
    FHeapNode *cur = heap->root;
    if (cur != NULL)
    {
        do // add all roots to array
        {

            int deg = cur->degree;
            // debug_print("(%d)%d,deg: %d\n", cur->pair->key, *(int *)(cur->pair->data), deg);
            array->insert(array, create_generic_pair(deg, cur));

            cur = cur->right;
        } while (cur != heap->root);
    }
    // debug_print("after add all roots to array\n");
    print_hash_table(array);

    // merge trees with the same degree
    int deg = 0;
    while (array->find(array, deg))
    { // while array[deg] is not empty
        // debug_print("deg: %d\n", deg);
        // debug_print("array[%d] is not empty\n", deg);
        GenericPair *result = array->find(array, deg);
        if (result == NULL)
        {
            // debug_print("result1 == NULL\n");
            deg++;
            continue;
        }

        // debug_print("result1\n");
        if (result->data == NULL)
        {
            // debug_print("result->data == NULL\n");
            deg++;
            continue;
        }
        else
        {
            FHeapNode *f_node = (FHeapNode *)(result->data);
            // debug_print("(%d)%d\n", f_node->pair->key, *(int *)(f_node->pair->data));
        }

        // print_generic_pair_node(result);

        FHeapNode *x = (FHeapNode *)(result->data);
        array->remove(array, deg);

        GenericPair *result2 = array->find(array, deg);
        if (result2 == NULL)
        {
            // debug_print("result2 == NULL\n");
            deg++;
            continue;
        }

        // debug_print("result2\n");
        if (result2->data == NULL)
        {
            // debug_print("result2->data == NULL\n");
            deg++;
            continue;
        }
        else
        {
            FHeapNode *f_node = (FHeapNode *)(result2->data);
            // debug_print("(%d)%d\n", f_node->pair->key, *(int *)(f_node->pair->data));
        }
        // print_generic_pair_node(result2);

        FHeapNode *y = (FHeapNode *)(result2->data);
        array->remove(array, deg);

        debug_print("after remove x,y from array\n");
        print_hash_table(array);

        if (x->pair->key > y->pair->key)
        {
            FHeapNode *temp = x;
            x = y;
            y = temp;
        }
        // x < y
        // x : be_parent
        // y : be_child

        f_heap_link(heap, x, y);
        debug_print("after link function : f heap \n");
        print_f_heap(heap);
        debug_print("after link function : hash table \n");
        print_hash_table(array);

        // update degree array of merged tree
        array->insert(array, create_generic_pair(deg + 1, x));
        debug_print("after insert x to array\n");
        debug_print("x : (%d)%d\n", x->pair->key, *(int *)(x->pair->data));
        print_hash_table(array);

        if( array->find(array,deg) == NULL ){
            deg++;
        }
    }

    debug_print("after merge trees with the same degree\n");
    // print_hash_table(array);

    // destroy_hash_table(&array);

    print_f_heap(heap);

    debug_print("finish consolidate\n");
}

// extract min
FHeapNode *f_heap_extract(FHeap *heap)
{
    debug_print("f_heap_extract\n");
    if( heap->root != NULL ){
        debug_print("root left : (%d)%d\n", heap->root->left->pair->key, *(int *)(heap->root->left->pair->data));
        debug_print("root : (%d)%d\n", heap->root->pair->key, *(int *)(heap->root->pair->data));
        debug_print("root right : (%d)%d\n", heap->root->right->pair->key, *(int *)(heap->root->right->pair->data));
    }
    else{
        debug_print("root == NULL\n");
    }

    if( heap->min != NULL ){
        debug_print("min left : (%d)%d\n", heap->min->left->pair->key, *(int *)(heap->min->left->pair->data));
        debug_print("min : (%d)%d\n", heap->min->pair->key, *(int *)(heap->min->pair->data));
        debug_print("min right : (%d)%d\n", heap->min->right->pair->key, *(int *)(heap->min->right->pair->data));
    }
    else{
        debug_print("min == NULL\n");
    }

    if (heap->min == NULL)
    {
        return NULL;
    }

    // update heap size
    --heap->size;

    // record original min children
    FHeapNode *child = heap->min->child;

    FHeapNode *copy = malloc(sizeof(FHeapNode));
    memcpy(copy, heap->min, sizeof(FHeapNode));

    // delete min from root list
    // debug_print("delete min from root list\n");
    if (heap->root->pair->key == heap->min->pair->key && *(int *)(heap->root->pair->data) == *(int *)(heap->min->pair->data))
    {
        // debug_print("heap->root == min\n");

        if (heap->min->right == heap->min) // only one node in root list
        {
            // debug_print("only one node in root list\n");
            heap->root = NULL;
            heap->min = NULL;
        }
        else // more than one node in root list
        {
            // debug_print("more than one node in root list\n");
            heap->min->right->left = heap->min->left;
            heap->min->left->right = heap->min->right;
            heap->root = heap->min->right;
        }

        // debug_print("after delete min from root list\n");
        // print_f_heap(heap);
    }
    else
    {
        // debug_print("heap->root != min\n");
        // print_f_heap(heap);
        // FHeapNode *min_left = heap->min->left;
        // FHeapNode *min_right = heap->min->right;
        // min_left->right = min_right;
        // min_right->left = min_left;
        // heap->min = min_right;

        FHeapNode *perv = heap->root->left;
        FHeapNode *next = heap->root;

        while( next != heap->min ){
            perv = next;
            next = next->right;
        }

        // remove `min` from root list
        perv->right = next->right;
        next->right->left = perv;




        // debug_print("after delete min from root list\n");
        // print_f_heap(heap);
    }

    // add children to root list
    if (child != NULL)
    {
        
        // debug_print("add children to root list\n");

        FHeapNode *cur = child;
        cur->left->right = NULL; // break the circular doubly linked list

        while (cur != NULL)
        {
            // debug_print("cur: (%d)%d\n", cur->pair->key, *(int *)(cur->pair->data));
            FHeapNode *right = cur->right;
            cur->parent = NULL;
            cur->left = NULL;
            cur->right = NULL;

            if( heap->root == NULL ){
                // debug_print("heap->root == NULL\n");

                heap->root = create_f_heap_node(cur->pair->key, *(int *)(cur->pair->data));
            }
            else{
                // debug_print("heap->root != NULL\n");
                heap->root->add_sibling(heap->root, cur);
            }

            cur = right;
        }

        // debug_print("after add children to root list\n");
        print_f_heap(heap);
    }
    else
    {
        // debug_print("no children\n");
    }

    // debug_print("update min before consolidate\n");
    f_heap_update_min(heap);
    // int new_min_key = 2147483647;
    // FHeapNode *temp = heap->root;
    // do
    // {
    //     if (temp->pair->key < new_min_key)
    //     {
    //         new_min_key = temp->pair->key;
    //         heap->min = temp;
    //     }
    //     temp = temp->right;
    // } while (temp != heap->root);

    // debug_print("after update min before consolidate\n");
    // print_f_heap(heap);

    // consolidate
    // debug_print("start consolidate\n");
    f_heap_consolidate(heap);

    // update min
    // debug_print("update min after consolidate\n");
    f_heap_update_min(heap);

    // update hash table
    heap->hash_table->remove_pair(heap->hash_table, copy->pair);

    return copy;
}

/* ------------------------ F-heap  Internal Functions ------------------------ */

// -----------------------------------------------------------------------------

/* ------------------------ F-heap  Public Functions ------------------------ */

GenericPair *get_min(FHeap *heap)
{
    if (heap->min == NULL)
    {
        return NULL;
    }
    return heap->min->pair;
}

// insert (key, value) into the F-heap
FHeapNode *public_insert(FHeap *heap, int key, int value)
{
    FHeapNode *new_node = create_f_heap_node(key, value);
    // debug_print("new_node->pair->key: %d\n", new_node->pair->key);
    // debug_print("new_node->pair->data: %d\n", *(int *)(new_node->pair->data));

    return f_heap_insert(heap, new_node);
}

// delete the node with key `key` and value `value` from the F-heap
FHeapNode *public_delete(FHeap *heap, int key, int value)
{
    // find node(key,val) in O(1) time
    GenericPair *pair = malloc(sizeof(GenericPair));
    pair->key = key;
    pair->data = malloc(sizeof(int));
    memcpy(pair->data, &value, sizeof(int));

    const GenericPair *result = heap->hash_table->find_pair(heap->hash_table, pair);
    if (result == NULL)
    {
        return NULL;
    }

    FHeapNode *oldNode = (FHeapNode *)(result->data);
    return f_heap_delete(heap, oldNode);
}

FHeapNode *public_extract(FHeap *heap)
{
    // debug_print("public_extract\n");
    return f_heap_extract(heap);
}

void public_decrease(FHeap *heap, int x, int val, int y)
{
    // debug_print("public_decrease\n");

    FHeapNode *node = (FHeapNode *)(heap->hash_table->find(heap->hash_table, x)->data);
    // debug_print("node->pair->key: %d\n", node->pair->key);
    // debug_print("node->pair->data: %d\n", *(int *)(node->pair->data));

    FHeapNode *cur = heap->root;
    if (cur != NULL)
    {
        do
        {
            if (cur == node)
            {
                // debug_print("cur == node\n");
                break;
            }
            cur = cur->right;
        } while (cur != heap->root);
        // debug_print("after find node in root list\n");
    }

    f_heap_decrease(heap, node, y);

    // debug_print("after f_heap_decrease\n");
    print_f_heap(heap);
}

/* ------------------------ F-heap  Public Functions ------------------------ */

// create a new F-heap
FHeap *create_f_heap()
{
    FHeap *heap = (FHeap *)malloc(sizeof(FHeap));
    heap->root = NULL;
    heap->min = NULL;
    heap->size = 0;
    // bind functions
    heap->insert = public_insert;
    heap->get_min = get_min;
    heap->delete = public_delete;
    heap->extract = public_extract;
    heap->decrease = public_decrease;

    // hash table
    heap->hash_table = create_hash_table(custom_hash_key);
    return heap;
}

// pre-defined operations
const char INSERT[] = "insert";
const char DELETE[] = "delete";
const char EXTRACT[] = "extract";
const char DECREASE[] = "decrease";
const char QUIT[] = "quit";

/* ------------------------ Debug Functions ------------------------ */

void print_hash_table(HashTable *hash_table)
{
    if (hash_table == NULL)
    {
        debug_print("print_hash_table : hash_table == NULL\n");
        return;
    }

    debug_print("===== Hash Table =====\n");
    int i;
    for (i = 0; i < hash_table->bucket_size; i++)
    {
        HashNode *node = hash_table->buckets[i];
        if (node == NULL)
        {
            continue;
        }

        debug_print("bucket[%d]: ", i);
        while (node != NULL)
        {
            if (node->pair.data == NULL)
            {
                debug_print("NULL ");
                node = node->next;
                continue;
            }
            int key = node->pair.key;
            FHeapNode *f_node = (FHeapNode *)(node->pair.data);
            debug_print("(%d)%d ", key, *(int *)(f_node->pair->data));
            node = node->next;
        }
        debug_print("\n");
    }
    debug_print("======================\n\n");
}

void print_f_heap(FHeap *heap)
{
    debug_print("===== F-Heap =====\n");
    debug_print("size: %d\n", heap->size);
    if (heap->min == NULL)
    {
        debug_print("min: NULL\n");
    }
    else
    {
        debug_print("min: (%d)%d\n", heap->min->pair->key, *(int *)(heap->min->pair->data));
    }
    debug_print("\n");

    // old version
    // debug_print("root: ");
    // FHeapNode *cur = heap->root;
    // if (cur != NULL)
    // {
    //     do
    //     {
    //         debug_print("(%d)%d ", cur->pair->key, *(int *)(cur->pair->data));
    //         cur = cur->right;
    //     } while (cur != heap->root);
    // }

    // print all roots & their children by level order
    HashTable *level_array = create_hash_table(custom_hash_key);

    FHeapNode *cur = heap->root;
    if (cur != NULL)
    {
        do
        {

            if (cur == NULL)
            {
                debug_print("cur == NULL\n");
                break;
            }

            int level = 1;

            // FHeapNode *copy = memcpy(malloc(sizeof(FHeapNode)), cur, sizeof(FHeapNode));
            // memcpy(copy->pair->data, &(copy->pair->key), sizeof(int));
            // level_array->insert(level_array, create_generic_pair(level, copy));

            level_array->insert(level_array, create_generic_pair(level, cur));

            // debug_print("root: (%d)%d\n", cur->pair->key, *(int *)(cur->pair->data));
            FHeapNode *temp = cur->child;
            if (temp == NULL)
            {
                cur = cur->right;
                continue;
            }

            while (temp)
            {
                level++;
                // debug_print("level[%d] : ", level);
                // level_array->insert(level_array, create_generic_pair(level, memcpy(malloc(sizeof(FHeapNode)), temp, sizeof(FHeapNode))));

                FHeapNode *sibling = temp;
                FHeapNode *grand_child = temp->child;
                do
                {
                    // FHeapNode *child_copy = memcpy(malloc(sizeof(FHeapNode)), sibling, sizeof(FHeapNode));
                    // memcpy(child_copy->pair->data, &(child_copy->pair->key), sizeof(int));
                    // level_array->insert(level_array, create_generic_pair(level, child_copy));

                    level_array->insert(level_array, create_generic_pair(level, sibling));
                    if( sibling->child != NULL ){
                        grand_child = sibling->child;
                    }

                    // debug_print("(%d)%d ", sibling->pair->key, *(int *)(sibling->pair->data));
                    sibling = sibling->right;
                } while (sibling != temp);

                temp = grand_child;
                // debug_print("\n");
            }

            cur = cur->right;
            debug_print("\n");
        } while (cur != heap->root);
    }

    debug_print("visualization:\n");
    int level = 1;
    while (level_array->find(level_array, level))
    {
        GenericPair *result = level_array->find(level_array, level);
        if (result == NULL)
        {
            level++;
            continue;
        }

        debug_print("level[%d]: ", level);
        while (result)
        {

            FHeapNode *node = (FHeapNode *)(result->data);

            debug_print("(%d)%d ", node->pair->key, *(int *)(node->pair->data));

            level_array->remove(level_array, level);
            result = level_array->find(level_array, level);
        }
        debug_print("\n");
        level++;
    }

    // while (level < 5)
    // {
    //     if (level == 1)
    //     {
    //         debug_print("root [%d]: ", level);

    //         FHeapNode *cur = heap->root;
    //         if (cur != NULL)
    //         {
    //             do
    //             {
    //                 debug_print("(%d)%d ", cur->pair->key, *(int *)(cur->pair->data));
    //                 cur = cur->right;
    //             } while (cur != heap->root);
    //         }
    //         debug_print("\n");
    //         level++;
    //         continue;
    //     }
    //     GenericPair *result = level_array->find(level_array, level);
    //     if( result == NULL ){
    //         level++;
    //         continue;
    //     }

    //     debug_print("level[%d]: ", level);
    //     while (result)
    //     {

    //         FHeapNode *node = (FHeapNode *)(result->data);

    //         FHeapNode *cur = node;
    //         do
    //         {
    //             debug_print("(%d)%d ", cur->pair->key, *(int *)(cur->pair->data));
    //             cur = cur->right;
    //         } while (cur != node);

    //         level_array->remove(level_array, level);
    //         result = level_array->find(level_array, level);
    //     }
    //     debug_print("\n");
    //     level++;
    // }

    // destroy_hash_table(level_array);

    debug_print("==================\n\n");

    // destroy_hash_table(&level_array);
}

void print_f_heap_node(FHeapNode *node)
{
    debug_print("===== F-Heap Node =====\n");
    print_generic_pair_int(node->pair);

    debug_print("degree: %d\n", node->degree);
    debug_print("marked: %d\n", node->marked);

    debug_print("left: \n");
    if (node->left != NULL)
    {
        print_generic_pair_node(node->left->pair);
    }
    else
    {
        debug_print("left: NULL\n");
    }

    debug_print("right: \n");
    if (node->right != NULL)
    {
        print_generic_pair_node(node->right->pair);
    }
    else
    {
        debug_print("right: NULL\n");
    }

    if (node->parent != NULL)
    {
        debug_print("parent: %d\n", node->parent->pair->key);
    }
    else
    {
        debug_print("parent: NULL\n");
    }

    if (node->child != NULL)
    {
        debug_print("child: %d\n", node->child->pair->key);
    }
    else
    {
        debug_print("child: NULL\n");
    }
    debug_print("=======================\n\n");
}

void print_generic_pair_node(GenericPair *pair)
{
    debug_print("===== Generic Pair =====\n");
    if (pair == NULL)
    {
        debug_print("pair: NULL\n");
        debug_print("========================\n\n");
        return;
    }

    debug_print("key: %d\n", pair->key);
    if (pair->data == NULL)
    {
        debug_print("value: NULL\n");
    }

    print_f_heap_node((FHeapNode *)(pair->data));
    debug_print("========================\n\n");
}

void print_generic_pair_int(GenericPair *pair)
{
    debug_print("===== Generic Pair =====\n");
    if (pair == NULL)
    {
        debug_print("pair: NULL\n");
        debug_print("========================\n\n");
        return;
    }

    debug_print("key: %d\n", pair->key);
    if (pair->data == NULL)
    {
        debug_print("value: NULL\n");
    }
    else
    {
        debug_print("value: %d\n", *(int *)(pair->data));
    }
    debug_print("========================\n\n");
}

int main(int argc, char *argv[])
{

    if (argc == 2)
    {
        DEBUG = atoi(argv[1]);
    }

    FHeap *f_heap = create_f_heap();

    int run = 1;
    char operation[10];
    while (run && scanf("%s", operation) != EOF)
    {
        debug_print("@%s\n", operation);
        if (strcmp(operation, INSERT) == 0)
        {
            int key, value;
            scanf("%d %d", &key, &value);
            debug_print("key: %d val: %d\n", key, value);
            f_heap->insert(f_heap, key, value);
        }
        else if (strcmp(operation, DELETE) == 0)
        {
            int key, value;
            scanf("%d %d", &key, &value);
            debug_print("key: %d val: %d\n", key, value);
            f_heap->delete (f_heap, key, value);
        }
        else if (strcmp(operation, EXTRACT) == 0)
        {
            FHeapNode *node = f_heap->extract(f_heap);
            if (node != NULL)
            {
                // debug_print("(%d)%d\n", node->pair->key, *(int *)(node->pair->data));
                
                printf("(%d)%d\n", node->pair->key, *(int *)(node->pair->data));
            }
        }
        else if (strcmp(operation, DECREASE) == 0)
        {
            int x, val, y;
            scanf("%d %d %d", &x, &val, &y);
            debug_print("x: %d val: %d y: %d\n", x, val, y);
            f_heap->decrease(f_heap, x, val, y);
        }
        else if (strcmp(operation, QUIT) == 0)
        {
            run = 0;
            break;
        }

        debug_print("[¯¯¯¯¯¯¯¯¯ Finish %s ¯¯¯¯¯¯¯¯¯]\n", operation);
        print_f_heap(f_heap);
        print_hash_table(f_heap->hash_table);
        debug_print("[_________ Finish %s _________]\n", operation);
    }

    return 0;
}