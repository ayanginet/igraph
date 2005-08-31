/* -*- mode: C -*-  */
/* 
   IGraph library.
   Copyright (C) 2003, 2004, 2005  Gabor Csardi <csardi@rmki.kfki.hu>
   MTA RMKI, Konkoly-Thege Miklos st. 29-33, Budapest 1121, Hungary
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "types.h"
#include "memory.h"
#include "random.h"

#include <assert.h>
#include <string.h> 		/* memcpy & co. */

/**
 */

int dqueue_init (dqueue_t* q, long int size) {
	if (size <= 0 ) { size=1; }
	q->stor_begin=Calloc(size, real_t);
	q->stor_end=q->stor_begin + size;
	q->begin=q->stor_begin;
	q->end=NULL;
	
	return 0;
}

/**
 */

int dqueue_destroy (dqueue_t* q) {
	assert( q->stor_begin != NULL);
	Free(q->stor_begin);
	return 0;
}

/**
 */

int dqueue_empty (dqueue_t* q) {
	return q->end == NULL;
}

/**
 */

int dqueue_clear   (dqueue_t* q) {

	q->begin=q->stor_begin;
	q->end=NULL;
	return 0;
}

/**
 */

int dqueue_full (dqueue_t* q) {
	return q->begin == q->end;
}

/**
 */

long int dqueue_size (dqueue_t* q) {
	if (q->end==NULL) {
		return 0;
	} else if (q->begin < q->end) {
		return q->end - q->begin;
	} else {
		return q->stor_end - q->begin + q->end - q->stor_begin;
	}
}

/**
 */

real_t dqueue_head (dqueue_t* q) {
	assert( q->end != NULL );
	return *(q->begin);
}

/**
 */

real_t dqueue_back (dqueue_t* q) {
	assert( q->end != NULL );
	return *(q->end-1);
}

/**
 */

real_t dqueue_pop (dqueue_t* q) {
	real_t tmp=*(q->begin);
	assert( q->end != NULL );
	(q->begin)++;
	if (q->begin==q->stor_end) {
		q->begin=q->stor_begin;
	}
	if (q->begin==q->end) {
		q->end=NULL;
	}

	return tmp;
}

/**
 */

real_t dqueue_pop_back (dqueue_t* q) {
	real_t tmp;
	assert( q->end != NULL );
	if (q->end != q->stor_begin) {
		tmp=*((q->end)-1);
		q->end = (q->end)-1;
	} else {
		tmp=*((q->stor_end)-1);
		q->end = (q->stor_end)-1;
	}
	if (q->begin==q->end) {
		q->end=NULL;
	}
	
	return tmp;
}

/**
 */

int dqueue_push (dqueue_t* q, real_t elem) {
	if (q->begin != q->end) {
		/* not full */
		if (q->end==NULL) {
			q->end=q->begin;
		}			
		*(q->end) = elem;
		(q->end)++;
		if (q->end==q->stor_end) {
			q->end=q->stor_begin;
		}
	} else {
		/* full, allocate more storage */
		
		real_t *bigger=NULL, *old=q->stor_begin;

		bigger=Calloc( 2*(q->stor_end - q->stor_begin)+1, real_t );

		if (q->stor_end - q->begin) {
			memcpy(bigger, q->begin, 
			       (q->stor_end - q->begin) * sizeof(real_t));
		}
		if (q->end - q->stor_begin > 0) {
			memcpy(bigger + (q->stor_end - q->begin),
			       q->stor_begin, (q->end - q->stor_begin) * sizeof(real_t));
		}
		
		q->end       =bigger + (q->stor_end - q->stor_begin);
		q->stor_end  =bigger + 2*(q->stor_end - q->stor_begin) + 1;
		q->stor_begin=bigger;
		q->begin     =bigger;
				
		*(q->end) = elem;
		(q->end)++;
		if (q->end==q->stor_end) {
			q->end=q->stor_begin;
		}

		Free(old);
	}
	
	return 0;
}

/**
 */

int vector_init      (vector_t* v, int long size) {	
        long int alloc_size= size > 0 ? size : 1;
	assert(v != NULL);
	if (size < 0) { size=0; }
	v->stor_begin=Calloc(alloc_size, real_t);
	v->stor_end=v->stor_begin + alloc_size;
	v->end=v->stor_begin+size;

	return 0;
}

/**
 */

