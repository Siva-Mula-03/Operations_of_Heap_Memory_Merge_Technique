#include <stdio.h>
#include <stdlib.h>
#define HEAPSIZE 1024 
typedef enum{FAILURE,SUCCESS} status_code; 

typedef struct allocate_node_tag//structure of allocated node
{
    int size, start, end;//start->starting address block,end->ending address of the block,size->size of the block
    struct allocate_node_tag *next;
    struct allocate_node_tag *prev;
    char var[10];//Name of the variable
} allocate_node;//Node that is used to maintain allocated list of the heap


typedef struct free_node_tag//structure of free node
{
    int size, start, end;//start->starting address block,end->ending address of the block,size->size of the block
    struct free_node_tag *next;
    struct free_node_tag *prev;
} free_node;//Node that is used to maintain allocated list of the heap


typedef struct heap_tag//structure of heap
{
    free_node *freelist;//maintaining freelist
    allocate_node *allocatelist;//maintaining allocated list
    int allocatesize;//total size allocated blocks
    int freesize;//total size of free free blocks
} heap;


void stringcopy(char *c1, char *c2)
{
    int i = 0;
    while (c2[i] != '\0')
    {
        c1[i] = c2[i];
        i++;
    }
    c1[i] = '\0';
}


int stringcomparision(char *c1, char *c2)//user defined string copy that gives either 1/0 as the return value just saying equal or not
{
    int retval = 0;
    int i = 0;
    while (c1[i] != '\0' && c2[i] != '\0' && retval == 0)
    {
        if (c1[i] == c2[i])
        {
            i++;
        }
        else
        {
            retval = 1;
        }
    }
    return retval;
}


free_node *makefreenode(int start, int size)
{
    free_node *nptr = (free_node *)(malloc(sizeof(free_node)));
    nptr->size = size;
    nptr->start = start;
    nptr->end = size + start - 1;
    nptr->next = NULL;
    nptr->prev = NULL;
    return nptr;
}


allocate_node *makeallocatenode(int size, char *name, int start)
{
    allocate_node *nptr = (allocate_node *)(malloc(sizeof(allocate_node)));//creating a allocate node using malloc
    nptr->next = NULL;
    nptr->prev = NULL;
    nptr->size = size;
    nptr->start = start;
    nptr->end = start + size - 1;
    stringcopy(nptr->var, name);
    return nptr;
}


void initialize_heap(heap *h, int size)
{                                      
    h->allocatesize = 0;
    h->freesize = HEAPSIZE;
    h->allocatelist = NULL;
    h->freelist = makefreenode(0, size);
}


void addtoallocatelist(allocate_node **listpointer, allocate_node *nptr)//function to add the allocate node to the allocated list that is in heap data structure
{
    allocate_node *allocatelistptr, *ptr, *prevptr;
    allocatelistptr = *listpointer;
    ptr = allocatelistptr;
    if (allocatelistptr == NULL)//if initially no node is allocated then allocatedlist will contain only the newly added node
    {
        allocatelistptr = nptr;
    }
    else
    {
        while (ptr->next != NULL && ptr->start < nptr->start)//we will skip all the nodes having start address of the block less than the new allocated block
        {
            ptr = ptr->next;
        }
        if (ptr->next == NULL && ptr->start < nptr->start)//case when all nodes in allocated list is before newly allocated list i,e. adding new node as last node
        {
            ptr->next = nptr;
            nptr->prev = ptr;
        }
        else
        {
            prevptr = ptr->prev;//as until before point of ptr they are before newly allocated node storing prev node
            if (ptr = allocatelistptr)//case when all nodes in allocated list are after the newly allocated block
            {
                allocatelistptr = nptr;
            }
            if (prevptr != NULL)//or else adding to prev pointer
            {
                prevptr->next = nptr;
            }
            nptr->prev = prevptr;
            nptr->next = ptr;
            ptr->prev = nptr;
        }
    }
    *listpointer = allocatelistptr;//updating the list pointer
}

