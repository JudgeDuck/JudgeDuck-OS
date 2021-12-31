#ifndef DUCK_CACHE_H
#define DUCK_CACHE_H

#include <stdint.h>

namespace DuckCache {
    struct Digest256 {
        uint64_t a[4];
    } __attribute__((packed));
    
    struct Metadata {
        uint64_t last_used_tsc;
        void *start_addr;
        uint64_t len;
    } __attribute__((packed));
    
    struct DuckCache {
        uint64_t cache_size;
        
        uint64_t n_max_objects;
        uint64_t n_cur_objects;
        
        void *free_list_addr;
        uint64_t n_free_pages;
        uint64_t n_total_pages;
        
        uint64_t *obj_hash;
        Digest256 *obj_digest;
        Metadata *obj_metadata;
        uint64_t *sort_ids;
        
        bool init(uint64_t n_max_objects, uint64_t cache_size);
        bool load(const Digest256 *digest, void *dst, uint64_t required_len);
        bool store(const Digest256 *digest, const void *src, uint64_t len);
        void info(char *output);
        
        private:
        bool find(const Digest256 *digest, uint64_t *lower_bound_index);
    };
    
    bool digest_from_hex(const char *hex, Digest256 *digest);
}

#endif
