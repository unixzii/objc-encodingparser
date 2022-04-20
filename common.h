//
//  common.h
//  ObjcEncodingParser
//
//  Created by Cyandev on 2022/4/20.
//

#ifndef common_h
#define common_h

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define CONFIG_WIN32 1
#endif

#if defined(CONFIG_WIN32)
    #define ALWAYS_INLINE __inline
    #define NO_INLINE __declspec(noinline)
    #define UNUSED
#else
    #define ALWAYS_INLINE inline __attribute__((always_inline))
    #define NO_INLINE __attribute__((noinline))
    #define UNUSED __attribute__((unused))
#endif

#endif /* common_h */