int vector_destroy   (vector_t* v) {
	assert(v != NULL);
	assert(v->stor_begin != NULL);

	Free(v->stor_begin);
	v->stor_begin = NULL;
	
	return 0;
}

/**
 */

int vector_reserve   (vector_t* v, long int size) {
	long int actual_size=vector_size(v);
	assert(v != NULL);
	
	if (size <= vector_size(v)) { return 0; }
	
	v->stor_begin=Realloc(v->stor_begin, size, real_t);
	v->stor_end=v->stor_begin + size;
	v->end=v->stor_begin+actual_size;
	
	return 0;
}

/**
 */

int vector_empty     (vector_t* v) {
	assert(v != NULL);
	assert(v->stor_begin != NULL);
	
	return v->stor_begin == v->end;
}

/**
 */

long int vector_size      (vector_t* v) {
	assert(v != NULL);
	
	return v->end - v->stor_begin;
}

/**
 */

int vector_clear     (vector_t* v) {
	assert(v != NULL);
	
	v->end = v->stor_begin;
	
	return 0;
}

/**
 */

int vector_push_back (vector_t* v, real_t e) {
	assert(v != NULL);
	
	/* full, allocate more storage */
	if (v->stor_end == v->end) {
		long int new_size = vector_size(v) * 2;
		if (new_size == 0) { new_size = 1; }
		vector_reserve(v, new_size);
	}
	
	*(v->end) = e;
	v->end += 1;
	
	return 0;
}

/**
 */

real_t vector_e         (vector_t* v, long int pos) {
	assert(v != NULL);
	
	return * (v->stor_begin + pos);
}

/**
 */

int vector_set       (vector_t* v, long int pos, real_t value) {
	assert(v != NULL);
	
	*(v->stor_begin + pos) = value;
	return 0;
}

/**
 */

int vector_add       (vector_t* v, long int pos, real_t value) {
	assert(v != NULL);
	
	*(v->stor_begin + pos) += value;
	return 0;
}

/**
 */

int vector_null      (vector_t* v) {
	if (vector_size(v)>0) {
		memset(v->stor_begin, 0, sizeof(real_t)*vector_size(v));
	}
	return 0;
}

/**
 */

int vector_replace_first(vector_t* v, real_t old, real_t new) {
  long int i;
  assert( v != NULL);
  for (i=0; i<vector_size(v); i++) {
    if (vector_e(v, i)==old) {
      vector_set(v, i, new);
      return 0;
    }
  }
  return 1;
}

/**
 */

long int vector_find(vector_t* v, real_t elem) {
  long int i, s;
  assert(v!=NULL);
  s=vector_size(v);
  for (i=0; i<s; i++) {
    if (vector_e(v, i)==elem) {
      return i;
    }
  }
  return -1;
}

/**
 */

real_t vector_pop_back(vector_t* v) {
  real_t tmp;
  assert(v!=NULL);
  assert(v->end != v->stor_begin);
  tmp=vector_e(v, vector_size(v)-1);
  v->end -= 1;
  return tmp;
}

/**
 */

int vector_change(vector_t* v, long int pos1, long int pos2) {
  real_t tmp;
  assert(v!=NULL);
  tmp=vector_e(v, pos1);
  vector_set(v, pos1, vector_e(v, pos2));
  vector_set(v, pos2, tmp);
  return 0;
}

/**
 */

int vector_order(vector_t* v, vector_t* res, integer_t nodes) {
  long int edges=vector_size(v);
  long int *ptr=Calloc(nodes+1, long int);
  long int *rad=Calloc(edges, long int);
  long int i, j;
  memset(ptr, 0, sizeof(long int)*nodes+1);
  vector_resize(res, edges);
  
  for (i=0; i<edges; i++) {
    long int radix=v->stor_begin[i];
    if (ptr[radix]!=0) {
      rad[i]=ptr[radix];
    }
    ptr[radix]=i+1;
  }
  
  j=0;
  for (i=0; i<nodes+1; i++) {
    if (ptr[i] != 0) {
      long int next=ptr[i]-1;
      res->stor_begin[j++]=next;
      while (rad[next] != 0) {
	next=rad[next]-1;
	res->stor_begin[j++]=next;
      }
    }
  }
  
  Free(ptr);
  Free(rad);
  return 0;
}

/**
 */

int vector_resize(vector_t* v, long int newsize) {
  vector_reserve(v, newsize);
  v->end = v->stor_begin+newsize;
  return 0;
}

/**
 */

