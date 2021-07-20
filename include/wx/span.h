#ifndef _WX_SPAN_H_
#define _WX_SPAN_H_

#if __cplusplus >= 202002L

#include <span>
namespace wx
{
template<typename T, size_t Extent = std::dynamic_extent>
using span = std::span<T, Extent>;
}
#else
#include <gsl/span>
namespace wx
{
using span = gsl::span;
}
#endif

#endif // _WX_SPAN_H_BASE_
