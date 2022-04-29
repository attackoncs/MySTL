#ifndef MYSTL_TEMPBUF_H_
#define MYSTL_TEMPBUF_H_

//临时缓冲区的申请与释放

#include <cstddef>
#include <cstdlib>
#include <climits>

#include "type_traits.h"
#include "uninitialized.h"

namespace mystl
{
    template <class T>
    pair<T *, ptrdiff_t> get_temporary_buffer(ptrdiff_t len)
    {
        return __get_temporary_buffer(len, static_cast<T *>(0));
    }

    template <class T>
    pair<T *, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T *)
    {
        return __get_temporary_buffer(len, static_cast<T *>(0));
    }

    template <class T>
    pair<T *, ptrdiff_t> __get_temporary_buffer(ptrdiff_t len, T *)
    {
        if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
            len = INT_MAX / sizeof(T);
        while (len > 0)
        {
            T *tmp = static_cast<T *>(malloc(static_cast<size_T>(len) * sizeof(T)));
            if (tmp)
                return pair<T *, ptrdiff_t>(tmp, len); //成功就返回
            len /= 2;                                  //失败则减少
        }
        return pair<T *, ptrdiff_t>((static_cast<T *>(0), static_cast<ptrdiff_t>(0)));
    }

    //释放指针指向的空间
    template <class T>
    void return_temporary_buffer(T *ptr)
    {
        free(ptr);
    }

    template <class ForwareIterator, class T>
    class temporary_buffer
    {
    private:
        ptrdiff_t original_len; //缓冲区申请的大小
        ptrdiff_t len;          //缓冲区实际的大小
        T *buffer;              //指向缓冲区的指针
    public:
        temporary_buffer(ForwareIterator first, ForwareIterator last);

        ~temporary_buffer()
        {
            mystl::destroy(buffer, buffer + len);
            free(buffer);
        }

    public:
        ptrdiff_t size() const { return len; }
        ptrdiff_t requested_size() const { return original_len; }
        T *begin() { return buffer; }
        T *end() { return buffer + len; }

    private:
        void __allocate_buffer();
        void __initialize_buffer(const T &, __true_bype) {}
        void __initialize_buffer(const T &value, __false_type) { mystl::uninitialized_fill_n(buffer, len, value); }

    private:
        temporary_buffer(const temporary_buffer &) {}
        void operator=(const temporary_buffer &) {}
    };

// 构造函数
template <class ForwardIterator, class T>
temporary_buffer<ForwardIterator, T>::
    temporary_buffer(ForwardIterator first, ForwardIterator last) {
    typedef typename __type_traits<T>::has_trivial_default_constructor Trivial;
    try {
        len = distance(first, last);
        __allocate_buffer();
        if (len > 0) {
            __initialize_buffer(*first, Trivial());
        }
    }
    catch (...) {
        free(buffer);
        buffer = nullptr;
        len = 0;
    }
}

    //__cllocate_buffer函数
    template <class ForwardIterator, class T>
    void temporary_buffer<ForwardIterator, T>::__allocate_buffer()
    {
        original_len = len;
        if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
            len = INT_MAX / sizeof(T);
        while (len > 0)
        {
            buffer = static_cast<T *>(malloc(len * sizeof(T)));
            if (buffer)
                break;
            len /= 2; //申请失败则减少申请空间
        }
    }
}

#endif