real_t vector_max(vector_t* v) {
  real_t max=*(v->stor_begin);
  real_t *ptr=v->stor_begin+1;
  while (ptr < v->stor_end) {
    if ((*ptr) > max) {
      max=*ptr;
    }
    ptr++;
  }
  return max;
}

/**
 */

vector_t vector_as_vector(real_t* data, long int length) {
  vector_t v;
  v.stor_begin=data;
  v.stor_end=v.end=data+length;
  
  return v;
}

int vector_init_copy(vector_t *v, real_t *data, long int length) {
  v->stor_begin=Calloc(length, real_t);
  v->stor_end=v->stor_begin+length;
  v->end=v->stor_end;
  memcpy(v->stor_begin, data, length*sizeof(real_t));
  
  return 0;
}

int vector_copy_to(vector_t *v, real_t* to) {
  if (v->end != v->stor_begin) {
    memcpy(to, v->stor_begin, sizeof(real_t) * (v->end - v->stor_begin));
  }
  return 0;
}

real_t vector_prod(vector_t *v) {
  real_t res=1;
  real_t *p;
  for (p=v->stor_begin; p<v->end; p++) {
    res *= *p;
  }
  return res;
}

int vector_init_seq(vector_t *v, real_t from, real_t to) {
  real_t *p;
  vector_init(v, to-from+1);
  for (p=v->stor_begin; p<v->end; p++) {
    *p = from++;
  }
  
  return 0;
}

int matrix_init(matrix_t *m, long int nrow, long int ncol) {
  vector_init(&m->data, nrow*ncol);
  m->nrow=nrow;
  m->ncol=ncol;
}

int matrix_destroy(matrix_t *m) {
  vector_destroy(&m->data);
}

int matrix_resize(matrix_t *m, long int nrow, long int ncol) {
  vector_resize(&m->data, nrow*ncol);
  m->nrow=nrow;
  m->ncol=ncol;
}

long int matrix_size(matrix_t *m) {
  return (m->nrow) * (m->ncol);
}

long int matrix_nrow(matrix_t *m) {
  return m->nrow;
}

long int matrix_ncol(matrix_t *m) {
  return m->ncol;
}

int matrix_copy_to(matrix_t *m, real_t *to) {
  vector_copy_to(&m->data, to);
  return 0;
}

int matrix_null(matrix_t *m) {
  vector_null(&m->data);
}

/**
 */

int stack_init       (stack_t* s, long int size) {
        long int alloc_size= size > 0 ? size : 1;
	assert (s != NULL);
	if (size < 0) { size=0; }
	s->stor_begin=Calloc(alloc_size, real_t);
	s->stor_end=s->stor_begin + alloc_size;
	s->end=s->stor_begin;
	
	return 0;
}

/**
 */

int stack_destroy    (stack_t* s) {
	assert (s != NULL);
	assert (s->stor_begin != NULL);
	Free(s->stor_begin);
	s->stor_begin=NULL;
	
	return 0;
}

/**
 * \todo
 */

int stack_reserve    (stack_t* s, long int size) {
  return 0;
}

/**
 */

int stack_empty      (stack_t* s) {
	assert (s != NULL);
	assert (s->stor_begin != NULL);
	assert (s->end != NULL);
	
	return s->stor_begin == s->end;
}

/**
 */

long int stack_size       (stack_t* s) {
	assert (s != NULL);
	assert (s->stor_begin != NULL);
	
	return s->end - s->stor_begin;
}

/**
 */

int stack_clear      (stack_t* s) {
	assert (s != NULL);
	assert (s->stor_begin != NULL);
	s->end = s->stor_begin;
	
	return 0;
}

/**
 */

int stack_push       (stack_t* s, real_t elem) {
	assert (s != NULL);
	assert (s->stor_begin != NULL);
	if (s->end == s->stor_end) {
		/* full, allocate more storage */
		
	        real_t *bigger=NULL, *old=s->stor_begin;
		
		bigger = Calloc(2*stack_size(s)+1, real_t);
		
		memcpy(bigger, s->stor_begin, 
		       stack_size(s)*sizeof(real_t));

		s->end        = bigger + (s->stor_end - s->stor_begin);
		s->stor_end   = bigger + 2*(s->stor_end - s->stor_begin)+1;
		s->stor_begin = bigger;
		
		*(s->end) = elem;
		(s->end) += 1;

		Free(old);
	} else {
		*(s->end) = elem;
		(s->end) += 1;
	}
	return 0;
}

