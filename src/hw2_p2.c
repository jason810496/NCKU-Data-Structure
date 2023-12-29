#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define RED 'r'
#define BLACK 'b'

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

int log2(int x)
{
    int result = 0;
    while (x >>= 1)
        result++;
    return result;
}

int pow(int x, int y)
{
    int result = 1;
    while (y--)
        result *= x;
    return result;
}

typedef struct __node__
{
    struct __node__ *left;
    struct __node__ *right;
    struct __node__ *parent;
    int key;
    char color;
} node;

// node functions prototype
node *create_node(int key, char color);

// node functions implementation
node *create_node(int key, char color)
{
    node *new_node = (node *)malloc(sizeof(node));
    new_node->key = key;
    new_node->color = color;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    return new_node;
}

typedef struct __rbtree__
{
    node *root;
    node *nil;
    int size;

    struct __node__ *(*search)(struct __rbtree__ *T, const int key);
    // return 1 if key exists, 0 otherwise
    int (*insert)(struct __rbtree__ *T, const int key);
    int (*delete)(struct __rbtree__ *T, const int key);
} rbtree;

// rbtree functions prototype
void print_rbtree(rbtree *T);
struct __node__ *search(struct __rbtree__ *T, const int key);
int insert(struct __rbtree__ *T, const int key);
void right_rotate(struct __rbtree__ *T, struct __node__ *cur);
void left_rotate(struct __rbtree__ *T, struct __node__ *x);
void insert_fixup(struct __rbtree__ *T, struct __node__ *cur);
int delete(struct __rbtree__ *T, const int key);
void delete_fixup(struct __rbtree__ *T, struct __node__ *cur);
node *successor(rbtree *T, node *cur);
rbtree *create_rbtree();

typedef struct __link_node__
{
    struct __link_node__ *next;
    node *data;
} link_node;

typedef struct __link_list__
{
    link_node *head;
    int size;
} link_list;

// link_list functions prototype
link_list *create_link_list();
void push_back(link_list **list, node *data);
node *pop_front(link_list **list);
// link_list functions implementation
link_list *create_link_list()
{
    link_list *new_list = (link_list *)malloc(sizeof(link_list));
    new_list->head = NULL;
    new_list->size = 0;
    return new_list;
}

void push_back(link_list **list, node *data)
{
    link_node *new_node = (link_node *)malloc(sizeof(link_node));
    new_node->data = data;
    new_node->next = NULL;

    if ((*list)->head == NULL)
    {
        (*list)->head = new_node;
    }
    else
    {
        link_node *cur = (*list)->head;
        while (cur->next != NULL)
            cur = cur->next;
        cur->next = new_node;
    }
    (*list)->size++;
}

node *pop_front(link_list **list)
{
    if ((*list)->head == NULL)
        return NULL;
    else
    {
        link_node *cur = (*list)->head;
        (*list)->head = (*list)->head->next;
        node *data = cur->data;
        free(cur);
        (*list)->size--;
        return data;
    }
}

void print_node(node *cur)
{
    if (cur == NULL)
    {
        if (DEBUG == 2)
        {
            debug_print("  ");
        }
        else
        {
            debug_print("NULL");
        }
        return;
    }

    if (cur->color == RED)
    {
        if (DEBUG == 2)
        {
            debug_print("\033[31m%d\033[0m", cur->key);
        }
        else
        {
            debug_print("%d(R)", cur->key);
        }
    }
    else
    {
        if (DEBUG == 2)
        {
            debug_print("\033[30m%d\033[0m", cur->key);
        }
        else
        {
            debug_print("%d(B)", cur->key);
        }
    }
}

