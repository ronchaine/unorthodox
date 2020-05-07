#ifndef UNORTHODOX_DYNAMIC_ARRAY_HPP
#define UNORTHODOX_DYNAMIC_ARRAY_HPP

#include <memory>
#include <iterator>

#include "allocators.hpp"
#include "concepts.hpp"
#include "extra_type_traits.hpp"
#include "util.hpp"

#include <iostream>

namespace unorthodox
{
    template <typename Allocator, bool make_static>
    struct allocator_wrapper {};
    
    template <typename Allocator>
    struct allocator_wrapper<Allocator, true> { inline static Allocator allocator; };

    template <typename Allocator>
    struct allocator_wrapper<Allocator, false> { Allocator allocator; };

    template <typename T, typename Allocator = allocators::nothrow_allocator<T>>
    struct dynamic_array : allocator_wrapper<Allocator, Allocator::is_always_equal::value>
    {
        static_assert(std::is_nothrow_constructible<Allocator>::value);
        static_assert(std::is_nothrow_default_constructible<Allocator>::value);
        static_assert(std::is_nothrow_copy_constructible<Allocator>::value);
        static_assert(std::is_nothrow_move_constructible<Allocator>::value);

        public:
            using value_type                = T;
            using size_type                 = std::size_t;
            using difference_type           = std::ptrdiff_t;
            using reference                 = T&;
            using const_reference           = const T&;
            using pointer                   = T*;
            using const_pointer             = const T*;

            template <bool is_iterator_const>
            class iterator_type;

            using iterator                  = iterator_type<false>;
            using const_iterator            = iterator_type<true>;

            using reverse_iterator          = std::reverse_iterator<iterator>;
            using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

            constexpr static size_type sbo_limit = 2 * sizeof(void*);
            constexpr static bool static_allocator = Allocator::is_always_equal::value;

            using allocator_type            = Allocator; 

            // Constructors
            explicit constexpr dynamic_array() noexcept = default;

            constexpr dynamic_array(std::initializer_list<value_type>) noexcept;
            constexpr dynamic_array(const dynamic_array& other) noexcept;
            constexpr dynamic_array(dynamic_array&& other) noexcept;

            template <typename InputIt> // requires std::input_iterator<InputIt>
            constexpr dynamic_array(InputIt first, InputIt last) noexcept;

            template <typename Iterable> requires iterable_type<Iterable>
            explicit constexpr dynamic_array(const Iterable& source) noexcept;

            constexpr ~dynamic_array();

            // Assignments
            constexpr dynamic_array&        operator=(const dynamic_array& other) noexcept;
            constexpr dynamic_array&        operator=(dynamic_array&& other) noexcept;
            constexpr dynamic_array&        operator=(std::initializer_list<value_type> ilist) noexcept;

            // Access
            constexpr reference             operator[](const size_type index) noexcept;
            constexpr const_reference       operator[](const size_type index) const noexcept;

            // For improving drop-in compatibility
            constexpr reference             front() noexcept;
            constexpr reference             back() noexcept;
            constexpr const_reference       front() const noexcept;
            constexpr const_reference       back() const noexcept;

            // Synonyms for front and back
            constexpr reference             first() noexcept;
            constexpr reference             last() noexcept;
            constexpr const_reference       first() const noexcept;
            constexpr const_reference       last() const noexcept;

            constexpr pointer               data() noexcept;
            constexpr const_pointer         data() const noexcept;

            // Iterators
            constexpr iterator              begin() noexcept;
            constexpr const_iterator        begin() const noexcept;
            constexpr iterator              end() noexcept;
            constexpr const_iterator        end() const noexcept;

            constexpr const_iterator        cbegin() const noexcept;
            constexpr const_iterator        cend() const noexcept;

            constexpr reverse_iterator       rbegin() noexcept;
            constexpr const_reverse_iterator rbegin() const noexcept;
            constexpr reverse_iterator       rend() noexcept;
            constexpr const_reverse_iterator rend() const noexcept;

            constexpr const_reverse_iterator crbegin() const noexcept;
            constexpr const_reverse_iterator crend() const noexcept;

