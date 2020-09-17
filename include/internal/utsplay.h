/*
Copyright (c) 2003-2018, Troy D. Hanson     http://troydhanson.github.com/uthash/
Copyright (c) 2020, Brice Videau, Swann Perarnau
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef UTSPLAY_H
#define UTSPLAY_H

#define UTSPLAY_VERSION 1.0.0

#include <string.h>   /* memcmp, memset, strlen */
#include <stddef.h>   /* ptrdiff_t */
#include <stdlib.h>   /* exit */

/* These macros use decltype or the earlier __typeof GNU extension.
   As decltype is only available in newer compilers (VS2010 or gcc 4.3+
   when compiling c++ source) this code uses whatever method is needed
   or, for VS2008 where neither is available, uses casting workarounds. */
#if !defined(SPDECLTYPE) && !defined(NO_DECLTYPE)
#if defined(_MSC_VER)   /* MS compiler */
#if _MSC_VER >= 1600 && defined(__cplusplus)  /* VS2010 or newer in C++ mode */
#define SPDECLTYPE(x) decltype(x)
#else                   /* VS2008 or older (or VS2010 in C mode) */
#define NO_DECLTYPE
#endif
#elif defined(__BORLANDC__) || defined(__ICCARM__) || defined(__LCC__) || defined(__WATCOMC__)
#define NO_DECLTYPE
#else                   /* GNU, Sun and other compilers */
#define SPDECLTYPE(x) __typeof(x)
#endif
#endif

#ifdef NO_DECLTYPE
#define IF_NO_DECLTYPE(x) x
#define SPLAY_SVTREE(tree) _sp_tree_sv = (char *)tree
#define SPLAY_RSTREE(tree) *(char**)&(tree) = _sp_tree_sv
#define SPLAY_UPTREE(tree, n) _sp_tree_sv = (char *)n
#define SPLAY_ASSIGN(a, b) a = (char *)b
#define SPLAY_ASSIGN2(a, b) *(char **)&a = (char *)b
#define SPLAY_EQL(a, b) (char*)a == (char*)b
#define SPLAY_PROCESSW(tree, elt) \
do { *(char **)(&tree) = (char*)elt; SPLAY_PROCESS(tree); } while(0)
#define SPLAY_INITW(tree, elt) \
do { *(char **)(&tree) = (char*)elt; SPLAY_INIT(tree); } while(0)
#define SPLAY_DEINITW(tree, elt) \
do { *(char **)(&tree) = (char*)elt; SPLAY_DEINIT(tree); } while(0)
#define SPDECLTYPE(x) char*
#define SPDECLTYPEREF(x) char**
#define SPDECLTYPE_ASSIGN(dst,src)                                                 \
do {                                                                             \
  char **_da_dst = (char**)(&(dst));                                             \
  *_da_dst = (char*)(src);                                                       \
} while (0)
#else
#define IF_NO_DECLTYPE(x)
#define SPLAY_SVTREE(tree)
#define SPLAY_RSTREE(tree)
#define SPLAY_UPTREE(tree, n) tree = n
#define SPLAY_ASSIGN(a, b) a = b
#define SPLAY_ASSIGN2(a, b) a = b
#define SPLAY_EQL(a, b) a == b
#define SPLAY_PROCESSW(tree, n) \
    SPLAY_PROCESS(n)
#define SPLAY_INITW(tree, n) \
    SPLAY_INIT(n)
#define SPLAY_DEINITW(tree, n) \
    SPLAY_DEINIT(n)
#define SPDECLTYPEREF(x) SPDECLTYPE(&x)
#define SPDECLTYPE_ASSIGN(dst,src)                                                 \
do {                                                                             \
  (dst) = (SPDECLTYPE(dst))(src);                                                    \
} while (0)
#endif

#ifndef utsplay_malloc
#define utsplay_malloc(sz) malloc(sz)      /* malloc fcn                      */
#endif
#ifndef utsplay_realloc
#define utsplay_realloc(ptr, sz) realloc(ptr, sz) /* realloc fcn                      */
#endif
#ifndef utsplay_free
#define utsplay_free(ptr,sz) free(ptr)     /* free fcn                        */
#endif
#ifndef utsplay_bzero
#define utsplay_bzero(a,n) memset((char*)(a),'\0',n)
#endif
#ifndef utsplay_strlen
#define utsplay_strlen(s) strlen(s)
#endif
#ifndef utsplay_memcmp
#define utsplay_memcmp(a,b,n) memcmp((const char*)(a),(const char*)(b),n)
#endif
#ifndef utsplay_strcmp
#define utsplay_strcmp(a,b) strcmp((const char*)(a),(const char*)(b))
#endif

#ifndef utsplay_oom
#define utsplay_oom() exit(-1)
#endif

