#include "flexdef.h"

/* global buffers. */
struct Buf userdef_buf; /* for user #definitions triggered by cmd-line. */


/* functions for growable buffer. */

/* Appends n characters in str to buf. */
struct Buf* buf_strnappend (buf,str,n)
    struct Buf* buf;
    const char* str;
    int n;
{
    buf_append(buf, str, n+1);

    /* "undo" the '\0' character that buf_append() already copied. */
    buf->nelts--;

    return buf;
}

/* Appends characters in str to buf. */
struct Buf* buf_strappend (buf,str)
    struct Buf* buf;
    const char* str;
{
    return buf_strnappend(buf, str, strlen(str));
}

/* appends "#define str def\n" */
struct Buf* buf_strdefine (buf,str,def)
    struct Buf* buf;
    const char* str;
    const char* def;
{
    buf_strappend(buf, "#define ");
    buf_strappend(buf, " ");
    buf_strappend(buf, str);
    buf_strappend(buf, " ");
    buf_strappend(buf, def);
    buf_strappend(buf, "\n");
    return buf;
}

/* create buf with 0 elements, each of size elem_size. */
void buf_init(buf, elem_size)
    struct Buf * buf;
    size_t elem_size;
{
    buf->elts      = (void*)0;
    buf->nelts     = 0;
    buf->elt_size  = elem_size;
    buf->nmax      = 0;
}

/* frees memory */
void buf_destroy(buf)
    struct Buf * buf;
{
    if(buf && buf->elts)
        flex_free(buf->elts);
    buf->elts = (void*)0;
}


/* appends ptr[] to buf, grow if necessary.
 * n_elem is number of elements in ptr[], NOT bytes.
 * returns buf.
 * We grow by mod(512) boundaries.
 */

struct Buf* buf_append(buf,ptr,n_elem)
    struct Buf * buf;
    const void * ptr;
    int n_elem;
{
    int n_alloc=0;

    if (!ptr || n_elem==0)
        return buf;

    /* May need to alloc more. */
    if (n_elem + buf->nelts > buf->nmax) {

        /* exact amount needed... */
        n_alloc = (n_elem + buf->nelts) * buf->elt_size;

        /* ...plus some extra */
        if (((n_alloc*buf->elt_size)%512) != 0 && buf->elt_size < 512)
            n_alloc += (512 - ((n_alloc*buf->elt_size)%512)) / buf->elt_size;

        if (!buf->elts)
            buf->elts = allocate_array( n_alloc , buf->elt_size);
        else
            buf->elts = reallocate_array(buf->elts, n_alloc, buf->elt_size);

        buf->nmax = n_alloc;
    }

    memcpy((char*)buf->elts + buf->nelts*buf->elt_size, ptr, n_elem*buf->elt_size);
    buf->nelts += n_elem;

    return buf;
}

/* vim:set tabstop=8 softtabstop=4 shiftwidth=4: */