            // Observers
            [[nodiscard]] constexpr bool    empty() const noexcept;
            constexpr size_type             size() const noexcept;
            constexpr size_type             max_size() const noexcept;
            constexpr size_type             capacity() const noexcept;

            // Operations
            constexpr void                  reserve(size_type new_size) noexcept;
            constexpr void                  resize(size_t) noexcept;
            constexpr void                  shrink_to_fit() noexcept;
            constexpr void                  clear() noexcept;
            constexpr void                  pop_back() noexcept;

            constexpr iterator              erase(const_iterator) noexcept;
            constexpr iterator              erase(const_iterator, const_iterator) noexcept;

            constexpr void                  swap(dynamic_array&) noexcept;

            constexpr iterator              insert(const_iterator, const T&) noexcept;
            constexpr iterator              insert(const_iterator, T&&) noexcept;
            constexpr iterator              insert(const_iterator, size_type, const T&) noexcept;
            constexpr iterator              insert(const_iterator, std::initializer_list<T>) noexcept;

            constexpr void                  push_back(const T&) noexcept;
            constexpr void                  push_back(T&&) noexcept;

            template <typename InputIt>     // requires std::input_iterator<InputIt>
            constexpr iterator              insert(const_iterator, InputIt first, InputIt last) noexcept;
            template <typename... Args>
            constexpr void                  erase(Args&&...) noexcept;
            template <typename... Args>
            constexpr iterator              emplace(const_iterator pos, Args&&...) noexcept;
            template <typename... Args>
            constexpr reference             emplace_back(Args&&...) noexcept;

            template <typename... Values>
            constexpr void                  prepend(Values...) noexcept;

            template <typename... Values>
            constexpr void                  append(Values...) noexcept;

            template <bool is_const_iterator> class iterator_type;

        private:
            using sbo_buffer_type = typename std::aligned_storage<sbo_limit, std::alignment_of<value_type>::value>::type;

            constexpr static size_t growth_multiplier = 2;

            constexpr bool use_sbo(size_type elements = 0) const
            {
                if (elements == 0)
                    return current_size * sizeof(value_type) <= sbo_limit;
                return elements * sizeof(value_type) <= sbo_limit;
            }

            void grow(size_type amount) noexcept;

            size_type element_count = 0;
            size_type current_size = sbo_limit / sizeof(value_type);

            union {
                T* ptr;
                sbo_buffer_type data;
            } store;
    };
}

// *****************************
//  IMPLEMENTATIONS
// *****************************
namespace unorthodox
{
    template <typename T, typename A> template <bool is_const_iterator>
    class dynamic_array<T,A>::iterator_type
    {
        public:
            // FIXME: change to contiguous_iterator_tag
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = typename std::conditional_t<is_const_iterator, T const*, T*>;
            using reference = typename std::conditional_t<is_const_iterator, T const&, T&>;

            constexpr iterator_type() noexcept = default;
            constexpr iterator_type(value_type* ptr) noexcept : ptr(ptr) {}
            constexpr iterator_type(const iterator& other) noexcept : ptr(other.ptr) {}

            template<bool B1, bool B2>
            constexpr iterator_type<B1> operator=(const iterator_type<B2>& other) const noexcept
            {
                iterator_type<B1> rval;
                rval.ptr = other.ptr;
                return rval;
            }

            constexpr iterator_type&    operator++()    noexcept { ptr++; return *this; }
            constexpr iterator_type     operator++(int) noexcept { iterator_type tmp(*this); ++(*this); return tmp; }
            constexpr iterator_type&    operator--()    noexcept { ptr--; return *this; }
            constexpr iterator_type     operator--(int) noexcept { iterator_type tmp(*this); --(*this); return tmp; }

            constexpr void              operator+=(const size_type n)           noexcept { ptr += n; }
            constexpr void              operator+=(const iterator_type& other)  noexcept { ptr += other.ptr; }
            constexpr iterator_type     operator+ (const size_type n)           noexcept { iterator_type tmp(*this); tmp +=n; return tmp; }
            constexpr size_type         operator+ (const iterator_type& other)  noexcept { return ptr + other.ptr; }