#ifndef SPLAY_KEYCMP
#define SPLAY_KEYCMP(a,b,n) utsplay_memcmp(a,b,n)
#endif

#ifndef SPLAY_STRCMP
#define SPLAY_STRCMP(a,b,n) utsplay_strcmp(a,b)
#endif

#ifndef SPLAY_INIT
#define SPLAY_INIT(e)
#endif

#ifndef SPLAY_PROCESS
#define SPLAY_PROCESS(e)
#endif

#ifndef SPLAY_DEINIT
#define SPLAY_DEINIT(e)
#endif

/*
 * This file contains macros to manipulate splaytrees.
 *
 * Your structure must have a "left", "right", and "parent" pointer.
 * Either way, the pointer to the head of the tree must be initialized to NULL.
 *
 * ----------------.EXAMPLE -------------------------
 * struct item {
 *      int key;
 *      struct item *left, *right, *parent;
 * }
 *
 * struct item *tree = NULL:
 *
 * int main() {
 *      struct item *item;
 *      ... allocate and populate item ...
 *      SPLAY_ADD(tree, item);
 * }
 * --------------------------------------------------
 *
 */

/* See https://en.wikipedia.org/wiki/Splay_tree */

#define SPLAY_ADD_KEYSTR(tree, node) \
    SPLAY_ADD_KEYSTR2(tree, key, 0, node)

#define SPLAY_ADD_KEYSTR2(tree, key, sz, node) \
    SPLAY_ADD_KEYPTR3(tree, SPLAY_KEYPTR(node, node, key), sz, node, key, left, right, parent)

#define SPLAY_ADD_KEYSTR3(tree, key_val, sz, node, key, left, right, parent) \
    SPLAY_INSERT_ADD(SPLAY_ADD_CREATE, SPLAY_KEYPTR, SPLAY_STRCMP, tree, key_val, sz, node, key, left, right, parent)

#define SPLAY_ADD_KEYPTR(tree, sz, node) \
    SPLAY_ADD_KEYPTR2(tree, key, sz, node)

#define SPLAY_ADD_KEYPTR2(tree, key, sz, node) \
    SPLAY_ADD_KEYPTR3(tree, SPLAY_KEYPTR(node, node, key), sz, node, key, left, right, parent)

#define SPLAY_ADD_KEYPTR3(tree, key_val, sz, node, key, left, right, parent) \
    SPLAY_INSERT_ADD(SPLAY_ADD_CREATE, SPLAY_KEYPTR, SPLAY_KEYCMP, tree, key_val, sz, node, key, left, right, parent)

#define SPLAY_ADD(tree, node) \
    SPLAY_ADD2(tree, key, sizeof(SPLAY_FIELD(node, node, key)), node)

#define SPLAY_ADD2(tree, key,  sz, node) \
    SPLAY_ADD3(tree, SPLAY_KEY(node, node, key), sz, node, key, left, right, parent)

#define SPLAY_ADD3(tree, key_val, sz, node, key, left, right, parent) \
    SPLAY_INSERT_ADD(SPLAY_ADD_CREATE, SPLAY_KEY, SPLAY_KEYCMP, tree, key_val, sz, node, key, left, right, parent)

#define SPLAY_ADD_CREATE(key_macro, tree, n, key_val, sz, node, key, left, right, parent) \
do { \
    SPLAY_ASSIGN(n, node); \
    SPLAY_RIGHT_ASSIGN(tree, n, right, NULL); \
    SPLAY_LEFT_ASSIGN(tree, n, left, NULL); \
    SPLAY_PARENT_ASSIGN(tree, n, parent, NULL); \
    node = NULL; \
} while(0)

#define SPLAY_INSERT_KEYSTR(tree, key_val) \
    SPLAY_INSERT_KEYSTR2(tree, key_val, (utsplay_strlen(key_val)+1), key, left, right, parent)

#define SPLAY_INSERT_KEYSTR2(tree, key_val, sz, key, left, right, parent) \
    SPLAY_INSERT_ADD(SPLAY_INSERT_CREATE_KEYPTR, SPLAY_KEYPTR, SPLAY_STRCMP, tree, key_val, sz, NULL, key, left, right, parent)

#define SPLAY_INSERT_KEYPTR(tree, key_val, sz) \
    SPLAY_INSERT_KEYPTR2(tree, key_val, sz, key, left, right, parent)

#define SPLAY_INSERT_KEYPTR2(tree, key_val, sz, key, left, right, parent) \
    SPLAY_INSERT_ADD(SPLAY_INSERT_CREATE_KEYPTR, SPLAY_KEYPTR, SPLAY_KEYCMP, tree, key_val, sz, NULL, key, left, right, parent)

