#include <string.h>
#include <assert.h>
#include <algorithm>

#include <inc/duck_cache.hpp>
#include <inc/memory.hpp>
#include <inc/utils.hpp>
#include <inc/timer.hpp>
#include <inc/logger.hpp>

using Memory::PAGE_SIZE;
using Utils::round_up;

namespace DuckCache {
    static const uint64_t ONE_PAGE_CONTENT_SIZE = PAGE_SIZE - sizeof(uint64_t);
    
    bool digest_from_hex(const char *hex, Digest256 *digest) {
        for (int idx = 0; idx < 4; idx++) {
            uint64_t sum = 0;
            
            for (int i = 0; i < 16; i++) {
                char c = hex[idx * 16 + i];
                uint64_t val;
                
                if (c >= '0' && c <= '9') {
                    val = c - '0';
                } else if (c >= 'a' && c <= 'f') {
                    val = c - 'a' + 10;
                } else if (c >= 'A' && c <= 'F') {
                    val = c - 'A' + 10;
                } else {
                    return false;
                }
                
                sum |= val << (i * 4);
            }
            
            digest->a[idx] = sum;
        }
        
        return true;
    }
    
    bool DuckCache::init(uint64_t n_max_objects, uint64_t cache_size) {
        char *cache = Memory::allocate_virtual_memory(cache_size);
		if (!cache) {
			LWARN("Allocate DuckCache failed");
			return false;
		}
        
        char *cache_end = cache + cache_size;
        
        this->cache_size = cache_size;
        
        this->n_max_objects = n_max_objects;
        this->n_cur_objects = 0;
        
        this->obj_hash = (uint64_t *) cache;
        cache += round_up(n_max_objects * sizeof(uint64_t), PAGE_SIZE);
        
        this->obj_digest = (Digest256 *) cache;
        cache += round_up(n_max_objects * sizeof(Digest256), PAGE_SIZE);
        
        this->obj_metadata = (Metadata *) cache;
        cache += round_up(n_max_objects * sizeof(Metadata), PAGE_SIZE);
        
        this->sort_ids = (uint64_t *) cache;
        cache += round_up(n_max_objects * sizeof(uint64_t), PAGE_SIZE);
        
        this->free_list_addr = (void *) cache;
        if (cache + PAGE_SIZE >= cache_end) {
            LFATAL("DuckCache too small");
            Utils::GG_reboot();
        }
        
        uint64_t n_pages = 0;
        for (; cache + PAGE_SIZE < cache_end; cache += PAGE_SIZE) {
            // Next pointer
            * (uint64_t *) cache = (uint64_t) (cache + PAGE_SIZE);
            n_pages++;
        }
        
        // Last page
        * (uint64_t *) cache = 0;
        n_pages++;
        
        this->n_free_pages = n_pages;
        this->n_total_pages = n_pages;
        
        LINFO(
            "Initialized DuckCache of %.0lf MiB (%lu objects)",
            cache_size / 1048576.0, n_max_objects
        );
        
        return true;
    }
    
    static inline int cmp_digest(const Digest256 *a, const Digest256 *b) {
        for (int i = 0; i < 4; i++) {
            if (a->a[i] < b->a[i]) return -1;
            if (a->a[i] > b->a[i]) return 1;
        }
        
        return 0;
    }
    
    bool DuckCache::find(const Digest256 *digest, uint64_t *lower_bound_index) {
        if (this->n_cur_objects == 0) {
            *lower_bound_index = 0;
            return false;
        }
        
        uint64_t hash = digest->a[0];
        uint64_t idx1 = std::lower_bound(this->obj_hash, this->obj_hash + this->n_cur_objects, hash) - this->obj_hash;
        uint64_t idx2 = std::upper_bound(this->obj_hash + idx1, this->obj_hash + this->n_cur_objects, hash) - this->obj_hash;
        
        uint64_t idx;
        for (idx = idx1; idx < idx2; idx++) {
            int r = cmp_digest(this->obj_digest + idx, digest);
            if (r >= 0) {
                *lower_bound_index = idx;
                return r == 0;
            }
        }
        
        *lower_bound_index = idx;
        return false;
    }
    
    bool DuckCache::load(const Digest256 *digest, void *dst, uint64_t required_len) {
        uint64_t idx;
        if (!this->find(digest, &idx)) {
            return false;
        }
        
        Metadata *metadata = this->obj_metadata + idx;
        if (metadata->len != required_len) {
            return false;
        }
        
        // Update metadata
        metadata->last_used_tsc = Timer::tsc_since_epoch();
        
        // Load
        void *addr = metadata->start_addr;
        for (uint64_t cur_len = 0; cur_len < required_len; cur_len += ONE_PAGE_CONTENT_SIZE) {
            memcpy(dst, (uint64_t *) addr + 1, std::min(required_len - cur_len, ONE_PAGE_CONTENT_SIZE));
            addr = (void *) * (uint64_t *) addr;
            dst = (void *) ((uint64_t) dst + ONE_PAGE_CONTENT_SIZE);
        }
        
        return true;
    }
    
