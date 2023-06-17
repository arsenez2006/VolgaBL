#include <bl/types.h>

typedef union {
    uint64_t u64;
    int64_t s64;
    struct {
        uint32_t lo; uint32_t hi;
    } u32;
    struct {
        int32_t lo; int32_t hi;
    } s32;
} gcc64;

static uint32_t __inline__ gcc64_hi(uint64_t n) {
    gcc64 w;
    w.u64 = n;
    return w.u32.hi;
}

static uint32_t __inline__ gcc64_lo(uint64_t n) {
    gcc64 w;
    w.u64 = n;
    return w.u32.lo;
}

#define _neg(a, b) (((a) ^ ((((int64_t)(b)) >= 0) - 1)) + (((int64_t)(b)) < 0))
#define _abs(a) _neg(a, a)

int64_t __absvdi2(int64_t a) {
    return _abs(a);
}

int64_t __ashldi3(int64_t a, int b) {
    gcc64 w;
    w.s64 = a;

    b &= 63;

    if (b >= 32) {
        w.u32.hi = w.u32.lo << (b - 32);
        w.u32.lo = 0;
    } else if (b) {
        w.u32.hi = (w.u32.lo >> (32 - b)) | (w.u32.hi << b);
        w.u32.lo <<= b;
    }

    return w.s64;
}

int64_t __ashrdi3(int64_t a, int b) {
    gcc64 w;
    w.s64 = a;

    b &= 63;

    if (b >= 32) {
        w.s32.lo = w.s32.hi >> (b - 32);
        w.s32.hi >>= 31;
    } else if (b) {
        w.u32.lo = (w.u32.hi << (32 - b)) | (w.u32.lo >> b);
        w.s32.hi >>= b;
    }

    return w.s64;
}

int __clzsi2(uint32_t a) {
    int b, n = 0;
    b = !(a & 0xffff0000) << 4; n += b; a <<= b;
    b = !(a & 0xff000000) << 3; n += b; a <<= b;
    b = !(a & 0xf0000000) << 2; n += b; a <<= b;
    b = !(a & 0xc0000000) << 1; n += b; a <<= b;
    return n + !(a & 0x80000000);
}

int __clzdi2(uint64_t a) {
    int b, n = 0;
    b = !(a & 0xffffffff00000000ULL) << 5; n += b; a <<= b;
    b = !(a & 0xffff000000000000ULL) << 4; n += b; a <<= b;
    b = !(a & 0xff00000000000000ULL) << 3; n += b; a <<= b;
    b = !(a & 0xf000000000000000ULL) << 2; n += b; a <<= b;
    b = !(a & 0xc000000000000000ULL) << 1; n += b; a <<= b;
    return n + !(a & 0x8000000000000000ULL);
}

int __ctzsi2(uint32_t a) {
    int b, n = 0;
    b = !(a & 0x0000ffff) << 4; n += b; a >>= b;
    b = !(a & 0x000000ff) << 3; n += b; a >>= b;
    b = !(a & 0x0000000f) << 2; n += b; a >>= b;
    b = !(a & 0x00000003) << 1; n += b; a >>= b;
    return n + !(a & 0x00000001);
}

int __ctzdi2(uint64_t a) {
    int b, n = 0;
    b = !(a & 0x00000000ffffffffULL) << 5; n += b; a >>= b;
    b = !(a & 0x000000000000ffffULL) << 4; n += b; a >>= b;
    b = !(a & 0x00000000000000ffULL) << 3; n += b; a >>= b;
    b = !(a & 0x000000000000000fULL) << 2; n += b; a >>= b;
    b = !(a & 0x0000000000000003ULL) << 1; n += b; a >>= b;
    return n + !(a & 0x0000000000000001ULL);
}

uint64_t __divmoddi4(uint64_t a, uint64_t b, uint64_t *c) {
    char bits;
    uint64_t rem;
    uint64_t wrap;

    if (b > a) {
        if (c) *c = a;
        return 0;
    }
    if (!gcc64_hi(b)) {
        if (b == 0) {
            volatile char x = 0; x = 1 / x;
        }
        if (b == 1) {
            if (c) *c = 0;
            return a;
        }
        if (!gcc64_hi(a)) {
            if (c)
                *c = gcc64_lo(a) % gcc64_lo(b);
            return gcc64_lo(a) / gcc64_lo(b);
        }
    }

    bits = __clzdi2(b) - __clzdi2(a) + 1;
    rem = a >> bits;
    a <<= 64 - bits;
    wrap = 0;
    while (bits-- > 0) {
        rem = (rem << 1) | (a >> 63);
        a = (a << 1) | (wrap & 1);
        wrap = ((int64_t)(b - rem - 1) >> 63);
        rem -= b & wrap;
    }
    if (c) *c = rem;
    return (a << 1) | (wrap & 1);
}

int64_t __divdi3(int64_t a, int64_t b) {
    uint64_t q = __divmoddi4(_abs(a), _abs(b), (void *)0);
    return _neg(q, a ^ b);
}

int __ffsdi2(uint64_t a) {
    return a ? __ctzdi2(a) + 1 : 0;
}

uint64_t __lshrdi3(uint64_t a, int b) {
    gcc64 w;
    w.u64 = a;

    b &= 63;

    if (b >= 32)
    {
        w.u32.lo = w.u32.hi >> (b - 32);
        w.u32.hi = 0;
    } else if (b)
    {
        w.u32.lo = (w.u32.hi << (32 - b)) | (w.u32.lo >> b);
        w.u32.hi >>= b;
    }
    return w.u64;
}

int64_t __moddi3(int64_t a, int64_t b) {
    uint64_t r;
    __divmoddi4(_abs(a), _abs(b), &r);
    return _neg(r, a);
}

int __popcountsi2(uint32_t a) {
    a = a - ((a >> 1) & 0x55555555);
    a = ((a >> 2) & 0x33333333) + (a & 0x33333333);
    a = (a + (a >> 4)) & 0x0F0F0F0F;
    a = (a + (a >> 16));
    return (a + (a >> 8)) & 63;
}

int __popcountdi2(uint64_t a) {
    a = a - ((a >> 1) & 0x5555555555555555ULL);
    a = ((a >> 2) & 0x3333333333333333ULL) + (a & 0x3333333333333333ULL);
    a = (a + (a >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    a = (a + (a >> 32));
    a = (a + (a >> 16));
    return (a + (a >> 8)) & 127;
}

uint64_t __udivdi3(uint64_t a, uint64_t b) {
    return __divmoddi4(a, b, (void *)0);
}

uint64_t __umoddi3(uint64_t a, uint64_t b) {
    uint64_t r;
    __divmoddi4(a, b, &r);
    return r;
}
