// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

void *refcount; // stores the number of references of each page
void *pa_start;

struct run
{
  struct run *next;
};

struct
{
  struct spinlock lock;
  struct run *freelist;
} kmem;

void kinit()
{
  initlock(&kmem.lock, "kmem");

  refcount = end;

  int count = (PHYSTOP - (uint64)refcount) / PGSIZE;

  uint64 sz = count * sizeof(uint64);

  pa_start = (void *)PGROUNDUP((uint64)refcount + sz);

  uint64 diff = (uint64)pa_start - (uint64)end;
  memset(end, 0, diff);

  freerange(pa_start, (void *)PHYSTOP);
}

static uint64 refidx(void *pa)
{
  return ((uint64)pa - (uint64)pa_start) / PGSIZE;
}

uint64 incref(void *pa)
{
  // increase the reference count of the page correspondinf to pa
  int index = refidx(pa);

  int *n = (int *)refcount;

  n[index]++;

  return n[index];
}

uint64 decref(void *pa)
{
  // increase the reference count of the page correspondinf to pa

  int index = refidx(pa);

  int *n = (int *)refcount;

  n[index]--;

  return n[index];
}

void freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char *)PGROUNDUP((uint64)pa_start);
  for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE)
  {
    incref(p); // increment the number of references of the page
    kfree(p);  // free the page p
  }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa)
{
  struct run *r;

  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < (char *)pa_start || (uint64)pa >= PHYSTOP)
    panic("kfree");

  int a;
  acquire(&kmem.lock);
  a = decref(pa); // decrement the reference number of the page
  release(&kmem.lock);

  if (a > 0)
  {
    return;
  }
  else
  {
    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run *)pa;

    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if (r)
  {
    incref(r); // increment page reference
    kmem.freelist = r->next;
  }
  release(&kmem.lock);

  if (r)
    memset((char *)r, 5, PGSIZE); // fill with junk
  return (void *)r;
}
