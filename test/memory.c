#include <stdio.h>
#include <string.h>

#include "storage/memory.h"
#include "emdb.h"


#include "test.h"


uint8_t test_memory ( ) {
  Entry *entry;
  unsigned char ret;
  Stats *stats;
  void *ctx;

  ctx = MemoryStorage.create_context(NULL);

  entry = MemoryStorage.store_read(ctx, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after read");

  ret = MemoryStorage.store_write(ctx, (unsigned char *) "foo", (unsigned char *) "bar", 4);

  check(ret == 1, "write succeeded");

  entry = MemoryStorage.store_read(ctx, (unsigned char *) "foo");

  check(entry->size == 4, "entry size is 4");
  check(strcmp(entry->ptr, "bar") == 0, "entry is correct");

  emdb_free_entry(entry);

  stats = MemoryStorage.stats(ctx);
#if defined __x86_64 || defined i386
  check(stats->memory_usage == 106, "memory usage is correctly reported");
#endif

#if defined __ARM_ARCH_6__ || defined __arm__ || defined __mips__
  check(stats->memory_usage == 66, "memory usage is correctly reported");
#endif

  ret = MemoryStorage.store_write(ctx, (unsigned char *) "bar", (unsigned char *) "baz", 4);

  check(ret == 1, "second write succeeded");

  entry = MemoryStorage.store_read(ctx, (unsigned char *) "bar");

  check(entry->size == 4, "entry size is 4");
  check(strcmp(entry->ptr, "baz") == 0, "entry is correct");

  emdb_free_entry(entry);

  MemoryStorage.store_delete(ctx, (unsigned char *) "foo");
  entry = MemoryStorage.store_read(ctx, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after delete called");

  stats = MemoryStorage.stats(ctx);

#if defined __x86_64 || defined i386
  check(stats->memory_usage == 106, "memory usage is correctly reported");
#endif

#if defined __ARM_ARCH_6__ || defined __arm__ || defined __mips__
  check(stats->memory_usage == 66, "memory usage is correctly reported");
#endif

  MemoryStorage.destroy_context(ctx);

  done();
}

uint8_t test_context_isolation ( ) {
  Entry *entry;
  unsigned char ret;
  Stats *stats;
  void *ctx1;
  void *ctx2;

  ctx1 = MemoryStorage.create_context(NULL);
  ctx2 = MemoryStorage.create_context(NULL);


  ret = MemoryStorage.store_write(ctx1, (unsigned char *) "foo", (unsigned char *) "bar", 4);

  check(ret == 1, "write to context 1 succeeded");

  entry = MemoryStorage.store_read(ctx1, (unsigned char *) "foo");

  check(entry->size == 4, "entry size is 4");
  check(strcmp(entry->ptr, "bar") == 0, "entry is correct");

  emdb_free_entry(entry);


  stats = MemoryStorage.stats(ctx1);

#if defined __x86_64 || defined i386
  check(stats->memory_usage == 106, "memory usage is correctly reported");
#endif

#if defined __ARM_ARCH_6__ || defined __arm__ || defined __mips__
  check(stats->memory_usage == 66, "memory usage is correctly reported");
#endif

  stats = MemoryStorage.stats(ctx2);

#if defined __x86_64 || defined i386
  check(stats->memory_usage == 32, "memory usage is correctly reported for context 2");
#endif

#if defined __ARM_ARCH_6__ || defined __arm__ || defined __mips__
  check(stats->memory_usage == 20, "memory usage is correctly reported for context 2");
#endif

  entry = MemoryStorage.store_read(ctx2, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after read on context 2");

  ret = MemoryStorage.store_write(ctx1, (unsigned char *) "bar", (unsigned char *) "baz", 4);

  check(ret == 1, "second write to context 1 succeeded");

  entry = MemoryStorage.store_read(ctx1, (unsigned char *) "bar");

  check(entry->size == 4, "entry size is 4");
  check(strcmp(entry->ptr, "baz") == 0, "entry is correct");

  emdb_free_entry(entry);

  entry = MemoryStorage.store_read(ctx2, (unsigned char *) "bar");

  check(entry == NULL, "entry is null after read on context 2");

  MemoryStorage.store_delete(ctx1, (unsigned char *) "foo");
  entry = MemoryStorage.store_read(ctx1, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after delete called");

  stats = MemoryStorage.stats(ctx1);

#if defined __x86_64 || defined i386
  check(stats->memory_usage == 106, "memory usage is correctly reported");
#endif

#if defined __ARM_ARCH_6__ || defined __arm__ || defined __mips__
  check(stats->memory_usage == 66, "memory usage is correctly reported");
#endif

  MemoryStorage.destroy_context(ctx1);
  MemoryStorage.destroy_context(ctx2);

  done();
}
