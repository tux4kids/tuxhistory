/*
Copyright (c) 2007, 2008, 2010 Robbert Haarman
    with some additions by Jesus Mager 2010

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/** \file
* Hashtable implementation internals.
*
* <em>This file contains the full documentation of all functions, types,
* constants, and variables used in the hashtable implementation, including
* those that are intended only for internal use. For the public interface
* to hash tables, see \ref hashtable.h.</em>
*/

/** Allocate memory for some type.
* @param T The type to allocate memory for.
* @return A pointer to the newly allocated storage.
*/
#define NEW(T) (T*) malloc(sizeof(T))

/** Allocate memory for an array.
* @param T The type of the elements of the array.
* @param N The number of elements in the array.
* @return A pointer to the array.
*/
#define NEWARRAY(T, N) (T*) calloc((N), sizeof(T))

/** Hash a key using a table's hash function.
* @param table The table whose hash function to use.
* @param key The key to hash.
* @return The hash of the key, clipped to the number of buckets in the table.
*/
static inline unsigned int compute_hash(const struct hashtable *table,
                                        const char *key)
{
    printf("Init compute hash for %s\n", key);
    unsigned int n = table->hash(key);
    return (n < table->nbuckets) ? n : (n % table->nbuckets);
}

/** Add an entry to a hash table.
* @param table The hash table the entry is to be added to.
* @param key The key for the entry.
* @param value The value for the entry.
* @return Nonzero on success, 0 on failure.
*/
int hashtable_add(struct hashtable *table, char *key, void *value)
{
    unsigned int n;
    struct hashtable_entry *entry = NEW(struct hashtable_entry);
    struct hashtable_entry *entries;

    if(!table) return 0;

    if(entry) {
        entry->key = key;
        entry->value = value;
        entry->next = NULL;
        entry->prev = NULL;
        n = compute_hash(table, key);
        entries = table->bucket[n];
        if(!entries) table->bucket[n] = entry;
        else {
          while(entries->next) 
            entries = entries->next;
          entries->next = entry;
          entry->prev = entries;
        }
        return 1;
    }
    
    return 0;
}

/** Insert all elements from one hash table into another.
* @param dest The hash table to insert elements into.
* @param src The hash table whose elements are to be inserted.
* @return \a dest
*/
struct hashtable *copy_hashtable(struct hashtable *dest, struct hashtable *src) {
    unsigned int i;
    struct hashtable_entry *entry;
    
    for(i = 0; i < src->nbuckets; i++) {
        for(entry = src->bucket[i]; entry; entry = entry->next) {
            hashtable_add(dest, entry->key, entry->value);
        }
    }  
}

/** The default hash function.
* This function can be used as the \c hash parameter
* to \c make_hash_table().
*
* @param key The key to be hashed.
* @returns The hash for the key.
*/
unsigned int hashtable_default_hash(const char *key) {
    unsigned int hash = 0;
    unsigned char *p = (unsigned char*) key;
    
    while(*p) {
        hash = (hash * 33) + ((unsigned int) *p);
        p++;
    }
    
    return hash;
}

/** Free all the memory used by a hash table.
* @param table The hash table to be deallocated.
*/
void free_hashtable(struct hashtable *table) {
    unsigned int i;
    struct hashtable_entry *entry;
    struct hashtable_entry *next;
    
    for(i = 0; i < table->nbuckets; i++) {
        entry = table->bucket[i];
        while(entry) {
            next = entry->next;
            free(entry);
            entry = next;
        }
    }
    
    free(table->bucket);
    free(table);
}

/** Call a function for each entry in a hash table.
* @param table The hash table whose entries to iterate over.
* @param func The function to call for each entry.
The function receives two argument:
1. The key of the entry.
2. The value of the entry.
*/
void hashtable_iter(const struct hashtable *table,
                    void (*func) (char *key, void *value))
{
    unsigned int i;
    struct hashtable_entry *entry;
    
    for(i = 0; i < table->nbuckets; i++) {
        for(entry = table->bucket[i]; entry; entry = entry->next) {
            func(entry->key, entry->value);
        }
    }
}

/** Look up the value bound to a key.
* @param table The hash table in which to look up the key.
* @param key The key to look up.
* @return The value belonging to the key if found, NULL if not found.
*/
void *hashtable_lookup(const struct hashtable *table,
                       const char *key)
{
    unsigned int n = compute_hash(table, key);
    struct hashtable_entry *entry = table->bucket[n];
    
    while(entry) {
        if(!strcmp(key, entry->key)) break;
        entry = entry->next;
    }
    
    return entry ? entry->value : NULL;
}

 /** remove the value bound to a key.
 * @param table The hash table in which to remove the key.
 * @param key The key to remove.
 * @return The 1 succesfull removal, 0 if not found.
 */
int hashtable_remove(const struct hashtable *table,
		       const char *key)
{
  unsigned int n = compute_hash(table, key);
  struct hashtable_entry *entry = table->bucket[n];

  while(entry) {
    if(!strcmp(key, entry->key)) break;
    entry = entry->next;
  }

  if(entry) {
    if(entry->next) {
      if(entry->prev) {
        entry->prev->next = entry->next;
        entry->next->prev = entry->prev;
      }
      else {
        table->bucket[n] = entry->next;
      }
    }
    else if(entry->prev)
    {
      entry->prev->next = NULL;
    }
    else {
      table->bucket[n] = NULL;
    }
    
    free(entry);
    entry = NULL;
    return 1;
  }
  else {
    return 0;
  }
}   

/** Create a hash table.
* @param hash The hash function to use with this hash table.
        The function has to map NUL-terminated strings to unsigned ints.
* @param nbuckets The number of buckets in the hash table.
* @return The new hash table on success, NULL on failure.
*/ 
struct hashtable *make_hashtable(unsigned int (*hash) (const char*),
                                unsigned int nbuckets)
{
    struct hashtable *table = NEW(struct hashtable);
    
    if(table) {
        table->bucket = NEWARRAY(struct hashtable_entry*,
                                nbuckets);
        if(!table->bucket) 
        {
            free(table);
            return NULL;
        }
        table->hash = hash;
        table->nbuckets = nbuckets;
    }
    return table;
}