/**
 */

real_t stack_pop        (stack_t* s) {

	assert (s != NULL);
	assert (s->stor_begin != NULL);
	assert (s->end != NULL);
	assert (s->end != s->stor_begin);
		
	(s->end)--;
	
	return *(s->end);
}

/**
 */

int multiset_init    (multiset_t* m, long int size) {
  size= size > 0 ? size : 1;
  assert(m != NULL);
  m->stor_begin=Calloc(size, real_t);
  m->stor_end=m->stor_begin + size;
  m->end=m->stor_begin;
  
  return 0;
}

/**
 */

int multiset_destroy (multiset_t* m) {
  assert(m != NULL);
  assert(m->stor_begin != NULL);
  
  Free(m->stor_begin);
  m->stor_begin = NULL;
  
  return 0;
}

/**
 */

int multiset_reserve (multiset_t* m, long int size) {
  long int actual_size;
  
  assert(m != NULL);
  assert(m->stor_begin != NULL);

  actual_size=multiset_size(m);
  if (size < actual_size) { return 0; }
  
  m->stor_begin=Realloc(m->stor_begin, size, real_t);
  m->stor_end=m->stor_begin + size;
  m->end=m->stor_begin+actual_size;
  
  return 0;
}

/**
 */

int multiset_add     (multiset_t* m, real_t elem) {
  assert(m != NULL);
  
  if (m->stor_end == m->end) {
    long int new_size=multiset_size(m) * 2;
    if (new_size==0) { new_size=1; }
    multiset_reserve(m, new_size);
  }

  *(m->end) = elem;
  m->end += 1;
  
  return 0;
}

/**
 */

real_t multiset_choose  (multiset_t* m) {
  assert(m != NULL);
  assert(m->end != m->stor_begin);
  
  return *(m->stor_begin);
}

/**
 */

real_t multiset_choose_remove (multiset_t* m) {
  real_t tmp;
  assert(m != NULL);
  assert(m->end != m->stor_begin);
  
  tmp= *(m->stor_begin);
  *(m->stor_begin)=*((m->end)-1);
  m->end -= 1;

  return tmp;
}

/**
 */

long int multiset_size(multiset_t* m) {
  assert(m != NULL);
  
  return m->end - m->stor_begin;
}

/**
 */

int multiset_remove (multiset_t* m, real_t elem) {
  real_t *p;
  assert(m != NULL);

  for (p=m->stor_begin; p<m->end; p++) {
    if (*p == elem) {
      *p=*((m->end)-1);
      m->end -= 1;
      return 0;
    }
  }
  return 1;
}

/**
 */

int multiset_remove_all (multiset_t* m, real_t elem) {
  real_t *p;
  assert(m != NULL);
  
  for (p=m->stor_begin; p<m->end; ) {
    if (*p==elem) {
      *p=*((m->end)-1);
      m->end -= 1;
    } else {
      p++;
    }
  }
    return 0;
}

/**
 */

real_t* multiset_get_vector(multiset_t* m) {
  assert (m != NULL);
  
  return m->stor_begin;  
}
     
/**
 */

real_t multiset_choose_random(multiset_t* m) {
  long int rnd;
  
  assert(m != NULL);
  
  rnd=RNG_INTEGER(0, multiset_size(m)-1);

  return *((m->stor_begin) + rnd);
}

/**
 */

real_t multiset_choose_remove_random(multiset_t* m) {
  long int rnd;
  real_t tmp;
  
  assert(m != NULL);
  assert(m->stor_begin != m->end);
  
  rnd=RNG_INTEGER(0, multiset_size(m)-1);
  tmp=*((m->stor_begin) + rnd);

  *((m->stor_begin)+rnd) = *((m->end)-1);
  m->end -= 1;

  return tmp;
}

/**
 */

int multiset_clear   (multiset_t* m) {
  assert(m != NULL);
  
  m->end = m->stor_begin;

  return 0;
}
  

/**
 */

long int multiset_count(multiset_t* m, real_t elem) {
  real_t *p;
  long int c;
  assert(m != NULL);
  
  c=0;
  for (p=m->stor_begin; p<m->end; p++) {
    if (*p == elem) {
      c++;
    }
  }
  
  return c;
}

/**
 */

long int multiset_count_different(multiset_t* m, real_t elem) {
  real_t *p;
  long int c;
  assert(m != NULL);
  
  c=0;
  for (p=m->stor_begin; p<m->end; p++) {
    if (*p != elem) {
      c++;
    }
  }
  
  return c;
}

