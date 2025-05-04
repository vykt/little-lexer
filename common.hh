#pragma once

#ifdef DEBUG
#define _PRIVATE public
#define _PROTECTED public
#define _STATIC
#define _INLINE
#else
#define _PRIVATE private
#define _PROTECTED protected
#define _STATIC static
#define _INLINE inline
#endif
