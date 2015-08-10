/* copyright Scott K Punshon, 2012
 *
 * AVL Tree (non-recursive implementation)
 */

#include "avl_tree.h"


#ifndef NULL
#define NULL ((void*) 0)
#endif

#define __AVL_MIN(a, b) 	( ((a) < (b)) ? (a) : (b) )
#define __AVL_MAX(a, b) 	( ((a) > (b)) ? (a) : (b) )
#define __AVL_SIGNUM(a) 	( ((a) > 0) - ((a) < 0) )


/* get pointer to node's refering pointer */
static inline
struct tree_entry** __referring_ptr(struct tree_head* tree, struct tree_entry* node)
{
	struct tree_entry** pptr = NULL;
	struct tree_entry* p = node->parent;
	if(p == NULL) {
		pptr = &(tree->root);
	} else {
		pptr = (p->left == node) ? &(p->left) : &(p->right);
	}
	return(pptr);
}


/* single right rotation */
//static inline
void __rotate_rr(struct tree_entry** ptr_root)
{
	struct tree_entry* t = *ptr_root;
	struct tree_entry* l = t->left;

	/* perform rotation */
	*ptr_root = l;
	l->parent = t->parent;

	t->left = l->right;
	if(l->right != NULL)
		l->right->parent = t;

	l->right = t;
	t->parent = l;

	/* update balances */
	t->balance += 1 - __AVL_MIN(0, l->balance);
	l->balance += 1 + __AVL_MAX(0, t->balance);
}


/* single left rotation */
//static inline
void __rotate_ll(struct tree_entry** ptr_root)
{
	struct tree_entry* t = *ptr_root;
	struct tree_entry* r = t->right;

	/* perform rotation */
	*ptr_root = r;
	r->parent = t->parent;

	t->right = r->left;
	if(r->left != NULL)
		r->left->parent = t;

	r->left = t;
	t->parent = r;

	/* update balances */
	t->balance -= 1 + __AVL_MAX(0, r->balance);
	r->balance -= 1 - __AVL_MIN(0, t->balance);
}


/* double right rotation */
static inline
void __rotate_rl(struct tree_entry** ptr_root)
{
	__rotate_ll(&((*ptr_root)->left));
	__rotate_rr(ptr_root);
}


/* double left rotation */
static inline
void __rotate_lr(struct tree_entry** ptr_root)
{
	__rotate_rr(&((*ptr_root)->right));
	__rotate_ll(ptr_root);
}


/* rebalance AVL Tree/Sub-tree */
static inline
void __balance(struct tree_entry** ptr_root)
{
	if(*ptr_root == NULL)
		return;

	if((*ptr_root)->balance < __AVL_LEFT) {
		/* root is left imbalanced --> need a right rotation of some sort */
		if((*ptr_root)->left->balance == __AVL_RIGHT) {
			/* sub-tree is heavy in the opposite direction --> RL double rotation needed */
			__rotate_rl(ptr_root);
		} else {
			/* sub-tree is either heavy in the same direction or otherwise evenly
			balanced (which can only occur during node deletion) --> RR single rotation needed */
			__rotate_rr(ptr_root);
		}
	} else if((*ptr_root)->balance > __AVL_RIGHT) {
		/* root is right imbalanced --> need a left rotation of some sort */
		if((*ptr_root)->right->balance == __AVL_LEFT) {
			/* sub-tree is heavy in the opposite direction --> LR double rotation needed */
			__rotate_lr(ptr_root);
		} else {
			/* sub-tree is either heavy in the same direction or otherwise evenly
			balanced (which can only occur during node deletion) --> LL single rotation needed */
			__rotate_ll(ptr_root);
		}
	}
}


/* default comparison function (if none is given) (just compares the pointer address values) */
int __default_compare(void* query, void* benchmark)
{
	if(query < benchmark)
		return(__AVL_LEFT);
	else if(query > benchmark)
		return(__AVL_RIGHT);

	return(__AVL_EVEN);
}




/* (see header file for details) find node with matching sortkey data according
to the AVL Tree's compare function */
struct tree_entry* avl_find(struct tree_head* tree, void* sortkey_data)
{
	if(tree == NULL || sortkey_data == NULL)
		return(NULL);

	struct tree_entry* n = tree->root;
	while(n != NULL) {
		int comparison = tree->compare(sortkey_data, n->sortkey_data);
		if(comparison <= __AVL_LEFT) {
			/* data belongs to the left */
			n = n->left;
		} else if(comparison >= __AVL_RIGHT) {
			/* data belongs to the right */
			n = n->right;
		} else {
			/* found matching */
			break;
		}
	}
	return(n);
}


