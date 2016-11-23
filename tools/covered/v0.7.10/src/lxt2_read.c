/*
 * Copyright (c) 2003-2004 Tony Bybell.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>

#include "defines.h"
#include "lxt2_read.h"
#include "util.h"


extern char user_msg[USER_MSG_LENGTH];
extern bool debug_mode;


/****************************************************************************/

#ifdef _WAVE_BE32

/*
 * reconstruct 8/16/24/32 bits out of the lxt's representation
 * of a big-endian integer.  this is for 32-bit PPC so no byte
 * swizzling needs to be done at all.  for 24-bit ints, we have no danger of
 * running off the end of memory provided we do the "-1" trick
 * since we'll never read a 24-bit int at the very start of a file which
 * means that we'll have a 32-bit word that we can read.
 */
   
#define lxt2_rd_get_byte(mm,offset)    ((unsigned int)(*((unsigned char *)(mm)+(offset))))
#define lxt2_rd_get_16(mm,offset)      ((unsigned int)(*((unsigned short *)(((unsigned char *)(mm))+(offset)))))
#define lxt2_rd_get_32(mm,offset)      (*(unsigned int *)(((unsigned char *)(mm))+(offset)))
#define lxt2_rd_get_24(mm,offset)      ((lxt2_rd_get_32((mm),(offset)-1)<<8)>>8)
#define lxt2_rd_get_64(mm,offset)      ((((lxtint64_t)lxt2_rd_get_32((mm),(offset)))<<32)|((lxtint64_t)lxt2_rd_get_32((mm),(offset)+4)))
 
#else

/*
 * reconstruct 8/16/24/32 bits out of the lxt's representation
 * of a big-endian integer.  x86 specific version...
 */

#if defined(__i386__) || defined(__x86_64__)

#define lxt2_rd_get_byte(mm,offset)		((unsigned int)(*((unsigned char *)(mm)+(offset))))

_LXT2_RD_INLINE static unsigned int lxt2_rd_get_16(void *mm, int offset)
{
  unsigned short x = *((unsigned short *)((unsigned char *)mm+offset));

#if defined(__i386__)
  __asm("xchgb %b0,%h0" : "=q" (x) : "0" (x));
#else
  __asm("xchgb %b0,%h0" : "=Q" (x) : "0" (x));
#endif

    return (unsigned int) x;
}

_LXT2_RD_INLINE static unsigned int lxt2_rd_get_32(void *mm, int offset)		/* note that bswap is really 486+ */
{
  unsigned int x = *((unsigned int *)((unsigned char *)mm+offset));

  __asm("bswap   %0": "=r" (x) : "0" (x));

  return x;
}

#define lxt2_rd_get_24(mm,offset)  	   ((lxt2_rd_get_32((mm),(offset)-1)<<8)>>8)
#define lxt2_rd_get_64(mm,offset)      ((((lxtint64_t)lxt2_rd_get_32((mm),(offset)))<<32)|((lxtint64_t)lxt2_rd_get_32((mm),(offset)+4)))

#else

/*
 * reconstruct 8/16/24/32 bits out of the lxt's representation
 * of a big-endian integer.  this should work on all architectures.
 */
#define lxt2_rd_get_byte(mm,offset) 	((unsigned int)(*((unsigned char *)(mm)+(offset))))

static unsigned int lxt2_rd_get_16( void *mm, int offset ) {

  unsigned char* nn =  (unsigned char*)mm + offset;
  unsigned int   m1 =* ((unsigned char*)(nn++));
  unsigned int   m2 =* ((unsigned char*)nn);

  return( (m1 << 8) | m2 );

}

static unsigned int lxt2_rd_get_24( void *mm, int offset ) {

  unsigned char* nn =  (unsigned char*)mm + offset;
  unsigned int   m1 =* ((unsigned char*)(nn++));
  unsigned int   m2 =* ((unsigned char*)(nn++));
  unsigned int   m3 =* ((unsigned char*)nn);

  return( (m1 << 16) | (m2 << 8) | m3 );

}

static unsigned int lxt2_rd_get_32( void *mm, int offset ) {

  unsigned char* nn =  (unsigned char*)mm+offset;
  unsigned int   m1 =* ((unsigned char*)(nn++));
  unsigned int   m2 =* ((unsigned char*)(nn++));
  unsigned int   m3 =* ((unsigned char*)(nn++));
  unsigned int   m4 =* ((unsigned char*)nn);

  return( (m1 << 24) | (m2 << 16) | (m3 << 8) | m4 );

}

static lxtint64_t lxt2_rd_get_64( void *mm, int offset ) {

  return( (((lxtint64_t)lxt2_rd_get_32( mm, offset )) << 32) | ((lxtint64_t)lxt2_rd_get_32( mm, (offset + 4))) );

}

#endif
#endif

/****************************************************************************/

/*
 * fast SWAR ones count for 32 and 64 bits
 */
#if LXT2_RD_GRANULE_SIZE > 32

static _LXT2_RD_INLINE granmsk_t lxt2_rd_ones_cnt( granmsk_t x ) {

  x -= ((x >> 1) & LXT2_RD_ULLDESC(0x5555555555555555));
  x = (((x >> 2) & LXT2_RD_ULLDESC(0x3333333333333333)) + (x & LXT2_RD_ULLDESC(0x3333333333333333)));
  x = (((x >> 4) + x) & LXT2_RD_ULLDESC(0x0f0f0f0f0f0f0f0f));

  return( (x * LXT2_RD_ULLDESC(0x0101010101010101)) >> 56 );

}

#else

static _LXT2_RD_INLINE granmsk_t lxt2_rd_ones_cnt( granmsk_t x ) {

  x -= ((x >> 1) & 0x55555555);
  x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
  x = (((x >> 4) + x) & 0x0f0f0f0f);

  return( (x * 0x01010101) >> 24 );

}

#endif


/*
 * total zero count to the right of the first rightmost one bit
 * encountered.  its intended use is to 
 * "return the bitposition of the least significant 1 in a granmsk_t"
 * (use x &= ~(x&-x) to clear out that bit quickly)
 */
static _LXT2_RD_INLINE granmsk_t lxt2_rd_tzc( granmsk_t x ) {
  return( lxt2_rd_ones_cnt( (x & -x) - LXT2_RD_GRAN_1VAL ) );
}

/****************************************************************************/

/*
 * i2c and c2i utility functions
 */
static char* lxt2_rd_expand_integer_to_bits(
  int          len,
  unsigned int value
) { PROFILE(LXT2_RD_EXPAND_INTEGER_TO_BITS);

  static char s[33];
  char*       p    = s;
  int         i;                          
  int         len2 = len - 1;

  for( i=0; i<len; i++ ) {        
    /*@-shiftnegative@*/
    *(p++) = '0' | ((value & (1 << (len2 - i))) != 0);
    /*@=shiftnegative@*/
  }

  *p = 0;  

  PROFILE_END;

  return( s );

}

static unsigned int lxt2_rd_expand_bits_to_integer(
  int   len,
  char* s
) { PROFILE(LXT2_RD_EXPAND_BITS_TO_INTEGER);

  unsigned int v = 0;
  int          i;

  for( i=0; i<len; i++ ) {
    v <<= 1;
    v |= ((*s) & 1);
    s++;
  }

  PROFILE_END;

  return( v );

}

/*
 * called for all value changes except for the 1st one in a block
 * (as they're all unique based on the timeslot scheme, no duplicate
 * checking is necessary...)
 */
