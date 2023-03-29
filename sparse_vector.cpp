#include "sparse_vector.hpp"

SparseVector::SparseVector(const std::string &input) {
    b = new BitVector(input);
    uint64_t bs = b->get_size();
    uint64_t index_total = 16 + ((bs >> 5) + 1 << 2) + ((bs >> 12) + 1 << 3) + ((bs >> 5) + 1 << 1);
    std::ifstream fin(input, std::ios::binary);
    fin.seekg(0, fin.end);
    uint64_t length = fin.tellg();
    fin.seekg(0, fin.beg);
    char *buffer = new char[length];
    fin.read(buffer, length);
    fin.close();
    uint64_t cur = index_total;
    while (cur < length) {
        std::string elem = "";
        while (buffer[cur] != '\0') 
            elem += buffer[cur ++];
        cur ++;
        uint64_t pos;
        memcpy((char *) &pos, buffer + cur, 8);
        cur += 8;
        append(elem, pos);
    }
    delete [] buffer;
    finalize();
}

SparseVector::SparseVector(uint64_t size) {
    b = new BitVector(size);
}

void SparseVector::append(std::string &elem, uint64_t pos) {
    s.push_back(std::make_pair(elem, pos));
    b->set(pos, 1);
}

void SparseVector::finalize() {
    b->build_index();
}

SparseVector::~SparseVector() {
    delete b;
}

std::string SparseVector::to_string() {
    std::string st = ""; // b->to_string() + "\n";
    for (auto pair : s) 
        st += pair.first + " " + std::to_string(pair.second) + "\n";
    return st;
}

uint64_t SparseVector::save(const std::string &output) {
    uint64_t index_total = b->save(output);
    std::ofstream fout(output, std::ios::binary | std::ios::app);
    uint64_t total = 0;
    for (auto pair : s) {
        std::string elem = pair.first;
        uint64_t pos = pair.second;
        total += (elem.size() + 1) + 8;
    }
    char *buffer = new char[total];
    uint64_t cur = 0;
    for (auto pair : s) {
        std::string elem = pair.first;
        uint64_t pos = pair.second;
        for (int i = 0; i < elem.size(); i ++) 
            buffer[cur ++] = elem[i];
        buffer[cur ++] = '\0';
        memcpy(buffer + cur, (char *) &pos, 8);
        cur += 8;
    }
    fout.write(buffer, total);
    delete [] buffer;
    fout.close();
    return index_total + total;
}

bool SparseVector::get_at_rank(uint64_t rank, std::string& elem) {
    if (rank <= 0 || rank > b->get_weight()) return false;
    elem = s[rank - 1].first;
    return true;
}

bool SparseVector::get_at_index(uint64_t index, std::string& elem) {
    if (b->get(index) == 0) return false;
    elem = s[b->rank1(index)].first;
    return true;
}

uint64_t SparseVector::get_index_of(uint64_t rank) {
    if (rank > b->get_weight()) return -1;
    return b->select1(rank) - 1;
}

uint64_t SparseVector::num_elem_at(uint64_t index) {
    return b->rank1(index) + b->get(index);
}

uint64_t SparseVector::get_size() {
    return b->get_size();
}

uint64_t SparseVector::num_elem() {
    return s.size();
}
