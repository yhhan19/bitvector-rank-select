#ifndef BIT_VECTOR_HPP
#define BIT_VECTOR_HPP
#include "utility.hpp"

class BitVector {
    public:
        BitVector(const std::string &input);
        uint64_t save(const std::string &output);
        BitVector(uint64_t size);
        BitVector(uint64_t size, unsigned int seed);
        ~BitVector();
        void build_index();
        uint64_t rank1(uint64_t index);
        uint64_t select1(uint64_t rank);
        int get(uint64_t index);
        void set(uint64_t index, int value);
        uint64_t overhead();
        uint64_t get_size();
        uint64_t get_weight();
        std::string to_string();
    private:
        static uint32_t masks[32];
        uint64_t size, weight;
        uint32_t *vector;
        uint64_t *sbrank;
        uint16_t *brank;
        uint64_t rank1_bf(uint64_t index);
        uint64_t rank1_log(uint64_t index);
};

#endif