/*!
 \throws anonymous Throw Throw
*/
static void lxt2_rd_iter_radix(
  struct lxt2_rd_trace* lt,
  struct lxt2_rd_block* b
) { PROFILE(LXT2_RD_ITER_RADIX);

  unsigned int which_time;
  int          offset;
  void**       top_elem;
  granmsk_t    msk = ~LXT2_RD_GRAN_1VAL;
  lxtint32_t   x;

  for( which_time=0; which_time<lt->num_time_table_entries; which_time++, msk <<= 1 ) {

    while( (top_elem = lt->radix_sort[which_time]) ) {

      lxtint32_t   idx = top_elem - lt->next_radix;
      unsigned int vch;
      unsigned int i;

      switch( lt->fac_curpos_width ) {
	case 1  :  vch = lxt2_rd_get_byte( lt->fac_curpos[idx], 0 ); break;
	case 2  :  vch = lxt2_rd_get_16( lt->fac_curpos[idx], 0 );   break;
	case 3  :  vch = lxt2_rd_get_24( lt->fac_curpos[idx], 0 );   break;
	case 4  :
	default :  vch = lxt2_rd_get_32( lt->fac_curpos[idx], 0 );   break;
      }

      lt->fac_curpos[idx] += lt->fac_curpos_width;

      /* Offset = next "which time" for this fac */
      offset = lxt2_rd_tzc( lt->fac_map[idx] &= msk );

      /* Get next list item for this "which time" bucket */
      lt->radix_sort[which_time] = lt->next_radix[idx];

      /* Promote fac to its next (higher) possible bucket (if any) */
      lt->next_radix[idx] = lt->radix_sort[offset];

      /* ... and put it at the head of that list */
      lt->radix_sort[offset] = &lt->next_radix[idx];

      switch( vch ) {
        case LXT2_RD_ENC_0:
        case LXT2_RD_ENC_1:
          memset( lt->value[idx], ('0' + (vch - LXT2_RD_ENC_0 )), lt->len[idx] );
          break;
        case LXT2_RD_ENC_INV:
          for( i=0; i<lt->len[idx]; i++ ) {
            lt->value[idx][i] ^= 1;
          }
          break;
        case LXT2_RD_ENC_LSH0:
        case LXT2_RD_ENC_LSH1:
          memmove( lt->value[idx], (lt->value[idx] + 1), (lt->len[idx] - 1) ); 
  	  lt->value[idx][lt->len[idx]-1] = '0' + (vch - LXT2_RD_ENC_LSH0); 
          break;
        case LXT2_RD_ENC_RSH0:
        case LXT2_RD_ENC_RSH1:
          memmove( (lt->value[idx] + 1), lt->value[idx], (lt->len[idx] - 1) ); 
          lt->value[idx][0] = '0' + (vch - LXT2_RD_ENC_RSH0); 
          break;
        case LXT2_RD_ENC_ADD1:
        case LXT2_RD_ENC_ADD2:
        case LXT2_RD_ENC_ADD3:
        case LXT2_RD_ENC_ADD4:
          x = lxt2_rd_expand_bits_to_integer( lt->len[idx], lt->value[idx] );
          x += (vch - LXT2_RD_ENC_ADD1 + 1); 
          memcpy( lt->value[idx], lxt2_rd_expand_integer_to_bits( lt->len[idx], x ), lt->len[idx] );
          break;
        case LXT2_RD_ENC_SUB1:
        case LXT2_RD_ENC_SUB2:
        case LXT2_RD_ENC_SUB3:
        case LXT2_RD_ENC_SUB4:
          x = lxt2_rd_expand_bits_to_integer( lt->len[idx], lt->value[idx] );
          x -= (vch - LXT2_RD_ENC_SUB1 + 1); 
          memcpy( lt->value[idx], lxt2_rd_expand_integer_to_bits( lt->len[idx], x ), lt->len[idx] );
          break;
        case LXT2_RD_ENC_X:
          memset( lt->value[idx], 'x', lt->len[idx] );
          break;
        case LXT2_RD_ENC_Z:
          memset( lt->value[idx], 'z', lt->len[idx] );
          break;
        case LXT2_RD_ENC_BLACKOUT:	
          lt->value[idx][0] = 0;
          break;
        default:
          vch -= LXT2_RD_DICT_START;
          if( vch >= b->num_dict_entries ) {
            unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Internal error:  vch(%u) >= num_dict_entries(%u)", vch, (unsigned int)b->num_dict_entries );
            assert( rv < USER_MSG_LENGTH );
            print_output( user_msg, FATAL, __FILE__, __LINE__ );
            Throw 0;
          }
          if( lt->flags[idx] & (LXT2_RD_SYM_F_DOUBLE | LXT2_RD_SYM_F_STRING) ) {
            free_safe( lt->value[idx], (strlen( lt->value[idx] ) + 1) );
            lt->value[idx] = strdup_safe( (char*)b->string_pointers[vch] );
            break;
          }
          if( lt->len[idx] == b->string_lens[vch] ) {
            memcpy( lt->value[idx], b->string_pointers[vch], lt->len[idx] );
          } else if( lt->len[idx] > b->string_lens[vch] ) {
            int lendelta = lt->len[idx] - b->string_lens[vch];
            memset( lt->value[idx], ((b->string_pointers[vch][0] != '1') ?  b->string_pointers[vch][0] : '0'), lendelta );
            strcpy( (lt->value[idx] + lendelta), (char*)b->string_pointers[vch] );
          } else {
            unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Internal error: %u ('%s') vs %u ('%s')",
                                        (unsigned int)lt->len[idx], lt->value[idx], (unsigned int)b->string_lens[vch], (char*)b->string_pointers[vch] );
            assert( rv < USER_MSG_LENGTH );
            print_output( user_msg, FATAL, __FILE__, __LINE__ );
            Throw 0;
          }
          break;
      }

      if( lt->time_table[which_time] != lt->prev_time ) {
	lt->prev_time = lt->time_table[which_time];
      }

      lt->value_change_callback(&lt, &lt->time_table[which_time], &idx, &lt->value[idx]);

    }

  }

  PROFILE_END;

}

/*
 * called for only 1st vch in a block: blocks out emission of duplicate
 * vch from preceeding block
 */