// rbtree functions implementation
void print_rbtree_old(rbtree *T)
{
    int level = log2(T->size);
    int padding_level = level + 10;

    link_list **array = (link_list **)malloc(sizeof(link_list *) * padding_level);
    for (int i = 0; i < padding_level; i++)
    {
        array[i] = create_link_list();
    }

    link_list *queue = create_link_list();

    push_back(&queue, T->root);

    debug_print("======  RBTree Information  ======\n");
    debug_print("size: %d\n", T->size);
    debug_print("level: %d\n", level);
    debug_print("======  RBTree Information  ======\n");

    // bfs
    int max_count = 0;
    int cur_level = 0;
    while (queue->size != 0)
    {
        int size = queue->size;
        for (int i = 0; i < size; i++)
        {
            node *cur = pop_front(&queue);

            push_back(&array[cur_level], cur);

            if (cur->left != T->nil && cur->left != NULL)
            {
                push_back(&queue, cur->left);
            }
            if (cur->right != T->nil && cur->right != NULL)
            {
                push_back(&queue, cur->right);
            }
        }
        if (size > max_count)
        {
            max_count = size;
        }
        cur_level++;
    }

    debug_print("====== RBTree Visualization ======\n");
    for (int lev = 0; lev < cur_level; lev++)
    {
        debug_print("level %d: ", lev);
        int space = pow(2, level - lev) - 1;
        if (space < 0)
        {
            space = 0;
        }
        for (int j = 0; j < space; j++)
        {
            debug_print(" ");
        }

        int size = array[lev]->size;
        for (int i = 0; i < size; i++)
        {
            node *cur = pop_front(&array[lev]);

            print_node(cur);
            debug_print(" ");
        }
        free(array[lev]);
        debug_print("\n");
    }
    free(array);

    debug_print("====== RBTree Visualization ======\n");
    return;
}

int store_recursive(rbtree *T, node **array, node *cur, int idx)
{
    if (cur == T->nil)
    {
        return 0;
    }

    array[idx] = cur;
    int ht = 0;
    int lht = store_recursive(T, array, cur->left, idx * 2 + 1);
    int rht = store_recursive(T, array, cur->right, idx * 2 + 2);
    ht = (lht > rht ? lht : rht);

    return ht + 1;
}

void print_rbtree(rbtree *T)
{
    int level = log2(T->size);
    int padding_level = level+3;

    node** array = malloc(sizeof(node*) * pow(2,padding_level) );

    link_list *queue = create_link_list();

    push_back(&queue, T->root);

    debug_print("======  RBTree Information  ======\n");
    debug_print("size: %d\n", T->size);
    debug_print("level: %d\n", level);
    debug_print("======  RBTree Information  ======\n");

    // dfs
    level = store_recursive(T,array,T->root,0);
    debug_print("finish recursive\n");
    debug_print("level %d\n" , level );

    debug_print("====== RBTree Visualization ======\n");
    for (int lev = 0; lev < level; lev++)
    {
        debug_print("level[%2d] ", lev);
        // calculate padding;
        int padding = pow(2,level-lev)/2;
        if( lev == 0){
            padding = padding/2 + 1;
        }
        if( lev == level-1 ){
            padding = 0;
        }
        while(padding--){
            debug_print("  ");
        }

        int cnt = pow(2,lev);
        int start = pow(2,lev)-1;
        for(int i=0;i<cnt;i++){
            print_node( array[start+i] );
            debug_print(" ");
        }

        debug_print("\n");
    }
    free(array);

    debug_print("====== RBTree Visualization ======\n");
    return;
}

struct __node__ *search(struct __rbtree__ *T, const int key)
{
    node *x = T->root;
    while (x != T->nil && x->key != key)
    {
        if (key < x->key)
            x = x->left;
        else
            x = x->right;
    }
    return x;
}

int insert(struct __rbtree__ *T, const int key)
{
    T->size++;

    node *insert_node = create_node(key, RED);
    node *y = T->nil;
    node *x = T->root;

    while (x != T->nil)
    {
        y = x;
        if (insert_node->key < x->key)
            x = x->left;
        else
            x = x->right;
    }

    insert_node->parent = y;
    if (y == T->nil)
        T->root = insert_node;
    else if (insert_node->key < y->key)
        y->left = insert_node;
    else
        y->right = insert_node;

    insert_node->left = T->nil;
    insert_node->right = T->nil;

    insert_fixup(T, insert_node);

    return 1;
}

void right_rotate(struct __rbtree__ *T, struct __node__ *cur)
{
    node *left = cur->left;
    cur->left = left->right;
    if (left->right != T->nil)
    {
        left->right->parent = cur;
    }

    left->parent = cur->parent;
    if (cur->parent == T->nil)
    {
        T->root = left;
    }
    else if (cur == cur->parent->right)
    {
        cur->parent->right = left;
    }
    else
    {
        cur->parent->left = left;
    }

    left->right = cur;
    cur->parent = left;
}