/**
 */

real_t multiset_choose_random_different(multiset_t* m, real_t elem) {
  long int all, s;
  real_t *p;
  
  assert(m != NULL);
  
  all=multiset_count_different(m, elem);
  if (all == 0) {
    return elem;
  }
  s=RNG_INTEGER(0, all-1);
  for (p=m->stor_begin; p<m->end; p++) {
    if (*p==elem) { 
      continue; 
    } else if (s==0) {
      return *p;
    } else {
      s--;
    }
  }
  return 0;
}

#define PARENT(x)     (((x)+1)/2-1)
#define LEFTCHILD(x)  (((x)+1)*2-1)
#define RIGHTCHILD(x) (((x)+1)*2)
  
/**
 */

int heap_init           (heap_t* h, long int alloc_size) {
  if (alloc_size <= 0 ) { alloc_size=1; }
  h->stor_begin=Calloc(alloc_size, real_t);
  h->stor_end=h->stor_begin + alloc_size;
  h->end=h->stor_begin;
  h->destroy=1;
  
  return 0;  
}

/**
 */

int heap_init_array     (heap_t *h, real_t* data, long int len) {
  h->stor_begin=Calloc(len, real_t);
  h->stor_end=h->stor_begin+len;
  h->end=h->stor_end;
  h->destroy=1;

  memcpy(h->stor_begin, data, len*sizeof(real_t));

  heap_i_build (h->stor_begin, h->end-h->stor_begin, 0);
  
  return 0;
}

/**
 */

int heap_destroy        (heap_t* h) {  
  if (h->destroy) {
    assert( h->stor_begin != NULL);
    Free(h->stor_begin);
  }
  return 0;
}


int heap_empty          (heap_t* h) {
  return h->stor_begin == h->end;
}

/**
 */

int heap_push           (heap_t* h, real_t elem) {
  assert(h != NULL);
	
  /* full, allocate more storage */
  if (h->stor_end == h->end) {
    long int new_size = heap_size(h) * 2;
    if (new_size == 0) { new_size = 1; }
    heap_reserve(h, new_size);
  }
	
  *(h->end) = elem;
  h->end += 1;

  /* maintain heap */
  heap_i_shift_up(h->stor_begin, heap_size(h), heap_size(h)-1);
	
  return 0;
}

/**
 */

real_t heap_max       (heap_t* h) {
  assert(h != NULL);
  assert(h->stor_begin != NULL);
  assert(h->stor_begin != h->end);
  
  return h->stor_begin[0];
}

/**
 */

real_t heap_delete_max(heap_t* h) {
  real_t tmp;

  assert(h != NULL);
  assert(h->stor_begin != NULL);

  tmp=h->stor_begin[0];
  heap_i_switch(h->stor_begin, 0, heap_size(h)-1);
  h->end -= 1;
  heap_i_sink(h->stor_begin, h->end-h->stor_begin, 0);
  
  return tmp;
}

/**
 */

long int heap_size      (heap_t* h) {
  assert (h != NULL);
  return h->end - h->stor_begin;
}

/**
 */

int heap_reserve        (heap_t* h, long int size) {
  long int actual_size=heap_size(h);
  assert(h != NULL);
  
  if (size <= actual_size) { return 0; }
  
  h->stor_begin=Realloc(h->stor_begin, size, real_t);
  h->stor_end=h->stor_begin + size;
  h->end=h->stor_begin+actual_size;
  
  return 0;
}

/**
 */

int heap_i_build(real_t* arr, long int size, long int head) {

  if (RIGHTCHILD(head) < size) { 
    /* both subtrees */
    heap_i_build(arr, size, LEFTCHILD(head) );
    heap_i_build(arr, size, RIGHTCHILD(head));
    heap_i_sink(arr, size, head);
  } else if (LEFTCHILD(head) < size) {
    /* only left */
    heap_i_build(arr, size, LEFTCHILD(head));
    heap_i_sink(arr, size, head);
  } else {
    /* none */
  }
  return 0;
}

/**
 */

int heap_i_shift_up(real_t* arr, long int size, long int elem) {
  
  if (elem==0 || arr[elem] < arr[PARENT(elem)]) { 
    /* at the top */
  } else {
    heap_i_switch(arr, elem, PARENT(elem));
    heap_i_shift_up(arr, size, PARENT(elem));
  }
  return 0;
}

