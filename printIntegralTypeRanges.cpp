//
// Write a program to determine the ranges of char, short, int, and long variables, both signed and unsigned
// by printing appropriate values from standard headers and by direct computation.
//

#include <iostream>
#include <limits>

void printIntegralTypeRanges()
{
    const int BITS_PER_BYTE = 8;
    
    std::cout << "signed char: " << SCHAR_MIN << " to " << SCHAR_MAX << std::endl;
    std::cout << "signed char: " << (int)(signed char)(1<<(sizeof(signed char)*BITS_PER_BYTE - 1))
              << " to " << (int)(signed char)(~(1<<(sizeof(signed char)*BITS_PER_BYTE - 1))) << std::endl;
    
    std::cout << "unsigned char: " << 0 << " to " << UCHAR_MAX << std::endl;
    std::cout << "unsigned char: " << 0 << " to " << (int)(unsigned char)(~0) << std::endl;
    
    std::cout << "int : " << INT_MIN << " to " << INT_MAX << std::endl;
    std::cout << "int : " << (1<<(sizeof(int)*BITS_PER_BYTE - 1)) << " to " << ~(1<<(sizeof(int)*BITS_PER_BYTE - 1)) << std::endl;
    
    std::cout << "unsigned int: " << 0 << " to " << UINT_MAX << std::endl;
    std::cout << "unsigned int: " << 0 << " to " << ~(unsigned int)0 << std::endl;
    
    
    std::cout << "long: " << LONG_MIN << " to " << LONG_MAX << std::endl;
    std::cout << "long: " << ((long)1 << (sizeof(long)*BITS_PER_BYTE - 1))
              << " to " << ~((long)1 << (sizeof(long)*BITS_PER_BYTE - 1)) << std::endl;
    
    std::cout << "long long: " << LONG_LONG_MIN << " to " << LONG_LONG_MAX << std::endl;
    std::cout << "long long: " << ((long long)1 << (sizeof(long long)*BITS_PER_BYTE - 1))
              << " to " << ~((long long)1 << (sizeof(long long)*BITS_PER_BYTE - 1)) << std::endl;
}

int main(int argc, const char* argv[])
{
    printIntegralTypeRanges();

    return 0;
}