    static Metadata *_obj_metadata;
    
    static bool cmp_obj_metadata_time(uint64_t i, uint64_t j) {
        return _obj_metadata[i].last_used_tsc < _obj_metadata[j].last_used_tsc;
    }
    
    bool DuckCache::store(const Digest256 *digest, const void *src, uint64_t len) {
        uint64_t idx;
        if (this->find(digest, &idx)) {
            return true;
        }
        
        // TODO! enough space?
        uint64_t n_need_pages = std::max((len + ONE_PAGE_CONTENT_SIZE - 1) / ONE_PAGE_CONTENT_SIZE, 1ul);
        if (n_need_pages > this->n_total_pages) {
            return false;
        }
        
        bool need_remove = false;
        if (n_need_pages > this->n_free_pages) {
            need_remove = true;
        } else if (this->n_cur_objects + 1 > this->n_max_objects) {
            need_remove = true;
        }
        
        if (need_remove) {
            uint64_t *ids = this->sort_ids;
            for (uint64_t i = 0; i < this->n_cur_objects; i++) {
                ids[i] = i;
            }
            
            _obj_metadata = this->obj_metadata;
            std::sort(ids, ids + this->n_cur_objects, cmp_obj_metadata_time);
            
            uint64_t n_removed_pages = 0;
            uint64_t n_removed_objects = 0;
            for (uint64_t i = 0; i < this->n_cur_objects; i++) {
                uint64_t id = ids[i];
                Metadata *metadata = this->obj_metadata + id;
                uint64_t len = metadata->len;
                uint64_t n_pages = std::max((len + ONE_PAGE_CONTENT_SIZE - 1) / ONE_PAGE_CONTENT_SIZE, 1ul);
                
                n_removed_objects++;
                n_removed_pages += n_pages;
                
                metadata->last_used_tsc = 0;  // Invalidate
                
                void *addr = metadata->start_addr;
                for (uint64_t page_id = 1; page_id < n_pages; page_id++) {
                    addr = (void *) * (uint64_t *) addr;
                }
                
                * (uint64_t *) addr = (uint64_t) this->free_list_addr;
                this->free_list_addr = metadata->start_addr;
                
                bool still_need_remove = false;
                if (n_need_pages > this->n_free_pages + n_removed_pages) {
                    still_need_remove = true;
                } else if (this->n_cur_objects + 1 > this->n_max_objects + n_removed_objects) {
                    still_need_remove = true;
                }
                
                if (!still_need_remove) break;
            }
            
            // Defrag
            uint64_t j = 0;
            for (uint64_t i = 0; i < this->n_cur_objects; i++) {
                if (this->obj_metadata[i].last_used_tsc == 0) {
                    // Invalidated
                    continue;
                } else {
                    this->obj_hash[j] = this->obj_hash[i];
                    this->obj_digest[j] = this->obj_digest[i];
                    this->obj_metadata[j] = this->obj_metadata[i];
                    j++;
                }
            }
            
            this->n_cur_objects -= n_removed_objects;
            this->n_free_pages += n_removed_pages;
            
            assert(this->find(digest, &idx) == false);
        }
        
        // Move [idx, n)
        memmove(this->obj_hash + idx + 1, this->obj_hash + idx, (this->n_cur_objects - idx) * sizeof(uint64_t));
        memmove(this->obj_digest + idx + 1, this->obj_digest + idx, (this->n_cur_objects - idx) * sizeof(Digest256));
        memmove(this->obj_metadata + idx + 1, this->obj_metadata + idx, (this->n_cur_objects - idx) * sizeof(Metadata));
        
        // Store at idx
        this->obj_hash[idx] = digest->a[0];
        this->obj_digest[idx] = *digest;
        this->n_cur_objects++;
        
        Metadata *metadata = this->obj_metadata + idx;
        metadata->last_used_tsc = Timer::tsc_since_epoch();
        metadata->start_addr = this->free_list_addr;
        metadata->len = len;
        
        // Copy contents
        void *addr = metadata->start_addr;
        for (uint64_t cur_len = 0; cur_len < len; cur_len += ONE_PAGE_CONTENT_SIZE) {
            memcpy((uint64_t *) addr + 1, src, std::min(len - cur_len, ONE_PAGE_CONTENT_SIZE));
            addr = (void *) * (uint64_t *) addr;
            src = (const void *) ((uint64_t) src + ONE_PAGE_CONTENT_SIZE);
        }
        
        if (len == 0) {
            addr = (void *) * (uint64_t *) addr;
        }
        
        this->n_free_pages -= n_need_pages;
        this->free_list_addr = addr;
        
        return true;
    }
    
    void DuckCache::info(char *output) {
        sprintf(
            output,
            "size %lu, n_objects %lu / %lu, n_pages %lu / %lu",
            this->cache_size,
            this->n_cur_objects, this->n_max_objects,
            this->n_total_pages - this->n_free_pages, this->n_total_pages
        );
    }
}