/*!
 \throws anonymous Throw
*/
static void lxt2_rd_iter_radix0(
  struct lxt2_rd_trace* lt,
  struct lxt2_rd_block* b,
  lxtint32_t            idx
) { PROFILE(LXT2_RD_ITER_RADIX0);

  unsigned int vch;
  unsigned int which_time;
  unsigned int i;
  int          uniq = 0;

  switch( lt->fac_curpos_width ) {
    case 1:  vch = lxt2_rd_get_byte( lt->fac_curpos[idx], 0 );  break;
    case 2:  vch = lxt2_rd_get_16( lt->fac_curpos[idx], 0 );    break;
    case 3:  vch = lxt2_rd_get_24( lt->fac_curpos[idx], 0 );    break;
    case 4:	
    default: vch = lxt2_rd_get_32( lt->fac_curpos[idx], 0 );    break;
  }

  lt->fac_curpos[idx] += lt->fac_curpos_width;
  which_time = 0;

  switch( vch ) {
    case LXT2_RD_ENC_0:
      for( i=0; i<lt->len[idx]; i++ ) {
        if( lt->value[idx][i] != '0' ) {
          memset( (lt->value[idx] + i), '0', (lt->len[idx] - i) );
          uniq = 1;
          break;
        }
      }
      break;
    case LXT2_RD_ENC_1:
      for( i=0; i<lt->len[idx]; i++) {
        if( lt->value[idx][i] != '1' ) {
          memset( (lt->value[idx] + i), '1', (lt->len[idx] - i) );
          uniq = 1;
          break;
        }
      }
      break;
    case LXT2_RD_ENC_INV:
    case LXT2_RD_ENC_LSH0:
    case LXT2_RD_ENC_LSH1:
    case LXT2_RD_ENC_RSH0:
    case LXT2_RD_ENC_RSH1:
    case LXT2_RD_ENC_ADD1:
    case LXT2_RD_ENC_ADD2:
    case LXT2_RD_ENC_ADD3:
    case LXT2_RD_ENC_ADD4:
    case LXT2_RD_ENC_SUB1:
    case LXT2_RD_ENC_SUB2:
    case LXT2_RD_ENC_SUB3:
    case LXT2_RD_ENC_SUB4:
      print_output( "Internal error in granule 0 position 0", FATAL, __FILE__, __LINE__ );
      Throw 0;
      /*@-unreachable@*/
      break;
      /*@=unreachable@*/
    case LXT2_RD_ENC_X:
      for( i=0; i<lt->len[idx]; i++ ) {
        if( lt->value[idx][i] != 'x' ) {
          memset( (lt->value[idx] + i), 'x', (lt->len[idx] - i) );
          uniq = 1;
          break;
        }
      }
      break;
    case LXT2_RD_ENC_Z:
      for( i=0; i<lt->len[idx]; i++ ) {
        if( lt->value[idx][i] != 'z' ) {
          memset( (lt->value[idx] + i), 'z', (lt->len[idx] - i) );
          uniq = 1;
          break;
        }
      }
      break;
    case LXT2_RD_ENC_BLACKOUT:	
      if( lt->value[idx] ) {
        lt->value[idx][0] = 0; 
	uniq = 1;
      }
      break;
    default:
      vch -= LXT2_RD_DICT_START;
      if( vch >= b->num_dict_entries ) {
        unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Internal error:  vch(%u) >= num_dict_entries(%u)", vch, (unsigned int)b->num_dict_entries );
        assert( rv < USER_MSG_LENGTH );
        print_output( user_msg, FATAL, __FILE__, __LINE__ );
        Throw 0;
      }
      if( lt->flags[idx] & (LXT2_RD_SYM_F_DOUBLE | LXT2_RD_SYM_F_STRING) ) {
        if( strcmp( lt->value[idx], (char*)b->string_pointers[vch] ) ) {
          free_safe( lt->value[idx], ((lt->len[idx] + 1) * sizeof( char )) );
          lt->value[idx] = strdup_safe( (char*)b->string_pointers[vch] );
          uniq = 1;
        }
        break;
      }
      if( lt->len[idx] == b->string_lens[vch] ) {
        for( i=0; i<lt->len[idx]; i++ ) {
          if( lt->value[idx][i] != b->string_pointers[vch][i] ) {
            memcpy( (lt->value[idx] + i), (b->string_pointers[vch] + i), (lt->len[idx] - i) );
            uniq = 1;
          }
        }
      } else if( lt->len[idx] > b->string_lens[vch] ) {
        unsigned int lendelta = lt->len[idx] - b->string_lens[vch];
        int fill = (b->string_pointers[vch][0] != '1') ?  b->string_pointers[vch][0] : '0';
        for( i=0; i<lendelta; i++ ) {
          if( lt->value[idx][i] != fill ) {
            memset( (lt->value[idx] + i), fill, (lendelta - i) );
            strcpy( (lt->value[idx] + lendelta), (char*)b->string_pointers[vch] );
            uniq = 1;
            goto fini;
          }
        }
        for( i=lendelta; i<lt->len[idx]; i++ ) {
          if( lt->value[idx][i] != b->string_pointers[vch][i-lendelta] ) {
            memcpy( (lt->value[idx] + i), (b->string_pointers[vch] + i - lendelta), (lt->len[idx] - i) );
            uniq = 1;
          }
        }
      } else {
        unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Internal error:  %u ('%s') vs %u ('%s')",
                                    (unsigned int)lt->len[idx], lt->value[idx], (unsigned int)b->string_lens[vch], (char*)b->string_pointers[vch] );
        assert( rv < USER_MSG_LENGTH );
        print_output( user_msg, FATAL, __FILE__, __LINE__ );
        Throw 0;
      }
      break;
  }

fini:
  if( uniq ) {
    if( lt->time_table[which_time] != lt->prev_time ) {
      lt->prev_time = lt->time_table[which_time];
    }
    lt->value_change_callback( &lt, &lt->time_table[which_time], &idx, &lt->value[idx] );
  }

  PROFILE_END;

}

/*
 * radix sort the fac entries based upon their first entry in the
 * time change table.  this runs in strict linear time: we can
 * do this because of the limited domain of the dataset.
 */
/*!
 \throws anonymous lxt2_rd_iter_radix0
*/
static void lxt2_rd_build_radix(
  struct lxt2_rd_trace* lt,
  struct lxt2_rd_block* b,
  int                   granule,
  lxtint32_t            strtfac,
  lxtint32_t endfac
) { PROFILE(LXT2_RD_BUILD_RADIX);

  unsigned int i;
  int offset;

  for( i=0; i<(LXT2_RD_GRANULE_SIZE+1); i++ ) {    /* +1 because tzc returns 33/65 when its arg == 0 */
    lt->radix_sort[i] = NULL;	/* each one is a linked list: we get fac number based on address of item from lt->next_radix */
  }

  for( i=strtfac; i<endfac; i++ ) {

    granmsk_t x;
    int       process_idx = i / 8;
    int       process_bit = i & 7;

    /*@-shiftnegative@*/
    if( lt->process_mask[process_idx] & (1 << process_bit) ) {
    /*@=shiftnegative@*/

      if( (x = lt->fac_map[i]) ) {
        if( !granule && (x & LXT2_RD_GRAN_1VAL) ) {
          lxt2_rd_iter_radix0( lt, b, i );		/* emit vcd only if it's unique */
          x &= ~LXT2_RD_GRAN_1VAL;		        /* clear out least sig bit */
          lt->fac_map[i] = x;
          if( !x ) {
            continue;
          }
        }
        offset = lxt2_rd_tzc( x );			/* get "which time" bucket number of new least sig one bit */
        lt->next_radix[i] = lt->radix_sort[offset];	/* insert item into head of radix sorted "which time" buckets */
        lt->radix_sort[offset] = &lt->next_radix[i];
      }

    }

  }

  PROFILE_END;

}

/****************************************************************************/

/*
 * build compressed process mask if necessary
 */
static void lxt2_rd_regenerate_process_mask(
  struct lxt2_rd_trace* lt
) { PROFILE(LXT2_RD_REGENERATE_PROCESS_MASK);

  unsigned int i, j, lim, idx;

  if( lt && lt->process_mask_dirty ) {

    lt->process_mask_dirty = 0;
    idx = 0;

    for( i=0; i<lt->numrealfacs; i+=LXT2_RD_PARTIAL_SIZE ) {

      if( (i + LXT2_RD_PARTIAL_SIZE) > lt->numrealfacs ) {
        lim = lt->numrealfacs;
      } else {
        lim = i + LXT2_RD_PARTIAL_SIZE;
      }
	
      lt->process_mask_compressed[idx] = 0;
      for( j=i; j<lim; j++ ) {
        int process_idx = j / 8;		/* no need to optimize this */
        int process_bit = j & 7;
        /*@-shiftnegative@*/
        if( lt->process_mask[process_idx] & (1 << process_bit) ) {
        /*@=shiftnegative@*/
          lt->process_mask_compressed[idx] = 1;
          break;
        }
      }

      idx++;

    }

  }

  PROFILE_END;

}

/****************************************************************************/


/*
 * process a single block and execute the vch callback as necessary
 */
