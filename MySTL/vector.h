#ifndef MYSTL_VECTOR_H_
#define MYSTL_VECTOR_H_

#include "iterator.h"
#include "memory.h"

namespace mystl
{

    //第一个参数代表数据类型，第二个代表分配器类型，默认alloc
    template <class T, class Alloc = mystl::alloc>
    class vector
    {
    public:
        typedef T value_type;
        typedef Alloc allocator_type;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef value_type &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef value_type *iterator;
        typedef const value_type *const_iterator;
        typedef mystl::reverse_iterator<iterator> reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

        typedef mystl::allocator<T, Alloc> data_allocator;
        allocator_type get_allocator() { return allocator_type(); }

    private:
        iterator start_;         //目前使用空间头
        iterator finish_;        //目前使用空间尾
        iterator end_of_storage; //目前可用空间尾

    public:
        vector() : start_(nullptr), finish_(nullptr), end_of_storage(nullptr) {}
        explicit vector(size_type n) { __fill_initialize(n, value_type()); }
        vector(size_type n, const value_type &value) { __fill_initialize(n, value); }
        template <class InputIterator>
        vector(InputIterator first, InputIterator last);

        vector(const vector &rhs);
        vector(vector &&rhs);

        vector &operator=(const vector &rhs);
        vector &operator=(vector &&rhs);

        ~vector() { __destroy_and_deallocate(); }

    public:
        iterator begin() { return start_; }
        const_iterator begin() const { return start_; }
        iterator end() { return end_; }
        const_iterator end() const { return end_; }
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        bool empty() const { return begin() == end(); }
        size_type size() const { return static_cast<size_type>(end() - begin()); }
        size_type max_size() const { return static_cast<size_type>(-1) / sizeof(T); }
        size_type capacity() const { return static_cast<size_type>(end_of_storage - start_); }
        void shrink_to_fit();
        void reserve(size_type n);

        reference operator[](difference_type n) { return *(begin() + n); }
        const_reference operator[](difference_type n) const { return *(begin() + n); }
        reference at(difference_type n) { return *(this)[n]; }
        const_reference at(difference_type n) const { return *(this)[n]; }
        reference front() const { return *begin(); }
        const_reference front() const { return *begin(); }
        reference back() const { return *(end() - 1); }
        const_reference back() const { return *(end() - 1); }
        pointer data() { return begin(); }
        const_pointer data() const { return begin(); }

        //修改容器相关操作
        void assign(size_type n) { __fill_initialize(n, value_type()); }
        void assign(size_type n, const value_type &value) { __fill_initialize(n, value); }
        template <class InputIterator>
        void assign(InputIterator first, InputIterator last);

        void push_back(const value_type &value);
        void pop_back();

        iterator insert(iterator position);
        iterator insert(iterator position, const value_type &value);
        void insert(iterator position, size_type n, const value_type &value);
        template <class InputIterator>
        void insert(iterator position, InputIterator first, InputIterator last);

        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);
        void clear();
        {
            erase(begin(), end());
        }

        void resieze(size_type new_size) { return resize(new_size, value_type()); }
        void resize(size_type new_size, const value_type &value);
        void reverse() { mystl::reverse(begin(), end()); }
        void swap(vector &rhs);

