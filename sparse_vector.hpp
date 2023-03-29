#ifndef SPARSE_VEC_HPP
#define SPARSE_VEC_HPP
#include "utility.hpp"
#include "bit_vector.hpp"

class SparseVector {
    public:
        SparseVector(const std::string &input);
        uint64_t save(const std::string &output);
        SparseVector(uint64_t size);
        ~SparseVector();
        void append(std::string &elem, uint64_t pos);
        void finalize();
        bool get_at_rank(uint64_t rank, std::string& elem);
        bool get_at_index(uint64_t index, std::string& elem);
        uint64_t get_index_of(uint64_t rank);
        uint64_t num_elem_at(uint64_t index);
        uint64_t get_size();
        uint64_t num_elem();
        std::string to_string();
    private:
        BitVector *b;
        std::vector<std::pair<std::string, uint64_t>> s;
};

#endif
