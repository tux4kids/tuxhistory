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