#define SPLAY_INSERT_CREATE_KEYPTR(key_macro, tree, n, key_val, sz, node, key, left, right, parent) \
do { \
    n = (SPDECLTYPE(tree))utsplay_malloc(sizeof(*tree) + sz); \
    if (!n) \
        utsplay_oom(); \
    utsplay_bzero(n, sizeof(*tree) + sz); \
    SPLAY_FIELD_ASSIGN(tree, n, key, (sizeof(*tree) + (char *)n)); \
    memcpy(key_macro(tree, n, key), key_val, sz); \
} while(0)

#define SPLAY_INSERT(tree, key_val, sz) \
    SPLAY_INSERT2(tree, key_val, sz, key, left, right, parent)

#define SPLAY_INSERT2(tree, key_val, sz, key, left, right, parent) \
    SPLAY_INSERT_ADD(SPLAY_INSERT_CREATE, SPLAY_KEY, SPLAY_KEYCMP, tree, key_val, sz, NULL, key, left, right, parent)

#define SPLAY_INSERT_CREATE(key_macro, tree, n, key_val, sz, node, key, left, right, parent) \
do { \
    n = (SPDECLTYPE(tree))utsplay_malloc(sizeof(*tree)); \
    if (!n) \
        utsplay_oom(); \
    utsplay_bzero(n, sizeof(*tree)); \
    memcpy(key_macro(tree, n, key), key_val, sz); \
} while(0)

#define SPLAY_COPY_KEY(node, key_val, sz, key) \

#define SPLAY_INSERT_ADD(create_macro, key_macro, cmp_macro, tree, key_val, sz, node, key, left, right, parent) \
do { \
    void *_sp_key_val = (void*)key_val; \
    SPDECLTYPE(tree) _sp_z; \
    SPDECLTYPE(tree) _sp_p; \
    IF_NO_DECLTYPE(SPDECLTYPE(list) _sp_tree_sv;) \
    SPLAY_SVTREE(tree); \
    SPLAY_ASSIGN(_sp_z, tree); \
    _sp_p = NULL; \
    int _sp_cmp = 0; \
    int _sp_found = 0; \
    while (_sp_z) { \
        _sp_p = _sp_z; \
        _sp_cmp = cmp_macro(key_macro(tree, _sp_z, key), _sp_key_val, sz); \
	if (_sp_cmp < 0) \
            SPLAY_ASSIGN(_sp_z, SPLAY_RIGHT(tree, _sp_z, right)); \
        else if (_sp_cmp > 0)  \
            SPLAY_ASSIGN(_sp_z, SPLAY_LEFT(tree, _sp_z, left)); \
        else { \
            _sp_found = 1; \
            break; \
        } \
    } \
    if (!_sp_found) { \
        create_macro(key_macro, tree, _sp_z,  _sp_key_val, sz, node, key, left, right, parent); \
        SPLAY_PARENT_ASSIGN(tree, _sp_z, parent, _sp_p); \
        if (!_sp_p) \
            SPLAY_UPTREE(tree, _sp_z); \
        else if (_sp_cmp < 0) \
            SPLAY_RIGHT_ASSIGN(tree, _sp_p, right, _sp_z); \
        else \
            SPLAY_LEFT_ASSIGN(tree, _sp_p, left, _sp_z); \
        SPLAY_INITW(tree, _sp_z); \
    } \
    SPLAY_SPLAY2(tree, _sp_z, left, right, parent); \
    SPLAY_RSTREE(tree); \
} while (0)

#define SPLAY_SPLAY(tree, node) \
    SPLAY_SPLAY2(tree, node, left, right, parent)

#define SPLAY_SPLAY2(tree, node, left, right, parent) \
do { \
    SPDECLTYPE(tree) _spp_p = NULL; \
    SPDECLTYPE(tree) _spp_g = NULL; \
    while ((SPLAY_ASSIGN(_spp_p, SPLAY_PARENT(tree, node, parent)))) { \
        SPLAY_ASSIGN(_spp_g, SPLAY_PARENT(tree, _spp_p, parent)); \
        if (!_spp_g) { \
            if (SPLAY_LEFT(tree, _spp_p, left)) \
                SPLAY_RIGHT_ROTATE2(tree, _spp_p, left, right, parent); \
            else \
                SPLAY_LEFT_ROTATE2(tree, _spp_p, left, right, parent); \
        } else if (SPLAY_EQL(SPLAY_LEFT(tree, _spp_p, left), node) && \
                   SPLAY_EQL(SPLAY_LEFT(tree, _spp_g, left), _spp_p)) { \
            SPLAY_RIGHT_ROTATE2(tree, _spp_g, left, right, parent); \
            SPLAY_RIGHT_ROTATE2(tree, _spp_p, left, right, parent); \
        } else if (SPLAY_EQL(SPLAY_RIGHT(tree, _spp_p, right), node) && \
                   SPLAY_EQL(SPLAY_RIGHT(tree, _spp_g, right), _spp_p)) { \
            SPLAY_LEFT_ROTATE2(tree, _spp_g, left, right, parent); \
            SPLAY_LEFT_ROTATE2(tree, _spp_p, left, right, parent); \
        } else if (SPLAY_EQL(SPLAY_LEFT(tree, _spp_p, left), node) && \
                   SPLAY_EQL(SPLAY_RIGHT(tree, _spp_g, right), _spp_p)) { \
            SPLAY_RIGHT_ROTATE2(tree, _spp_p, left, right, parent); \
            SPLAY_LEFT_ROTATE2(tree, _spp_g, left, right, parent); \
        } else { \
            SPLAY_LEFT_ROTATE2(tree, _spp_p, left, right, parent); \
            SPLAY_RIGHT_ROTATE2(tree, _spp_g, left, right, parent); \
        } \
    } \
} while(0)