/*!
 \throws anonymous lxt2_rd_build_radix lxt2_rd_iter_radix Throw Throw Throw Throw
*/
static int lxt2_rd_process_block(
  struct lxt2_rd_trace* lt,
  struct lxt2_rd_block* b
) { PROFILE(LXT2_RD_PROCESS_BLOCK);

  char           vld;
  unsigned char* pnt;
  unsigned int   i;
  int            granule      = 0;
  char           sect_typ;
  lxtint32_t     strtfac_gran = 0;
  char           granvld      = 0;

  b->num_map_entries  = lxt2_rd_get_32( b->mem, (b->uncompressed_siz -  4) );
  b->num_dict_entries = lxt2_rd_get_32( b->mem, (b->uncompressed_siz - 12) );

#if LXT2_RD_GRANULE_SIZE > 32
  if( lt->granule_size == LXT2_RD_GRANULE_SIZE ) {
    b->map_start = (b->mem + b->uncompressed_siz - 12) - sizeof( granmsk_t ) * b->num_map_entries;
  } else {
    b->map_start = (b->mem + b->uncompressed_siz - 12) - sizeof( granmsk_smaller_t ) * b->num_map_entries;
  }
#else
  b->map_start = (b->mem + b->uncompressed_siz - 12) - sizeof( granmsk_t ) * b->num_map_entries;
#endif

  b->dict_start = b->map_start - lxt2_rd_get_32( b->mem, (b->uncompressed_siz - 8) );

  vld = lxt2_rd_get_byte( (b->dict_start - 1), 0 );

  if( vld != LXT2_RD_GRAN_SECT_DICT ) {
    print_output( "Malformed section", FATAL, __FILE__, __LINE__ );
    Throw 0;
  }

  if( b->num_dict_entries ) {
    b->string_pointers = malloc_safe_nolimit( b->num_dict_entries * sizeof( char* ) );
    b->string_lens     = malloc_safe_nolimit( b->num_dict_entries * sizeof( unsigned int ) );
    pnt                = b->dict_start;
    for( i=0; i<b->num_dict_entries; i++ ) {
      b->string_pointers[i] = pnt;
      b->string_lens[i]     = strlen( (char*)pnt );
      pnt                  += (b->string_lens[i] + 1);
    }
    if( pnt != b->map_start ) {
      print_output( "Dictionary corrupt, exiting...", FATAL, __FILE__, __LINE__ );
      free_safe( b->string_pointers, (b->num_dict_entries * sizeof( char* )) );
      free_safe( b->string_lens, (b->num_dict_entries * sizeof( unsigned int )) );
      Throw 0;
    }
  }

  pnt = b->mem;

  while( ((sect_typ=*pnt) == LXT2_RD_GRAN_SECT_TIME) || (sect_typ == LXT2_RD_GRAN_SECT_TIME_PARTIAL) ) {

    lxtint32_t strtfac, endfac;

    if( sect_typ == LXT2_RD_GRAN_SECT_TIME_PARTIAL ) {

      lxtint32_t sublen;

      lxt2_rd_regenerate_process_mask( lt );
      strtfac = lxt2_rd_get_32( pnt, 1 );
      sublen  = lxt2_rd_get_32( pnt, 5 );

      if( !granvld ) {
        granvld      = 1;
        strtfac_gran = strtfac;
      } else {
        granule += (strtfac == strtfac_gran);
      }

      if( !lt->process_mask_compressed[strtfac/LXT2_RD_PARTIAL_SIZE] ) {
        pnt += 9;
        pnt += sublen;
        continue;
      }

      endfac = strtfac + LXT2_RD_PARTIAL_SIZE;

      if( endfac > lt->numrealfacs ) {
        endfac = lt->numrealfacs;
      }

      pnt += 8;

    } else {

      strtfac = 0;
      endfac  = lt->numrealfacs;

    }

    pnt++;
    lt->num_time_table_entries = lxt2_rd_get_byte( pnt, 0 );
    pnt++;
    for( i=0; i<lt->num_time_table_entries; i++ ) {
      lt->time_table[i] = lxt2_rd_get_64( pnt, 0 );
      pnt               += 8;
    }

    lt->fac_map_index_width = lxt2_rd_get_byte( pnt, 0 );
    if( !lt->fac_map_index_width || (lt->fac_map_index_width > 4) ) {
      /*@-formatcode@*/
      unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Map index width of %hhu is illegal, exiting...", lt->fac_map_index_width );
      /*@=formatcode@*/
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, FATAL, __FILE__, __LINE__ );
      Throw 0;
    }
    pnt++;

    for( i=strtfac; i<endfac; i++ ) {

      lxtint32_t mskindx;

      switch( lt->fac_map_index_width ) {
        case 1 :  mskindx = lxt2_rd_get_byte( pnt, 0 );  break;
        case 2 :  mskindx = lxt2_rd_get_16( pnt, 0 );    break;
        case 3 :  mskindx = lxt2_rd_get_24( pnt, 0 );    break;
        case 4 : 
        default:  mskindx = lxt2_rd_get_32( pnt, 0 );    break;
      }

      pnt += lt->fac_map_index_width;

#if LXT2_RD_GRANULE_SIZE > 32
      if( lt->granule_size==LXT2_RD_GRANULE_SIZE ) {
        lt->fac_map[i] = get_fac_msk( b->map_start, (mskindx * sizeof( granmsk_t )) );
      } else {
        lt->fac_map[i] = get_fac_msk_smaller( b->map_start, (mskindx * sizeof( granmsk_smaller_t )) );
      }
#else
      lt->fac_map[i] = get_fac_msk( b->map_start, (mskindx * sizeof( granmsk_t )) );
#endif

    }

    lt->fac_curpos_width = lxt2_rd_get_byte( pnt, 0 );
    if( !lt->fac_curpos_width || (lt->fac_curpos_width > 4) ) {
      /*@-formatcode@*/
      unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Curpos index width of %hhu is illegal, exiting...", lt->fac_curpos_width );
      /*@=formatcode@*/
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, FATAL, __FILE__, __LINE__ );
      Throw 0;
    }
    pnt++;

    for( i=strtfac; i<endfac; i++ ) {
      lt->fac_curpos[i] = pnt;
      if( lt->fac_map[i] ) {
        pnt += (lxt2_rd_ones_cnt( lt->fac_map[i] ) * lt->fac_curpos_width);
      }
    }

    lxt2_rd_build_radix( lt, b, granule, strtfac, endfac );
    lxt2_rd_iter_radix( lt, b );

    if( sect_typ != LXT2_RD_GRAN_SECT_TIME_PARTIAL ) {
      granule++;
    }

  }

  PROFILE_END;

  return( 1 );

}


/****************************************************************************/

/*
 * null callback used when a user passes NULL as an argument to lxt2_rd_iter_blocks()
 */
static void lxt2_rd_null_callback(
  /*@unused@*/ struct lxt2_rd_trace** lt,
  /*@unused@*/ lxtint64_t*            pnt_time,
  /*@unused@*/ lxtint32_t*            pnt_facidx,
  /*@unused@*/ char**                 pnt_value
) {
}

/****************************************************************************/

/*
 * initialize the trace, get compressed facnames, get geometries,
 * and get block offset/size/timestart/timeend...
 */
#ifndef HAVE_LIBZ
struct lxt2_rd_trace* lxt2_rd_init( const char* name ) {

  print_output( "Unable to parse LXT dumpfile due to missing zlib library in the build of Covered", FATAL, __FILE__, __LINE__ );
  return( NULL );

}
#else
struct lxt2_rd_trace* lxt2_rd_init( const char* name ) { PROFILE(LXT2_RD_INIT);

  struct lxt2_rd_trace* lt = (struct lxt2_rd_trace*)calloc_safe( 1, sizeof( struct lxt2_rd_trace ) );
  unsigned int          i;

