#define AMD64

#ifdef _NOSTD
#include "syscalls.h"

#ifdef   __cplusplus
# define __BEGIN_DECLS  extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

#define BUFSIZ    8192
#define _NFILEMAX 20

#endif /* _NOSTD */

typedef long int            i64;
typedef int                 i32;
typedef short int           i16;
typedef signed char         i8;

typedef long unsigned int   u64;
typedef unsigned int        u32;
typedef unsigned short int  u16;
typedef unsigned char       u8;

typedef long int            ifast64;
typedef long int	        ifast32;
typedef long int	        ifast16;
typedef signed char         ifast8;

typedef unsigned long int	ufast64;
typedef unsigned long int	ufast32;
typedef unsigned long int	ufast16;
typedef unsigned char		ufast8;

typedef long int            ileast64;
typedef int                 ileast32;
typedef short int           ileast16;
typedef signed char         ileast8;

typedef unsigned long int   uleast64;
typedef unsigned int        uleast32;
typedef unsigned short int  uleast16;
typedef unsigned char       uleast8;


typedef ifast64 intptr;
typedef ufast64 uintptr;

typedef intptr  ssize_t;
typedef uintptr  size_t;
typedef intptr   time_t;
typedef u32      mode_t;
typedef i64      off_t;

typedef i64 b60;
typedef i32 b32;
typedef i16 b16;
typedef i8  b8;

typedef ifast64 align;


#ifdef _NOSTD /* TODO: wip */
#  include "../std.c"
#  include <termios.h>
#  include <sys/ioctl.h>

extern __const u16 **__ctype_b_loc (void) __attribute__ ((__const));
#define __isctype(c, t) ((*__ctype_b_loc())[(int)(c)]&(u16)t)

#else 

#  include <termios.h>
#  include <time.h>
#  include <ctype.h>
#  include <errno.h> 
#  include <fcntl.h> 
#  include <unistd.h>
#  include <stdlib.h>
#  include <stdio.h>
#  include <string.h> 
#  include <stdarg.h>
#  include <sys/types.h>
#  include <sys/ioctl.h>

typedef FILE fd_t;

#endif /* _NOSTD */


#include "../plethora.h"
#include "../editor.c"
