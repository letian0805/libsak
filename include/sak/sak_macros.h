#ifndef __SAK_MACROS_H
#ifndef __SAK_DEV
#error Do not include this file, please include sak.h instead!!!
#endif

#define __SAK_MACROS_H

#ifndef MAX
#define MAX(_x, _y) (((_x)>(_y))?(_x):(_y))
#endif
#ifndef MIN
#define MIN(_x, _y) (((_x)<(_y))?(_x):(_y))
#endif

#define FLAG(_bit) (((ull)(_bit))?(1ull << ((ull)(_bit) - 1)):0)
#define MASK(_from, _to) ((1ull << (ull)(_to)) - FLAG(_from))
#define SET_BIT(_num, _bit) ((_num) |= FLAG(_bit))
#define SET_BITS(_num, _from, _to) ((_num) |= MASK(_from, _to))
#define CLEAR_BIT(_num, _bit) ((_num) &= ~FLAG(_bit))
#define CLEAR_BITS(_num, _from, _to) ((_num) &= ~MASK(_from, _to))

#endif //!__SAK_MACROS_H
