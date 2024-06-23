#pragma once
#include "array_ptr.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <initializer_list>


struct ReserveProxyObj
{   
    ReserveProxyObj(size_t capacity = 0) : capacity_(capacity) {}
    size_t capacity_ = 0;
};

template <typename Type>
ReserveProxyObj Reserve(Type capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}; 

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;
    
     explicit SimpleVector(size_t size) : array_ptr_(ArrayPtr<Type>(size)) {
        // Напишите тело конструктора самостоятельно
        if (size > 0) {
            ArrayPtr<Type> temp(size);
            //std::fill(temp.Get(), temp.Get() + size, Type{});
            FillArray(temp.Get(), temp.Get() + size);
            array_ptr_.swap(temp);
            capacity_ = size;
            size_ = size;
        }

    }

    SimpleVector(ReserveProxyObj reserve) : array_ptr_(ArrayPtr<Type>(reserve.capacity_)) {
        size_ = 0;
        capacity_ = reserve.capacity_;
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : array_ptr_(ArrayPtr<Type>(size)) {
        //std::fill(array_ptr_.Get(), array_ptr_.Get() + size, value);
        
        for (auto it = begin(); it != end(); ++it) {
            *it = value;
        }

        size_ = size;
        capacity_ = size;
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) : array_ptr_(ArrayPtr<Type>(init.size())) {
        //std::copy(init.begin(), init.end(), array_ptr_.Get());
        std::move(std::make_move_iterator(init.begin()), std::make_move_iterator(init.end()), array_ptr_.Get());
        size_ = init.size();
        capacity_ = init.size();
    }

    // simple vector p2 до swap
    SimpleVector(const SimpleVector& other) {
        SimpleVector temp(other.size_);
        std::copy(other.begin(), other.end(), temp.begin());
        swap(temp);
    }

    SimpleVector(SimpleVector&& other) {
        swap(other);
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {

            SimpleVector temp(rhs.size_);
            std::copy(rhs.begin(), rhs.end(), temp.begin());
            swap(temp);
        };
        return *this;

    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs) {
            swap(rhs);
            
        }
        return *this;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> temp(new_capacity);
            //работает ли
            //std::copy(cbegin(), cend(), temp.Get());

            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), temp.Get());

            array_ptr_.swap(temp);
            capacity_ = new_capacity;
        }
    }; 
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    
    void PushBack(const Type& item) {

        if (capacity_ == 0) {
            Reserve(1);
        } else if (capacity_ == size_) {
            Reserve(2 * capacity_);
        }
        //capacity_ = new_capacity;
        array_ptr_[size_] = item;
        ++size_;

    }

    void PushBack(Type&& item) {

        //дублирование кода, вынести в функцию, когда починю
        if (capacity_ == 0) {
            Reserve(1);
        } else if (capacity_ == size_) {
            Reserve(2 * capacity_);
        }

        array_ptr_[size_] = std::move(item);
        ++size_;

    }
    
   


    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t item = std::distance(cbegin(), pos);

        if (size_ < capacity_) {

            std::copy_backward(begin() + item, end(), end() + 1);
        }
        else if (size_ == capacity_) {

            if (size_ != 0)
            {
                Reserve(2 * size_);
                std::copy_backward(begin() + item, end(), end() + 1);
            }
            else
            {
                Reserve(1);
            }

        }
        array_ptr_[item] = value;
        ++size_;
        return begin() + item;

    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t item = std::distance(cbegin(), pos);

        if (size_ < capacity_) {

            std::copy_backward(std::make_move_iterator(begin() + item), std::make_move_iterator(end()), end() + 1);
        }
        else if (size_ == capacity_) {

            if (size_ != 0)
            {
                Reserve(2 * size_);
                std::copy_backward(std::make_move_iterator(begin() + item), std::make_move_iterator(end()), end() + 1);
            }
            else
            {
                Reserve(1);
            }

        }
        array_ptr_[item] = std::move(value);
        ++size_;
        return begin() + item;

    }

    Iterator Erase(ConstIterator pos) {
        size_t item = std::distance(cbegin(), pos);
        std::move(std::make_move_iterator(begin() + item + 1) , std::make_move_iterator(end()), &array_ptr_[item]); // ??? будет работать
        --size_;
        return begin() + item;

    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (size_ != 0) {
            --size_;           
        }
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        //новый размер меньше текущего, все просто
        if (new_size < size_) {
            size_ = new_size;
        } else if (new_size <= capacity_) {
            //std::fill(end(), array_ptr_.Get() + new_size, Type{});
            FillArray(end(), begin() + new_size);
            size_ = new_size;
            
        } else if (new_size > capacity_) {
           
            ArrayPtr<Type> temp(new_size);
            std::move(std::make_move_iterator(begin()), std::make_move_iterator(end()), temp.Get());
            //std::fill(temp.Get() + size_, temp.Get() + new_size, Type{});
            FillArray(temp.Get() + size_, temp.Get() + new_size);
           
            array_ptr_.swap(temp);
            size_ = new_size;
            capacity_ = new_size;
        }
    }

    /*
    void Resize(size_t new_size) {
        //новый размер меньше текущего, все просто
        if (new_size < size_) {
            size_ = new_size;
        } else if (new_size <= capacity_) {
            std::fill(end(), array_ptr_.Get() + new_size, Type{});
            size_ = new_size;
        } else {
            size_t new_capacity = new_size;
            ArrayPtr<Type> temp(new_capacity);
            std::copy(cbegin(), cend(), temp.Get());
            std::fill(temp.Get() + size_, temp.Get() + new_size, Type{});
            array_ptr_.swap(temp);
            size_ = new_size;
            capacity_ = new_capacity;
        }
    }
    */
    // Удаляет элемент вектора в указанной позиции

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        this->array_ptr_.swap(other.array_ptr_);
        std::swap(this->size_, other.size_);
        std::swap(this->capacity_, other.capacity_);
    }

    void swap(SimpleVector&& other) noexcept
    {
        array_ptr_.swap(other.array_ptr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return array_ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return array_ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index more or equal size");
        }
        return (*this)[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index more or equal size");
        }
        return (*this)[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }


    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return array_ptr_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return  array_ptr_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return array_ptr_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return  array_ptr_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return array_ptr_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return  array_ptr_.Get() + size_;
    }
private:
    
    ArrayPtr<Type> array_ptr_;
    size_t capacity_ = 0;
    size_t size_ = 0;

    template<typename Iterator>
    void FillArray(Iterator first, Iterator last) {
        for (; first != last; ++first) {
                *first = std::move(Type{});
            }
    }

};


template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {

    return !(lhs < rhs) && !(rhs < lhs);
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {

    return lhs < rhs || rhs < lhs;
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {

    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {

    return (lhs < rhs) || (lhs == rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {

    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {

    return (lhs > rhs) || (lhs == rhs);
}