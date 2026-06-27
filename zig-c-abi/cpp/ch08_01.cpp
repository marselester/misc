// zig c++ ./cpp/ch08_01.cpp ./llvm/ch08_01.s -o main
// See https://github.com/Apress/modern-x86-assembly-language-programming-3e/blob/main/Include/XmmVal.h.
#include <iostream>
#include <cstdint>

struct alignas(16) XmmVal {
    union {
        int8_t   m_I8[16];
        int16_t  m_I16[8];
        int32_t  m_I32[4];
        int64_t  m_I64[2];
        uint8_t  m_U8[16];
        uint16_t m_U16[8];
        uint32_t m_U32[4];
        uint64_t m_U64[2];
        float    m_F32[4];
        double   m_F64[2];
    };
};

extern "C" void AddI16_avx(XmmVal* c1, XmmVal* c2, const XmmVal* a, const XmmVal* b);

void printI16(const XmmVal& vec) {
    std::cout << "{ ";

    for (int i = 0; i < 8; ++i) {
        std::cout << vec.m_I16[i];
        if (i < 7) {
            std::cout << ", ";
        }
    }

    std::cout << " }\n";
}

int main() {
    XmmVal c1, c2;
    const XmmVal a = { .m_I16 = { 10, 200, 30, -32766, 50, 60, 32000, -32000 } };
    const XmmVal b = { .m_I16 = { 100, -200, 32760, -400, 500, -600, 1200, -950 } };

    AddI16_avx(&c1, &c2, &a, &b);

    std::cout << "C++ says" << std::endl;
    std::cout << "c1="; printI16(c1);
    std::cout << "c2="; printI16(c2);

    return 0;
}