void left_rotate(struct __rbtree__ *T, struct __node__ *x)
{
    node *right = x->right;
    x->right = right->left;
    if (right->left != T->nil)
    {
        right->left->parent = x;
    }

    right->parent = x->parent;
    if (x->parent == T->nil)
    {
        T->root = right;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = right;
    }
    else
    {
        x->parent->right = right;
    }

    right->left = x;
    x->parent = right;
}

void insert_fixup(struct __rbtree__ *T, struct __node__ *cur)
{
    /*
    - case 1 : uncle is red ( cur can be red/black )
        - recolor
    - case 2 : uncle is black ( cur can be red/black )
        - cur is right child
            - left rotate
            - case 3 ( after left rotate , cur is left child )
    - case 3 : uncle is black ( cur can be red/black )
        - cur is left child
            - right rotate
    */

    while (cur->parent->color == RED)
    {
        // parent is grandparent of left child
        if (cur->parent == cur->parent->parent->left)
        {
            node *uncle = cur->parent->parent->right;
            if (uncle->color == RED) // case 1
            {
                cur->parent->color = BLACK;
                uncle->color = BLACK;
                cur->parent->parent->color = RED;
                cur = cur->parent->parent;
            }
            else
            {
                if (cur == cur->parent->right) // case 2
                {
                    cur = cur->parent;
                    left_rotate(T, cur);
                }
                // case 3
                cur->parent->color = BLACK;
                cur->parent->parent->color = RED;
                right_rotate(T, cur->parent->parent);
            }
        }
        else // parent is grandparent of right child
        {
            node *uncle = cur->parent->parent->left;
            if (uncle->color == RED) // case 1
            {
                cur->parent->color = BLACK;
                uncle->color = BLACK;
                cur->parent->parent->color = RED;
                cur = cur->parent->parent;
            }
            else
            {
                if (cur == cur->parent->left)
                {
                    cur = cur->parent;
                    right_rotate(T, cur);
                }
                cur->parent->color = BLACK;
                cur->parent->parent->color = RED;
                left_rotate(T, cur->parent->parent);
            }
        }
    }

    // make sure root is black
    T->root->color = BLACK;
}

node *successor(rbtree *T, node *cur)
{
    if (cur->right != T->nil)
    {
        cur = cur->right;
        // return leftmost node
        while (cur->left != T->nil)
        {
            cur = cur->left;
        }
        return cur;
    }

    node *parent = cur->parent;
    while (parent != T->nil && cur == parent->right)
    {
        cur = parent;
        parent = parent->parent;
    }
}

int delete(struct __rbtree__ *T, const int key)
{
    node *to_delete = search(T, key);
    if (to_delete == T->nil)
    {
        debug_print("Not found\n");
        return 0;
    }

    T->size--;

    node *cur;
    node *child;

    if (to_delete->left == T->nil || to_delete->right == T->nil)
    {
        cur = to_delete;
    }
    else
    {
        cur = successor(T, to_delete);
        debug_print("successor: %d\n", cur->key);
    }

    if (cur->left != T->nil)
        child = cur->left;
    else
        child = cur->right;

    child->parent = cur->parent;

    if (cur->parent == T->nil)
    {
        T->root = child;
    }
    else if (cur == cur->parent->left)
    {
        cur->parent->left = child;
    }
    else
    {
        cur->parent->right = child;
    }

    if (cur != to_delete)
    {
        to_delete->key = cur->key;
    }

    if (cur->color == BLACK)
    {
        delete_fixup(T, child);
    }

    // free(cur);

    return 1;
}

