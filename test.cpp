#include "utility.hpp"
#include "bit_vector.hpp"
#include "sparse_vector.hpp"

int main() {
    uint64_t N = (int) 1e9, step = N / 10;
    /*
    for (uint64_t i = step; i <= N; i += step) {
        BitVector b_(i, 0);
        b_.save("tmp");
        BitVector b("tmp");
        auto start = std::chrono::high_resolution_clock::now();
        for (uint64_t k = 0; k <= i; k ++) b.rank1(k);
        auto end = std::chrono::high_resolution_clock::now();
        auto r_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        start = std::chrono::high_resolution_clock::now();
        for (uint64_t k = 0; k <= b.get_weight(); k ++) b.select1(k);
        end = std::chrono::high_resolution_clock::now();
        auto s_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << i << "," << r_duration.count() << "," << s_duration.count() << "," << b.overhead() << std::endl;
    }
    */
    for (uint64_t i = step; i <= N; i += step) {
        uint64_t ks[3] = {20, 50, 100};
        for (uint64_t k = 0; k < 3; k ++) {
            SparseVector sv(i);
            uint64_t s0 = 0, s1 = 0;
            for (uint64_t j = rand() % (i / ks[k]); j < i; j += rand() % (i / ks[k]) + 1) {
                int length = 10 + rand() % 10;
                std::string elem = "";
                for (int j = 0; j < length; j ++) 
                    elem += 'a' + rand() % 26;
                sv.append(elem, j);
                s0 += 8 * length;
                s1 += 64;
            }
            sv.finalize();
            std::string elem;
            uint64_t weight = sv.num_elem(), size = sv.get_size();
            auto start = std::chrono::high_resolution_clock::now();
            for (uint64_t j = 0; j <= sv.get_size(); j ++) sv.get_at_index(j % size, elem);
            auto end = std::chrono::high_resolution_clock::now();
            auto d0 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            start = std::chrono::high_resolution_clock::now();
            for (uint64_t j = 0; j <= sv.get_size(); j ++) sv.get_at_rank(j % weight, elem);
            end = std::chrono::high_resolution_clock::now();
            auto d1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            start = std::chrono::high_resolution_clock::now();
            for (uint64_t j = 0; j <= sv.get_size(); j ++) sv.get_index_of(j % weight);
            end = std::chrono::high_resolution_clock::now();
            auto d3 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            start = std::chrono::high_resolution_clock::now();
            for (uint64_t j = 0; j <= sv.get_size(); j ++) sv.num_elem_at(j % size);
            end = std::chrono::high_resolution_clock::now();
            auto d2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << i << ',' << 1.0 / ks[k] << ',' << s0 + s1 << ',' << s0 + 8 * i << ',';
            std::cout << d0.count() << ',' << d1.count() << ',' << d2.count() << ',' << d3.count() << std::endl;
        }
    }
    return 0;
}
