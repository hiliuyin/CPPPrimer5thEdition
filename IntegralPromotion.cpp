// A real code in one of my experienced project

#define VALUE_1 ((char)0xAA)
#define VALUE_2 ((char)0xAB)

int main()
{
    uint8_t x = VALUE_1; // x = 0xAA;
    if (x == VALUE_1)
        printf("VALUE_1\n");  // VALUE_1 and X are all promoted to int, because char here is signed, VALUE_1 is promoted 0xFFAA
    else
        printf("VALUE_2\n");

    return 0;
}