void delete_fixup(struct __rbtree__ *T, struct __node__ *cur)
{
    /*
    cur is black
    - sibling is red
        - case 1
    - sibling is black
        - sibling's children are black
            - case 2
        - sibling's left child is red & right child is black
            - case 3
        - sibling's right child is red
            - case 4
    */

    // case 1 : cur is red
    /*
     - recolor sibling to black
     - recolor parent to red
     - left rotate parent of cur
     - move sibling to cur's parent's right child
    */

    debug_print("in delete_fixup\n");

    while (cur != T->root && cur->color == BLACK)
    {
        // cur is left child
        if (cur == cur->parent->left)
        {

            node *sibling = cur->parent->right;

            // case 1 : sibling is red
            if (sibling->color == RED)
            {
                sibling->color = BLACK;
                cur->parent->color = RED;
                left_rotate(T, cur->parent);
                sibling = cur->parent->right;
            }

            // sibling must be black now
            // case 2 : sibling children are all black
            if (sibling->left->color == BLACK && sibling->right->color == BLACK)
            {
                sibling->color = RED;
                cur = cur->parent;
            }
            else // case 3 & 4
            {
                // case 3 : sibling's left child is red & right child is black
                if (sibling->right->color == BLACK)
                {
                    sibling->left->color = BLACK;
                    sibling->color = RED;
                    right_rotate(T, sibling);
                    sibling = cur->parent->right;
                }
                // after case 3, sibling's right child must be red
                // now must be case 4

                // case 4 : sibling's right child is red
                sibling->color = cur->parent->color;
                cur->parent->color = BLACK;
                sibling->right->color = BLACK;
                left_rotate(T, cur->parent);
                cur = T->root;
            }
        }
        else // cur is right child
        {
            node *sibling = cur->parent->left;
            // case 1 : sibling is red
            if (sibling->color == RED)
            {
                sibling->color = BLACK;
                cur->parent->color = RED;
                right_rotate(T, cur->parent);
                sibling = cur->parent->left;
            }

            // sibling must be black now
            // case 2 : sibling children are all black
            if (sibling->left->color == BLACK && sibling->right->color == BLACK)
            {
                sibling->color = RED;
                cur = cur->parent;
            }
            else
            {
                // case 3 : sibling's right child is red & left child is black
                if (sibling->left->color == BLACK)
                {
                    sibling->right->color = BLACK;
                    sibling->color = RED;
                    left_rotate(T, sibling);
                    sibling = cur->parent->left;
                }
                // case 4 : sibling's left child is red
                sibling->color = cur->parent->color;
                cur->parent->color = BLACK;
                sibling->left->color = BLACK;
                right_rotate(T, cur->parent);
                cur = T->root;
            }
        }
    }

    cur->color = BLACK;
}

rbtree *create_rbtree()
{
    rbtree *new_rbtree = (rbtree *)malloc(sizeof(rbtree));
    new_rbtree->nil = create_node(0, BLACK);
    new_rbtree->root = new_rbtree->nil;
    new_rbtree->size = 0;
    new_rbtree->search = search;
    new_rbtree->insert = insert;
    new_rbtree->delete = delete;
    return new_rbtree;
}

const char SEARCH[] = "search";
const char INSERT[] = "insert";
const char DELETE[] = "delete";
const char QUIT[] = "quit";
const char RED_COLOR[] = "red";
const char BLACK_COLOR[] = "black";
const char NO_FOUND[] = "Not found";

int main(int argc, char *argv[])
{

    if (argc == 2)
    {
        DEBUG = atoi(argv[1]);
    }

    char operation[10];
    rbtree *T = create_rbtree();

    while (scanf("%s", operation) != EOF)
    {
        debug_print("@operation: %s\n", operation);
        if (strcmp(operation, SEARCH) == 0)
        {
            int key;
            scanf("%d", &key);
            node *result = T->search(T, key);
            if (result == T->nil)
            {
                printf("%s\n", NO_FOUND);
            }
            else
            {
                printf("%s\n", result->color == RED ? RED_COLOR : BLACK_COLOR);
            }
        }
        else if (strcmp(operation, INSERT) == 0)
        {
            int key;
            scanf("%d", &key);
            debug_print("key: %d\n", key);
            int result = T->insert(T, key);
            if (result == 1)
            {
                debug_print("Insert success\n");
            }
            else
            {
                debug_print("Insert fail\n");
            }
        }
        else if (strcmp(operation, DELETE) == 0)
        {
            int key;
            scanf("%d", &key);
            int result = T->delete (T, key);
            if (result == 1)
            {
                debug_print("Delete success\n");
            }
            else
            {
                debug_print("Delete fail\n");
            }
        }
        else if (strcmp(operation, QUIT) == 0)
        {
            break;
        }

        if (DEBUG)
        {
            print_rbtree(T);
        }
    }
    return 0;
}