/**
 */

int heap_i_sink(real_t* arr, long int size, long int head) {

  if (LEFTCHILD(head) >= size) { 
    /* no subtrees */
  } else if (RIGHTCHILD(head) == size ||
	     arr[LEFTCHILD(head)] >= arr[RIGHTCHILD(head)]) {
    /* sink to the left if needed */
    if (arr[head] < arr[LEFTCHILD(head)]) {
      heap_i_switch(arr, head, LEFTCHILD(head));
      heap_i_sink(arr, size, LEFTCHILD(head));
    }
  } else {
    /* sink to the right */
    if (arr[head] < arr[RIGHTCHILD(head)]) {
      heap_i_switch(arr, head, RIGHTCHILD(head));
      heap_i_sink(arr, size, RIGHTCHILD(head));
    }
  }

  return 0;
}

int heap_i_switch(real_t* arr, long int e1, long int e2) {
  if (e1!=e2) {
    real_t tmp=arr[e1];
    arr[e1]=arr[e2];
    arr[e2]=tmp;
  }

  return 0;
}

/**
 */

int indheap_init           (indheap_t* h, long int alloc_size) {
 if (alloc_size <= 0 ) { alloc_size=1; }
  h->stor_begin=Calloc(alloc_size, real_t);
  h->stor_end=h->stor_begin + alloc_size;
  h->end=h->stor_begin;
  h->destroy=1;
  h->index_begin=Calloc(alloc_size, long int);
  
  return 0;  
}

/**
 */

int indheap_init_array     (indheap_t *h, real_t* data, long int len) {
  long int i;

  h->stor_begin=Calloc(len, real_t);
  h->stor_end=h->stor_begin+len;
  h->end=h->stor_end;
  h->destroy=1;
  h->index_begin=Calloc(len, long int);

  memcpy(h->stor_begin, data, len*sizeof(real_t));
  for (i=0; i<len; i++) {
    h->index_begin[i]=i+1;
  }

  indheap_i_build (h, 0);
  
  return 0;
}

/**
 */

int indheap_destroy        (indheap_t* h) {  
  if (h->destroy) {
    assert( h->stor_begin != NULL);
    Free(h->stor_begin);
    Free(h->index_begin);
  }
  return 0;
}

int indheap_empty          (indheap_t* h) {
  return h->stor_begin == h->end;
}

/**
 */

int indheap_push           (indheap_t* h, real_t elem) {
  assert(h != NULL);
	
  /* full, allocate more storage */
  if (h->stor_end == h->end) {
    long int new_size = indheap_size(h) * 2;
    if (new_size == 0) { new_size = 1; }
    indheap_reserve(h, new_size);
  }
	
  *(h->end) = elem;
  h->end += 1;
  *(h->index_begin+indheap_size(h)-1)=indheap_size(h)-1;

  /* maintain indheap */
  indheap_i_shift_up(h, indheap_size(h)-1);
	
  return 0;
}

/**
 */

real_t indheap_max       (indheap_t* h) {
  assert(h != NULL);
  assert(h->stor_begin != NULL);
  assert(h->stor_begin != h->end);
  
  return h->stor_begin[0];
}

/**
 */

real_t indheap_delete_max(indheap_t* h) {
  real_t tmp;

  assert(h != NULL);
  assert(h->stor_begin != NULL);

  tmp=h->stor_begin[0];
  indheap_i_switch(h, 0, indheap_size(h)-1);
  h->end -= 1;
  indheap_i_sink(h, 0);
  
  return tmp;
}

/**
 */

long int indheap_size      (indheap_t* h) {
  assert (h != NULL);
  return h->end - h->stor_begin;
}

/**
 */

int indheap_reserve        (indheap_t* h, long int size) {
  long int actual_size=indheap_size(h);
  assert(h != NULL);
  
  if (size <= actual_size) { return 0; }
  
  h->stor_begin=Realloc(h->stor_begin, size, real_t);
  h->stor_end=h->stor_begin + size;
  h->end=h->stor_begin+actual_size;
  h->index_begin=Realloc(h->index_begin, size, long int);
  
  return 0;
}

long int indheap_max_index(indheap_t *h) {
  return h->index_begin[0];  
}

/**
 */

