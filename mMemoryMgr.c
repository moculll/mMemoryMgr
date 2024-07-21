/**
 * @author mocul
 * @link https://github.com/moculll
 * @copyright 2024, mocul, Apache-2.0
 */

#include <mMemoryMgr/mMemoryMgr.h>
#include <rbtree/rbtree.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
/**
 * @brief mMgr use partInfo lower 3 bits to store <mode> and <occupy>
 * @param mode the second bit of partInfo
 * @param mode[0] aligned part mode, increase speed
 * @param mode[1] reversed part mode, decrease memory usage
 * @param mode[2] list part mode, decrease more memory usage than mode[1]
 * @param occupy the first bit of partInfo, indicates that if a part is being using
 */
#define MMGR_GET_PART_OCCUPY(part) (((mMemoryPart_t *)(part))->partInfo & 1)
#define MMGR_GET_PART_MODE(part) ((((mMemoryPart_t *)(part))->partInfo >> 1) & 0x03)
#define MMGR_SET_PART_OCCUPY(part, occupy) (((mMemoryPart_t *)(part))->partInfo = ((mMemoryPart_t *)(part))->partInfo & ~1 | (occupy))
#define MMGR_SET_PART_MODE(part, mode) (((mMemoryPart_t *)(part))->partInfo = ((mMemoryPart_t *)(part))->partInfo & ~0x06 | ((mode) << 1))

typedef struct {
    /* namespace(data) address */
    void *addr;
    /* namespace(data) size */
    size_t size;
    struct rb_node node;

} mMemoryNs_t __attribute__((aligned(sizeof(long))));


typedef struct {



} mMemoryNsl_t __attribute__((aligned(sizeof(long))));


typedef struct mMemoryPart {
    /* pointer of free space */
    void *addr;
    /* total part space, won't change */
    size_t space;
    /* used space */
    size_t leftSpace;
    /* rb key: address */
    struct rb_root root;
    /* rb key: size */
    struct rb_root recycleRoot;
    /* bit[0]: occupy, bit[1]: mode */
    uint8_t partInfo;
    /* next part */
    struct mMemoryPart *next;
} mMemoryPart_t __attribute__((aligned(sizeof(long))));


typedef struct {
    mMemoryPart_t *parts;
} mMemoryMgr_t;

static mMemoryMgr_t *mMemoryMgr = NULL;

/* 
//print rbt functions, just for test
void print_node(mMemoryNs_t *ns)
{
    printf("Node address: %p, size: %zu, addr: %p\n", (void *)ns, ns->size, ns->addr);
}
void print_rb_tree(struct rb_root *root)
{
    struct rb_node *node = root->rb_node;
    if (!node) {
        printf("RB tree is empty.\n");
        return;
    }
    printf("RB tree structure:\n");
    print_rb_node(node, 0);
}

void print_rb_node(struct rb_node *node, int level)
{
    if (!node)
        return;
    mMemoryNs_t *ns = container_of(node, mMemoryNs_t, node);
    for (int i = 0; i < level; ++i) printf("  ");
    printf("Node: ");
    print_node(ns);
    if (node->rb_left) {
        for (int i = 0; i < level; ++i) printf("  ");
        printf("Left:\n");
        print_rb_node(node->rb_left, level + 1);
    }
    if (node->rb_right) {
        for (int i = 0; i < level; ++i) printf("  ");
        printf("Right:\n");
        print_rb_node(node->rb_right, level + 1);
    }
} */

/**
 * @brief search if address is in rbt root
 * @param root (rb_root *) pointer
 * @param address the address search in rbt root
 * @retval (mMemoryNs_t *) pointer, NULL if not found
 */
static inline mMemoryNs_t *searchNodeInRoot(struct rb_root *root, void *address)
{
    struct rb_node *node = root->rb_node;
    while(node){
        mMemoryNs_t *nsForSearch = container_of(node, mMemoryNs_t, node);
        
        if(nsForSearch->addr < address)
            node = node->rb_right;
        else if(nsForSearch->addr > address)
            node = node->rb_left;
        else
            return nsForSearch;

    }
    return NULL;

}

/**
 * @brief search if there is a node->size <= size
 * @param root (rb_root *) pointer
 * @param size the size search in rbt root
 * @retval (mMemoryNs_t *) pointer, NULL if not found
 */
static inline mMemoryNs_t *searchNodeInRecycle(struct rb_root *root, size_t size)
{
    struct rb_node *node = root->rb_node;
    while(node){
        mMemoryNs_t *nsForSearch = container_of(node, mMemoryNs_t, node);
        if(nsForSearch->size < size)
            node = node->rb_right;
        else
            return nsForSearch;

    }
    return NULL;

}

/**
 * @brief insert namespace->node in rbt
 * @param root (rb_root *) pointer
 * @param ns (mMemoryNs_t *) pointer
 * @retval [0]: failed, [1]: successful 
 * @note make sure ns->addr has been set before we call this function
 */
