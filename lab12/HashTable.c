// HashTable.c ... implementation of HashTable ADT
// Written by John Shepherd, May 2013

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "HashTable.h"
#include "List.h"

// Types and functions local to HashTable ADT

typedef struct HashTabRep {
	List *lists;  // either use this
	int   nslots; // # elements in array
	int   nitems; // # items stored in HashTable
} HashTabRep;

// convert key into index (from Sedgewick)
unsigned int hash(Key k, int tableSize)
{
	unsigned int h = 0;
	int a = 31415, b = 27183;
	for (; *k != '\0'; k++) {
		a = a*b % (tableSize-1);
		h = (a*h + *k) % tableSize;
	}
	return (h % tableSize);
}


// Interface functions for HashTable ADT

// create an empty HashTable
HashTable newHashTable(int N)
{
	HashTabRep *new = malloc(sizeof(HashTabRep));
	assert(new != NULL);
	new->lists = malloc(N*sizeof(List));
	assert(new->lists != NULL);
	int i;
	for (i = 0; i < N; i++)
		new->lists[i] = newList();
	new->nslots = N; new->nitems = 0;
	return new;
}

// free memory associated with HashTable
void dropHashTable(HashTable ht)
{
	assert(ht != NULL);
	int i;
    for (i = 0; i < ht->nslots; i++)
		dropList(ht->lists[i]);
	free(ht);
}

// display HashTable stats
void HashTableStats(HashTable ht)
{

	int length = ht->nitems;
	int slots = ht->nslots;

	assert(ht != NULL);
	printf("Hash Table Stats:\n");
	printf("Number of slots = %d\n", slots);
	printf("Number of items = %d\n", length);
	printf("Chain length distribution\n");
	printf("%8s %8s\n","Length","#Chains");


	int i;
	int *chains = malloc(sizeof(int)*length);
	for (i = 0; i < length; i++) {
		chains[i] = 0;
	}

	// go through each slot in the table and get length
	for (i = 0; i < slots; i++) {
		//printf("LL is%d\n", ListLength(ht->lists[i]));
		chains[ListLength(ht->lists[i])]++;
	}
    /*
	// find last non-zero index
	int lastNonZero = slots-1;
	for (i = slots - 1; i >= 0; i--) {
		if (chains[i] != 0) {
			lastNonZero = i;
			break;
		}
	}*/

	// print stats
	for (i = 0; i < length; i++) {
	    if (chains[i] == 0) continue;
		printf("%8d %8d\n", i, chains[i]);
	}
}

// insert a new value into a HashTable
void HashTableInsert(HashTable ht, Item it)
{
	assert(ht != NULL);
	int i = hash(key(it), ht->nslots);
	if (ListSearch(ht->lists[i], key(it)) == NULL) {
		ListInsert(ht->lists[i], it);
		ht->nitems++;
	}
}

// delete a value from a HashTable
void HashTableDelete(HashTable ht, Key k)
{
	assert(ht != NULL);
	int h = hash(k, ht->nslots);
	ListDelete(ht->lists[h], k);
}

// get Item from HashTable using Key
Item *HashTableSearch(HashTable ht, Key k)
{
	assert(ht != NULL);
	int i = hash(k, ht->nslots);
	return ListSearch(ht->lists[i], k);
}