    private:
        template <class Integer>
        void __vector_initialize(Integer n, Integer value, __true_type);
        template <class InputIterator>
        void __vector_initialize(InputIterator first, InputIterator last, __false_type);
        void __destroy_and_deallocate();
        void __fill_initialize(size_type n, const value_type &value);
        template <class InputIterator>
        void __range_initialize(InputIterator first, InputIterator last);
        template <class Integer>
        void __assign_dispatch(Integer n, Integer value, __true_type);
        template <class InputIterator>
        void __assign_dispatch(InputIterator first, InputIterator last, __false_type);
        void __fill_assign(size_type n, const value_type &value);
        template <class InputIterator>
        void __range_assign(InputIterator first, InputIterator last, input_iterator_tag);
        template <class ForwardIterator>
        void __range_assign(ForwardIterator first, ForwardIterator last, forward_iterator_tag);
        void __insert_aux(iterator position, const value_type &value);
        template <class Integer>
        void __insert_dispatch(iterator position, Integer n, Integer x, __true_type);
        template <class InputIterator>
        void __insert_dispatch(iterator position, InputIterator first, InputIterator last, __false_type);
        void __fill_insert(iterator position, size_type n, const value_type &value);
        template <class InputIterator>
        void __range_insert(iterator position, InputIterator first, InputIterator last, input_iterator_tag);
        template <class ForwardIterator>
        void __range_insert(iterator position, ForwardIterator first, ForwardIterator last, forward_iterator_tag);
    };

    // 构造函数
    template <class T, class Alloc>
    template <class InputIterator>
    vector<T, Alloc>::vector(InputIterator first, InputIterator last)
    {
        typedef typename __is_integer<InputIterator>::is_integer Integer;
        __vector_initialize(first, last, Integer());
    }

    // 复制构造函数
    template <class T, class Alloc>
    vector<T, Alloc>::vector(const vector &rhs)
    {
        __range_initialize(rhs.start_, rsh.finish_);
    }

    // 移动构造函数
    template <class T, class Alloc>
    vector<T, Alloc>::vector(vector &&rhs) : start_(rhs.start_),
                                             finish_(rhs.finish_), end_of_storage_(rhs.end_of_storage_)
    {
        rhs.start_ = rhs.finish_ = rhs.end_of_storage_ = nullptr;
    }

    // 复制赋值操作符
    template <class T, class Alloc>
    vector<T, Alloc> &vector<T, Alloc>::operator=(const vector &rhs)
    {
        if (this != &rhs)
        {
            const auto xlen = rhs.size();
            if (xlen > capacity())
            {
                __destroy_and_deallocate();
                __range_initialize(rhs.begin(), rhs.end());
            }
            else if (size() >= xlen)
            { //原vector大于等于要赋值vector大小
                auto i = mystl::copy(rhs.begin(), rhs.end(), begin());
                mystl::destroy(i, finish_);
                finish_ = start_ + xlen;
            }
            else
            { //原vector小于要赋值的vector大小
                auto i = mystl::copy(rhs.begin(), rhs.begin() + size(), start_);
                mystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), finish_;);
                end_of_storage_ = finish_ = start_ + xlen;
            }
        }
        return *this;
    }

    // 移动赋值操作符
    template <class T, class Alloc>
    vector<T, Alloc> &vector<T, Alloc>::operator=(vector &&rhs)
    {
        if (this != &rhs)
        {
            __destroy_and_deallocate();
            start_ = rhs.start_;
            finish_ = rhs.finish_;
            end_of_storage_ = rhs.end_of_storage_;
            rhs.start_ = rhs.finish_ = rhs.end_of_storage_ = nullptr;
        }
        return *this;
    }

    // 放弃多余的容量
    template <class T, class Alloc>
    void vector<T, Alloc>::shrink_to_fit()
    {
        if (finish_ != end_of_storage_)
        {
            data_allocator::deallocate(finish_ + 1, end_of_storage_ - finish_);
            end_of_storage_ = finish_;
        }
    }

    // 预留空间大小，当原容量小于要求大小时，才会重新分配
    template <class T, class Alloc>
    void vector<T, Alloc>::reserve(size_type n)
    {
        if (capacity() < n)
        {
            const auto old_size = size();
            auto tmp = data_allocator::allocate(n);
            mystl::uninitialized_copy(start_, finish_, tmp);
            __destroy_and_deallocate();
            start_ = tmp;
            finish_ = tmp + old_size;
            end_of_storage_ = start_ + n;
        }
    }

    // 初始化容器
    template <class T, class Alloc>
    template <class InputIterator>
    void vector<T, Alloc>::assign(InputIterator first, InputIterator last)
    {
        typedef typename __is_integer<InputIterator>::is_integer Integer;
        __assign_dispatch(first, last, Integer());
    }

    // 在容器尾部插入元素
    template <class T, class Alloc>
    void vector<T, Alloc>::push_back(const value_type &value)
    {
        if (finish_ != end_of_storage_)
        {
            mystl::construct(finish_, value);
            ++finish_;
        }
        else
        {
            __insert_aux(end(), value);
        }
    }

    // 弹出最后一个元素
    template <class T, class Alloc>
    void vector<T, Alloc>::pop_back()
    {
        --finish_;
        mystl::destroy(finish_);
    }

    // 在 position 位置插入元素
    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator
    vector<T, Alloc>::insert(iterator position, const value_type &value)
    {
        auto n = position - begin();
        if (finish_ != end_of_storage_ && position == end())
        {
            mystl::construct(finish_, value);
            ++finish_;
        }
        else
        {
            __insert_aux(position, value);
        }
        return begin() + n;
    }

    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator
    vector<T, Alloc>::insert(iterator position)
    {
        auto n = position - begin();
        if (finish_ != end_of_storage_ && position == end())
        {
            mystl::construct(finish_);
            ++finish_;
        }
        else
        {
            __insert_aux(position, value_type());
        }
        return begin() + n;
    }

    // 在 position 位置开始插入 n 个元素
    template <class T, class Alloc>
    void vector<T, Alloc>::insert(iterator position, size_type n, const value_type &value)
    {
        __fill_initialize(position, n, value);
    }

    // 在 position 位置插入[first, last)内的元素
    template <class T, class Alloc>
    template <class InputIterator>
    void vector<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last)
    {
        typedef typename __is_integer<InputIterator>::is_integer Integer;
        __insert_dispatch(position, first, last, Integer());
    }

    // 删除 position 位置上的元素
    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator
    vector<T, Alloc>::erase(iterator position)
    {
        if (position + 1 != end())
            mystl::copy(position + 1, finish_, position);
        --finish_;
        mystl::destroy(finish_);
        return position;
    }

    // 删除[first, last)上的元素
    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator
    vector<T, Alloc>::erase(iterator first, iterator last)
    {
        mystl::copy(last, finish_, first);

        mystl::destroy(i, finish_);
        finish_ = finish_ - (last - first);
        return first;
    }

    // 重置容器大小
    template <class T, class Alloc>
    void vector<T, Alloc>::resize(size_type new_size, const value_type &value)
    {
        if (new_size < size())
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), value);
    }

    // 与另一个 vector 交换
    template <class T, class Alloc>
    void vector<T, Alloc>::swap(vector<T, Alloc> &rhs)
    {
        if (this != &rhs)
        {
            mystl::swap(start_, rhs.start_);
            mystl::swap(finish_, rhs.finish_);
            mystl::swap(end_of_storage_, rhs.end_of_storage_);
        }
    }

    // __vector_initialize 函数
    template <class T, class Alloc>
    template <class Integer>
    void vector<T, Alloc>::__vector_initialize(Integer n, Integer value, __true_type)
    {
        __fill_initialize(static_cast<size_type>(n), static_cast<T>(value));
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void vector<T, Alloc>::__vector_initialize(InputIterator first, InputIterator last, __false_type)
    {
        __range_initialize(first, last);
    }

    // __fill_initialize 函数
    template <class T, class Alloc>
    void vector<T, Alloc>::__fill_initialize(size_type n, const value_type &value)
    {
        start_ = data_allocator::allocate(n);
        finish_ = mystl::uninitialized_fill_n(start_, n, value);
        end_of_storage_ = finish_;
    }

    // __range_initialize 函数
    template <class T, class Alloc>
    template <class InputIterator>
    void vector<T, Alloc>::__range_initialize(InputIterator first, InputIterator last)
    {
        auto n = last - first;
        start_ = data_allocator::allocate(static_cast<size_type>(n));
        finish_ = mystl::uninitialized_copy(first, last, start_);
        end_of_storage_ = finish_;
    }

    // __destroy_and_deallocate 函数//释放原资源
    template <class T, class Alloc>
    void vector<T, Alloc>::__destroy_and_deallocate()
    {
        mystl::destroy(start_, finish_);
        data_allocator::deallocate(finish_, end_of_storage_ - start_);
    }

    // __assign_dispatch 函数
    template <class T, class Alloc>
    template <class Integer>
    void vector<T, Alloc>::__assign_dispatch(Integer n, Integer value, __true_type)
    {
        __fill_assign(static_cast<size_type>(n), static_cast<T>(value));
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void vector<T, Alloc>::__assign_dispatch(InputIterator first, InputIterator last, __false_type)
    {
        __range_assign(first, last, iterator_category(first));
    }

    // __fill_assign 函数
    template <class T, class Alloc>
    void vector<T, Alloc>::__fill_assign(size_type n, const value_type &value)
    {
        if (n > capacity())
        {
            vector<T, Alloc> tmp(n, value);
            tmp.swap(*this);
        }
        else if (n > size())
        {
            mystl::fill(begin(), end(), value);
            finish_ = mystl::uninitialized_fill_n(finish_, n - size(), value);
        }
        else
        {
            erase(mystl::fill_n(start_, value), finish_);
        }
    }

    // __range_assign 函数
    template <class T, class Alloc>
    template <class InputIterator>
    void vector<T, Alloc>::__range_assign(InputIterator first, InputIterator last, input_iterator_tag)
    {
        auto cur = begin();
        for (; first != last && cur != end(); ++first, ++cur)
        {
            *cur = *first;
        }
        if (first == last)
            erase(cur, end());
        else
            insert(end(), first, last);
    }

    template <class T, class Alloc>
    template <class ForwardIterator>
    void vector<T, Alloc>::__range_assign(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
    {
        auto len = distance(first, last);
        if (static_cast<size_t>(len) > capacity())
        { //区间长大于容器
            __destroy_and_deallocate();
            __range_initialize(first, last);
        }
        else if (size() >= static_cast<size_type>(len))
        { //区间长小于等于容器
            auto new_finish = mystl::copy(first, last, start_);
            mystl::destroy(new_finish, finish_);
            finish_ = new_finish;
        }
        else
        {                     //区间长大于容器且小于容器
            auto mid = first; //把区间分为前后段
            advance(mid, size());
            mystl::copy(first, mid, start_); //先复制前一段区间在复制后一段
            finish_ = mystl::uninitialized_copy(mid, last, finish_);
        }
    }

    // __insert_aux 函数
    template <class T, class Alloc>
    void vector<T, Alloc>::__insert_aux(iterator position, const value_type &value)
    {
        if (finish_ != end_of_storage_)
        {
            mystl::construct(finish_, *(finish_ - 1));
            ++finish_;
            mystl::copy_backward(position, finish_ - 2, finish_ - 1);
            auto value_type = value;
            *position = value_type;
        }
        else
        { //备用空间不足
            const auto old_size = size();
            const auto len = old_size == 0 ? 1 : (old_size << 1);
            auto new_start = data_allocator::allocate(len);
            auto new_finish = new_start;
            try
            {
                new_finish = mystl::uninitialized_copy(start_, position, new_start);
                mystl::construct(new_finish, value);
                ++new_finish;
                new_finish = mystl::uninitialized_copy(position, finish_, new_finish);
            }
            catch (...)
            {
                mystl::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
            }
            __destroy_and_deallocate();
            start = new_start;
            finish_ = new_finish;
            end_of_storage_ = new_start + len;
        }
    }

    // __insert_dispatch 函数
    template <class T, class Alloc>
    template <class Integer>
    void vector<T, Alloc>::__insert_dispatch(iterator position, Integer n, Integer x, __true_type)
    {
        __fill_insert(position, static_cast<size_type>(n), static_cast<T>(x));
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void vector<T, Alloc>::__insert_dispatch(iterator position, InputIterator first,
                                             InputIterator last, __false_type)
    {
        InputIterator last, __false_type)
        {
            __range_insert(position, first, last, mystl::iterator_category(first));
        }

        // __fill_insert 函数
        template <class T, class Alloc>
        void vector<T, Alloc>::__fill_insert(iterator position, size_type n, const value_type &value)
        {
            if (n != 0)
            {
                if (static_cast<size_type>(end_of_storage_ - finish_) >= n)
                {
                    // 如果备用空间大于等于增加的空间
                    auto value_copy = value;
                    const auto after_elems = finish_ - position; // 插入点后的元素个数
                    auto old_finish = finish_;
                    if (static_cast<size_type>(after_elems) > n)
                    {                                                             // 插入点后元素较多
                        mystl::uninitialized_copy(finish_ - n, finish_, finish_); // 复制原元素
                        finish_ += n;
                        mystl::copy_backward(position, old_finish - n, old_finish); // 复制剩余元素
                        mystl::fill_n(position, n, value_copy);                     // 填充新元素
                    }
                    else
                    {
                        mystl::uninitialized_fill_n(finish_, n - after_elems, value_copy); // 填充新元素
                        finish_ += n - after_elems;
                        mystl::uninitialized_copy(position, old_finish, finish_); // 复制原元素
                        finish_ += after_elems;
                        mystl::fill(position, old_finish, value_copy); // 填充新元素
                    }
                }
                else
                { // 备用空间不足
                    // 新长度在 旧长度的两倍，旧长度 + 新增元素个数 中取较大值
                    const auto old_size = size();
                    const auto len = old_size + mystl::max(old_size, n);
                    auto new_start = data_allocator::allocate(len); // 配置新的 vector 空间
                    auto new_finish = new_start;
                    try
                    {
                        // 先将旧 vector 插入点前的元素复制到新 vector 空间
                        new_finish = mystl::uninitialized_copy(start_, position, new_start);
                        // 将新增元素加入新空间
                        new_finish = mystl::uninitialized_fill_n(new_finish, n, value);
                        // 将旧 vector 插入点后的元素复制到新空间
                        new_finish = mystl::uninitialized_copy(position, finish_, new_finish);
                    }
                    catch (...)
                    {
                        mystl::destroy(new_start, new_finish);
                        data_allocator::deallocate(new_start, len);
                    }
                    __destroy_and_deallocate(); // 析构释放原 vector
                    start_ = new_start;         // 调整迭代器，指向新的 vector
                    finish_ = new_finish;
                    end_of_storage_ = start_ + len;
                }
            }
        }

        // __range_insert 函数
        template <class T, class Alloc>
        template <class InputIterator>
        void vector<T, Alloc>::__range_insert(iterator position, InputIterator first,
                                              InputIterator last, input_iterator_tag)
        {
            for (; first != last; ++first)
            {
                position = insert(position, *first);
                ++position;
            }
        }

        template <class T, class Alloc>
        template <class ForwardIterator>
        void vector<T, Alloc>::__range_insert(iterator position, ForwardIterator first,
                                              ForwardIterator last, forward_iterator_tag)
        {
            if (first != last)
            {
                auto n = distance(first, last);
                if ((end_of_storage_ - finish_) >= n)
                {                                                // 如果备用空间大小足够
                    const auto after_elems = finish_ - position; // position 后面的元素个数
                    auto old_finish = finish_;
                    if (after_elems > n)
                    { // position 后面的元素较多
                        // 先把最后 n 个元素复制到 finish_ 为起始的位置
                        mystl::uninitialized_copy(finish_ - n, finish_, finish_);
                        finish_ += n;
                        mystl::copy_backward(position, old_finish - n, old_finish);
                        mystl::copy(first, last, position); // 将要插入的元素复制到 position 处
                    }
                    else
                    { // position 后面的元素较少
                        auto mid = first;
                        advance(mid, after_elems); // 以 after_elems 把要插入的区间分为前后段
                        // 把后段复制到 finish_ 为起始的位置
                        mystl::uninitialized_copy(mid, last, finish_);
                        finish_ += n - after_elems;
                        // 原元素复制到 finish_ 为起始的位置
                        mystl::uninitialized_copy(position, old_finish, finish_);
                        finish_ += after_elems;
                        mystl::copy(first, mid, position); // 复制前半段到 position 位置
                    }
                }
                else
                { // 备用空间不足
                    // 新长度在 旧长度的两倍，旧长度 + 新增元素个数 中取较大值
                    const auto old_size = size();
                    const auto len = old_size + mystl::max(old_size, static_cast<size_type>(n));
                    auto new_start = data_allocator::allocate(len);
                    auto new_finish = new_start;
                    try
                    {
                        // 原 vector 的前半段复制到新的 vector 空间
                        new_finish = mystl::uninitialized_copy(start_, position, new_start);
                        // 把插入区间复制到新 vector 空间尾端
                        new_finish = mystl::uninitialized_copy(first, last, new_finish);
                        // 原 vector 的后半段复制到新的 vector 空间尾端
                        new_finish = mystl::uninitialized_copy(position, finish_, new_finish);
                    }
                    catch (...)
                    {
                        mystl::destroy(new_start, new_finish);
                        data_allocator::deallocate(new_start, len);
                    }
                    __destroy_and_deallocate(); // 析构释放原 vector
                    start_ = new_start;         // 调整迭代器，指向新的 vector
                    finish_ = new_finish;
                    end_of_storage_ = start_ + len;
                }
            }
        }

        // 重载比较操作符
        template <class T, class Alloc>
        inline bool operator==(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
        {
            return lhs.size() == rhs.size() && mystl::equal(lhs.begin(), lhs.end(), rhs.begin())
        }

        template <class T, class Alloc>
        inline bool operator<(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
        {
            return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
        }

        template <class T, class Alloc>
        inline bool operator!=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
        {
            return !(lhs == rhs);
        }

        template <class T, class Alloc>
        inline bool operator>(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
        {
            return rhs < lhs;
        }

        template <class T, class Alloc>
        inline bool operator<=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
        {
            return !(rhs < lhs);
        }

        template <class T, class Alloc>
        inline bool operator>=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
        {
            return !(lhs < rhs);
        }

        // 重载 mystl 的 swap
        template <class T, class Alloc>
        inline void swap(vector<T, Alloc> & lhs, vector<T, Alloc> & rhs)
        {
            lhs.swap(rhs);
        }
    }

#endif