static inline uint8_t insertNodeInPart(struct rb_root *root, mMemoryNs_t *ns)
{
    struct rb_node **new = &(root->rb_node), *parent = NULL;
    /* Figure out where to put new node */
    while (*new) {
        mMemoryNs_t *this = container_of(*new, mMemoryNs_t, node);

        parent = *new;
        if (ns->addr < this->addr)
            new = &((*new)->rb_left);
        else if (ns->addr > this->addr)
            new = &((*new)->rb_right);
        else
            return 0;
    }

    // Add new node and rebalance tree
    rb_link_node(&ns->node, parent, new);
    rb_insert_color(&ns->node, root);

    /* print_rb_tree(root); */

    return 1;
}

/* static inline void mMemoryMerge(mMemoryPart_t *curPart)
{


} */

void mMemoryMgrInit(size_t memoryPoolSize, size_t spiltParts, size_t partsMode)
{
    mMemoryPart_t **mgrPart = NULL;
    size_t perPartSize = memoryPoolSize / spiltParts;

    mMemoryMgr = (mMemoryMgr_t *)malloc(sizeof(mMemoryMgr_t));

    mgrPart = &mMemoryMgr->parts;
    /* we use single list to store parts */
    for(int i = 0; i < spiltParts; i++){
        mMemoryPart_t *newPart = (mMemoryPart_t *)malloc(sizeof(mMemoryPart_t));
        
        MMGR_SET_PART_OCCUPY(newPart, 0);
        MMGR_SET_PART_MODE(newPart, (partsMode >> (i * 2)) & 0x03);
        newPart->root.rb_node = NULL;
        newPart->recycleRoot.rb_node = NULL;
        
        newPart->addr = malloc(perPartSize);
        /* important, we use ns->size==0 to recognize if a namespace is used, if there are some area!=0, there could be some errors */
        memset(newPart->addr, 0, perPartSize);
        newPart->space = perPartSize;
        newPart->leftSpace = perPartSize;
        newPart->next = NULL;
        
        *mgrPart = newPart;
        mgrPart = &newPart->next;
    }

}


