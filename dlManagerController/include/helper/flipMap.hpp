#ifndef FLIPMAP_HPP
#include <map>
#include <algorithm>

template<typename A, typename B> std::pair<B, A> flipPair(const std::pair<A, B>& p)
{
    return std::pair<B, A>(p.second, p.first);
}

template<typename A, typename B> std::map<B, A> flipMap(const std::map<A, B>& src)
{
    std::map<B, A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), flipPair<A, B>);
    return dst;
}
#endif
