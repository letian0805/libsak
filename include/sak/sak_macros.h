#ifndef __SAK_MACROS_H
#ifndef __SAK_DEV
#error Do not include this file, please include sak.h instead!!!
#endif

#define __SAK_MACROS_H

#define SAK_MAX(_x, _y) (((_x)>(_y))?(_x):(_y))
#define SAK_MIN(_x, _y) (((_x)<(_y))?(_x):(_y))

#define SAK_FLAG(_bit) (((ull)(_bit))?(1ull << ((ull)(_bit) - 1)):0)
#define SAK_MASK(_from, _to) ((1ull << (ull)(_to)) - SAK_FLAG(_from))
#define SAK_SET_BIT(_num, _bit) ((_num) |= SAK_FLAG(_bit))
#define SAK_SET_BITS(_num, _from, _to) ((_num) |= SAK_MASK(_from, _to))
#define SAK_CLEAR_BIT(_num, _bit) ((_num) &= ~SAK_FLAG(_bit))
#define SAK_CLEAR_BITS(_num, _from, _to) ((_num) &= ~SAK_MASK(_from, _to))

#define SAK_MAKE_INT16(_c1, _c2) ((((uint16_t)(_c1)&0xff) << 8) | ((uint16_t)(_c2)&0xff))
#define SAK_MAKE_INT32(_c1, _c2, _c3, _c4) (((uint32_t)SAK_MAKE_INT16(_c1, _c2) << 16) | (uint32_t)SAK_MAKE_INT16(_c3, _c4))
#define SAK_MAKE_INT64(_c1, _c2, _c3, _c4, _c5, _c6, _c7, _c8) \
(((uint64_t)SAK_MAKE_INT32(_c1, _c2, _c3, _c4) << 32) | (uint64_t)SAK_MAKE_INT32(_c5, _c6, _c7, _c8))

#endif //!__SAK_MACROS_H
