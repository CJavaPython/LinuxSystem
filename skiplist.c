
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h> // for kmalloc
#include <linux/timekeeping.h>

#include <linux/random.h>

 

#define MAX_HEIGHT (32)

typedef struct skiplist * Skiplist;

 

 

/* create an empty skiplist */

Skiplist skiplistCreate(void);

 

/* destroy a skiplist */

void skiplistDestroy(Skiplist s);

 

/* return maximum key less than or equal to key */

/* or INT_MIN if there is none */

int skiplistSearch(Skiplist s, int key);

 

/* insert a new key into s */

void skiplistInsert(Skiplist s, int key);

 

/* delete a key from s */

void skiplistDelete(Skiplist s, int key);







 

struct skiplist {
    int key;
    int height;                /* number of next pointers */
    struct skiplist *next[1];  /* first of many */

};

 

/* choose a height according to a geometric distribution */

static int chooseHeight(void)
{
    int i;
    int rand_num;
    get_random_bytes(&rand_num, sizeof(int));
    for(i = 1; i < MAX_HEIGHT && rand_num % 2 == 0; i++); 
    return i;
}
 

/* create a skiplist node with the given key and height */

/* does not fill in next pointers */

static Skiplist skiplistCreateNode(int key, int height)

{
    Skiplist s;
    s = kmalloc(sizeof(struct skiplist) + sizeof(struct skiplist *) * (height - 1) , GFP_KERNEL);
    s->key = key;
    s->height = height;
    printk("2 : %d\n", s->key);
    return s;
}

 

/* create an empty skiplist */

Skiplist skiplistCreate(void)

{
    Skiplist s;
    int i;
    /* s is a dummy head element */
    s = skiplistCreateNode(INT_MIN, MAX_HEIGHT);
    /* this tracks the maximum height of any node */
    s->height = 1;
    for(i = 0; i < MAX_HEIGHT; i++) {
        s->next[i] = 0;
    }
    return s;
}

 

/* free a skiplist */

void skiplistDestroy(Skiplist s)

{

    Skiplist next;
    while(s) {
        next = s->next[0];
        kfree(s);
        s = next;
    }
}

 

/* return maximum key less than or equal to key */

/* or INT_MIN if there is none */

int skiplistSearch(Skiplist s, int key)
{
    int level;
    for(level = s->height - 1; level >= 0; level--) {
        while(s->next[level] && s->next[level]->key <= key) {
            s = s->next[level];
        }
    }
    return s->key;
}

 

/* insert a new key into s */

void skiplistInsert(Skiplist s, int key)

{
    int level;
    Skiplist elt;
    printk("1 : %d\n", key);

    elt = skiplistCreateNode(key, chooseHeight()); 

    if(elt->height > s->height) {

        s->height = elt->height;

    }
    /* search through levels taller than elt */

    for(level = s->height - 1; level >= elt->height; level--) {

        while(s->next[level] && s->next[level]->key < key) {

            s = s->next[level];

        }

    }

 

    /* now level is elt->height - 1, we can start inserting */

    for(; level >= 0; level--) {

        while(s->next[level] && s->next[level]->key < key) {

            s = s->next[level];

        }

 

        /* s is last entry on this level < new element */

        /* do list insert */

        elt->next[level] = s->next[level];

        s->next[level] = elt;

    }

    printk("3 : %d\n", s->key);
}

 

/* delete a key from s */

void skiplistDelete(Skiplist s, int key)

{

    int level;

    Skiplist target;

 

    /* first we have to find leftmost instance of key */

    target = s;

 

    for(level = s->height - 1; level >= 0; level--) {

        while(target->next[level] && target->next[level]->key < key) {

            target = target->next[level];

        }

    }

 

    /* take one extra step at bottom */

    target = target->next[0];

 

    if(target == 0 || target->key != key) {

        return;

    }

 

    /* now we found target, splice it out */

    for(level = s->height - 1; level >= 0; level--) {

        while(s->next[level] && s->next[level]->key < key) {

            s = s->next[level];

        }

 

        if(s->next[level] == target) {

            s->next[level] = target->next[level];

        }

    }

 

    kfree(target);

}

void struct_example(void){
	
    int n;
    Skiplist s;
    int i;

    n = 1000;
    
    s = skiplistCreate();

    for(i = 0; i < n; i += 2) {
     	
	printk("1 : %d\n", s->key);
        skiplistInsert(s, i);
    	
	printk("insert final value : %d\n", s->key);
    }
    
    
    
    for(i = 0; i < n; i += 4) {
        printk("delete start value : %d\n", s->key);
     
        skiplistDelete(s, i);
     
        printk("delete end value : %d\n", s->key);
    }



    for(i = 0; i < n; i += 2) {
        printk("search %d : %d\n", i, skiplistSearch(s, i));
    }
    
    skiplistDestroy(s);
}



int __init skip_module_init(void){
	struct_example();
	printk("module init\n");
	return 0;
}

void __exit skip_module_cleanup(void){
	printk("Bye Module\n");
}

module_init(skip_module_init);
module_exit(skip_module_cleanup);
MODULE_LICENSE("GPL");
