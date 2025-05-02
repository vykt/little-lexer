#pragma once

#ifdef DEBUG
#define _PRIVATE public
#define _PROTECTED public
#define _STATIC
#else
#define _PRIVATE private
#define _PROTECTED protected
#define _STATIC static
#endif