void *mMemoryMalloc(size_t size)
{
    if(!size)
        return NULL;

    mMemoryPart_t *curPart;
    mMemoryNs_t *namespace;
    void *ret;
    for(curPart = mMemoryMgr->parts; curPart != NULL; curPart = curPart->next){
        /* FIXME: this needs to be mutex, and each part should have a mutex */
        if(MMGR_GET_PART_OCCUPY(curPart))
            continue;

        mMemoryNs_t *newNs = NULL;
        /* search if there have namespace->size >= size */
        namespace = searchNodeInRecycle(&curPart->recycleRoot, size);
        MMGR_SET_PART_OCCUPY(curPart, 1);
        /* if so, use recycleable space */
        if(namespace){
            ret = namespace->addr;
            
            switch(MMGR_GET_PART_MODE(curPart)){
                case MMGR_MODE_ALIGN:
                    rb_erase(&namespace->node, &curPart->recycleRoot);
                    insertNodeInPart(&curPart->root, namespace);
                    if( (namespace->size == size) \
                        ||(curPart->leftSpace < sizeof(mMemoryNs_t)) \
                        ||((namespace->size - size) < sizeof(long)) )\
                    {
                        MMGR_SET_PART_OCCUPY(curPart, 0);
                        return ret;
                    }

                    newNs = (mMemoryNs_t *)curPart->addr;
                    *(char **)&curPart->addr += sizeof(mMemoryNs_t);
                    curPart->leftSpace -= sizeof(mMemoryNs_t);
                    
                    memset((void *)newNs, 0, sizeof(mMemoryNs_t));
                    
                    newNs->addr = namespace->addr + size;
                    newNs->size = namespace->size - size;
                    namespace->size = size;
                    
                    insertNodeInPart(&curPart->recycleRoot, newNs);
                    MMGR_SET_PART_OCCUPY(curPart, 0);
                    return ret;
                break;

                case MMGR_MODE_REVERSE:
                    if(namespace->size == size || curPart->leftSpace < sizeof(mMemoryNs_t)){
                        rb_erase(&namespace->node, &curPart->recycleRoot);
                        insertNodeInPart(&curPart->root, namespace);

                        MMGR_SET_PART_OCCUPY(curPart, 0);
                        return namespace->addr;
                    }

                    /* if namespace size > requireSize, spilt it */
                    rb_erase(&namespace->node, &curPart->recycleRoot);
                    newNs = (mMemoryNs_t *)(curPart->addr + curPart->leftSpace - sizeof(mMemoryNs_t));
                    memset((void *)newNs, 0, sizeof(mMemoryNs_t));
                    newNs->size = size;
                    newNs->addr = namespace->addr;
                    insertNodeInPart(&curPart->root, newNs);
                    namespace->addr += size;
                    namespace->size -= size;
                    
                    insertNodeInPart(&curPart->recycleRoot, namespace);
                    
                    curPart->leftSpace -= sizeof(mMemoryNs_t);
                    MMGR_SET_PART_OCCUPY(curPart, 0);
                    return newNs->addr;
                break;

                default:

                break;
            }

        }
        /* if not, create a new rbt node */
        /* aligned mode */

        switch(MMGR_GET_PART_MODE(curPart)){
            
            case MMGR_MODE_ALIGN: {
                uint8_t alignOffset = (sizeof(long) - size) > 0 ? sizeof(long) - size : sizeof(long) - (size % sizeof(long));
                if(curPart->leftSpace < sizeof(mMemoryNs_t) + size + alignOffset){
                    MMGR_SET_PART_OCCUPY(curPart, 0);
                    continue;
                }
                newNs = (mMemoryNs_t *)curPart->addr;
                
                memset((void *)newNs, 0, sizeof(mMemoryNs_t) + size + alignOffset);
                newNs->addr = (char *)curPart->addr + sizeof(mMemoryNs_t);
                newNs->size = size + alignOffset;

                /* union member switch to newNs->node */
                if(!insertNodeInPart(&curPart->root, newNs)){
                    newNs->addr = 0;
                    newNs->size = 0;
                    MMGR_SET_PART_OCCUPY(curPart, 0);
                    continue;
                }
                    
                *(char **)&curPart->addr += sizeof(mMemoryNs_t) + size + alignOffset;
                curPart->leftSpace -= sizeof(mMemoryNs_t) + size + alignOffset;
                MMGR_SET_PART_OCCUPY(curPart, 0);
            return newNs->addr;
            }

            case MMGR_MODE_REVERSE:
                /* reverse mode */
                if(curPart->leftSpace < sizeof(mMemoryNs_t) + size){
                    MMGR_SET_PART_OCCUPY(curPart, 0);
                    continue;
                }
                
                newNs = (mMemoryNs_t *)(curPart->addr + curPart->leftSpace - sizeof(mMemoryNs_t));
                memset((void *)newNs, 0, sizeof(mMemoryNs_t));
                newNs->addr = curPart->addr;
                newNs->size = size;
                if(!insertNodeInPart(&curPart->root, newNs)){
                    newNs->addr = 0;
                    newNs->size = 0;
                    MMGR_SET_PART_OCCUPY(curPart, 0);
                    continue;
                }
                /* curPart->addr doesn't need to add sizeof(mMemoryNs_t), because all namespace headers are stored at the end of a part */
                curPart->leftSpace -= sizeof(mMemoryNs_t) + size;
                *(char **)&curPart->addr += size;
                MMGR_SET_PART_OCCUPY(curPart, 0);
            return newNs->addr;
            

            default:

            break;

        }

    }
    /* FIXME: if all parts have no left space, try to merge two parts */
    /* mMemoryMerge(); */

    return NULL;
}

void *mMemoryMallocFrom(size_t size, uint8_t partNum)
{
    mMemoryPart_t *part = NULL, *partTmp = NULL, *nextTmp = NULL;
    void *ret = NULL;
    uint8_t index = 0;
    for(part = mMemoryMgr->parts, index = 0; part != NULL; part = part->next, index++){
        if(index == partNum){
            partTmp = mMemoryMgr->parts;
            mMemoryMgr->parts = part;
            nextTmp = part->next;
            part->next = NULL;
            ret = mMemoryMalloc(size);
            part->next = nextTmp;
            mMemoryMgr->parts = partTmp;
            return ret;
        }
    }
    return ret;
}


void mMemoryFree(void *free)
{
    mMemoryPart_t *curPart;
    mMemoryNs_t *namespace;
    for(curPart = mMemoryMgr->parts; curPart != NULL; curPart = curPart->next){
        /* search if there have the equal address of this part */
        namespace = searchNodeInRoot(&curPart->root, free);
        if(namespace){
            /* if so, delete it in root and insert it into recycle root for future usage */
            rb_erase(&namespace->node, &curPart->root);
            if(insertNodeInPart(&curPart->recycleRoot, namespace)){
                memset(namespace->addr, 0, namespace->size);
                return;
            }
        }

    }

}

void mMemoryDeinit(void)
{
    if(!mMemoryMgr)
        return;
    mMemoryPart_t *curPart = mMemoryMgr->parts, *nextPart = NULL;

    while(curPart){
        nextPart = curPart->next;
        memset(curPart->addr, 0, curPart->space);
        free(curPart->addr);
        memset(curPart, 0, sizeof(mMemoryPart_t));
        free((void *)curPart);
        curPart = nextPart;
    }
    memset((void *)mMemoryMgr, 0, sizeof(mMemoryMgr_t));
    free((void *)mMemoryMgr);
    mMemoryMgr = NULL;
}
