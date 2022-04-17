#ifndef MYSTL_UNINITIALIZED_H_
#define MYSTL_UNINITIALIZED_H_

//用于对未初始化空间构造元素

#include <cstring>

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"

namespace mystl
{

    // uninitialized_copy 把[first,last)上内容复制到result为起始处的空间，返回一个迭代器指向赋值结束的尾部
    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type)
    {
        return mystl::copy(first, last, result);
    }

    template <class InputIterator, class ForwardsIterator>
    inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type)
    {
        ForwardIterator cur = result;
        for (; first != last; ++first, ++cur)
        {
            mystl::construct(&*cur, *first);
        }
        return cur;
    }

    template <class InputIterator, class ForwardIterator, class T>
    inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last,
                                                ForwardIterator result, T *)
    {
        typedef typename __type_traits<T>::is_POD_type PODType;
        return mystl::__uninitialized_copy_aux(first, last, result, PODType());
    }

    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
                                              ForwardIterator result)
    {
        return mystl::__uninitialized_copy(first, last, result, value_type(result));
    }

    // 针对 char* 和 wchar_t* 的特化版本
    inline char *uninitialized_copy(const char *first, const char *last, char *result)
    {
        std::memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t *uninitialized_copy(const wchar_t *first, const wchar_t *last, wchar_t *result)
    {
        std::memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }

    // uninitialized_fill，再区间[first,last)内填充元素值
    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __true_type)
    {
        mystl::fill(first, last, x);
    }

    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __false_type)
    {
        auto cur = first;
        for (; cur != last; ++cur)
        {
            mystl::construct(&*cur, x);
        }
    }

    template <class ForwardIterator, class T, class Ty>
    inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x, Ty *)
    {
        typedef typename __type_traits<Ty>::is_POD_type PODType;
        mystl::__uninitialized_fill_aux(first, last, x, PODType());
    }

    template <class ForwardIterator, class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x)
    {
        mystl::__uninitialized_fill(first, last, x, value_type(first));
    }

    // uninitialized_fill_n
    // first开始，填充n个元素，返回一个迭代器指向填充结束的位置
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __true_type)
    {
        return mystl::fill_n(first, n, x);
    }

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __false_type)
    {
        auto cur = first;
        for (; n > 0; --n, ++cur)
        {
            mystl::construct(&*cur, x);
        }
        return cur;
    }

    template <class ForwardIterator, class Size, class T, class Ty>
    inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T &x, Ty *)
    {
        typedef typename __type_traits<Ty>::is_POD_type PODType;
        return mystl::__uninitialized_fill_n_aux(first, n, x, PODType());
    }

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &x)
    {
        return mystl::__uninitialized_fill_n(first, n, x, value_type(first));
    }

}

#endif