#ifndef MYSTL_MEMORY_H_
#define MYSTL_MEMORY_H_

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "tempbuf.h"
#include "uninitialized.h"

namespace mystl
{

    //模板类smart_ptr，小型智能指针
    template <class T>
    class smart_ptr
    {
    public:
        typedef T elem_type;

    private:
        T *m_ptr; // 实际指针

    public:
        // 构造、复制、析构函数
        explicit smart_ptr(T *p = nullptr) : m_ptr(p) {}
        smart_ptr(smart_ptr &rhs) : m_ptr(rhs.release()) {}
        template <class U>
        smart_ptr(smart_ptr<U> &rhs) : m_ptr(rhs.release()) {}

        smart_ptr &operator=(smart_ptr &rhs)
        {
            if (this != &rhs)
            {
                delete m_ptr;
                m_ptr = rhs.release();
            }
            return *this;
        }
        template <class U>
        smart_ptr &operator=(smart_ptr<U> &rhs)
        {
            if (this->get() != rhs.get())
            {
                delete m_ptr;
                m_ptr = rhs.release();
            }
            return *this;
        }

        ~smart_ptr() { delete m_ptr; }

    public:
        // 重载 operator* 和 operator->
        T &operator*() const { return *m_ptr; }
        T *operator->() const { return m_ptr; }

        // 获得指针
        T *get() const { return m_ptr; }

        // 释放指针
        T *release()
        {
            T *tmp = m_ptr;
            m_ptr = nullptr;
            return tmp;
        }

        // 重置指针
        void reset(T *p = nullptr)
        {
            if (m_ptr != p)
            {
                delete m_ptr;
                m_ptr = p;
            }
        }
    };

}

#endif