  if( !(lt->handle = fopen( name, "rb" )) ) {

    lxt2_rd_close( lt );
    lt = NULL;

  } else {

    lxtint16_t   id = 0, version = 0;
    unsigned int rv;

    /* Cutoff after this number of bytes and force flush */
    lt->block_mem_max = LXT2_RD_MAX_BLOCK_MEM_USAGE;

    /* Keeps gzip from acting weird in tandem with fopen */
    rv = setvbuf( lt->handle, (char*)NULL, _IONBF, 0 );
    assert( rv == 0 );

    (void)fread( &id, 2, 1, lt->handle );
    (void)fread( &version, 2, 1, lt->handle );
    (void)fread( &lt->granule_size, 1, 1, lt->handle );
	
    if( lxt2_rd_get_16( &id, 0 ) != LXT2_RD_HDRID ) {

      print_output( "File specified with -lxt is not an LXT formatted dumpfile", FATAL, __FILE__, __LINE__ );
      lxt2_rd_close( lt );
      lt = NULL;

    } else if( (version = lxt2_rd_get_16( &version, 0 )) > LXT2_RD_VERSION ) {

      unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT dumpfile is version %u which is not supported by Covered", (unsigned int)version );
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, FATAL, __FILE__, __LINE__ );
      lxt2_rd_close( lt );
      lt = NULL;

    } else if( lt->granule_size > LXT2_RD_GRANULE_SIZE ) {

      unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT dumpfile contains granule size of %u (>%d) which is not supported by Covered",
                                  (unsigned int)lt->granule_size, LXT2_RD_GRANULE_SIZE );
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, FATAL, __FILE__, __LINE__ );
      lxt2_rd_close( lt );
      lt = NULL;

    } else {

      unsigned int          rc;
      char*                 m;
      off_t                 pos, fend;
      unsigned int          t;
      struct lxt2_rd_block* b;
      unsigned int          rv;

      (void)fread( &lt->numfacs, 4, 1, lt->handle );
      lt->numfacs = lxt2_rd_get_32( &lt->numfacs, 0 );

      (void)fread( &lt->numfacbytes, 4, 1, lt->handle );
      lt->numfacbytes = lxt2_rd_get_32( &lt->numfacbytes, 0 );

      (void)fread( &lt->longestname, 4, 1, lt->handle );
      lt->longestname = lxt2_rd_get_32( &lt->longestname, 0 );

      (void)fread( &lt->zfacnamesize, 4, 1, lt->handle );
      lt->zfacnamesize = lxt2_rd_get_32( &lt->zfacnamesize, 0 );

      (void)fread( &lt->zfacname_predec_size, 4, 1, lt->handle );
      lt->zfacname_predec_size = lxt2_rd_get_32( &lt->zfacname_predec_size, 0 );

      (void)fread( &lt->zfacgeometrysize, 4, 1, lt->handle );
      lt->zfacgeometrysize = lxt2_rd_get_32( &lt->zfacgeometrysize, 0 );

      (void)fread( &lt->timescale, 1, 1, lt->handle );  /* No swap necessary */

#ifdef DEBUG_MODE
      if( debug_mode ) {
        unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT:  %d facilities", lt->numfacs );
        assert( rv < USER_MSG_LENGTH ); 
        print_output( user_msg, DEBUG, __FILE__, __LINE__ );
      }
#endif
      
      pos = ftello( lt->handle );

      lt->process_mask            = calloc_safe( 1, ((lt->numfacs / 8) + 1) );
      lt->process_mask_compressed = calloc_safe( 1, ((lt->numfacs / LXT2_RD_PARTIAL_SIZE) + 1) );

      /* Open and read the name section of the file that is compressed */
      lt->zhandle = gzdopen( dup( fileno( lt->handle ) ), "rb" );
      m  = (char*)malloc_safe_nolimit( lt->zfacname_predec_size );
      rc = gzread( lt->zhandle, m, lt->zfacname_predec_size );
      rv = gzclose( lt->zhandle );
      assert( rv == Z_OK );
      lt->zhandle = NULL;

      if( rc != lt->zfacname_predec_size ) {
        unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT:  Name section mangled %u (actual) vs %u (expected)", rc, (unsigned int)lt->zfacname_predec_size );
        assert( rv < USER_MSG_LENGTH );
        print_output( user_msg, FATAL, __FILE__, __LINE__ );
        free_safe( m, lt->zfacname_predec_size );
        lxt2_rd_close( lt );
        lt = NULL;
        return( lt );
      }

      lt->zfacnames = m;

      lt->faccache = calloc_safe( 1, sizeof( struct lxt2_rd_facname_cache ) );
      lt->faccache->old_facidx = lt->numfacs;   /* Causes lxt2_rd_get_facname to initialize its unroll ptr as this is always invalid */
      lt->faccache->bufcurr = malloc_safe_nolimit( lt->longestname + 1 );
      lt->faccache->bufprev = malloc_safe_nolimit( lt->longestname + 1 );

      fseeko( lt->handle, (pos = (pos + lt->zfacnamesize)), SEEK_SET );
      lt->zhandle = gzdopen( dup( fileno( lt->handle ) ), "rb" );

      t  = lt->numfacs * 4 * sizeof( lxtint32_t );
      m  = (char*)malloc_safe_nolimit( t );				
      rc = gzread( lt->zhandle, m, t );
      rv = gzclose( lt->zhandle );
      assert( rv == Z_OK );
      lt->zhandle = NULL;

      if( rc != t ) {
        unsigned int rv;
#ifdef DEBUG_MODE
        if( debug_mode ) {
          rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT:  Geometry section mangled %d (actual) vs %d (expected)", rc, t );
          assert( rv < USER_MSG_LENGTH );
          print_output( user_msg, DEBUG, __FILE__, __LINE__ );
        }
#endif
        rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT:  No LXT dumpfile information available in file %s", name );
        assert( rv < USER_MSG_LENGTH );
        print_output( user_msg, FATAL, __FILE__, __LINE__ );
        free_safe( m, t );
        lxt2_rd_close( lt );
        lt = NULL;
        return( lt );
      }

      pos = pos + lt->zfacgeometrysize;

      lt->rows       = malloc_safe_nolimit( lt->numfacs * sizeof( lxtint32_t ) );
      lt->msb        = malloc_safe_nolimit( lt->numfacs * sizeof( lxtint32_t ) );
      lt->lsb        = malloc_safe_nolimit( lt->numfacs * sizeof( lxtint32_t ) );
      lt->flags      = malloc_safe_nolimit( lt->numfacs * sizeof( lxtint32_t ) );
      lt->len        = malloc_safe_nolimit( lt->numfacs * sizeof( lxtint32_t ) );
      lt->value      = malloc_safe_nolimit( lt->numfacs * sizeof( char* ) );
      lt->next_radix = malloc_safe_nolimit( lt->numfacs * sizeof( void* ) );

      for( i=0; i<lt->numfacs; i++ ) {
        unsigned int j;
        lt->rows[i]  = lxt2_rd_get_32( m + i * 16, 0 );
        lt->msb[i]   = lxt2_rd_get_32( m + i * 16, 4 );
        lt->lsb[i]   = lxt2_rd_get_32( m + i * 16, 8 );
        lt->flags[i] = lxt2_rd_get_32( m + i * 16, 12 );

        if( !(lt->flags[i] & LXT2_RD_SYM_F_INTEGER) ) {
          lt->len[i] = (lt->msb[i] <= lt->lsb[i]) ? (lt->lsb[i] - lt->msb[i] + 1) : (lt->msb[i] - lt->lsb[i] + 1);
        } else {
          lt->len[i] = 32;
        }
        lt->value[i] = calloc_safe( (lt->len[i] + 1), sizeof( char ) );
        for( j=0; j<lt->len[i]; j++ ) {
          lt->value[i][j] = 'a';
        }
        lt->value[i][j] = '\0';
      }

      for( lt->numrealfacs=0; lt->numrealfacs<lt->numfacs; lt->numrealfacs++ ) {
        if( lt->flags[lt->numrealfacs] & LXT2_RD_SYM_F_ALIAS ) {
          break;
        }
      }

      if( lt->numrealfacs > lt->numfacs ) {
        lt->numrealfacs = lt->numfacs;
      }

      lt->prev_time = ~(LXT2_RD_GRAN_0VAL);
      free_safe( m, t );

      lt->fac_map    = malloc_safe_nolimit( lt->numfacs * sizeof( granmsk_t ) );
      lt->fac_curpos = malloc_safe_nolimit( lt->numfacs * sizeof( char* ) );

      for( ; ; ) {

        fseeko( lt->handle, 0L, SEEK_END );
        fend = ftello( lt->handle );

        if( pos >= fend ) {
          break;
        }

        fseeko( lt->handle, pos, SEEK_SET );

        b = calloc_safe( 1, sizeof( struct lxt2_rd_block ) );
		
        (void)fread( &b->uncompressed_siz, 4, 1, lt->handle );
        b->uncompressed_siz = lxt2_rd_get_32( &b->uncompressed_siz, 0 );

        (void)fread( &b->compressed_siz, 4, 1, lt->handle );
        b->compressed_siz = lxt2_rd_get_32( &b->compressed_siz, 0 );

        (void)fread( &b->start, 8, 1, lt->handle );
        b->start = lxt2_rd_get_64( &b->start, 0 );

        (void)fread( &b->end, 8, 1, lt->handle );
        b->end = lxt2_rd_get_64( &b->end, 0 );

        pos = ftello( lt->handle );
        fseeko( lt->handle, pos, SEEK_SET );

        if( pos >= fend ) {
          free_safe( b, 0 );  /* TBD */
          break;
        }

        /* Mark startpos for later in case we purge it from memory */
        b->filepos = pos;
	
        if( b->uncompressed_siz && b->compressed_siz && b->end ) {
          fseeko(lt->handle, b->compressed_siz, SEEK_CUR);
          lt->numblocks++;
          if( lt->block_curr ) {
            lt->block_curr->next = b;
            lt->block_curr       = b;
            lt->end              = b->end;
          } else {
            lt->block_head = lt->block_curr = b;
            lt->start      = b->start;
            lt->end        = b->end;
          }			
        } else {
          free_safe( b, 0 );  /* TBD */
          break;
        }
	
        pos += b->compressed_siz;

      }

      if( lt->numblocks ) {

#ifdef DEBUG_MODE
        if( debug_mode ) {
          unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT:  Read %d block header%s OK", lt->numblocks, ((lt->numblocks != 1) ? "s" : "") );
          assert( rv < USER_MSG_LENGTH );
          print_output( user_msg, DEBUG, __FILE__, __LINE__ );

          rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT:  Start time = "LXT2_RD_LLD"", lt->start );
          assert( rv < USER_MSG_LENGTH );
          print_output( user_msg, DEBUG, __FILE__, __LINE__ );

          rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT:  End time   = "LXT2_RD_LLD"", lt->end );
          assert( rv < USER_MSG_LENGTH );
          print_output( user_msg, DEBUG, __FILE__, __LINE__ );
        }
#endif

        lt->value_change_callback = lxt2_rd_null_callback;

      } else {

        unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "No blocks exist in LXT dumpfile %s", name );
        assert( rv < USER_MSG_LENGTH );
        print_output( user_msg, FATAL, __FILE__, __LINE__ );
        lxt2_rd_close( lt );
        lt = NULL;

      }

    }

  }

  PROFILE_END;

  return( lt );

}
#endif

