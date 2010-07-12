/*
Copyright (c) 2007, 2008, 2010 Robbert Haarman

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

#ifndef HASHTABLE_H
#define HASHTABLE_H

struct hashtable_entry {
    char *key;
    void *value;
    struct hashtable_entry *next;
};

struct hashtable {
    unsigned int (*hash) (const char*);
    unsigned int nbuckets;
    struct hashtable_entry **bucket;
};

void free_hashtable(struct hashtable *table);
int hashtable_add(struct hashtable *table, char *key, void *value);
unsigned int hashtable_default_hash(const char *key);
void hashtable_iter(const struct hashtable *table,
void (*func) (char *key, void *value));
void *hashtable_lookup(const struct hashtable *table,
                        const char *key);
struct hashtable *make_hashtable(unsigned int (*hash) (const char*),
                                unsigned int nbuckets);

#endif /* ndef HASHTABLE_H */