int indheap_i_build(indheap_t* h, long int head) {

  long int size=indheap_size(h);
  if (RIGHTCHILD(head) < size) { 
    /* both subtrees */
    indheap_i_build(h, LEFTCHILD(head) );
    indheap_i_build(h, RIGHTCHILD(head));
    indheap_i_sink(h, head);
  } else if (LEFTCHILD(head) < size) {
    /* only left */
    indheap_i_build(h, LEFTCHILD(head));
    indheap_i_sink(h, head);
  } else {
    /* none */
  }
  return 0;
}

/**
 */

int indheap_i_shift_up(indheap_t *h, long int elem) {
  
  if (elem==0 || h->stor_begin[elem] < h->stor_begin[PARENT(elem)]) { 
    /* at the top */
  } else {
    indheap_i_switch(h, elem, PARENT(elem));
    indheap_i_shift_up(h, PARENT(elem));
  }
  return 0;
}

/**
 */

int indheap_i_sink(indheap_t* h, long int head) {

  long int size=indheap_size(h);
  if (LEFTCHILD(head) >= size) { 
    /* no subtrees */
  } else if (RIGHTCHILD(head) == size ||
	     h->stor_begin[LEFTCHILD(head)]>=h->stor_begin[RIGHTCHILD(head)]) {
    /* sink to the left if needed */
    if (h->stor_begin[head] < h->stor_begin[LEFTCHILD(head)]) {
      indheap_i_switch(h, head, LEFTCHILD(head));
      indheap_i_sink(h, LEFTCHILD(head));
    }
  } else {
    /* sink to the right */
    if (h->stor_begin[head] < h->stor_begin[RIGHTCHILD(head)]) {
      indheap_i_switch(h, head, RIGHTCHILD(head));
      indheap_i_sink(h, RIGHTCHILD(head));
    }
  }

  return 0;
}

int indheap_i_switch(indheap_t* h, long int e1, long int e2) {
  if (e1!=e2) {
    real_t tmp=h->stor_begin[e1];
    h->stor_begin[e1]=h->stor_begin[e2];
    h->stor_begin[e2]=tmp;
    
    tmp=h->index_begin[e1];
    h->index_begin[e1]=h->index_begin[e2];
    h->index_begin[e2]=tmp;
  }

  return 0;
}


/**
 */

int d_indheap_init           (d_indheap_t* h, long int alloc_size) {
 if (alloc_size <= 0 ) { alloc_size=1; }
  h->stor_begin=Calloc(alloc_size, real_t);
  h->stor_end=h->stor_begin + alloc_size;
  h->end=h->stor_begin;
  h->destroy=1;
  h->index_begin=Calloc(alloc_size, long int);
  h->index2_begin=Calloc(alloc_size, long int);
  
  return 0;  
}

/**
 */

int d_indheap_destroy        (d_indheap_t* h) {  
  if (h->destroy) {
    assert( h->stor_begin != NULL);
    Free(h->stor_begin);
    Free(h->index_begin);
    Free(h->index2_begin);
  }
  return 0;
}

int d_indheap_empty          (d_indheap_t* h) {
  return h->stor_begin == h->end;
}

/**
 */

int d_indheap_push           (d_indheap_t* h, real_t elem, 
			      long int idx, long int idx2) {
  assert(h != NULL);
	
  /* full, allocate more storage */
  if (h->stor_end == h->end) {
    long int new_size = d_indheap_size(h) * 2;
    if (new_size == 0) { new_size = 1; }
    d_indheap_reserve(h, new_size);
  }
	
  *(h->end) = elem;
  h->end += 1;
  *(h->index_begin+d_indheap_size(h)-1)=idx ;
  *(h->index2_begin+d_indheap_size(h)-1)=idx2 ;

  /* maintain d_indheap */
  d_indheap_i_shift_up(h, d_indheap_size(h)-1);
	
  return 0;
}

/**
 */

real_t d_indheap_max       (d_indheap_t* h) {
  assert(h != NULL);
  assert(h->stor_begin != NULL);
  assert(h->stor_begin != h->end);
  
  return h->stor_begin[0];
}

/**
 */

real_t d_indheap_delete_max(d_indheap_t* h) {
  real_t tmp;

  assert(h != NULL);
  assert(h->stor_begin != NULL);

  tmp=h->stor_begin[0];
  d_indheap_i_switch(h, 0, d_indheap_size(h)-1);
  h->end -= 1;
  d_indheap_i_sink(h, 0);
  
  return tmp;
}

/**
 */

long int d_indheap_size      (d_indheap_t* h) {
  assert (h != NULL);
  return h->end - h->stor_begin;
}

/**
 */