/*
 * free up/deallocate any resources still left out there:
 * blindly do it based on NULL pointer comparisons (ok, since
 * calloc_safe() is used to allocate all structs) as performance
 * isn't an issue for this set of cleanup code
 */
void lxt2_rd_close(
  struct lxt2_rd_trace* lt
) { PROFILE(LXT2_RD_CLOSE);

  if( lt ) {

    struct lxt2_rd_block* b;
    struct lxt2_rd_block* bt;
    unsigned int          i;

    if( lt->value != NULL ) {
      for( i=0; i<lt->numfacs; i++ ) {
        free_safe( lt->value[i], (strlen( lt->value[i] ) + 1) );
        lt->value[i] = NULL; 
      }
      free_safe( lt->value, (lt->numfacs * sizeof( char* )) );
      lt->value = NULL;
    }

    free_safe( lt->process_mask, ((lt->numfacs / 8) + 1) );
    free_safe( lt->process_mask_compressed, ((lt->numfacs / LXT2_RD_PARTIAL_SIZE) + 1) );
    free_safe( lt->rows,  (lt->numfacs * sizeof( lxtint32_t )) );
    free_safe( lt->msb,   (lt->numfacs * sizeof( lxtint32_t )) );
    free_safe( lt->lsb,   (lt->numfacs * sizeof( lxtint32_t )) );
    free_safe( lt->flags, (lt->numfacs * sizeof( lxtint32_t )) );
    free_safe( lt->len,   (lt->numfacs * sizeof( lxtint32_t )) );
    free_safe( lt->next_radix, (lt->numfacs * sizeof( void* )) );

    lt->process_mask            = NULL;
    lt->process_mask_compressed = NULL;
    lt->rows                    = NULL;
    lt->msb                     = NULL;
    lt->lsb                     = NULL;
    lt->flags                   = NULL;
    lt->len                     = NULL;
    lt->next_radix              = NULL;

    free_safe( lt->zfacnames, lt->zfacname_predec_size );
    lt->zfacnames = NULL;

    if( lt->faccache ) {

      free_safe( lt->faccache->bufprev, (lt->longestname + 1) );
      free_safe( lt->faccache->bufcurr, (lt->longestname + 1) );

      lt->faccache->bufprev = NULL;
      lt->faccache->bufcurr = NULL;

      free_safe( lt->faccache, sizeof( struct lxt2_rd_facname_cache ) );
      lt->faccache = NULL;

    }

    free_safe( lt->fac_map, lt->numfacs * sizeof( granmsk_t ) );
    lt->fac_map = NULL;
    free_safe( lt->fac_curpos, lt->numfacs * sizeof( char* ) );
    lt->fac_curpos = NULL;

    b = lt->block_head;
    while( b ) {

      bt = b->next;

      free_safe( b->mem, b->uncompressed_siz );
      free_safe( b->string_pointers, (b->num_dict_entries * sizeof( char* )) );
      free_safe( b->string_lens, (b->num_dict_entries * sizeof( unsigned int)) );

      b->mem             = NULL;
      b->string_pointers = NULL;
      b->string_lens     = NULL;

      free_safe( b, sizeof( struct lxt2_rd_block ) );

      b = bt;

    }

    lt->block_head = lt->block_curr = NULL;

#ifdef HAVE_LIBZ
    if( lt->zhandle ) {
      unsigned int rv = gzclose( lt->zhandle );
      assert( rv == Z_OK );
      lt->zhandle = NULL;
    }
#endif

    if( lt->handle ) {
      unsigned int rv = fclose( lt->handle );
      assert( rv == 0 );
      lt->handle = NULL;
    }

    free_safe( lt, sizeof( struct lxt2_rd_trace ) );

  }

  PROFILE_END;

}

/****************************************************************************/

/* 
 * return number of facs in trace
 */
_LXT2_RD_INLINE lxtint32_t lxt2_rd_get_num_facs( struct lxt2_rd_trace* lt ) {

  return( lt ? lt->numfacs : 0 );

}


/*
 * return fac geometry for a given index
 */
struct lxt2_rd_geometry* lxt2_rd_get_fac_geometry(
  struct lxt2_rd_trace* lt,
  lxtint32_t            facidx
) {

  if( lt && (facidx < lt->numfacs) ) {
    lt->geometry.rows  = lt->rows[facidx];	
    lt->geometry.msb   = lt->msb[facidx];	
    lt->geometry.lsb   = lt->lsb[facidx];	
    lt->geometry.flags = lt->flags[facidx];	
    lt->geometry.len   = lt->len[facidx];	
    return( &lt->geometry );
  } else {
    return( NULL );
  }

}


/*
 * return partial fac geometry for a given index
 */
_LXT2_RD_INLINE lxtint32_t lxt2_rd_get_fac_rows( struct lxt2_rd_trace* lt, lxtint32_t facidx ) {

  if( lt && (facidx < lt->numfacs) ) {
    return( lt->rows[facidx] );
  } else {
    return( 0 );
  }

}


_LXT2_RD_INLINE lxtint32_t lxt2_rd_get_fac_msb(struct lxt2_rd_trace *lt, lxtint32_t facidx ) {

  if( lt && (facidx < lt->numfacs) ) {
    return( lt->msb[facidx] );
  } else {
    return( 0 );
  }

}


_LXT2_RD_INLINE lxtint32_t lxt2_rd_get_fac_lsb( struct lxt2_rd_trace* lt, lxtint32_t facidx ) {

  if( lt && (facidx < lt->numfacs) ) {
    return( lt->lsb[facidx] );
  } else {
    return( 0 );
  }

}


_LXT2_RD_INLINE lxtint32_t lxt2_rd_get_fac_flags( struct lxt2_rd_trace* lt, lxtint32_t facidx ) {

  if( lt && (facidx < lt->numfacs) ) {
    return( lt->flags[facidx] );
  } else {
    return( 0 );
  }

}

_LXT2_RD_INLINE lxtint32_t lxt2_rd_get_fac_len( struct lxt2_rd_trace* lt, lxtint32_t facidx ) {

  if( lt && (facidx < lt->numfacs) ) {
    return( lt->len[facidx] );
  } else {
    return( 0 );
  }

}

_LXT2_RD_INLINE lxtint32_t lxt2_rd_get_alias_root(struct lxt2_rd_trace *lt, lxtint32_t facidx) {

  if( lt && (facidx < lt->numfacs) ) {

    while( lt->flags[facidx] & LXT2_RD_SYM_F_ALIAS ) {
      facidx = lt->rows[facidx];	/* iterate to next alias */
    }

    return( facidx );

  } else {

    return( ~((lxtint32_t)0) );

  }

}