#define SPLAY_LEFT_ROTATE(tree, node) \
    SPLAY_LEFT_ROTATE2(tree, node, left, right, parent)

#define SPLAY_LEFT_ROTATE2(tree, node, left, right, parent) \
do { \
    SPDECLTYPE(tree) _splr_r = NULL; \
    SPDECLTYPE(tree) _splr_p = NULL; \
    SPDECLTYPE(tree) _splr_tmp = NULL; \
    SPLAY_ASSIGN(_splr_r, SPLAY_RIGHT(tree, node, right)); \
    SPLAY_ASSIGN(_splr_p, SPLAY_PARENT(tree, node, parent)); \
    SPLAY_ASSIGN(_splr_tmp, SPLAY_LEFT(tree, _splr_r, left)); \
    SPLAY_RIGHT_ASSIGN(tree, node, right, _splr_tmp); \
    if (_splr_tmp) \
       SPLAY_PARENT_ASSIGN(tree, _splr_tmp, parent, node); \
    SPLAY_PARENT_ASSIGN(tree, _splr_r, parent, _splr_p); \
    if (!_splr_p) \
        SPLAY_UPTREE(tree, _splr_r); \
    else if (SPLAY_EQL(SPLAY_LEFT(tree, _splr_p, left), node)) \
        SPLAY_LEFT_ASSIGN(tree, _splr_p, left, _splr_r); \
    else \
        SPLAY_RIGHT_ASSIGN(tree, _splr_p, right, _splr_r); \
    SPLAY_LEFT_ASSIGN(tree, _splr_r, left, node); \
    SPLAY_PARENT_ASSIGN(tree, node, parent, _splr_r); \
    SPLAY_PROCESSW(tree, node); \
    SPLAY_PROCESSW(tree, _splr_r); \
} while (0)

#define SPLAY_RIGHT_ROTATE(tree, node) \
    SPLAY_RIGHT_ROTATE2(tree, node, left, right, parent)

#define SPLAY_RIGHT_ROTATE2(tree, node, left, right, parent) \
do { \
    SPDECLTYPE(tree) _sprr_l = NULL; \
    SPDECLTYPE(tree) _sprr_p = NULL; \
    SPDECLTYPE(tree) _sprr_tmp = NULL; \
    SPLAY_ASSIGN(_sprr_l, SPLAY_LEFT(tree, node, left)); \
    SPLAY_ASSIGN(_sprr_p, SPLAY_PARENT(tree, node, parent)); \
    SPLAY_ASSIGN(_sprr_tmp, SPLAY_RIGHT(tree, _sprr_l, right)); \
    SPLAY_LEFT_ASSIGN(tree, node, left, _sprr_tmp); \
    if (_sprr_tmp) \
        SPLAY_PARENT_ASSIGN(tree, _sprr_tmp, parent, node); \
    SPLAY_PARENT_ASSIGN(tree, _sprr_l, parent, _sprr_p); \
    if (!_sprr_p) \
        SPLAY_UPTREE(tree, _sprr_l); \
    else if (SPLAY_EQL(SPLAY_LEFT(tree, _sprr_p, left), node)) \
        SPLAY_LEFT_ASSIGN(tree, _sprr_p, left, _sprr_l); \
    else \
        SPLAY_RIGHT_ASSIGN(tree, _sprr_p, right, _sprr_l); \
    SPLAY_RIGHT_ASSIGN(tree, _sprr_l, right, node); \
    SPLAY_PARENT_ASSIGN(tree, node, parent, _sprr_l); \
    SPLAY_PROCESSW(tree, node); \
    SPLAY_PROCESSW(tree, _sprr_l); \
} while (0)

#define SPLAY_FIND_KEYSTR(tree, key_val, out) \
    SPLAY_FIND_KEYSTR2(tree, key_val, (utsplay_strlen(key_val)+1), key, left, right, parent, out)