void addtofreelist(free_node **listpointer, free_node *fptr)//function to add the free node to the free list that is in heap data structure
{
    free_node *ptr, *prevptr, *freelistptr;
    freelistptr = *listpointer;
    ptr = freelistptr;
    if (*listpointer == NULL)//if initially no node is allocated then freelist will contain only the newly added node
    {
        freelistptr = fptr;
    }
    else
    {
        while (ptr->next != NULL && ptr->start < fptr->start)//we will skip all the nodes having start address of the block less than the new free block
        {
            ptr = ptr->next;
        }
        if (ptr->next == NULL && ptr->start < fptr->start)//case when all nodes in free list is before newly free list i,e. adding new node as last node
        {
            ptr->next = fptr;
            fptr->prev = ptr;
        }
        else
        {
            prevptr = ptr->prev;//as until before point of ptr they are before newly allocated free node storing prev node
            if (ptr = freelistptr)//case when all nodes in freelist are after the newly allocated free block
            {
                freelistptr = fptr;
            }
            if (prevptr != NULL)//or else adding to prev pointer
            {
                prevptr->next = fptr;
            }
            fptr->prev = prevptr;
            fptr->next = ptr;
            ptr->prev = fptr;
        }
    }
    *listpointer = freelistptr;//updating the list pointer
}
void deletefromfreelist(free_node **listpointer, free_node *fptr)//function to delete the freenode from the free list when free node is given
{
    free_node *prevnode, *nextnode;
    prevnode = fptr->prev;
    nextnode = fptr->next;
    if (prevnode == NULL)//if prev node is NULL that means first node is being deleted
    {
        *listpointer = nextnode;
    }
    else//else conecting next node to prev node
    {
        prevnode->next = nextnode;
    }
    if (nextnode != NULL)//if nextnode is not null then connecting prev to next node
    {
        nextnode->prev = prevnode;
    }
    free(fptr);//freeing up the space that is allocated to fptr
    fptr = NULL;
}
void deletefromallocatedlist(allocate_node **listpointer, allocate_node *aptr)//function to delete the allocatenode from the allocatedlist when allocated node is given
{
    allocate_node *nextnode, *prevnode;
    prevnode = aptr->prev;
    nextnode = aptr->next;
    if (prevnode == NULL)//if prev node is NULL that means first node is being deleted
    {
        *listpointer = nextnode;
    }
    else//else conecting next node to prev node
    {
        prevnode->next = nextnode;
    }
    if (nextnode != NULL)//if nextnode is not null then connecting prev to next node
    {
        nextnode->prev = prevnode;
    }
    free(aptr);//freeing up the space that is allocated to fptr
    aptr = NULL;
}
status_code allocateusingfirstfit(heap *h, int varsize, char *name)//function to allocate a variable in heap using firstfit
{
    free_node *freelistptr, *fptr;
    allocate_node *allocatelistptr, *aptr;
    freelistptr = h->freelist;
    fptr = freelistptr;
    allocatelistptr = h->allocatelist;
    aptr = allocatelistptr;
    status_code sc = SUCCESS;
    int found = 0;
    while (fptr != NULL && found == 0)//searching till we get the enough block size that is needed to allocate
    {
        if (fptr->size >= varsize)//when we found space to allocate it
        {
            found = 1;
        }
        else
        {
            fptr = fptr->next;
        }
    }
    if (found == 0)//no enough space is found to allocate
    {
        sc = FAILURE;
    }
    else
    {
        allocate_node *newallocate = makeallocatenode(varsize, name, fptr->start);//when found creating a allocatenode to add
        addtoallocatelist(&allocatelistptr, newallocate);//adding to allocate list
        fptr->start = fptr->start + varsize;//updating the free node blocks
        fptr->size = fptr->size - varsize;//updating the free node size
        h->allocatesize = h->allocatesize + varsize;//updating heap allocated size
        h->freesize = h->freesize - varsize;//updating heap free size
        if (fptr->size == 0)//checking when complete free node is used or not
        {
            deletefromfreelist(&freelistptr, fptr);//if all blocks in free node is used we need to delete it
        }
    }
    h->allocatelist = allocatelistptr;//updating the  list
    h->freelist = freelistptr;//updating the free list
    return sc;
}
status_code allocateusingbestfit(heap *h, int varsize, char *name)//function to allocate a variable in heap using bestfit
{
    int size=HEAPSIZE;
    free_node *freelistptr, *fptr,*traverseptr;
    allocate_node *allocatelistptr, *aptr;
    freelistptr = h->freelist;
    traverseptr = freelistptr;
    allocatelistptr = h->allocatelist;
    aptr = allocatelistptr;
    status_code sc = SUCCESS;
    int found = 0;
    while (traverseptr != NULL)//searching till we get the best enough block size and the complete list that is needed to allocate
    {
        if (traverseptr->size >= varsize)//when we found space to allocate it
        {
            found=1;
            if(traverseptr->size<size)//checking for best fit condition
            {
                size=traverseptr->size;
                fptr=traverseptr;//storing that particular best fit node
            }
        }
        traverseptr = traverseptr->next;
    }
    if (found == 0)//no enough space is found to allocate
    {
        sc = FAILURE;
    }
    else
    {
        allocate_node *newallocate = makeallocatenode(varsize, name, fptr->start);//when found creating a allocatenode to add
        addtoallocatelist(&allocatelistptr, newallocate);//adding to allocate list
        fptr->start = fptr->start + varsize;//updating the free node blocks
        fptr->size = fptr->size - varsize;//updating the free node size
        h->allocatesize = h->allocatesize + varsize;//updating heap allocated size
        h->freesize = h->freesize - varsize;//updating heap free size
        if (fptr->size == 0)//checking when complete free node is used or not
        {
            deletefromfreelist(&freelistptr, fptr);//if all blocks in free node is used we need to delete it
        }
    }
    h->allocatelist = allocatelistptr;//updating the  list
    h->freelist = freelistptr;//updating the free list
    return sc;
}
status_code freememoryblock(heap *h, char *name)//deallocating the memory block based on variable name
{
    int found = 0;
    status_code sc = SUCCESS;
    allocate_node *aptr, *allocatelistpointer, *prevptr;
    free_node *fptr, *freelistpointer;
    allocatelistpointer = h->allocatelist;
    aptr = allocatelistpointer;
    freelistpointer = h->freelist;
    fptr = freelistpointer;
    while (aptr != NULL && found == 0)//checking for the memory block with given name using userdefined stringcomparision function
    {
        if (stringcomparision(name, aptr->var) == 0)
        {
            found = 1;
        }
        else
        {
            aptr = aptr->next;
        }
    }
    if (found == 0)//no memory block allocated with the given variable name
    {
        sc = FAILURE;
    }
    else//if found
    {
        h->freesize = h->freesize + aptr->size;//updating heap free size
        h->allocatesize = h->allocatesize - aptr->size;//updating heap allocated size
        free_node *freenodeofdeletealocatednode = makefreenode(aptr->start, aptr->size);//creating a free node with the size of the allocated node that needed to be deleted
        deletefromallocatedlist(&allocatelistpointer, aptr);//deleting the found allocated node from allocated list
        addtofreelist(&freelistpointer, freenodeofdeletealocatednode);//adding the created free node to freelist
        free_node *newfreenextnode, *newfreeprevnode;
        newfreenextnode = freenodeofdeletealocatednode->next;
        newfreeprevnode = freenodeofdeletealocatednode->prev;
        if (newfreenextnode != NULL)//checking next free node to the newly added free node is NULL or not i,e newly created node is last node or not
        {
            if (freenodeofdeletealocatednode->start + freenodeofdeletealocatednode->size == newfreenextnode->start)//if they are adjecent free nodes we need to merge it
            {
                freenodeofdeletealocatednode->size = freenodeofdeletealocatednode->size + newfreenextnode->size;//updating size
                freenodeofdeletealocatednode->end = newfreenextnode->end;//updting the last address of the block
                deletefromfreelist(&freelistpointer, newfreenextnode);//deleting the extra node after merging
            }
        }
        if (newfreeprevnode != NULL)//checking previous free node to the newly added free node is NULL or not i,e newly created node is first node or not
        {
            if (newfreeprevnode->start + newfreeprevnode->size == freenodeofdeletealocatednode->start)//if they are adjecent free nodes we need to merge it
            {
                newfreeprevnode->size = newfreeprevnode->size + freenodeofdeletealocatednode->size;//updating size
                newfreeprevnode->end = freenodeofdeletealocatednode->end;//updting the last address of the block
                deletefromfreelist(&freelistpointer, freenodeofdeletealocatednode);//deleting the extra node after merging
            }
        }
    }
    h->allocatelist = allocatelistpointer;//updating the allocated list
    h->freelist = freelistpointer;//updating the free list
    return sc;
}
void printheapblocks(heap *h)//function to print the heap blocks details
{
    allocate_node *aptr = h->allocatelist;
    free_node *fptr = h->freelist;
    printf("Start_Address\tEnd_Address\tSize\tAllocation Status\n");//In the form   Start_Address   End_Address     Size    Allocation Status
    while (aptr != NULL && fptr != NULL)//until one of the lisked list is pointing to NULL  printing details in the sorted way based on starting address of blocks
    {
        if (aptr->start < fptr->start && h->allocatesize!=0)
        {
            printf("%d\t\t%d\t\t%d\tAllocated\n", aptr->start, aptr->end, aptr->size);
            aptr = aptr->next;
        }
        else if(aptr->start > fptr->start && h->freesize!=0)
        {
            printf("%d\t\t%d\t\t%d\tfree\n", fptr->start, fptr->end, fptr->size);
            fptr = fptr->next;
        }
    }
    if (aptr != NULL && h->allocatesize!=0)//if one of the linked list is not NULL here for allocated list
    {
        while (aptr != NULL)
        {
            printf("%d\t\t%d\t\t%d\tAllocated\n", aptr->start, aptr->end, aptr->size);
            aptr = aptr->next;
        }
    }
    else if(fptr != NULL && h->freesize!=0)//if one of the linked list is not NULL here for free list
    {
        while (fptr != NULL)
        {
            printf("%d\t\t%d\t\t%d\tfree\n", fptr->start, fptr->end, fptr->size);
            fptr = fptr->next;
        }
    }
}
void printfreelist(free_node **freeptr)//function to print free list blocks
{
    free_node *fptr = *freeptr;
    printf("Start_Address\tEnd_Address\tSize\n");//In the form   Start_Address   End_Address     Size
    while (fptr != NULL)
    {
        printf("%d\t\t%d\t\t%d\n", fptr->start, fptr->end, fptr->size);
        fptr = fptr->next;
    }
}
void printallocatedlist(allocate_node **allocateptr)//function to print allocated list
{
    allocate_node *aptr = *allocateptr;
    printf("Variable name\tStart_Address\tEnd_Address\tSize\n");//In the form    Variable name   Start_Address   End_Address     Size
    while (aptr != NULL)
    {
        printf("%s\t\t%d\t\t%d\t\t%d\n", aptr->var, aptr->start, aptr->end, aptr->size);
        aptr = aptr->next;
    }
}
void freeuserheap(heap *h)//freeing the linked list while exiting the program as we used heap space for creation of nodes
{
    allocate_node *aptr = h->allocatelist;
    allocate_node *alistptr = aptr;
    free_node *fptr = h->freelist;
    free_node *flistptr = fptr;
    while (alistptr != NULL)//freeing up the nodes in allocated list
    {
        aptr = alistptr;
        alistptr = alistptr->next;
        free(aptr);
        aptr = NULL;
    }
    while (flistptr != NULL)//freeing up the nodes in allocated list
    {
        fptr = flistptr;
        flistptr = flistptr->next;
        free(fptr);
        fptr = NULL;
    }
    h->allocatelist = NULL;
    h->freelist = NULL;
    h->freesize = HEAPSIZE;//updating the free size
    h->allocatesize = 0;//updating the allocated size
}
int main()
{
    heap userheap;//creating the heap with name userheap
    int allocatesize;
    char variablename[10];
    status_code sc;
    initialize_heap(&userheap, HEAPSIZE);//initializing the heap
    int input,flag=0;
    printf("The maximum size of the heap is %d bytes\n", HEAPSIZE);//message showing to the user about the maximum size of the heap
    printf("Enter 1 to allocate via first fit allocation.\nEnter 2 to allocate via best fit allocation.\nEnter 3 to deallocate.\nEnter 4 to display heap.\nEnter 5 to display free blocks.\nEnter 6 to display allocated blocks.\nEnter 0 to exit the program.\n\nEnter the input :");
    scanf("%d", &input);//asking user to select
    while (flag==0)
    {
        if (input == 1)//allocating based on first fit
        {
            printf("Enter the name of the variable: ");
            scanf("%s", variablename);
            printf("Enter the size of the variable: ");
            scanf("%d", &allocatesize);
            if (userheap.freesize < allocatesize)//checking whether enough space is present in heap or not
            {
                printf("There is no enough space in the heap.\n");
            }
            else if (allocatesize == 0)//checking whether the allocated block have space or not
            {
                printf("There is no data to be allocated as size is 0 bytes\n");
            }
            else
            {
                sc = allocateusingfirstfit(&userheap, allocatesize, variablename);
                if (sc == FAILURE)
                {
                    printf("Malloc is falied.\n");
                }
                else
                {
                    printf("The variable %s of size %d is allocated.\n", variablename, allocatesize);
                }
            }
            printf("\n\n\n");
        }
        else if(input==2)//allocating based on best fit
        {
            printf("Enter the name of the variable: ");
            scanf("%s", variablename);
            printf("Enter the size of the variable: ");
            scanf("%d", &allocatesize);
            if (userheap.freesize < allocatesize)//checking whether enough space is present in heap or not
            {
                printf("There is no enough space in the heap.\n");
            }
            else if (allocatesize == 0)//checking whether the allocated block have space or not
            {
                printf("There is no data to be allocated as size is 0 bytes\n");
            }
            else
            {
                sc = allocateusingbestfit(&userheap, allocatesize, variablename);
                if (sc == FAILURE)
                {
                    printf("Malloc is falied.\n");
                }
                else
                {
                    printf("The variable %s of size %d is allocated.\n", variablename, allocatesize);
                }
            }
            printf("\n\n\n");
        }
        else if (input == 3)//deallocating the space
        {
            printf("Enter the name of the variable: ");
            scanf("%s", variablename);
            sc = freememoryblock(&userheap, variablename);//calling function to free/ deallocate and storing sucess or failure
            if (sc == SUCCESS)
            {
                printf("The variable %s is deallocated is sucessfully.", variablename);
            }
            else
            {
                printf("The variable %s is not found.", variablename);
            }
            printf("\n\n\n");
        }
        else if (input == 4)//to print the heap blocks according to starting address
        {
            printf("\n\n\n");
            printf("The heap blocks are\n");
            printheapblocks(&userheap);
            printf("\n\n\n");
        }
        else if (input == 5)//to print free list
        {
            printf("\n\n\n");
            printf("The total free blocks available are %d.\n", userheap.freesize);
            if(userheap.freesize!=0)//if no free space is present
            {
                printf("The free blocks are\n\n");
                printfreelist(&userheap.freelist);
            }
            else
            {
                printf("There are no free blocks available\n");
            }
            printf("\n\n\n");
        }
        else if (input == 6)//to print allocated list
        {
            printf("\n\n\n");
            printf("The total allocated blocks are %d.\n", userheap.allocatesize);
            if(userheap.allocatesize!=0)//if no free space is present
            { 
                printf("The allocated blocks are\n\n");
                printallocatedlist(&userheap.allocatelist);
            }
            else
            {
                printf("There are no allocated blocks available\n");
            }
            printf("\n\n\n");
        }
        else if (input == 0)//User asked to exit
        {
            flag=1;
            printf("Program exited\n");
            freeuserheap(&userheap);//deleting all the nodes that we creaated
        }
        else
        {
            printf("Please chose correct option.\n");
        }
        if(input != 0)//when user doesn't want to exit
        {
            printf("Enter 1 to allocate via first fit allocation.\nEnter 2 to allocate via first best allocation.\nEnter 3 to deallocate.\nEnter 4 to display heap.\nEnter 5 to display free blocks.\nEnter 6 to display allocated blocks.\nEnter 0 to exit the program.\n\nEnter the input :");
            scanf("%d", &input);
        }
    }
}