/*
 * time queries
 */ 
_LXT2_RD_INLINE lxtint64_t lxt2_rd_get_start_time( struct lxt2_rd_trace* lt ) {

  return( lt ? lt->start : LXT2_RD_GRAN_0VAL );

}

_LXT2_RD_INLINE lxtint64_t lxt2_rd_get_end_time( struct lxt2_rd_trace* lt ) {

  return( lt ? lt->end : LXT2_RD_GRAN_0VAL );

}

_LXT2_RD_INLINE char lxt2_rd_get_timescale( struct lxt2_rd_trace* lt ) {

  return( lt ? lt->timescale : 0 );

}

/*
 * extract facname from prefix-compressed table.  this
 * performs best when extracting facs with monotonically
 * increasing indices...
 */
char* lxt2_rd_get_facname(
  struct lxt2_rd_trace* lt,
  lxtint32_t            facidx
) { PROFILE(LXT2_RD_GET_FACNAME);

  char* pnt;
  unsigned int clone, j;

  if( lt ) {

    if( (facidx == (lt->faccache->old_facidx + 1)) || !facidx ) {

      if( !facidx ) {
        lt->faccache->n          = lt->zfacnames;
        lt->faccache->bufcurr[0] = 0;			
        lt->faccache->bufprev[0] = 0;			
      }

      if( facidx != lt->numfacs ) {

        pnt = lt->faccache->bufcurr;
        lt->faccache->bufcurr = lt->faccache->bufprev;
        lt->faccache->bufprev = pnt;

        clone = lxt2_rd_get_16( lt->faccache->n, 0 );
        lt->faccache->n += 2;
        pnt = lt->faccache->bufcurr;

        for( j=0; j<clone; j++ ) {
          *(pnt++) = lt->faccache->bufprev[j];
        }

        while( (*(pnt++) = lxt2_rd_get_byte( lt->faccache->n++, 0 )) );

	lt->faccache->old_facidx = facidx;

        return( lt->faccache->bufcurr );

      } else {

        return( NULL );	/* no more left */

      }

    } else {

      if( facidx < lt->numfacs ) {		

        int strt;

        if( facidx == lt->faccache->old_facidx ) {
          return( lt->faccache->bufcurr );
        }

        if( facidx > (lt->faccache->old_facidx + 1) ) {
          strt = lt->faccache->old_facidx + 1;
        } else {
          strt = 0;
        }

        for( j=strt; j<facidx; j++ ) {
          (void)lxt2_rd_get_facname( lt, j );
        }
	
        return( lxt2_rd_get_facname( lt, j ) );

      }

    }

  }

  PROFILE_END;

  return( NULL );

}

/*
 * iter mask manipulation util functions
 */
_LXT2_RD_INLINE int lxt2_rd_get_fac_process_mask( struct lxt2_rd_trace* lt, lxtint32_t facidx ) {

  if( lt ) {

    if( facidx < lt->numfacs ) {
      int process_idx = facidx / 8;
      int process_bit = facidx & 7;
      /*@-shiftnegative@*/
      return( (lt->process_mask[process_idx] & (1 << process_bit)) != 0 );
      /*@=shiftnegative@*/
    }

  }

  return( 0 );

}

_LXT2_RD_INLINE int lxt2_rd_set_fac_process_mask( struct lxt2_rd_trace* lt, lxtint32_t facidx ) {

  int rc = 0;

  if( lt ) {

    lt->process_mask_dirty = 1;

    if( facidx < lt->numfacs ) {
      int idx    = facidx / 8;
      int bitpos = facidx & 7;
      /*@-shiftnegative@*/
      lt->process_mask[idx] |= (1 << bitpos);
      /*@=shiftnegative@*/
    }

    rc = 1;

  }

  return( rc );

}


_LXT2_RD_INLINE int lxt2_rd_clr_fac_process_mask( struct lxt2_rd_trace *lt, lxtint32_t facidx ) {

  int rc = 0;

  if( lt ) {

    lt->process_mask_dirty = 1;

    if( facidx < lt->numfacs) {
      int idx    = facidx / 8;
      int bitpos = facidx & 7;
      /*@-shiftnegative@*/
      lt->process_mask[idx] &= ~(1 << bitpos);
      /*@=shiftnegative@*/
    }

    rc = 1;

  }

  return( rc );

}

_LXT2_RD_INLINE int lxt2_rd_set_fac_process_mask_all( struct lxt2_rd_trace* lt ) {

  int rc = 0;

  if( lt ) {
    lt->process_mask_dirty = 1;
    memset( lt->process_mask, 0xff, ((lt->numfacs + 7) / 8) );
    rc = 1;
  }

  return( rc );

}

_LXT2_RD_INLINE int lxt2_rd_clr_fac_process_mask_all( struct lxt2_rd_trace* lt ) {

  int rc = 0;

  if( lt ) {
    lt->process_mask_dirty = 1;
    memset( lt->process_mask, 0x00, ((lt->numfacs + 7) / 8) );
    rc = 1;
  }

  return( rc );

}

/*
 * block memory set/get used to control buffering
 */
_LXT2_RD_INLINE lxtint64_t lxt2_rd_set_max_block_mem_usage( struct lxt2_rd_trace* lt, lxtint64_t block_mem_max ) {

  lxtint64_t rc = lt->block_mem_max;

  lt->block_mem_max = block_mem_max;

  return( rc );

}

_LXT2_RD_INLINE lxtint64_t lxt2_rd_get_block_mem_usage( struct lxt2_rd_trace* lt ) {

  return( lt->block_mem_consumed );

}

/*
 * return total number of blocks
 */
_LXT2_RD_INLINE unsigned int lxt2_rd_get_num_blocks( struct lxt2_rd_trace* lt ) {

  return( lt->numblocks );

}

/*
 * return number of active blocks
 */
unsigned int lxt2_rd_get_num_active_blocks( struct lxt2_rd_trace* lt ) {

  int blk = 0;

  if( lt ) {

    struct lxt2_rd_block* b = lt->block_head;

    while( b ) {
      if( !b->short_read_ignore && !b->exclude_block ) {
        blk++;
      }
      b = b->next;
    }

  }

  return( blk );

}

/****************************************************************************/

#ifndef HAVE_LIBZ
int lxt2_rd_iter_blocks( struct lxt2_rd_trace* lt,
                         void (*value_change_callback)( struct lxt2_rd_trace** lt, lxtint64_t* time, lxtint32_t* facidx, char** value ),
                         void *user_callback_data_pointer ) {
  return( 0 );
}
#else
/*
 * block iteration...purge/reload code here isn't sophisticated as it
 * merely caches the FIRST set of blocks which fit in lt->block_mem_max.
 * n.b., returns number of blocks processed
 */