#define SPLAY_FIND_KEYSTR2(tree, key_val, sz, key, left, right, parent, out) \
    SPLAY_FIND3(SPLAY_KEYPTR, SPLAY_STRCMP, tree, key_val, sz, key, left, right, parent, out)

#define SPLAY_FIND_KEYPTR(tree, key_val, sz, out) \
    SPLAY_FIND_KEYPTR2(tree, key_val, sz, key, left, right, parent, out)

#define SPLAY_FIND_KEYPTR2(tree, key_val, sz, key, left, right, parent, out) \
    SPLAY_FIND3(SPLAY_KEYPTR, SPLAY_KEYCMP, tree, key_val, sz, key, left, right, parent, out)

#define SPLAY_FIND(tree, key_val, sz, out) \
    SPLAY_FIND2(tree, key_val, sz, key, left, right, parent, out)

#define SPLAY_FIND2(tree, key_val, sz, key, left, right, parent, out) \
    SPLAY_FIND3(SPLAY_KEY, SPLAY_KEYCMP, tree, key_val, sz, key, left, right, parent, out)

#define SPLAY_FIND3(key_macro, cmp_macro, tree, key_val, sz, key, left, right, parent, out) \
do { \
    IF_NO_DECLTYPE(SPDECLTYPE(list) _sp_tree_sv;) \
    SPLAY_SVTREE(tree); \
    SPDECLTYPE(tree) _spf_z; \
    SPLAY_ASSIGN(_spf_z, tree); \
    out = NULL; \
    int _spf_cmp = 0; \
    while (_spf_z) { \
        _spf_cmp = cmp_macro(key_macro(tree, _spf_z, key), key_val, sz); \
        if (_spf_cmp < 0) { \
            SPLAY_ASSIGN(_spf_z, SPLAY_RIGHT(tree, _spf_z, right)); \
        } else if (_spf_cmp > 0) { \
            SPLAY_ASSIGN(_spf_z, SPLAY_LEFT(tree, _spf_z, left)); \
        } else {\
            SPLAY_ASSIGN2(out, _spf_z); \
            SPLAY_SPLAY2(tree, out, left, right, parent); \
            break; \
        } \
    } \
    SPLAY_RSTREE(tree); \
} while(0)

#define SPLAY_REMOVE_KEYSTR(tree, key_val, node) \
    SPLAY_REMOVE_KEYSTR2(tree, key_val, (utsplay_strlen(key_val)+1), node, key, left, right, parent)

#define SPLAY_REMOVE_KEYSTR2(tree, key_val, sz, node, key, left, right, parent) \
    SPLAY_ERASE_REMOVE(SPLAY_REMOVE_DELETE, SPLAY_KEYPTR, SPLAY_STRCMP, tree, key_val, sz, node, key, left, right, parent)

#define SPLAY_REMOVE_KEYPTR(tree, key_val, sz, node) \
    SPLAY_REMOVE_KEYPTR2(tree, key_val, sz, node, key, left, right, parent)

#define SPLAY_REMOVE_KEYPTR2(tree, key_val, sz, node, key, left, right, parent) \
    SPLAY_ERASE_REMOVE(SPLAY_REMOVE_DELETE, SPLAY_KEYPTR, SPLAY_KEYCMP, tree, key_val, sz, node, key, left, right, parent)

#define SPLAY_REMOVE(tree, key_val, sz, node) \
    SPLAY_REMOVE2(tree, key_val, sz, node, key, left, right, parent)

#define SPLAY_REMOVE2(tree, key_val, sz, node, key, left, right, parent) \
    SPLAY_ERASE_REMOVE(SPLAY_REMOVE_DELETE, SPLAY_KEY, SPLAY_KEYCMP, tree, key_val, sz, node, key, left, right, parent)

#define SPLAY_REMOVE_DELETE(tree, n, node, left, right, parent) \
do { \
    SPLAY_PARENT_ASSIGN(tree, n, parent, NULL); \
    SPLAY_LEFT_ASSIGN(tree, n, left, NULL); \
    SPLAY_RIGHT_ASSIGN(tree, n, right, NULL); \
    SPLAY_ASSIGN2(node, n); \
} while (0)

#define SPLAY_ERASE_KEYSTR(tree, key_val) \
    SPLAY_ERASE_KEYSTR2(tree, key_val, (utsplay_strlen(key_val)+1), key, left, right, parent)

#define SPLAY_ERASE_KEYSTR2(tree, key_val, sz, key, left, right, parent) \
    SPLAY_ERASE_REMOVE(SPLAY_ERASE_DELETE, SPLAY_KEYPTR, SPLAY_STRCMP, tree, key_val, sz, NULL, key, left, right, parent)

