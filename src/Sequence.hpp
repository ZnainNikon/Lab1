#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstddef>
using std::string;

class SequenceException : public std::exception
{
    protected:
    string message;
    
    public:
    SequenceException(const string& msg);
    const char* what() const noexcept override; 
};

class IndexOutOfRangeException : public SequenceException
{
    public:
    IndexOutOfRangeException(const string& msg);
};

class SequenceEmptyException : public SequenceException
{
    public:
    SequenceEmptyException(const string& msg);
};

class NullPointerException : public SequenceException
{
    public:
    NullPointerException(const string& msg);
};

class InvalidBitException : public SequenceException
{
    public:
    InvalidBitException(const string& msg);
};

class DifferentSizeException : public SequenceException
{
    public:
    DifferentSizeException(const string& msg);
};

template <class T> class Sequence
{
    public:
    virtual ~Sequence() = default;
    
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual T Get(size_t index) const = 0;
    virtual Sequence<T>* GetSubsequence(size_t start, size_t end) const = 0;
    virtual size_t GetLength() const = 0;

    Sequence<T>* Append(T item);
    Sequence<T>* Prepend(T item);
    Sequence<T>* InsertAt(T item, size_t index);
    virtual Sequence<T>* Concat(Sequence<T>* list) = 0;
    
    protected:
    virtual Sequence<T>* AppendImpl(const T& item) = 0;
    virtual Sequence<T>* InsertAtImpl(const T& item, size_t index) = 0;
    virtual Sequence<T>* PrependImpl(const T& item) = 0;
};

#include "Sequence.tpp"

#endif