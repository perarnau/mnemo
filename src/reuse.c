#include <mnemo.h>

#include <internal/uthash.h>
#include <internal/utsplay.h>

/* a record for reuse distance purposes:
 * - a key identifying uniquely the entry being accessed
 * - a timestamp for this access
 * - a handle for an hashmap (uthash).
 * - fields for insertion in a tree.
 */
struct mnemo_record {
	unsigned long long toto;
	unsigned long long time;
	UT_hash_handle hh;
	struct mnemo_record *left, *right, *parent;
	int weight;
};

#undef SPLAY_INIT
#define SPLAY_INIT(e) \
do { \
	e->weight = 1; \
} while(0)

#undef SPLAY_PROCESS
#define SPLAY_PROCESS(e) \
do { \
	e->weight = 1; \
	if (e->right) e->weight += e->right->weight; \
	if (e->left) e->weight += e->left->weight; \
} while (0)

#undef SPLAY_KEYCMP
#define SPLAY_KEYCMP(a, b, sz) \
((*(unsigned long long *)a < *(unsigned long long*)b) ? -1 : ((unsigned long long*)a > (unsigned long long*)b) ? 1 : 0)

/* the actual info needed to build reuse distance information
 * - a current timestamp
 * - an hashmap for (entry, last_time_of_access)
 * - a splay tree for counting the number of unique accesses in between two
 *   access to the same entry.
 */
struct mnemo_reusedm {
	unsigned long long now;
	struct mnemo_record *last_seen;
	struct mnemo_record *splay;
};

/* allocate and init a new reuse record.
 * @param max the maximum number of keys the trace will contain, 0 if unknown.
 */
struct mnemo_reusedm *mnemo_reusedm_init(size_t max)
{
	struct mnemo_reusedm *ret;

	assert(max == 0);
	ret = calloc(1, sizeof(struct mnemo_reusedm));
	assert(ret != NULL);
	ret->now = 0;
	ret->last_seen = NULL;
	ret->splay = NULL;
	return ret;
}

int mnemo_reusedm_add(struct mnemo_reusedm *reuse, unsigned long long key)
{
	assert(reuse != NULL);
	struct mnemo_record *rec = NULL;
	int distance = -1;
	HASH_FIND(hh, reuse->last_seen, &key, sizeof(key), rec);
	if (rec != NULL) {
		/* fun fact: since the record has both a hash handle and a splay
		 * node, we don't need to use find, and can directly splay the
		 * node
		 */
		SPLAY_SPLAY(reuse->splay, rec);
		if(rec->right == NULL)
			distance = 0;
		else
			distance = rec->right->weight;
		SPLAY_REMOVE2(reuse->splay, &rec->time, sizeof(rec->time), rec,
			      time, left, right, parent);
		HASH_DEL(reuse->last_seen, rec);
	}
	else {
		rec = calloc(1, sizeof(struct mnemo_record));
		assert(rec != NULL);
	}
	rec->toto = key;
	rec->time = reuse->now++;
	HASH_ADD(hh, reuse->last_seen, toto, sizeof(rec->toto), rec);
	SPLAY_ADD2(reuse->splay, time, sizeof(rec->time), rec);
	return distance;
}

void mnemo_reusedm_reset(struct mnemo_reusedm *reuse)
{
	assert(reuse != NULL);
	HASH_CLEAR(hh, reuse->last_seen);
	SPLAY_CLEAR(reuse->splay);
}

void mnemo_reusedm_fini(struct mnemo_reusedm *reuse)
{
	mnemo_reusedm_reset(reuse);
	free(reuse);
}
