/* copyright Scott K Punshon, 2012
 *
 * AVL Tree (non-recursive implementation)
 */
#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__


#define __AVL_LEFT	(-1 )
#define __AVL_EVEN	( 0 )
#define __AVL_RIGHT	( 1 )


/**
 * struct tree_head 	- AVL Tree structure
 * @root 	- pointer to the root node of the AVL tree
 * @compare 	- function to compare @query against @benchmark, returning __AVL_LEFT if
 * 				@query is before (left), __AVL_RIGHT if @query is after or __AVL_EVEN if
 * 				no difference
*/
struct tree_head {
	struct tree_entry* 	root;
	int (*compare)(void* query, void* benchmark);
};

/**
 * struct tree_entry 	- AVL Tree node structure
 * @left 	- pointer to the left subtree node
 * @right 	- pointer to the right subtree node
 * @parent 	- pointer to the parent node
 * @sortkey_data 	- pointer to private data used by the node comparison function
 * @balance 	- the current balance factor of this node
*/
struct tree_entry {
	struct tree_entry* 	left;
	struct tree_entry* 	right;
	struct tree_entry* 	parent;
	void* sortkey_data;
	signed char balance;
};

/**
 * avl_find 	- finds a node whose sortkey data matches the
 * specified sortkey data according to the AVL Tree's compare routine
 * @tree - pointer to the tree base structure
 * @sortkey_data - pointer to the sortkey data to search for
 * returns: pointer to matching node, otherwise NULL if none found
*/
extern struct tree_entry* avl_find(struct tree_head* tree, void* sortkey_data);

/**
 * avl_insert 	- inserts a new node into an AVL Tree
 * @tree - pointer to the tree base structure
 * @entry - pointer to the new node to insert
 * returns: TRUE if node was successfully inserted, otherwise FALSE
*/
extern int avl_insert(struct tree_head* tree, struct tree_entry* entry);

/**
 * avl_remove 	- remove a node from an AVL Tree
 * @tree - pointer to the tree base structure
 * @entry - pointer to the node to remove
*/
extern void avl_remove(struct tree_head* tree, struct tree_entry* entry);

/**
 * avl_init_tree 	- initialise a new AVL Tree
 * @new_tree - the new struct tree_head to initialise
 * @compare(void* query, void* benchmark) - function used to compare the sortkey data of
 * 		two tree entries. If the data @query comes before @benchmark then the function is
 * 		to return __AVL_LEFT, if @query comes after @benchmark then it is to return
 * 		__AVL_RIGHT, otherwise they are equal and it is to return __AVL_EVEN.
*/
extern void avl_init_tree(struct tree_head* new_tree, int (*compare)(void* query, void* benchmark));


#endif /* __AVL_TREE_H__ */