/*!
 \throws anonymous Throw lxt2_rd_process_block
*/
int lxt2_rd_iter_blocks(
  struct lxt2_rd_trace* lt, 
  void (*value_change_callback)( struct lxt2_rd_trace** lt, lxtint64_t* time, lxtint32_t* facidx, char** value ),
  void *user_callback_data_pointer
) { PROFILE(LXT2_RD_ITER_BLOCKS);

  struct lxt2_rd_block* b;
  int                   blk       = 0;
  int                   blkfinal  = 0;
  int                   processed = 0;
  struct lxt2_rd_block* bcutoff   = NULL;
  struct lxt2_rd_block* bfinal    = NULL;
  int                   striped_kill          = 0;
  unsigned int          real_uncompressed_siz = 0;
  unsigned char         gzid[2];

  if( lt ) {

    lt->value_change_callback      = value_change_callback ? value_change_callback : lxt2_rd_null_callback; 
    lt->user_callback_data_pointer = user_callback_data_pointer;

    b   = lt->block_head;
    blk = 0;

    while( b ) {

      if( !b->mem && !b->short_read_ignore && !b->exclude_block ) {

        int rc;

	if( processed < 5 ) {
          int gate = (processed == 4) && b->next;
#ifdef DEBUG_MODE
          if( debug_mode ) {
            unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT block [%d] processing "LXT2_RD_LLD" / "LXT2_RD_LLD"%s",
                                        blk, b->start, b->end, (gate ? " ..." : "") ); 
            assert( rv < USER_MSG_LENGTH );
            print_output( user_msg, DEBUG, __FILE__, __LINE__ );
          }
#endif
          if( gate ) {
            bcutoff = b;
          }
        }

        processed++;

        fseeko( lt->handle, b->filepos, SEEK_SET );
        gzid[0] = gzid[1] = 0;
        (void)fread( &gzid, 2, 1, lt->handle );
        fseeko( lt->handle, b->filepos, SEEK_SET );

        if( (striped_kill = (gzid[0] != 0x1f) || (gzid[1] != 0x8b)) ) {

          lxtint32_t        clen, unclen, iter = 0;
          unsigned char*    pnt;
          off_t             fspos = b->filepos;
          unsigned int      rc;
          unsigned char*    zbuff = NULL;
          unsigned int      zlen  = 0;
          struct z_stream_s strm;

          real_uncompressed_siz = b->uncompressed_siz;
          pnt = b->mem = (unsigned char*)malloc_safe_nolimit( b->uncompressed_siz );
          b->uncompressed_siz = 0;

          lxt2_rd_regenerate_process_mask( lt );

          while( iter != 0xFFFFFFFF ) {

            clen = unclen = iter = 0;				

            (void)fread( &clen, 4, 1, lt->handle );
            clen = lxt2_rd_get_32( &clen, 0 );

            (void)fread( &unclen, 4, 1, lt->handle );
            unclen = lxt2_rd_get_32( &unclen, 0 );

            (void)fread( &iter, 4, 1, lt->handle );
            iter = lxt2_rd_get_32( &iter, 0 );

            fspos += 12;

            if( (iter == 0xFFFFFFFF) || lt->process_mask_compressed[iter/LXT2_RD_PARTIAL_SIZE] ) {

              if( clen > zlen ) {
                free_safe( zbuff, 0 );  /* TBD */
                zbuff = (unsigned char*)malloc_safe_nolimit( zlen = (clen * 2) );
              }

              (void)fread( zbuff, clen, 1, lt->handle );

              strm.avail_in  = clen - 10;
              strm.avail_out = unclen;
              strm.total_in  = strm.total_out = 0;
              strm.zalloc    = NULL;
              strm.zfree     = NULL;
              strm.opaque    = NULL;
              strm.next_in   = zbuff + 10;
              strm.next_out  = pnt;

              (void)inflateInit2( &strm, -MAX_WBITS );

              while (Z_OK == (inflate( &strm, Z_NO_FLUSH )) );

              (void)inflateEnd( &strm );

              if( strm.total_out != unclen ) {
                unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT:  Short read on subblock %lu vs %u (expected), ignoring...",
                                            strm.total_out, (unsigned int)unclen );
                assert( rv < USER_MSG_LENGTH );
                print_output( user_msg, WARNING, __FILE__, __LINE__ );
                free_safe( b->mem, 0 );  /* TBD */
                b->mem = NULL;
                b->short_read_ignore = 1;
                b->uncompressed_siz = real_uncompressed_siz;
                break;
              }

              b->uncompressed_siz += strm.total_out;
              pnt                 += strm.total_out;
              fspos               += clen;

            } else {

              fspos += clen;
              fseeko( lt->handle, fspos, SEEK_SET );

            }

          }

          free_safe( zbuff, 0 );  /* TBD */

        } else {

          unsigned int rv;
          b->mem      = malloc_safe_nolimit( b->uncompressed_siz );
          lt->zhandle = gzdopen( dup( fileno( lt->handle ) ), "rb" );
          rc          = gzread( lt->zhandle, b->mem, b->uncompressed_siz );
          rv          = gzclose( lt->zhandle );
          assert( rv == Z_OK );
          lt->zhandle = NULL;

          if( rc != b->uncompressed_siz ) {
            unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT:  Short read on block %d vs %u (expected), ignoring...", rc, (unsigned int)b->uncompressed_siz );
            assert( rv < USER_MSG_LENGTH );
            print_output( user_msg, WARNING, __FILE__, __LINE__ );
            free_safe( b->mem, 0 );  /* TBD */
            b->mem = NULL;
            b->short_read_ignore = 1;
          } else {
            lt->block_mem_consumed += b->uncompressed_siz;
          }

        }

        bfinal   = b;
        blkfinal = blk;

      }

      if( b->mem ) {

        Try {
          (void)lxt2_rd_process_block( lt, b );
        } Catch_anonymous {
          free_safe( b->mem, b->uncompressed_siz );
          Throw 0;
        }

        if( striped_kill ) {
          free_safe( b->mem, b->uncompressed_siz );
          b->mem = NULL;
          b->uncompressed_siz = real_uncompressed_siz;
        } else if( lt->numblocks > 1 ) {	/* no sense freeing up the single block case */
          if( lt->block_mem_consumed > lt->block_mem_max ) {
            lt->block_mem_consumed -= b->uncompressed_siz;
            free_safe( b->mem, b->uncompressed_siz );
            b->mem = NULL;
          }
        }

      }

      blk++;
      b = b->next;

    }

  }

#ifdef DEBUG_MODE
  if( debug_mode ) {
    if( bcutoff && (bfinal != bcutoff) ) {
      unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "LXT:  Block [%d] processed "LXT2_RD_LLD" / "LXT2_RD_LLD"", blkfinal, bfinal->start, bfinal->end );
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, DEBUG, __FILE__, __LINE__ );
    }
  }
#endif

  return( blk );

}
#endif


/*
 * callback access to the user callback data pointer (if required)
 */ 
_LXT2_RD_INLINE void* lxt2_rd_get_user_callback_data_pointer( struct lxt2_rd_trace* lt ) {

  if( lt ) {
    return( lt->user_callback_data_pointer );
  } else {
    return( NULL );
  }

}

/*!
 \return Returns the number of active blocks within the time range

 Limits access to a given timerange in the LXT file
*/
unsigned int lxt2_rd_limit_time_range(
  struct lxt2_rd_trace* lt,         /*!< Pointer to LXT read tracer */
  lxtint64_t            strt_time,  /*!< Starting time to limit access to */
  lxtint64_t            end_time    /*!< Ending time to limit access to */
) { PROFILE(LXT2_RD_LIMIT_TIME_RANGE);

  lxtint64_t tmp_time;
  int        blk = 0;

  if( lt ) {

    struct lxt2_rd_block* b     = lt->block_head;
    struct lxt2_rd_block* bprev = NULL;
    int                   state = 0;

    if( strt_time > end_time ) {
      tmp_time  = strt_time;
      strt_time = end_time;
      end_time  = tmp_time;
    }

    while( b ) {

      switch( state ) {
        case 0 :
          if( b->end >= strt_time ) {
            state = 1;
            if( (b->start > strt_time) && bprev ) {
              bprev->exclude_block = 0;	
              blk++;
            }
          }
          break;
        case 1 :
          if( b->start > end_time ) {
            state = 2;
          }
          break;
        default:  break;
      }

      if( (state == 1) && !b->short_read_ignore ) {
        b->exclude_block = 0;
        blk++;
      } else {
        b->exclude_block = 1;
      }

      bprev = b;
      b = b->next;

    }

  }

  PROFILE_END;

  return( blk );

}

/*!
 \return Returns the number of active blocks

 Unrestricts access to the whole LXT file
*/
unsigned int lxt2_rd_unlimit_time_range(
  struct lxt2_rd_trace* lt  /*!< Pointer to LXT read tracer */
) { PROFILE(LXT2_RD_UNLIMIT_TIME_RANGE);

  int blk = 0;

  if( lt ) {

    struct lxt2_rd_block *b = lt->block_head;

    while( b ) {
      b->exclude_block = 0;
      if( !b->short_read_ignore ) {
        blk++;
      }
      b = b->next;
    }

  }

  PROFILE_END;

  return( blk );

}


