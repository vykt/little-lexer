#pragma once

#ifdef DEBUG
#define _PRIVATE public
#define _PROTECTED public
#else
#define _PRIVATE private
#define _PROTECTED protected
#endif
