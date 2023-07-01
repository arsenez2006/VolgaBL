#ifndef BL_DEFINES_H
#define BL_DEFINES_H

#define __noreturn __attribute__((noreturn))
#define __packed __attribute__((packed))
#define __unused __attribute__((unused))
#define __print_fmt(fmt, va) __attribute__((format(printf, fmt, va)))
#define __check_ret __attribute__((warn_unused_result))
#define __align(n) __attribute__((aligned(n)))

#endif /* BL_DEFINES_H */
