#include "Sequence.hpp"
using std::string;

SequenceException::SequenceException(const string& msg) : message(msg) {}

const char* SequenceException::what() const noexcept
{ 
    return message.c_str(); 
}

IndexOutOfRangeException::IndexOutOfRangeException(const string& msg) : SequenceException(msg) {}

SequenceEmptyException::SequenceEmptyException(const string& msg) : SequenceException(msg) {}

NullPointerException::NullPointerException(const string& msg) : SequenceException(msg) {}

InvalidBitException::InvalidBitException(const string& msg) : SequenceException(msg) {}

DifferentSizeException::DifferentSizeException(const string& msg) : SequenceException(msg) {}