            constexpr void              operator-=(const size_type n)           noexcept { ptr -= n; }
            constexpr void              operator-=(const iterator_type& other)  noexcept { ptr -= other.ptr; }
            constexpr iterator_type     operator- (const size_type n)           noexcept { iterator_type tmp(*this); tmp -=n; return tmp; }
            constexpr size_type         operator- (const iterator_type& other)  noexcept { return ptr - other.ptr; }

            constexpr bool operator< (const iterator_type& other) const noexcept { return ptr <  other.ptr; }
            constexpr bool operator<=(const iterator_type& other) const noexcept { return ptr <= other.ptr; }
            constexpr bool operator> (const iterator_type& other) const noexcept { return ptr >  other.ptr; }
            constexpr bool operator>=(const iterator_type& other) const noexcept { return ptr >= other.ptr; }
            constexpr bool operator==(const iterator_type& other) const noexcept { return ptr == other.ptr; }
            constexpr bool operator!=(const iterator_type& other) const noexcept { return ptr != other.ptr; }

            constexpr reference operator[](const size_type index) noexcept { return *(ptr + index); }

            constexpr reference operator*() noexcept { return *ptr; }
            constexpr pointer operator->() noexcept { return ptr; }

            constexpr iterator_type<false> unconst_iterator()
            {
                return iterator_type<false>{ptr};
            }

        private:
            T* ptr = nullptr;

            friend class iterator_type<true>;
            friend class iterator_type<false>;
    };

    // ********************
    //  Constructors

    // Initialiser list
    template <typename T, typename A>
    constexpr dynamic_array<T,A>::dynamic_array(std::initializer_list<value_type> ilist) noexcept
    {
        static_assert(std::is_nothrow_constructible_v<value_type>);
        static_assert(std::is_nothrow_copy_constructible_v<value_type>);
        reserve(ilist.size());

        pointer write_ptr = data();
        for (auto it = std::begin(ilist); it != std::end(ilist); ++it)
            *write_ptr++ = *it;
        
        element_count = ilist.size();
    }

    // Copy
    template <typename T, typename A>
    constexpr dynamic_array<T,A>::dynamic_array(const dynamic_array<T,A>& other) noexcept
    {
        static_assert(std::is_nothrow_copy_constructible<T>::value);
        reserve(other.element_count);
        element_count = other.element_count;
        nothrow_copy(other.begin(), other.end(), begin());
    }

    // Move
    template <typename T, typename A>
    constexpr dynamic_array<T,A>::dynamic_array(dynamic_array&& other) noexcept
    {
        std::swap(*this, other);
    }

    // From range
    template <typename T, typename A> template <typename InputIt>
    constexpr dynamic_array<T,A>::dynamic_array(InputIt first, InputIt last) noexcept
    {
        size_type new_count;
        if constexpr(std::is_same_v<typename std::iterator_traits<InputIt>::iterator_category,
                                             std::random_access_iterator_tag>)
            new_count = last - first;
        else
            new_count = std::distance(first, last);

        reserve(new_count);
        nothrow_copy(first, last, begin());
        element_count = new_count;
    }

    template<typename T, typename A> template <typename Iterable> requires iterable_type<Iterable>
    constexpr dynamic_array<T,A>::dynamic_array(const Iterable& source) noexcept
        : dynamic_array(source.begin(), source.end())
    {}

    template <typename T, typename A>
    constexpr dynamic_array<T,A>::~dynamic_array()
    {
        for (T& element : *this)
            element.~T();

        if (!use_sbo() && store.ptr != nullptr)
            this->allocator.deallocate(store.ptr, current_size);
    }

    // ********************
    //  Access
    //
    //   constexpr reference             operator[](const size_type index) noexcept;
    //   constexpr const_reference       operator[](const size_type index) const noexcept;

    //   constexpr reference             front() noexcept;
    //   constexpr reference             back() noexcept;
    //   constexpr const_reference       front() const noexcept;
    //   constexpr const_reference       back() const noexcept;