/* (see header file for details) insert new node into AVL Tree */
int avl_insert(struct tree_head* tree, struct tree_entry* entry)
{
	if(tree == NULL || entry == NULL)
		return(0);

	struct tree_entry* p = NULL;
	struct tree_entry** pptr_n = &(tree->root);
	struct tree_entry* n = *pptr_n;
	/* find the position for new node */
	while(n != NULL) {
		int comparison = tree->compare(entry->sortkey_data, n->sortkey_data);
		if(comparison <= __AVL_LEFT) {
			/* node belongs to the left */
			pptr_n = &(n->left);
		} else if(comparison >= __AVL_RIGHT) {
			/* node belongs to the right */
			pptr_n = &(n->right);
		} else {
			/* a node with the same key exists */
			pptr_n = NULL;
			entry = NULL;
			break;
		}
		p = n;
		n = *pptr_n;
	}

	if(entry != NULL) {
		/* insert new node */
		entry->left = NULL;
		entry->right = NULL;
		entry->parent = p;
		entry->balance = __AVL_EVEN;
		*pptr_n = entry;

		/* update ancestor node balance factors & address any imbalances */
		n = entry->parent;
		p = entry;
		while(n != NULL) {
			n->balance += (n->left == p) ? __AVL_LEFT : __AVL_RIGHT;
			pptr_n = __referring_ptr(tree, n);
			__balance(pptr_n);
			if(n->balance == __AVL_EVEN) {
				/* all rotation cases, which address any AVL violations by reducing the height
				of the heavy sub-tree, that are possible during insertion (i.e. not 2/0
				or -2/0) *always* result in the sub-root's balance becoming zero/EVEN. We test
				here to see if the balancing function performed any rotations, because if so then
				the heavy sub-tree's height will have been reduced by one, hence couter-acting the
				height change from the insertion, and so we can stop rebalancing ancestors. */
				break;
			}
			p = n;
			n = n->parent;
		}
	}

	return((entry != NULL) ? 1 : 0);
}


/* (see header file for details) remove node from AVL Tree */
void avl_remove(struct tree_head* tree, struct tree_entry* entry)
{
	if(tree == NULL || entry == NULL)
		return;

	struct tree_entry tmp;
	struct tree_entry* c = NULL;
	struct tree_entry* p = NULL;
	struct tree_entry** pptr_del = NULL;
	struct tree_entry* del = avl_find(tree, entry->sortkey_data);

	if(del == NULL || del != entry)
		return;

	/* setup work for cases where the deletion node has two sub-trees */
	if(del->left != NULL && del->right != NULL) {
		p = del->parent;
		pptr_del = __referring_ptr(tree, del);
		/* put the right-most node in left sub-tree in its place */
		c = del->left;
		while(c->right != NULL)
			c = c->right;

		struct tree_entry** pptr_c = __referring_ptr(tree, c);
		/* create a temporary node that is a copy of the replacement node, and
		put the temporary copy in it's place */
		tmp = *c;
		*pptr_c = &tmp;
		/* move the real replacement node into the deletion node's position */
		c->balance = del->balance;
		del->left->parent = c;
		c->left = del->left;
		del->right->parent = c;
		c->right = del->right;
		c->parent = del->parent;
		*pptr_del = c;
		/* clear deleted node's tree fields */
		del->left = NULL;
		del->right = NULL;
		del->parent = NULL;
		/* a temporary node, guarenteed to have at most one child, has taken the place of the
		replacement node's original position - mark it to be deleted */
		del = &tmp;
	}

	/* delete the node that has only one or zero child sub-trees */
	p = del->parent;
	pptr_del = __referring_ptr(tree, del);

	c = (del->left != NULL) ? del->left : del->right;
	*pptr_del = c;
	if(c != NULL)
		c->parent = p;

	/* move upwards to adjust balance factors changes */
	c = del;
	while(p != NULL) {
		p->balance -= (c == p->left) ? __AVL_LEFT : __AVL_RIGHT;
		__balance(__referring_ptr(tree, p));
		if(p->balance != __AVL_EVEN) {
			/* current sub-tree has non-even balance after balancing, hence
			no change in sub-tree height so no need to check for any more imbalances */
			break;
			/* this is because the deletion made this sub-tree either:
			(a) heavy on one side (from an original even balance) --> no height change
			(b) imbalanced in a 2/0 or -2/0 case, in which case the balancing rotations
			result in a non-even/heavy balance and cause no height change */
		}
		c = p;
		p = p->parent;
	}

	/* clear deleted node's tree fields */
	del->parent = NULL;
	del->left = NULL;
	del->right = NULL;
	del = NULL;
}


/* (see header file for details) initialise a new AVL Tree */
void avl_init_tree(struct tree_head* new_tree, int (*compare)(void* query, void* benchmark))
{
	if(new_tree == NULL)
		return;

	new_tree->root = NULL;
	new_tree->compare = (compare != NULL) ? compare : __default_compare;
}

