#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_

#include "alloc.h"

namespace mystl
{

    //模板类，参数一表示对象类型，如vector、queue等，参数二表示空间配置器类型：
    // allocate:  对象内存分配
    // deallocate:对象内存释放
    template <class T, class Alloc>
    class allocator
    {
    public:
        typedef T value_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        static T *allocate();
        static T *allocate(size_type n);
        static void deallocate(T *ptr);
        static void deallocate(T *ptr, size_type n);
    };

    // 返回分配内存的起始位置
    template <class T, class Alloc>
    T *allocator<T, Alloc>::allocate()
    {
        return static_cast<T *>(Alloc::allocate(sizeof(T)));
    }

    // 表示要分配的对象个数，返回分配内存的起始位置
    template <class T, class Alloc>
    T *allocator<T, Alloc>::allocate(size_type n)
    {
        if (n == 0)
            return nullptr;
        return static_cast<T *>(Alloc::allocate(n * sizeof(T)));
    }

    // 释放内存，无返回值
    template <class T, class Alloc>
    void allocator<T, Alloc>::deallocate(T *ptr)
    {
        if (ptr == nullptr)
            return;
        Alloc::deallocate(ptr, sizeof(T));
    }

    // 释放 n 个对象的内存，无返回值
    template <class T, class Alloc>
    void allocator<T, Alloc>::deallocate(T *ptr, size_type n)
    {
        if (ptr == nullptr)
            return;
        Alloc::deallocate(ptr, n * sizeof(T));
    }

}

#endif