//  (C) Copyright John Maddock 2001.
//  (C) Copyright Jens Maurer 2001.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  config for libstdc++ v3
//  not much to go in here:

#define GECODE_BOOST_GNU_STDLIB 1

#ifdef __GLIBCXX__
#define GECODE_BOOST_STDLIB "GNU libstdc++ version " GECODE_BOOST_STRINGIZE(__GLIBCXX__)
#else
#define GECODE_BOOST_STDLIB "GNU libstdc++ version " GECODE_BOOST_STRINGIZE(__GLIBCPP__)
#endif

#if !defined(_GLIBCPP_USE_WCHAR_T) && !defined(_GLIBCXX_USE_WCHAR_T)
#  define GECODE_BOOST_NO_CWCHAR
#  define GECODE_BOOST_NO_CWCTYPE
#  define GECODE_BOOST_NO_STD_WSTRING
#  define GECODE_BOOST_NO_STD_WSTREAMBUF
#endif

#if defined(__osf__) && !defined(_REENTRANT) \
  && ( defined(_GLIBCXX_HAVE_GTHR_DEFAULT) || defined(_GLIBCPP_HAVE_GTHR_DEFAULT) )
// GCC 3 on Tru64 forces the definition of _REENTRANT when any std lib header
// file is included, therefore for consistency we define it here as well.
#  define _REENTRANT
#endif

#ifdef __GLIBCXX__ // gcc 3.4 and greater:
#  if defined(_GLIBCXX_HAVE_GTHR_DEFAULT) \
        || defined(_GLIBCXX__PTHREADS) \
        || defined(_GLIBCXX_HAS_GTHREADS) \
        || defined(_WIN32)
      //
      // If the std lib has thread support turned on, then turn it on in Boost
      // as well.  We do this because some gcc-3.4 std lib headers define _REENTANT
      // while others do not...
      //
#     define GECODE_BOOST_HAS_THREADS
#  else
#     define GECODE_BOOST_DISABLE_THREADS
#  endif
#elif defined(__GLIBCPP__) \
        && !defined(_GLIBCPP_HAVE_GTHR_DEFAULT) \
        && !defined(_GLIBCPP__PTHREADS)
   // disable thread support if the std lib was built single threaded:
#  define GECODE_BOOST_DISABLE_THREADS
#endif

#if (defined(linux) || defined(__linux) || defined(__linux__)) && defined(__arm__) && defined(_GLIBCPP_HAVE_GTHR_DEFAULT)
// linux on arm apparently doesn't define _REENTRANT
// so just turn on threading support whenever the std lib is thread safe:
#  define GECODE_BOOST_HAS_THREADS
#endif

#if !defined(_GLIBCPP_USE_LONG_LONG) \
    && !defined(_GLIBCXX_USE_LONG_LONG)\
    && defined(GECODE_BOOST_HAS_LONG_LONG)
// May have been set by compiler/*.hpp, but "long long" without library
// support is useless.
#  undef GECODE_BOOST_HAS_LONG_LONG
#endif

// Apple doesn't seem to reliably defined a *unix* macro
#if !defined(CYGWIN) && (  defined(__unix__)  \
                        || defined(__unix)    \
                        || defined(unix)      \
                        || defined(__APPLE__) \
                        || defined(__APPLE)   \
                        || defined(APPLE))
#  include <unistd.h>
#endif

#if defined(__GLIBCXX__) || (defined(__GLIBCPP__) && __GLIBCPP__>=20020514) // GCC >= 3.1.0
#  define GECODE_BOOST_STD_EXTENSION_NAMESPACE __gnu_cxx
#  define GECODE_BOOST_HAS_SLIST
#  define GECODE_BOOST_HAS_HASH
#  define GECODE_BOOST_SLIST_HEADER <ext/slist>
# if !defined(__GNUC__) || __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)
#   define GECODE_BOOST_HASH_SET_HEADER <ext/hash_set>
#   define GECODE_BOOST_HASH_MAP_HEADER <ext/hash_map>
# else
#   define GECODE_BOOST_HASH_SET_HEADER <backward/hash_set>
#   define GECODE_BOOST_HASH_MAP_HEADER <backward/hash_map>
# endif
#endif