    //   constexpr pointer               data() noexcept;
    //   constexpr const_pointer         data() const noexcept;

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::reference dynamic_array<T,A>::operator[](const size_type index) noexcept
    { return *(data() + index); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_reference dynamic_array<T,A>::operator[](const size_type index) const noexcept
    { return *(data() + index); }
    
    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::reference dynamic_array<T,A>::front() noexcept
    { return *data(); }
    
    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_reference dynamic_array<T,A>::front() const noexcept
    { return *data(); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::reference dynamic_array<T,A>::back() noexcept
    { return *(data() + element_count - 1); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_reference dynamic_array<T,A>::back() const noexcept
    { return *(data() + element_count - 1); }
    
    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::reference dynamic_array<T,A>::first() noexcept
    { return *data(); }
    
    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_reference dynamic_array<T,A>::first() const noexcept
    { return *data(); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::reference dynamic_array<T,A>::last() noexcept
    { return *(data() + element_count - 1); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_reference dynamic_array<T,A>::last() const noexcept
    { return *(data() + element_count - 1); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::pointer dynamic_array<T,A>::data() noexcept
    {
        if (!use_sbo() && store.ptr != nullptr)
            return store.ptr;
        else
            return std::launder(reinterpret_cast<pointer>(&store.data));
    }
    
    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_pointer dynamic_array<T,A>::data() const noexcept
    {
        if (!use_sbo() && store.ptr != nullptr)
            return store.ptr;
        else
            return std::launder(reinterpret_cast<const_pointer>(&store.data));
    }

    // ********************
    //  Iterators

    //   iterator                        begin() noexcept;
    //   const_iterator                  begin() const noexcept;
    //   iterator                        end() noexcept;
    //   const_iterator                  end() const noexcept;

    //   const_iterator                  cbegin() const noexcept;
    //   const_iterator                  cend() const noexcept;

    //   reverse_iterator                rbegin() noexcept;
    //   const_reverse_iterator          rbegin() const noexcept;
    //   reverse_iterator                rend() noexcept;
    //   const_reverse_iterator          rend() const noexcept;

    //   const_reverse_iterator          crbegin() const noexcept;
    //   const_reverse_iterator          crend() const noexcept;

    // basic stuff
    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::iterator dynamic_array<T,A>::begin() noexcept { return iterator(data()); }
    
    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::iterator dynamic_array<T,A>::end() noexcept { return iterator(data() + element_count); }
    
    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_iterator dynamic_array<T,A>::begin() const noexcept
    {
        if (!use_sbo())
            return store.ptr;

        return reinterpret_cast<pointer>(const_cast<sbo_buffer_type*>(&store.data));
    }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_iterator dynamic_array<T,A>::end() const noexcept
    {
        if (!use_sbo())
            return store.ptr + element_count;

        return (reinterpret_cast<pointer>(const_cast<sbo_buffer_type*>(&store.data)) + element_count);
    }

    // cbegin, cend...
    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_iterator dynamic_array<T,A>::cbegin() const noexcept { return begin(); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_iterator dynamic_array<T,A>::cend() const noexcept { return end(); }

    // reverse iterators...
    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::reverse_iterator dynamic_array<T,A>::rbegin() noexcept
    { return std::make_reverse_iterator(end()); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::reverse_iterator dynamic_array<T,A>::rend() noexcept
    { return std::make_reverse_iterator(begin()); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_reverse_iterator dynamic_array<T,A>::rbegin() const noexcept
    { return std::make_reverse_iterator(end()); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_reverse_iterator dynamic_array<T,A>::rend() const noexcept
    { return std::make_reverse_iterator(begin()); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_reverse_iterator dynamic_array<T,A>::crbegin() const noexcept
    { return std::make_reverse_iterator(end()); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::const_reverse_iterator dynamic_array<T,A>::crend() const noexcept
    { return std::make_reverse_iterator(begin()); }

    // ********************
    //  Observers

    //   [[nodiscard]] constexpr bool    empty() const noexcept;
    //   constexpr size_type             size() const noexcept;
    //   constexpr size_type             max_size() const noexcept;
    //   constexpr size_type             capacity() const noexcept;
 
    template <typename T, typename A>
    [[nodiscard]] constexpr bool dynamic_array<T,A>::empty() const noexcept { return element_count == 0; }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::size_type dynamic_array<T,A>::size() const noexcept { return element_count; }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::size_type dynamic_array<T,A>::max_size() const noexcept { return std::numeric_limits<size_type>::max() / sizeof(T); }

    template <typename T, typename A>
    constexpr typename dynamic_array<T,A>::size_type dynamic_array<T,A>::capacity() const noexcept { return current_size; }

    // ********************
    //  Operations

    //   constexpr void                  reserve(size_type new_size) noexcept;
    //   constexpr void                  resize(size_t) noexcept;
    //   constexpr void                  shrink_to_fit() noexcept;
    //   constexpr void                  clear() noexcept;
    //   constexpr void                  pop_back() noexcept;

    //   constexpr iterator              erase(const_iterator) noexcept;
    //   constexpr iterator              erase(const_iterator, const_iterator) noexcept;

    //   constexpr void                  swap(dynamic_array&) noexcept;

    //   constexpr iterator              insert(const_iterator, const T&) noexcept;
    //   constexpr iterator              insert(const_iterator, T&&) noexcept;
    //   constexpr iterator              insert(const_iterator, size_type, const T&) noexcept;
    //   constexpr iterator              insert(const_iterator, std::initializer_list<T>) noexcept;

    //   constexpr void                  push_back(const T&) noexcept;
    //   constexpr void                  push_back(T&&) noexcept;

    //   template <typename InputIt>     // requires std::input_iterator<InputIt>
    //   constexpr iterator              insert(const_iterator, InputIt first, InputIt last) noexcept;
    //   template <typename... Args>
    //   constexpr void                  erase(Args&&...) noexcept;
    //   template <typename... Args>
    //   constexpr iterator              emplace(const_iterator pos, Args&&...) noexcept;
    //   template <typename... Args>
    //   constexpr reference             emplace_back(Args&&...) noexcept;

    //   template <typename... Values>
    //   constexpr void                  prepend(Values...) noexcept;

    //   template <typename... Values>
    //   constexpr void                  append(Values...) noexcept;

    template <typename T, typename A>
    constexpr void dynamic_array<T,A>::reserve(size_type new_size) noexcept
    {
        if (current_size >= new_size)
            return;

        if (use_sbo(new_size))
        {
            current_size = new_size;
            return;
        }

        pointer new_ptr = this->allocator.allocate(new_size);
        if (new_ptr == nullptr) // heap exhaustion?
            return;

        nothrow_copy(begin(), end(), new_ptr);

        if (!use_sbo()) // is there a case where we don't have an allocated buffer but use_sbo is not true?
            this->allocator.deallocate(store.ptr, current_size);

        store.ptr = new_ptr;
        current_size = new_size;
    }

    template <typename T, typename A>
    constexpr void dynamic_array<T,A>::resize(size_type new_size) noexcept
    {
        if ((element_count == new_size) || ((current_size == 0) && (new_size == 0)))
            return;

        if (current_size > new_size)
        {
            if (new_size < element_count)
            {
                for (size_t i = new_size; i < element_count; ++i)
                    (*(data() + i)).~value_type();
            } else {
                for (size_t i = element_count; i < new_size; ++i)
                    ::new(data() + i) value_type{};
            }
            element_count = new_size;
            return;
        }
        if (use_sbo() && use_sbo(new_size))
        {
            for (size_type i = element_count; i < new_size; ++i)
                ::new(data() + i) value_type{};

            element_count = new_size;
            return;
        }

        pointer new_ptr = this->allocator.allocate(new_size);
        for (size_type i = 0; i < new_size; ++i)
        {
            if (i < element_count)
            {
                // copy / move construct
                ::new(new_ptr + i) value_type(std::move(*(data() + i)));
            } else {
                // placement new
                ::new(new_ptr + i) value_type{};
            }
        }
        if (!use_sbo())
            this->allocator.deallocate(store.ptr, current_size);

        store.ptr = new_ptr;

        current_size = new_size;
        element_count = new_size;
    }

    template <typename T, typename A>
    constexpr void dynamic_array<T,A>::swap(dynamic_array& other) noexcept
    {
        dynamic_array temp = *this;
        *this = other;
        other = temp;
    }
}

#endif
