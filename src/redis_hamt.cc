#include <inttypes.h>
#include <string>
#include <string.h>

extern "C" {
#include "redis_hamt/dict.h"
#include "redis_hamt/sds.h"
}

static unsigned int IntHash(const void* key) {
  std::hash<int64_t> h;

  return h((int64_t)key);
}

static unsigned int StrHash(const void* key) {
	sds s = (sds)key;
	std::size_t hi = std::_Hash_impl::hash(s, sdslen(s));

	return hi;
}

static int dictSdsKeyCompare(void* privdata, const void* key1, const void* key2) {
  int l1, l2;
  DICT_NOTUSED(privdata);

  l1 = sdslen((sds)key1);
  l2 = sdslen((sds)key2);
  if (l1 != l2)
    return 0;
  return memcmp(key1, key2, l1) == 0;
}

static dictType IntDict = {IntHash, NULL, NULL, NULL, NULL, NULL};
static dictType StrDict = {StrHash, NULL, NULL, dictSdsKeyCompare, NULL, NULL};

#define SETUP_INT dict* hash = dictCreate(&IntDict, NULL);
#define RESERVE_INT(size) dictExpand(hash, size)
#define INSERT_INT(key, value) \
	dictAdd(hash, (void*)(key), (void*)value)

#define DELETE_INT(key) dictDelete(hash, (void*)key)

#define FIND_INT_EXISTING(key) if(dictFind(hash, (void*)key) == NULL) { printf("error"); exit(1); }
#define FIND_INT_MISSING(key) if(dictFind(hash, (void*)key) != NULL) { printf("error"); exit(1); }
#define FIND_INT_EXISTING_COUNT(key, count) if(dictFind(hash, (void*)key) != NULL) { count++; }

#define ITERATE_INT(it)
#define CHECK_INT_ITERATOR_VALUE(iterator, value)

#define LOAD_FACTOR_INT_HASH(hash) (double(dictSize(hash)) / dictSlots(hash))
#define CLEAR_INT

#define SETUP_STR dict* str_hash = dictCreate(&StrDict, NULL); \
                  sds sk = sdsempty();

#define RESERVE_STR(size) dictExpand(str_hash, size)

#define LOAD_FACTOR_STR_HASH(str_hash)  (double(dictSize(str_hash)) / dictSlots(str_hash))

#define INSERT_STR(key, value) \
	dictAdd(str_hash, sdsnewlen(key.data(), key.size()), (void*)value)


#define DELETE_STR(key) sk = sdscpylen(sk, key.data(), key.size()); dictDelete(str_hash, sk);

#define FIND_STR_EXISTING(key) \
  sk = sdscpylen(sk, key.data(), key.size()); \
	if(dictFind(str_hash, sk) == NULL) { std::cerr << "error\n"; exit(4); }

#define FIND_STR_MISSING(key) \
	sk = sdscpylen(sk, key.data(), key.size()); \
	if(dictFind(str_hash, sk) != NULL) { std::cerr << "error\n"; exit(5); }


#define FIND_STR_EXISTING_COUNT(key, count) \
	sk = sdscpylen(sk, key.data(), key.size()); \
	if(dictFind(str_hash, sk) != NULL) { count++; }


#define CLEAR_STR

#include "template.cc"
