/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to TO_YEAR PrismTech
 *   Limited, its affiliated companies and licensors. All rights reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */
#ifndef DDSI_SER_H
#define DDSI_SER_H

#include "os_abstract.h"
#include "os_mutex.h"
#include "q_plist.h" /* for nn_prismtech_writer_info */
#include "ut_avl.h"

#ifdef OSPL_BUILD_DDSI2
#define OS_API OS_API_EXPORT
#else
#define OS_API OS_API_IMPORT
#endif


#ifndef PLATFORM_IS_LITTLE_ENDIAN
#ifdef PA_BIG_ENDIAN
#define PLATFORM_IS_LITTLE_ENDIAN 0
#endif
#ifdef PA_LITTLE_ENDIAN
#ifdef PLATFORM_IS_LITTLE_ENDIAN
#error PA_BIG_ENDIAN and PA_LITTLE_ENDIAN both defined?
#endif
#define PLATFORM_IS_LITTLE_ENDIAN 1
#endif /* PA_LITTLE_ENDIAN */
#ifndef PLATFORM_IS_LITTLE_ENDIAN
#error PLATFORM_IS_LITTLE_ENDIAN undefined
#endif
#endif /* PLATFORM_IS_LITTLE_ENDIAN */

#if PLATFORM_IS_LITTLE_ENDIAN
#define CDR_BE 0x0000
#define CDR_LE 0x0100
#else
#define CDR_BE 0x0000
#define CDR_LE 0x0001
#endif

typedef struct serstatepool * serstatepool_t;
typedef struct serstate * serstate_t;
typedef struct serdata * serdata_t;
typedef struct sertopic * sertopic_t;

struct CDRHeader
{
  unsigned short identifier;
  unsigned short options;
};

struct serdata_msginfo
{
  unsigned statusinfo;
  nn_wctime_t timestamp;
#ifndef NDEBUG
  unsigned have_wrinfo: 2;
#else
  unsigned have_wrinfo: 1;
#endif
  struct nn_prismtech_writer_info wrinfo;
};

enum serstate_kind {
  STK_EMPTY,
  STK_KEY,
  STK_DATA
};

struct serstate
{
  serdata_t data;
  nn_mtime_t twrite; /* write time, not source timestamp, set post-throttling */
  pa_uint32_t refcount;
  size_t pos;
  size_t size;
  int keyidx; /* current index in topic.keys */
  const struct sertopic * topic;
  enum serstate_kind kind;
  serstatepool_t pool;
  struct serstate *next; /* in pool->freelist */
};

struct serstatepool
{
#if USE_ATOMIC_LIFO
  os_atomic_lifo_t freelist;
  pa_uint32_t approx_nfree;
#else
  os_mutex lock;
  int nalloced;
  int nfree;
  serstate_t freelist;
#endif
};


struct serdata_base
{
  serstate_t st;        /* back pointer to (opaque) serstate so RTPS impl only needs serdata */
  struct serdata_msginfo msginfo;
  int hash_valid;       /* whether hash is valid or must be computed from key/data */
  os_uint32 hash;       /* cached serdata hash, valid only if hash_valid != 0 */
  char key[32];         /* copies + 32-bit offsets (from &key[0] into data) to CDR strings */
  unsigned isstringref; /* (isstringref & (1 << k)) iff key[k] is first of an offset */
};

struct serdata
{
  struct serdata_base v;
  /* padding to ensure CDRHeader is at an offset 4 mod 8 from the
     start of the memory, so that data is 8-byte aligned provided
     serdata is 8-byte aligned */
  char pad[8 - ((sizeof (struct serdata_base) + 4) % 8)];
  struct CDRHeader hdr;
  char data[1];
};


typedef enum dds_keytype
{
  DDS_KEY_ONEBYTE,
  DDS_KEY_TWOBYTES,
  DDS_KEY_FOURBYTES,
  DDS_KEY_EIGHTBYTES,
  DDS_KEY_STRINGREF,
  DDS_KEY_STRINGINLINE
}
dds_keytype_t;

typedef struct dds_key_descriptor
{
  unsigned long long ord;       /* serialization order (lower values of ord go earlier) */
  unsigned off;                 /* offset from start of (input) sample */
  unsigned m_seroff;            /* offset in serdata_base.key */
  dds_keytype_t m_keytype;      /* type class */
  unsigned short align;         /* required alignment */
  unsigned short specord_idx;   /* index in serialization order where current index in specification order can be found (i.e. keys[keys[i].specord_idx] <=> key i in specification order) */
  void * type;                  /* full type for key (only for (de)serializing keys/keyhashes) */
}
dds_key_descriptor_t;


struct v_topic_s;

struct sertopic
{
  ut_avlNode_t avlnode;
  char * name_typename;
  char * name;
  char * typename;
  void * type;
  unsigned nkeys;

  struct v_topic_s *ospl_topic;
  struct sd_cdrInfo *ci;
  unsigned keysersize;
  struct dds_key_descriptor keys[1];

  /*
    Array of keys, represented as offset in the OpenSplice internal
    format data blob. Keys must be stored in the order visited by
    serializer (so that the serializer can simply compare the current
    offset with the next key offset). Also: keys[nkeys].off =def=
    ~0u, which won't equal any real offset so that there is no need
    to test for the end of the array.

    Offsets work 'cos only primitive types, enums and strings are
    accepted as keys. So there is no ambiguity if a key happens to
    be inside a nested struct.
  */
};

serstatepool_t ddsi_serstatepool_new (void);
void ddsi_serstatepool_free (serstatepool_t pool);

serdata_t ddsi_serdata_ref (serdata_t serdata);
OS_API void ddsi_serdata_unref (serdata_t serdata);
int ddsi_serdata_refcount_is_1 (serdata_t serdata);
nn_mtime_t ddsi_serdata_twrite (const struct serdata * serdata);
void ddsi_serdata_set_twrite (struct serdata * serdata, nn_mtime_t twrite);
os_uint32 ddsi_serdata_size (const struct serdata * serdata);
int ddsi_serdata_is_key (const struct serdata * serdata);
int ddsi_serdata_is_empty (const struct serdata * serdata);

OS_API void ddsi_serstate_append_blob (serstate_t st, size_t align, size_t sz, const void *data);
OS_API void ddsi_serstate_set_msginfo
(
  serstate_t st, unsigned statusinfo, nn_wctime_t timestamp,
  const struct nn_prismtech_writer_info *wri
);
OS_API serstate_t ddsi_serstate_new (serstatepool_t pool, const struct sertopic * topic);
OS_API serdata_t ddsi_serstate_fix (serstate_t st);
nn_mtime_t ddsi_serstate_twrite (const struct serstate *serstate);
void ddsi_serstate_set_twrite (struct serstate *serstate, nn_mtime_t twrite);
void ddsi_serstate_release (serstate_t st);
void * ddsi_serstate_append (serstate_t st, size_t n);
void * ddsi_serstate_append_align (serstate_t st, size_t a);
void * ddsi_serstate_append_aligned (serstate_t st, size_t n, size_t a);
#undef OS_API
#endif

/* SHA1 not available (unoffical build.) */
