/*
    PURPOSE: (Test if item is an STL)
*/

#ifndef CHECKPOINT_IS_STL_CONTAINER
#define CHECKPOINT_IS_STL_CONTAINER

#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <utility>

template <typename T>
struct is_stl_container {
  static const bool value = false;
};

template <typename T,typename Alloc>
struct is_stl_container<std::vector<T,Alloc> > {
  static const bool value = true;
};

template <typename T,typename Alloc>
struct is_stl_container<std::list<T,Alloc> > {
  static const bool value = true;
};

template <typename T,typename Alloc>
struct is_stl_container<std::deque<T,Alloc> > {
  static const bool value = true;
};

template <typename T,typename Compare,typename Alloc>
struct is_stl_container<std::set<T,Compare,Alloc> > {
  static const bool value = true;
};

template <typename T,typename Compare,typename Alloc>
struct is_stl_container<std::multiset<T,Compare,Alloc> > {
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

#endif

