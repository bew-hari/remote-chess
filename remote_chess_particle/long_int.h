#ifndef LONG_INT_H
#define LONG_INT_H

struct LongInt {
    int m_top;
    int m_bottom;

    LongInt();
    LongInt(int top, int bottom);
    
    LongInt operator^(const LongInt& other);
    
    bool isSet(int index);
    int* locateSetBits();
};

#endif
