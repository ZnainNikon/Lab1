#pragma once

template <typename T>
class MyVector
{
    private:
    T* data;
    std::size_t siz;
    std::size_t capacity;

    public:
    MyVector() : data(nullptr), siz(0), capacity(0) {}
    ~MyVector() 
    { 
        delete[] data; 
    }

    MyVector(const MyVector& other) = delete;
    MyVector& operator=(const MyVector& other) = delete;

    void push_back(T value)
    {
        if (siz == capacity)
        {
            std::size_t NewCap {};
            if (capacity == 0) 
                NewCap = 8;
            else 
                NewCap = capacity * 2;

            T* NewData = new T[NewCap];
            for (std::size_t i = 0; i < siz; ++i)
                NewData[i] = std::move(data[i]);
            delete[] data;

            data = NewData;
            capacity  = NewCap;
        }
        data[siz++] = std::move(value);
    }

    T& operator[](std::size_t i) 
    { 
        return data[i]; 
    }
    const T& operator[](std::size_t i) const 
    { 
        return data[i]; 
    }
    std::size_t size() const 
    { 
        return siz; 
    }
};