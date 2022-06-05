#ifndef UTIL_WARNING_SUPPRESSION_HPP_
#define UTIL_WARNING_SUPPRESSION_HPP_

// Multi-compiler-compatible local warning suppression

#if defined(_MSC_VER)
    #define DISABLE_WARNING_PUSH           __pragma(warning( push ))
    #define DISABLE_WARNING_POP            __pragma(warning( pop ))
    #define DISABLE_WARNING(warningNumber) __pragma(warning( disable : warningNumber ))

    #define DISABLE_WARNING_CLASS_MEMACCESS
    // TODO: find the right warning number for this
    #define DISABLE_WARNING_IGNORED_ATTRIBUTES

#elif defined(__GNUC__) || defined(__clang__)
    #define DO_PRAGMA(X) _Pragma(#X)
    #define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
    #define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop)
    #define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

    #define DISABLE_WARNING_IGNORED_ATTRIBUTES   DISABLE_WARNING(-Wignored-attributes)
#if defined(__clang__)
    #define DISABLE_WARNING_CLASS_MEMACCESS
#else
    #define DISABLE_WARNING_CLASS_MEMACCESS   DISABLE_WARNING(-Wclass-memaccess)
#endif
#else
    #define DISABLE_WARNING_PUSH
    #define DISABLE_WARNING_POP
    #define DISABLE_WARNING_IGNORED_ATTRIBUTES
    #define DISABLE_WARNING_CLASS_MEMACCESS
#endif

#endif // UTIL_WARNING_SUPPRESSION_HPP_