#define SPLAY_ERASE_KEYPTR(tree, key_val, sz) \
    SPLAY_ERASE_KEYPTR2(tree, key_val, sz, key, left, right, parent)

#define SPLAY_ERASE_KEYPTR2(tree, key_val, sz, key, left, right, parent) \
    SPLAY_ERASE_REMOVE(SPLAY_ERASE_DELETE, SPLAY_KEYPTR, SPLAY_KEYCMP, tree, key_val, sz, NULL, key, left, right, parent)

#define SPLAY_ERASE(tree, key_val, sz) \
    SPLAY_ERASE2(tree, key_val, sz, key, left, right, parent)

#define SPLAY_ERASE2(tree, key_val, sz, key, left, right, parent) \
    SPLAY_ERASE_REMOVE(SPLAY_ERASE_DELETE, SPLAY_KEY, SPLAY_KEYCMP, tree, key_val, sz, NULL, key, left, right, parent)

#define SPLAY_ERASE_DELETE(tree, n, node, left, right, parent) \
do { \
    utsplay_free(_spe_z, sizeof(*tree)); \
} while (0)

#define SPLAY_ERASE_REMOVE(delete_macro, key_macro, cmp_macro, tree, key_val, sz, node, key, left, right, parent) \
do { \
    SPDECLTYPE(tree) _spe_z; \
    SPDECLTYPE(tree) _spe_p; \
    SPDECLTYPE(tree) _spe_tmp; \
    SPLAY_FIND3(key_macro, cmp_macro, tree, key_val, sz, key, left, right, parent, _spe_z); \
    IF_NO_DECLTYPE(SPDECLTYPE(list) _sp_tree_sv;) \
    SPLAY_SVTREE(tree); \
    if (_spe_z) { \
        if (!SPLAY_LEFT(tree, _spe_z, left)) { \
            SPLAY_ASSIGN(_spe_tmp, SPLAY_RIGHT(tree, _spe_z, right)); \
            SPLAY_REPLACE2(tree, _spe_z, _spe_tmp, left, right, parent); \
        } else if (!SPLAY_RIGHT(tree, _spe_z, right)) { \
            SPLAY_ASSIGN(_spe_tmp, SPLAY_LEFT(tree, _spe_z, left)); \
            SPLAY_REPLACE2(tree, _spe_z, _spe_tmp, left, right, parent); \
        } else  { \
            SPDECLTYPE(tree) _spe_l; \
            SPLAY_ASSIGN(_spe_l, SPLAY_RIGHT(tree, _spe_z, right)); \
            while (SPLAY_LEFT(tree, _spe_l, left)) \
                SPLAY_ASSIGN(_spe_l, SPLAY_LEFT(tree, _spe_l, left)); \
            SPLAY_ASSIGN(_spe_p, SPLAY_PARENT(tree, _spe_l, parent)); \
            if (_spe_p != _spe_z) { \
                SPLAY_ASSIGN(_spe_tmp, SPLAY_RIGHT(tree, _spe_l, right)); \
                SPLAY_REPLACE2(tree, _spe_l, _spe_tmp, left, right, parent); \
                SPLAY_ASSIGN(_spe_tmp, SPLAY_RIGHT(tree, _spe_z, right)); \
                SPLAY_RIGHT_ASSIGN(tree, _spe_l, right, _spe_tmp); \
                SPLAY_ASSIGN(_spe_tmp, SPLAY_RIGHT(tree, _spe_l, right)); \
                SPLAY_PARENT_ASSIGN(tree, _spe_tmp, parent, _spe_l); \
                SPLAY_PROCESSW(tree, _spe_p); \
            	while((SPLAY_ASSIGN(_spe_p, SPLAY_PARENT(tree, _spe_p, parent))) != _spe_l) \
                	SPLAY_PROCESSW(tree, _spe_p); \
            } \
            SPLAY_REPLACE2(tree, _spe_z, _spe_l, left, right, parent); \
            SPLAY_ASSIGN(_spe_tmp, SPLAY_LEFT(tree, _spe_z, left)); \
            SPLAY_LEFT_ASSIGN(tree, _spe_l, left, _spe_tmp); \
            SPLAY_ASSIGN(_spe_tmp, SPLAY_LEFT(tree, _spe_l, left)); \
            SPLAY_PARENT_ASSIGN(tree, _spe_tmp, parent, _spe_l); \
            SPLAY_PROCESSW(tree, _spe_l); \
        } \
        delete_macro(tree, _spe_z, node, left, right, parent); \
    } \
    SPLAY_RSTREE(tree); \
} while (0)

#define SPLAY_REPLACE(tree, u, v) \
    SPLAY_REPLACE2(tree, u, v, left, right, parent)

