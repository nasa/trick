/*
    PURPOSE: (Test if item is an STL)
*/

#ifndef CHECKPOINT_IS_STL_CONTAINER
#define CHECKPOINT_IS_STL_CONTAINER

#include <array>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <utility>

template <typename T>
struct is_stl_container {
  static const bool value = false;
};

template <typename T,std::size_t N>
struct is_stl_container<std::array<T,N> > {
  static const bool value = true;
};

template <typename T,typename _Alloc>
struct is_stl_container<std::vector<T,_Alloc> > {
  static const bool value = true;
};

template <typename T,typename _Alloc>
struct is_stl_container<std::list<T,_Alloc> > {
  static const bool value = true;
};

template <typename T,typename _Alloc>
struct is_stl_container<std::deque<T,_Alloc> > {
  static const bool value = true;
};

template <typename T,typename _Compare,typename _Alloc>
struct is_stl_container<std::set<T,_Compare,_Alloc> > {
  static const bool value = true;
};

template <typename T,typename _Compare,typename _Alloc>
struct is_stl_container<std::multiset<T,_Compare,_Alloc> > {
  static const bool value = true;
};

template <typename _Key, typename _Tp , typename _Compare, typename _Alloc>
struct is_stl_container<std::map<_Key,_Tp ,_Compare,_Alloc> > {
  static const bool value = true;
};

template <typename _Key, typename _Tp , typename _Compare, typename _Alloc>
struct is_stl_container<std::multimap<_Key,_Tp ,_Compare,_Alloc> > {
  static const bool value = true;
};

template <typename _T1,typename _T2>
struct is_stl_container<std::pair<_T1,_T2> > {
  static const bool value = true;
};

template <typename _Tp, typename _Sequence>
struct is_stl_container<std::queue<_Tp,_Sequence> > {
  static const bool value = true;
};

template <typename _Tp, typename _Container, typename _Compare>
struct is_stl_container<std::priority_queue<_Tp,_Container,_Compare> > {
  static const bool value = true;
};

template <typename _Tp, typename _Sequence>
struct is_stl_container<std::stack<_Tp,_Sequence> > {
  static const bool value = true;
};

#endif

