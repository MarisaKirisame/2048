#ifndef Q2048_GLOBAL_HPP
#define Q2048_GLOBAL_HPP

#include <QtCore/qglobal.h>

#if defined(Q2048_LIBRARY)
#  define Q2048SHARED_EXPORT Q_DECL_EXPORT
#else
#  define Q2048SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // Q2048_GLOBAL_HPP