#define SPLAY_REPLACE2(tree, u, v, left, right, parent) \
do { \
    if (tree) { \
        SPDECLTYPE(tree) _spr_p; \
        SPLAY_ASSIGN(_spr_p, SPLAY_PARENT(tree, u, parent)); \
        if (!_spr_p) \
            SPLAY_UPTREE(tree, v); \
        else if (SPLAY_EQL(SPLAY_LEFT(tree, _spr_p, left), u)) {\
            SPLAY_LEFT_ASSIGN(tree, _spr_p, left, v); \
            SPLAY_PROCESSW(tree, _spr_p); \
        } else { \
            SPLAY_RIGHT_ASSIGN(tree, _spr_p, right, v); \
            SPLAY_PROCESSW(tree, _spr_p); \
        } \
        if (v) \
            SPLAY_PARENT_ASSIGN(tree, v, parent, _spr_p); \
    } \
} while(0)

#define SPLAY_CLEAR(tree) \
    SPLAY_CLEAR2(tree, left, right, parent)

#define SPLAY_CLEAR2(tree, left, right, parent) \
do { \
    SPDECLTYPE(tree) _spc_z; \
    SPDECLTYPE(tree) _spc_p; \
    SPLAY_ASSIGN(_spc_z, tree); \
    while (_spc_z) { \
        if (SPLAY_LEFT(tree, _spc_z, left)) \
            SPLAY_ASSIGN(_spc_z, SPLAY_LEFT(tree, _spc_z, left)); \
        else if (SPLAY_RIGHT(tree, _spc_z, right)) \
            SPLAY_ASSIGN(_spc_z, SPLAY_RIGHT(tree, _spc_z, right)); \
        else { \
            SPLAY_ASSIGN(_spc_p, SPLAY_PARENT(tree, _spc_z, parent)); \
            if (_spc_p) { \
                if (SPLAY_EQL(SPLAY_LEFT(tree, _spc_p, left), _spc_z)) \
                    SPLAY_LEFT_ASSIGN(tree, _spc_p, left, NULL); \
                else \
                    SPLAY_RIGHT_ASSIGN(tree, _spc_p, right, NULL); \
            } \
            SPLAY_DEINITW(tree, _spc_z); \
            utsplay_free(_spc_z, sizeof(*tree)); \
            _spc_z = _spc_p; \
        } \
    } \
    tree = NULL; \
} while(0)

static inline
void * _splay_buffalloc(void *pe) {
    if (!pe)
        return NULL;
    void **mem = (void **)utsplay_malloc(10 * sizeof(void *));
    if (!mem)
        utsplay_oom();
    *mem = pe;
    return (void *)mem;
}

static inline
void * _splay_buffalloc2(void *pe) {
    if (!pe)
        return NULL;
    void **mem = (void **)utsplay_malloc(10 * sizeof(void *));
    if (!mem)
        utsplay_oom();
    return (void *)mem;
}


struct _sp_stack_s {
    void **arr;
    void *_sp_tree_sv;
    size_t c;
    size_t sz;
};

#define splay_init_stack(tree) { \
    (void**)_splay_buffalloc((void *)tree), \
    (void *)tree, (size_t)(tree ? 1 : 0), (size_t)(tree ? 10 : 0) }

#define splay_deinit_stack(stack) ( \
    utsplay_free(stack.arr, stack.sz * sizeof(void*)) )

#define splay_maybe_realloc_stack(tree, stack) ( \
    stack.c == stack.sz ? ( \
        stack.arr = (void**)utsplay_realloc(stack.arr, stack.sz * 2 * sizeof(void*)), \
        (stack.arr ? 0 : (utsplay_oom(), 0)), \
        stack.sz = stack.sz * 2) : 1 )

#define splay_push_back_field(tree, stack, el, field) ( \
  SPLAY_FIELD(tree, el, field ) ? splay_push_back(stack, SPLAY_FIELD(tree, el, field )) : 0 )

#define splay_push_back(stack, el) (\
    splay_maybe_realloc_stack(tree,stack), \
    SPLAY_ASSIGN(stack.arr[stack.c++], el) )

#define splay_pop_back(stack) stack.arr[--(stack.c)]

#define splay_peek_back(stack) stack.arr[stack.c - 1]

#define SPLAY_FOREACH_DFSPRE(tree, el) \
    SPLAY_FOREACH_DFSPRE2(tree, el, left, right, parent)

#define SPLAY_FOREACH_DFSPRE2(tree, el, left, right, parent) \
for ( struct _sp_stack_s _sp_stack = splay_init_stack(tree); \
     (_sp_stack.c ? 1 : (splay_deinit_stack(_sp_stack), SPLAY_ASSIGN2(tree, (SPDECLTYPE(tree))(_sp_stack._sp_tree_sv)), 0)) && \
         ( SPLAY_ASSIGN2(el, (SPDECLTYPE(tree))splay_pop_back(_sp_stack)), 1 ); \
     splay_push_back_field(tree, _sp_stack, el, right), \
     splay_push_back_field(tree, _sp_stack, el, left))

