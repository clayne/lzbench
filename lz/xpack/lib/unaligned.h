/*
 * unaligned.h - inline functions for unaligned memory accesses
 */

#ifndef LIB_UNALIGNED_H
#define LIB_UNALIGNED_H

#include "xpack/common/common_defs.h"

/*
 * Naming note:
 *
 * {load,store}_*_unaligned() deal with raw bytes without endianness conversion.
 * {get,put}_unaligned_*() deal with a specific endianness.
 */

DEFINE_UNALIGNED_TYPE(u16)
DEFINE_UNALIGNED_TYPE(u32)
DEFINE_UNALIGNED_TYPE(u64)
DEFINE_UNALIGNED_TYPE(machine_word_t)

#define load_word_unaligned	load_machine_word_t_unaligned
#define store_word_unaligned	store_machine_word_t_unaligned

/***** Unaligned loads  *****/

static forceinline u16
get_unaligned_le16(const u8 *p)
{
	if (UNALIGNED_ACCESS_IS_FAST)
		return le16_bswap(load_u16_unaligned(p));
	else
		return ((u16)p[1] << 8) | p[0];
}

static forceinline u32
get_unaligned_le32(const u8 *p)
{
	if (UNALIGNED_ACCESS_IS_FAST)
		return le32_bswap(load_u32_unaligned(p));
	else
		return ((u32)p[3] << 24) | ((u32)p[2] << 16) |
			((u32)p[1] << 8) | p[0];
}

static forceinline u64
get_unaligned_le64(const u8 *p)
{
	if (UNALIGNED_ACCESS_IS_FAST)
		return le64_bswap(load_u64_unaligned(p));
	else
		return ((u64)p[7] << 56) | ((u64)p[6] << 48) |
			((u64)p[5] << 40) | ((u64)p[4] << 32) |
			((u64)p[3] << 24) | ((u64)p[2] << 16) |
			((u64)p[1] << 8) | p[0];
}

static forceinline machine_word_t
get_unaligned_leword(const u8 *p)
{
	STATIC_ASSERT(WORDBYTES == 4 || WORDBYTES == 8);
	if (WORDBYTES == 4)
		return get_unaligned_le32(p);
	else
		return get_unaligned_le64(p);
}

/***** Unaligned stores  *****/

static forceinline void
put_unaligned_le16(u16 v, u8 *p)
{
	if (UNALIGNED_ACCESS_IS_FAST) {
		store_u16_unaligned(le16_bswap(v), p);
	} else {
		p[0] = (u8)(v >> 0);
		p[1] = (u8)(v >> 8);
	}
}

static forceinline void
put_unaligned_le32(u32 v, u8 *p)
{
	if (UNALIGNED_ACCESS_IS_FAST) {
		store_u32_unaligned(le32_bswap(v), p);
	} else {
		p[0] = (u8)(v >> 0);
		p[1] = (u8)(v >> 8);
		p[2] = (u8)(v >> 16);
		p[3] = (u8)(v >> 24);
	}
}

static forceinline void
put_unaligned_le64(u64 v, u8 *p)
{
	if (UNALIGNED_ACCESS_IS_FAST) {
		store_u64_unaligned(le64_bswap(v), p);
	} else {
		p[0] = (u8)(v >> 0);
		p[1] = (u8)(v >> 8);
		p[2] = (u8)(v >> 16);
		p[3] = (u8)(v >> 24);
		p[4] = (u8)(v >> 32);
		p[5] = (u8)(v >> 40);
		p[6] = (u8)(v >> 48);
		p[7] = (u8)(v >> 56);
	}
}

static forceinline void
put_unaligned_leword(machine_word_t v, u8 *p)
{
	STATIC_ASSERT(WORDBITS == 32 || WORDBITS == 64);
	if (WORDBITS == 32)
		put_unaligned_le32(v, p);
	else
		put_unaligned_le64(v, p);
}

/***** 24-bit loads *****/

/*
 * Given a 32-bit value that was loaded with the platform's native endianness,
 * return a 32-bit value whose high-order 8 bits are 0 and whose low-order 24
 * bits contain the first 3 bytes, arranged in octets in a platform-dependent
 * order, at the memory location from which the input 32-bit value was loaded.
 */
static forceinline u32
loaded_u32_to_u24(u32 v)
{
	if (CPU_IS_LITTLE_ENDIAN())
		return v & 0xFFFFFF;
	else
		return v >> 8;
}

/*
 * Load the next 3 bytes from the memory location @p into the 24 low-order bits
 * of a 32-bit value.  The order in which the 3 bytes will be arranged as octets
 * in the 24 bits is platform-dependent.  At least LOAD_U24_REQUIRED_NBYTES
 * bytes must be available at @p; note that this may be more than 3.
 */
static forceinline u32
load_u24_unaligned(const u8 *p)
{
#if UNALIGNED_ACCESS_IS_FAST
#  define LOAD_U24_REQUIRED_NBYTES 4
	return loaded_u32_to_u24(load_u32_unaligned(p));
#else
#  define LOAD_U24_REQUIRED_NBYTES 3
	if (CPU_IS_LITTLE_ENDIAN())
		return ((u32)p[0] << 0) | ((u32)p[1] << 8) | ((u32)p[2] << 16);
	else
		return ((u32)p[2] << 0) | ((u32)p[1] << 8) | ((u32)p[0] << 16);
#endif
}

#endif /* LIB_UNALIGNED_H */