//  stdlibc++ C++0x support is detected via __GNUC__, __GNUC_MINOR__, and possibly
//  __GNUC_PATCHLEVEL__ at the suggestion of Jonathan Wakely, one of the stdlibc++
//  developers. He also commented:
//
//       "I'm not sure how useful __GLIBCXX__ is for your purposes, for instance in
//       GCC 4.2.4 it is set to 20080519 but in GCC 4.3.0 it is set to 20080305.
//       Although 4.3.0 was released earlier than 4.2.4, it has better C++0x support
//       than any release in the 4.2 series."
//
//  Another resource for understanding stdlibc++ features is:
//  http://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html#manual.intro.status.standard.200x

//  C++0x headers in GCC 4.3.0 and later
//
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3) || !defined(__GXX_EXPERIMENTAL_CXX0X__)
#  define GECODE_BOOST_NO_0X_HDR_ARRAY
#  define GECODE_BOOST_NO_0X_HDR_REGEX
#  define GECODE_BOOST_NO_0X_HDR_TUPLE
#  define GECODE_BOOST_NO_STD_UNORDERED  // deprecated; see following
#  define GECODE_BOOST_NO_0X_HDR_UNORDERED_MAP
#  define GECODE_BOOST_NO_0X_HDR_UNORDERED_SET
#endif

//  C++0x headers in GCC 4.4.0 and later
//
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 4) || !defined(__GXX_EXPERIMENTAL_CXX0X__)
#  define GECODE_BOOST_NO_0X_HDR_CHRONO
#  define GECODE_BOOST_NO_0X_HDR_CONDITION_VARIABLE
#  define GECODE_BOOST_NO_0X_HDR_FORWARD_LIST
#  define GECODE_BOOST_NO_0X_HDR_INITIALIZER_LIST
#  define GECODE_BOOST_NO_0X_HDR_MUTEX
#  define GECODE_BOOST_NO_0X_HDR_RATIO
#  define GECODE_BOOST_NO_0X_HDR_SYSTEM_ERROR
#else
#  define GECODE_BOOST_HAS_TR1_COMPLEX_INVERSE_TRIG 
#  define GECODE_BOOST_HAS_TR1_COMPLEX_OVERLOADS 
#endif

#if (!defined(_GLIBCXX_HAS_GTHREADS) || !defined(_GLIBCXX_USE_C99_STDINT_TR1)) && (!defined(GECODE_BOOST_NO_0X_HDR_CONDITION_VARIABLE) || !defined(GECODE_BOOST_NO_0X_HDR_MUTEX))
#  define GECODE_BOOST_NO_0X_HDR_CONDITION_VARIABLE
#  define GECODE_BOOST_NO_0X_HDR_MUTEX
#endif

//  C++0x features in GCC 4.5.0 and later
//
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 5) || !defined(__GXX_EXPERIMENTAL_CXX0X__)
#  define GECODE_BOOST_NO_NUMERIC_LIMITS_LOWEST
#  define GECODE_BOOST_NO_0X_HDR_FUTURE
#  define GECODE_BOOST_NO_0X_HDR_RANDOM
#endif

//  C++0x features in GCC 4.5.0 and later
//
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6) || !defined(__GXX_EXPERIMENTAL_CXX0X__)
#  define GECODE_BOOST_NO_0X_HDR_TYPEINDEX
#endif
//  C++0x headers not yet (fully!) implemented
//
#  define GECODE_BOOST_NO_0X_HDR_THREAD
#  define GECODE_BOOST_NO_0X_HDR_TYPE_TRAITS
#  define GECODE_BOOST_NO_0X_HDR_CODECVT

//  --- end ---
