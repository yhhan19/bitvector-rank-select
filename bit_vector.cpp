#include "utility.hpp"
#include "bit_vector.hpp"

BitVector::BitVector(uint64_t size) {
    this->size = size;
    vector = new uint32_t[(size >> 5) + 1];
    memset(vector, 0, (size >> 5) + 1 << 2);
    sbrank = NULL; brank = NULL;
}

BitVector::BitVector(uint64_t size, unsigned int seed) {
    srand(seed);
    this->size = size;
    sbrank = NULL; brank = NULL;
    vector = new uint32_t[(size >> 5) + 1];
    for (uint64_t i = 0; i < size; i ++) 
        set(i, rand() % 2);
    build_index();
}

uint64_t BitVector::overhead() {
    return 128 + ((size  >> 12) + 1) * 64 + ((size >> 5) + 1) * 16;
}

uint64_t BitVector::get_size() {
    return size;
}

uint64_t BitVector::get_weight() {
    return weight;
}

void BitVector::build_index() {
    if (sbrank != NULL) return ;
    sbrank = new uint64_t[(size  >> 12) + 1];
    brank = new uint16_t[(size >> 5) + 1];
    uint64_t rank = 0, last_sb_rank = 0, sb_count = 0, b_count = 0;
    sbrank[0] = brank[0] = 0;
    for (uint64_t i = 0, j = 0, k = 0; i < size; i ++) {
        rank += get(i);
        if (++ k == 32) {
            brank[++ b_count] = rank - last_sb_rank;
            k = 0;
        }
        if (++ j == 4096) {
            last_sb_rank = sbrank[++ sb_count] = rank;
            j = 0;
        }
    }
    weight = rank;
}

uint64_t BitVector::rank1_bf(uint64_t index) {
    uint64_t rank = 0;
    for (uint64_t i = 0; i < index; i ++) 
        rank += get(i);
    return rank;
}

uint64_t BitVector::rank1_log(uint64_t index) {
    // std::cout << rank1_bf(index) << std::endl;
    uint64_t rank = sbrank[index >> 12] + brank[index >> 5];
    for (uint64_t i = index >> 5 << 5; i < index; i ++) 
        rank += get(i);
    return rank;
}

uint64_t BitVector::rank1(uint64_t index) {
    // std::cout << rank1_log(index) << std::endl;
    uint64_t rank = sbrank[index >> 12] + brank[index >> 5];
    uint64_t delta = 32 - (index - (index >> 5 << 5));
    uint32_t i = (uint64_t) vector[index >> 5] >> delta;
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return rank + ((((i + (i >> 4)) & 0x0f0f0f0f) * 0x01010101) >> 24);
}

uint64_t BitVector::select1(uint64_t rank) {
    uint64_t l = 0, r = size;
    while (l < r) {
        uint64_t m = (l + r) / 2, mrank = rank1(m);
        if (rank < mrank) 
            r = m - 1;
        else if (rank == mrank)
            r = m;
        else 
            l = m + 1; 
    }
    // std::cout << l <<  ' ' << rank1(l) << ' ' << (l > 0 ? rank1(l - 1) : 0) << ' ' << rank << std::endl;
    return l;
}

BitVector::BitVector(const std::string &input) {
    std::ifstream fin(input, std::ios::binary);
    fin.seekg(0, fin.end);
    uint64_t length = fin.tellg();
    fin.seekg(0, fin.beg);
    char *buffer = new char[length];
    fin.read(buffer, length);
    fin.close();
    memcpy((char*) &size, buffer, 8);
    memcpy((char*) &weight, buffer + 8, 8);
    uint64_t a = 16, b = (size >> 5) + 1 << 2, c = (size >> 12) + 1 << 3, d = (size >> 5) + 1 << 1;
    vector = new uint32_t[(size >> 5) + 1];
    sbrank = new uint64_t[(size >> 12) + 1];
    brank = new uint16_t[(size >> 5) + 1];
    memcpy((char *) vector, buffer + a, b);
    memcpy((char *) sbrank, buffer + a + b, c);
    memcpy((char *) brank, buffer + a + b + c, d);
    delete [] buffer;
}

BitVector::~BitVector() {
    if (vector != NULL) delete [] vector;
    if (sbrank != NULL) delete [] sbrank;
    if (brank != NULL) delete [] brank;
}

uint64_t BitVector::save(const std::string &output) {
    std::ofstream fout(output, std::ios::binary);
    uint64_t a = 16, b = (size >> 5) + 1 << 2, c = (size >> 12) + 1 << 3, d = (size >> 5) + 1 << 1;
    char *buffer = new char[a + b + c + d];
    memcpy(buffer, (char*) &size, 8);
    memcpy(buffer + 8, (char*) &weight, 8);
    memcpy(buffer + a, (char *) vector, b);
    memcpy(buffer + a + b, (char *) sbrank, c);
    memcpy(buffer + a + b + c, (char *) brank, d);
    fout.write(buffer, a + b + c + d);
    delete [] buffer;
    fout.close();
    return a + b + c + d;
}

std::string BitVector::to_string() {
    std::string s = "";
    for (uint64_t i = 0; i < size; i ++) 
        s += std::to_string((int) get(i));
    return s;
}

void BitVector::set(uint64_t index, int value) {
    if (sbrank != NULL || index < 0 || index >= size) return ;
    if (value == 0) 
        vector[index >> 5] &= ~masks[index & 31];
    else 
        vector[index >> 5] |= masks[index & 31];
}

int BitVector::get(uint64_t index) {
    if (index < 0 || index >= size) return 0;
    uint32_t value = vector[index >> 5] & masks[index & 31];
    if (value == 0) 
        return 0;
    return 1;
}

uint32_t BitVector::masks[32] = {
    0x80000000,
    0x40000000,
    0x20000000,
    0x10000000,
    0x08000000,
    0x04000000,
    0x02000000,
    0x01000000,
    0x00800000,
    0x00400000,
    0x00200000,
    0x00100000,
    0x00080000,
    0x00040000,
    0x00020000,
    0x00010000,
    0x00008000,
    0x00004000,
    0x00002000,
    0x00001000,
    0x00000800,
    0x00000400,
    0x00000200,
    0x00000100,
    0x00000080,
    0x00000040,
    0x00000020,
    0x00000010,
    0x00000008,
    0x00000004,
    0x00000002,
    0x00000001
};
