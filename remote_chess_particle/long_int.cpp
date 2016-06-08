#include "long_int.h"

LongInt::LongInt() {
    m_top = 0;
    m_bottom = 0;
}

LongInt::LongInt(int top, int bottom) {
    m_top = top;
    m_bottom = bottom;
}

LongInt LongInt::operator^(const LongInt& other) {
    int top_diff = m_top ^ other.m_top;
    int bottom_diff = m_bottom ^ other.m_bottom;
    
    return LongInt(top_diff, bottom_diff);
}

// Checks if bit is set at index
bool LongInt::isSet(int index) {
    int target;
    
    if (index < 32) {
        target = m_bottom;
    } else if (index < 64) {
        target = m_top;
    } else {
        return false;
    }
    
    if (target & (1 << (index % 32))) {
        return true;
    }
    
    return false;
}

int* LongInt::locateSetBits() {
    // Initialize indices array
    int indices[65];
    for (int i=0; i<64; i++) {
        indices[i] = -1;
    }
    
    // Go through bottom and top arrays and save indices with set bit
    int count = 0;
    int i = 0;
    int m = 1;
    while(i<32) {
        if (m_bottom & m) {
            indices[count] = i;
            count++;
        }
        i++;
        m << 1;
    }
    
    i = 0;
    while(i<32) {
        if (m_top & m) {
            indices[count] = i + 32;
            count++;
        }
        i++;
        m << 1;
    }
    
    // Save count at the end
    indices[64] = count;
    
    return indices;
}