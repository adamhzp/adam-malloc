#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

static struct mementry *root = 0, *last = 0;


void printHeap()
{
	printf("/************/\n");
	struct mementry *p = root;
	int i = 1;
	while(p!=0)
	{
		printf("block %d size %d",i, p->size);
		if(p->isfree){
			printf(" free\n");
		}
		else{
			printf(" not free\n");
		}
		i++;
		p = p->succ;
	}


}


void* mymalloc(unsigned int size)
{
	struct mementry *p, *succ;
	p = root;

	while(p!=0)
	{
		if(p->size<size)
		{
			p = p->succ;
		}
		else if(!p->isfree)
		{
			p = p->succ;
		}
		else if(p->size<(size+sizeof(struct mementry)))
		{
			p->isfree = 0;
			printHeap();
			return (char*)p+sizeof(struct mementry);
		}
		else
		{
			succ = (struct mementry*)((char*)p+sizeof(struct mementry)+size);
			succ->succ = p->succ;
			if(p->succ != 0) p->succ->prev =succ;
			p->succ = succ;
			succ -> size = p->size -sizeof(struct mementry)-size;
			succ -> isfree = 1;
			p->size = size;
			p->isfree=0;
			printHeap();
			return (char*) p +sizeof(struct mementry);
		}
	}
	if((p = (struct mementry*) sbrk (sizeof(struct mementry)*size))==(void*)-1)
	{
		printHeap();
		return 0;
	}
	else if(last == 0)
	{
		p->prev	= p->succ = 0;
		p->size	= size;
		p->isfree = 0;
		root = last = p;
		printHeap();
		return (char*) p + sizeof(struct mementry);
	}
	else
	{
		p->prev = last;
		p->succ = last->succ;
		p->size = size;
		p->isfree = 0;
		last->succ = p;
		last = p;
		printHeap();
		return p+1;
	}
}





void myfree(void *p, char* file, unsigned int line)
{
	struct mementry *ptr, *pred, *succ;
	ptr = (struct mementry *) ((char*)p-sizeof(struct mementry));
	struct mementry *curr = root;
	if(ptr->isfree)
	{
		printf("ERROR: freeing a freed block in file %s line %d\n", file , line);
		return;
	}

	while(curr!=0){
		if(curr == ptr){
		if((pred = ptr->prev) != 0 && pred->isfree)
		{
			pred->size += sizeof(struct mementry)+ptr->size;
			pred->succ = ptr->succ;
			if(ptr->succ != 0)
			{
				ptr->succ->prev = pred;
			}
		}
		else{
			ptr->isfree = 1;
			pred = ptr;
		}
		if((succ = ptr->succ)!=0 && (succ->isfree)){
			pred->size += sizeof (struct mementry)+succ->size;
			pred->succ = succ->succ;
			if(succ->succ!=0) succ->succ->prev = pred;
		}
		printHeap();
		return;
		}
		curr = curr->succ;
	}
	printf("Invalid free in file %s line %d\n", file, line);
	printHeap();

}