struct _sp_stack2_s {
    void **arr;
    void *_sp_tree_sv;
    size_t c;
    size_t sz;
    int i;
};

#define splay_init_stack2(tree, el) { \
    (el = tree, (void**)_splay_buffalloc2((void *)tree)), \
    (void *)tree, 0, (size_t)(tree ? 10 : 0), 0 }

#define SPLAY_FOREACH_DFSIN(tree, el) \
    SPLAY_FOREACH_DFSIN2(tree, el, left, right, parent)

#define SPLAY_FOREACH_DFSIN2(tree, el, left, right, parent) \
for ( struct _sp_stack2_s _sp2_stack = splay_init_stack2(tree, el); \
    ((_sp2_stack.c || el) ? 1 : (splay_deinit_stack(_sp2_stack), SPLAY_ASSIGN2(tree, (SPDECLTYPE(tree))(_sp2_stack._sp_tree_sv)), 0)); ) if (el) { \
        splay_push_back(_sp2_stack, el); SPLAY_ASSIGN2(el, SPLAY_LEFT(tree, el, left)); } \
    else for (_sp2_stack.i = 0; _sp2_stack.i < 1 && (SPLAY_ASSIGN2(el, (SPDECLTYPE(tree))splay_pop_back(_sp2_stack)), 1); _sp2_stack.i++, SPLAY_ASSIGN2(el, SPLAY_RIGHT(tree, el, right)))

struct _sp_stack3_s {
    void **arr;
    size_t c;
    size_t sz;
    void *last;
    int i;
};

#define splay_init_stack3(tree, el) { \
    (el = tree, (void**)_splay_buffalloc2((void *)tree)), \
    0, (size_t)(tree ? 10 : 0), NULL, 0 }

#define SPLAY_FOREACH_DFSPOST(tree, el) \
    SPLAY_FOREACH_DFSPOST2(tree, el, left, right, parent)

#define SPLAY_FOREACH_DFSPOST2(tree, el, left, right, parent) \
for ( struct _sp_stack3_s _sp3_stack = splay_init_stack3(tree, el); \
    ((_sp3_stack.c || el) ? 1 : (splay_deinit_stack(_sp3_stack), 0)); ) if (el) { \
        splay_push_back(_sp3_stack, el); SPLAY_ASSIGN2(el, SPLAY_LEFT(tree, el, left));} \
    else if ((SPLAY_ASSIGN2(el, (SPDECLTYPE(tree))splay_peek_back(_sp3_stack)), SPLAY_RIGHT(tree, el, right)) && (char*)SPLAY_RIGHT(tree, el, right) != (char*)_sp3_stack.last) { SPLAY_ASSIGN2(el, SPLAY_RIGHT(tree, el, right)); } \
         else for (_sp3_stack.i = 0; _sp3_stack.i < 1; _sp3_stack.i++, _sp3_stack.last = (SPDECLTYPE(tree))splay_pop_back(_sp3_stack), el = NULL)

#ifdef NO_DECLTYPE
#define SPLAY_FIELD_ASSIGN(tree, elt, name, val) (*(char **)(&tree) = (char*)elt, *(char**)&((tree)->name) = (char *)val)
#define SPLAY_FIELD(tree, elt, name) (*(char **)(&tree) = (char*)elt, (tree)->name)
#define SPLAY_KEY(tree, elt, key) (*(char **)(&tree) = (char*)elt, (&((tree)->key)))
#else
#define SPLAY_FIELD_ASSIGN(tree, elt, name, val) ((elt)->name = val)
#define SPLAY_FIELD(tree, elt, name) ((elt)->name)
#define SPLAY_KEY(tree, elt, key) (&((elt)->key))
#endif
#define SPLAY_KEYPTR(tree, elt, key) \
    SPLAY_FIELD(tree, elt, key)
#define SPLAY_LEFT(tree, elt, left) \
    SPLAY_FIELD(tree, elt, left)
#define SPLAY_RIGHT(tree, elt, right) \
    SPLAY_FIELD(tree, elt, right)
#define SPLAY_PARENT(tree, elt, parent) \
    SPLAY_FIELD(tree, elt, parent)
#define SPLAY_LEFT_ASSIGN(tree, elt, left, v) \
    SPLAY_FIELD_ASSIGN(tree, elt, left, v)
#define SPLAY_RIGHT_ASSIGN(tree, elt, right, v) \
    SPLAY_FIELD_ASSIGN(tree, elt, right, v)
#define SPLAY_PARENT_ASSIGN(tree, elt, parent, v) \
    SPLAY_FIELD_ASSIGN(tree, elt, parent, v)

#endif /*UTSPLAY_H*/