int d_indheap_reserve        (d_indheap_t* h, long int size) {
  long int actual_size=d_indheap_size(h);
  assert(h != NULL);
  
  if (size <= actual_size) { return 0; }
  
  h->stor_begin=Realloc(h->stor_begin, size, real_t);
  h->stor_end=h->stor_begin + size;
  h->end=h->stor_begin+actual_size;
  h->index_begin=Realloc(h->index_begin, size, long int);
  h->index2_begin=Realloc(h->index2_begin, size, long int);
  
  return 0;
}

int d_indheap_max_index(d_indheap_t *h, long int *idx, long int *idx2) {
  (*idx)=h->index_begin[0];
  (*idx2)=h->index2_begin[0];

  return 0;
}

/**
 */

int d_indheap_i_build(d_indheap_t* h, long int head) {

  long int size=d_indheap_size(h);
  if (RIGHTCHILD(head) < size) { 
    /* both subtrees */
    d_indheap_i_build(h, LEFTCHILD(head) );
    d_indheap_i_build(h, RIGHTCHILD(head));
    d_indheap_i_sink(h, head);
  } else if (LEFTCHILD(head) < size) {
    /* only left */
    d_indheap_i_build(h, LEFTCHILD(head));
    d_indheap_i_sink(h, head);
  } else {
    /* none */
  }
  return 0;
}

/**
 */

int d_indheap_i_shift_up(d_indheap_t *h, long int elem) {
  
  if (elem==0 || h->stor_begin[elem] < h->stor_begin[PARENT(elem)]) { 
    /* at the top */
  } else {
    d_indheap_i_switch(h, elem, PARENT(elem));
    d_indheap_i_shift_up(h, PARENT(elem));
  }
  return 0;
}

/**
 */

int d_indheap_i_sink(d_indheap_t* h, long int head) {

  long int size=d_indheap_size(h);
  if (LEFTCHILD(head) >= size) { 
    /* no subtrees */
  } else if (RIGHTCHILD(head) == size ||
	     h->stor_begin[LEFTCHILD(head)]>=h->stor_begin[RIGHTCHILD(head)]) {
    /* sink to the left if needed */
    if (h->stor_begin[head] < h->stor_begin[LEFTCHILD(head)]) {
      d_indheap_i_switch(h, head, LEFTCHILD(head));
      d_indheap_i_sink(h, LEFTCHILD(head));
    }
  } else {
    /* sink to the right */
    if (h->stor_begin[head] < h->stor_begin[RIGHTCHILD(head)]) {
      d_indheap_i_switch(h, head, RIGHTCHILD(head));
      d_indheap_i_sink(h, RIGHTCHILD(head));
    }
  }

  return 0;
}

int d_indheap_i_switch(d_indheap_t* h, long int e1, long int e2) {
  if (e1!=e2) {
    long int tmpi;
    real_t tmp=h->stor_begin[e1];
    h->stor_begin[e1]=h->stor_begin[e2];
    h->stor_begin[e2]=tmp;
    
    tmpi=h->index_begin[e1];
    h->index_begin[e1]=h->index_begin[e2];
    h->index_begin[e2]=tmpi;

    tmpi=h->index2_begin[e1];
    h->index2_begin[e1]=h->index2_begin[e2];
    h->index2_begin[e2]=tmpi;
  }

  return 0;
}

/**********************************************************
 * Testing purposes, vector                               *
 *********************************************************/

/* int print_vector(vector_t *v) { */
/*   long int i; */
/*   for (i=0; i<vector_size(v); i++) { */
/*     printf("%f ", VECTOR(*v)[i]); */
/*   } */
/*   printf("\n"); */
  
/*   return 0; */
/* } */

/* int main() { */
  
/*   vector_t v, order; */
/*   long int i; */
/*   vector_init(&v, 10); */
/*   vector_null(&v); */

/*   /\* indexing *\/ */
/*   for (i=0; i<vector_size(&v); i++) { */
/*     VECTOR(v)[i]=i+1; */
/*   } */
/*   print_vector(&v);   */
  
/*   /\* resize *\/ */
/*   vector_resize(&v, 12); */
/*   VECTOR(v)[10]=100; */
/*   VECTOR(v)[11]=1; */
/*   print_vector(&v); */
  
/*   /\* order *\/ */
/*   vector_init(&order,0); */
/*   vector_order(&v, &order, 100); */
/*   print_vector(&order); */

/*   return 0; */
/* } */
