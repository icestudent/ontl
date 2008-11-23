#ifndef NTL__PARALLEL_HASH_MAP
#define NTL__PARALLEL_HASH_MAP

/* scalable concurrent hash map by remark */

namespace rl 
{ 

template<typename key_t, typename value_t> 
struct concurrent_hash_map_traits 
{ 
    typedef key_t       key_in_t; 
    typedef key_t&      key_out_t; 
    typedef value_t     value_in_t; 
    typedef value_t&    value_out_t; 

    static size_t hash(key_in_t k) 
    { 
        return static_cast<size_t>(k); 
    } 

    static bool compare_key(key_in_t k1, key_in_t k2) 
    { 
        return k1 == k2; 
    } 

    static void copy_key(key_out_t k1, key_in_t k2) 
    { 
        k1 = k2; 
    } 

    static void copy_value(value_out_t v1, value_in_t v2) 
    { 
        v1 = v2; 
    } 

    static void atomic_copy_value(value_out_t v1, value_in_t v2) 
    { 
        v1 = v2; 
    } 

    static void construct_key(void* k) 
    { 
        new (k) key_t (); 
    } 

    static void construct_value(void* v) 
    { 
        new (v) value_t (); 
    } 

    static void destroy_key(key_t* k) 
    { 
    } 

    static void destroy_value(value_t* v) 
    { 
    } 

}; 

template<typename type> 
struct is_pod 
{ 
    static bool const result = __is_pod(type); 

}; 

template<> struct is_pod<int> 
    {static bool const result = true;}; 
template<> struct is_pod<unsigned int> 
    {static bool const result = true;}; 
template<> struct is_pod<long> 
    {static bool const result = true;}; 
template<> struct is_pod<unsigned long> 
    {static bool const result = true;}; 
template<> struct is_pod<__int64> 
    {static bool const result = true;}; 
template<> struct is_pod<unsigned __int64> 
    {static bool const result = true;}; 

template<typename key_t, typename value_t, 
    size_t key_size, bool key_pod, size_t value_size, bool value_pod> 
struct concurrent_hash_map_layout; 

template<typename key_t, typename value_t> 
struct concurrent_hash_map_layout<key_t, value_t, 4, true, 4, true> 
{ 
    static size_t const initial_size = 256; 
    static size_t const cell_to_add_cell_ratio = 128; 
    static size_t const cell_item_count = 3; 
    static size_t const add_cell_item_count = 10; 

    static unsigned const state_count = 0x1; 
    static unsigned const state_count_mask = 0x3; 
    static unsigned const state_mask_bit = 0x4; 
    static unsigned const state_add_list = 0x40; 
    static unsigned const state_lock = 0x80; 
    static unsigned const state_ver = 0x100; 

    struct add_item_t 
    { 
        add_item_t* volatile    next_; 
        key_t                   key_; 
        value_t                 value_; 
    }; 

    struct add_cell_t 
    { 
        unsigned volatile       lock_; 
        add_item_t*             head_; 
        add_item_t              item_ [add_cell_item_count]; 
    }; 

}; 

template<typename key_t, typename value_t> 
struct concurrent_hash_map_layout<key_t, value_t, 8, true, 8, true> 
{ 
    static size_t const initial_size = 256; 
    static size_t const cell_to_add_cell_ratio = 128; 
    static size_t const cell_item_count = 3; 
    static size_t const add_cell_item_count = 10; 

    static unsigned const state_count = 0x1; 
    static unsigned const state_count_mask = 0x3; 
    static unsigned const state_mask_bit = 0x4; 
    static unsigned const state_add_list = 0x40; 
    static unsigned const state_lock = 0x80; 
    static unsigned const state_ver = 0x100; 

    struct add_item_t 
    { 
        add_item_t* volatile    next_; 
        key_t                   key_; 
        value_t                 value_; 
    }; 

    struct add_cell_t 
    { 
        unsigned volatile       lock_; 
        add_item_t*             head_; 
        add_item_t              item_ [add_cell_item_count]; 
        char                    unused_pad_ [8]; 
    }; 

}; 

template<typename key_t, typename value_t> 
struct concurrent_hash_map_layout<key_t, value_t, 4, true, 8, true> 
{ 
    static size_t const initial_size = 256; 
    static size_t const cell_to_add_cell_ratio = 128; 
    static size_t const cell_item_count = 4; 
    static size_t const add_cell_item_count = 7; 

    static unsigned const state_count = 0x1; 
    static unsigned const state_count_mask = 0x3; 
    static unsigned const state_mask_bit = 0x4; 
    static unsigned const state_add_list = 0x40; 
    static unsigned const state_lock = 0x80; 
    static unsigned const state_ver = 0x100; 

    struct add_item_t 
    { 
        add_item_t* volatile    next_; 
        key_t                   key_; 
        value_t                 value_; 
    }; 

    struct add_cell_t 
    { 
        unsigned volatile       lock_; 
        add_item_t*             head_; 
        add_item_t              item_ [add_cell_item_count]; 
        char                    unused_pad_ [8]; 
    }; 

}; 

template<typename key_t, typename value_t> 
struct concurrent_hash_map_layout<key_t, value_t, 8, true, 4, true> 
{ 
    static size_t const initial_size = 256; 
    static size_t const cell_to_add_cell_ratio = 128; 
    static size_t const cell_item_count = 4; 
    static size_t const add_cell_item_count = 7; 

    static unsigned const state_count = 0x1; 
    static unsigned const state_count_mask = 0x3; 
    static unsigned const state_mask_bit = 0x4; 
    static unsigned const state_add_list = 0x40; 
    static unsigned const state_lock = 0x80; 
    static unsigned const state_ver = 0x100; 

    static size_t const add_item_size = 128; 

    struct add_item_t 
    { 
        key_t                   key_; 
        value_t                 value_; 
        add_item_t* volatile    next_; 
    }; 

    struct add_cell_t 
    { 
        unsigned volatile       lock_; 
        add_item_t*             head_; 
        add_item_t              item_ [add_cell_item_count]; 
        char                    unused_pad_ [8]; 
    }; 

}; 

template<typename key_t, typename value_t, 
    typename traits_t = concurrent_hash_map_traits<key_t, value_t> > 
class concurrent_hash_map 
{ 
public: 
    typedef typename traits_t::key_in_t key_in_t; 
    typedef typename traits_t::key_out_t key_out_t; 
    typedef typename traits_t::value_in_t value_in_t; 
    typedef typename traits_t::value_out_t value_out_t; 
    typedef size_t hash_t; 
    typedef concurrent_hash_map_layout<key_t, value_t, 
        sizeof(key_t), is_pod<key_t>::result, 
        sizeof(value_t), is_pod<value_t>::result> layout_t; 

    static size_t const initial_size = layout_t::initial_size; 
    static size_t const cell_to_add_cell_ratio = 
        layout_t::cell_to_add_cell_ratio; 
    static size_t const cell_item_count = layout_t::cell_item_count; 
    static size_t const add_cell_item_count = 
        layout_t::add_cell_item_count; 

    static unsigned const state_count = layout_t::state_count; 
    static unsigned const state_count_mask = 
        layout_t::state_count_mask; 
    static unsigned const state_mask_bit = layout_t::state_mask_bit; 
    static unsigned const state_add_list = layout_t::state_add_list; 
    static unsigned const state_lock = layout_t::state_lock; 
    static unsigned const state_ver = layout_t::state_ver; 

    typedef typename layout_t::add_item_t add_item_t; 
    typedef typename layout_t::add_cell_t add_cell_t; 

    struct cell_t 
    { 
        unsigned volatile       state_; 
        add_item_t* volatile    head_; 
        key_t                   key_ [cell_item_count]; 
        value_t                 value_ [cell_item_count]; 
    }; 

    struct block_hdr_t : pcx_node 
    { 
        hash_t                  cell_mask_; 
        size_t                  cell_count_; 
        size_t                  add_cell_count_; 
        cell_t*                 cell_; 
        add_cell_t*             add_cell_; 
        void*                   mem_; 
    }; 

    struct handle_t 
    { 
        cell_t*                 cell_; 
        unsigned                state_; 
        value_t*                value_; 
    }; 

    concurrent_hash_map() 
    { 
        anchor_ = create_block(initial_size); 
        resizing_ = 0; 
    } 

    ~concurrent_hash_map() 
    { 
        block_dtor(anchor_); 
    } 

    bool find(key_in_t k, value_out_t v) 
    { 
        hash_t const hash = traits_t::hash(k); 
retry: 
        block_hdr_t* b = anchor_; // memory_order_consume 
        size_t const cell_idx = hash & b->cell_mask_; 
        cell_t& cell = b->cell_[cell_idx]; 
        unsigned const state = cell.state_; // load_acquire_wrt_loads 
        unsigned const item_count = state & state_count_mask; 
        if (item_count > 0) 
        { 
            if (traits_t::compare_key(k, cell.key_[0])) 
            { 
                if (state & (state_mask_bit << 0)) 
                { 
                    traits_t::copy_value(v, cell.value_[0]); 
                    // load_release_wrt_loads 
                    unsigned const state2 = cell.state_; 
                    if (state == state2) 
                        return true; 
                    else 
                        goto retry; 
                } 
                else 
                { 
                    // load_release_wrt_loads 
                    unsigned const state2 = cell.state_; 
                    if (state == state2) 
                        return false; 
                    else 
                        goto retry; 
                } 
            } 

            if (item_count > 1) 
            { 
                if (traits_t::compare_key(k, cell.key_[1])) 
                { 
                    if (state & (state_mask_bit << 1)) 
                    { 
                        traits_t::copy_value(v, cell.value_[1]); 
                        // load_release_wrt_loads 
                        unsigned const state2 = cell.state_; 
                        if (state == state2) 
                            return true; 
                        else 
                            goto retry; 
                    } 
                    else 
                    { 
                        // load_release_wrt_loads 
                        unsigned const state2 = cell.state_; 
                        if (state == state2) 
                            return false; 
                        else 
                            goto retry; 
                    } 
                } 

                if (item_count > 2) 
                { 
                    if (traits_t::compare_key(k, cell.key_[2])) 
                    { 
                        if (state & (state_mask_bit << 2)) 
                        { 
                            traits_t::copy_value(v, cell.value_[2]); 
                             // load_release_wrt_loads 
                            unsigned const state2 = cell.state_; 
                            if (state == state2) 
                                return true; 
                            else 
                                goto retry; 
                        } 
                        else 
                        { 
                            // load_release_wrt_loads 
                            unsigned const state2 = cell.state_; 
                            if (state == state2) 
                                return false; 
                            else 
                                goto retry; 
                        } 
                    } 

                    add_item_t* aitem = cell.head_; 
                    while (aitem) 
                    { 
                        if (traits_t::compare_key(k, aitem->key_)) 
                        { 
                            traits_t::copy_value(v, aitem->value_); 
                            // load_release_wrt_loads 
                            unsigned const state2 = cell.state_; 
                            if (state == state2) 
                                return true; 
                            else 
                                goto retry; 
                        } 
                        aitem = aitem->next_; 
                    } 
                } 
            } 
        } 
        unsigned const state2 = cell.state_; // load_release_wrt_loads 
        if (state == state2) 
            return false; 
        else 
            goto retry; 
    } 

    bool find_and_lock(key_in_t k, value_out_t v, handle_t& h) 
    { 
        size_t const hash = traits_t::hash(k); 
        block_hdr_t* b; 
        unsigned state; 
        cell_t& cell = lock_cell(hash, b, state); 
        unsigned const item_count = state & state_count_mask; 
        for (unsigned i = 0; i != item_count; ++i) 
        { 
            if (traits_t::compare_key(k, cell.key_[i])) 
            { 
                traits_t::copy_value(v, cell.value_[i]); 
                h.cell_ = &cell; 
                h.state_ = state; 
                h.value_ = &cell.value_[i]; 
                return true; 
            } 
        } 
        add_item_t* aitem = cell.head_; 
        while (aitem) 
        { 
            if (traits_t::compare_key(k, aitem->key_)) 
            { 
                traits_t::copy_value(v, aitem->value_); 
                h.cell_ = &cell; 
                h.state_ = state; 
                h.value_ = &aitem->value_; 
                return true; 
            } 
            aitem = aitem->next_; 
        } 
        unsigned const state2 = state & ~state_lock; 
        cell.state_ = state2; // memory_order_release 
        return false; 
    } 

    void update_and_unlock(value_in_t v, handle_t const& h) 
    { 
        traits_t::atomic_copy_value(*h.value_, v); 
        unsigned const state2 = (h.state_ & ~state_lock) + state_ver; 
        h.cell_->state_ = state2; // memory_order_release 
    } 

    void unlock(handle_t const& h) 
    { 
        unsigned const state2 = h.state_ & ~state_lock; 
        h.cell_->state_ = state2; 
    } 

    bool insert(key_in_t k, value_in_t v) 
    { 
        size_t const hash = traits_t::hash(k); 
retry: 
        block_hdr_t* b; 
        unsigned state; 
        cell_t& cell = lock_cell(hash, b, state); 
        unsigned const item_count = state & state_count_mask; 
        for (unsigned i = 0; i != item_count; ++i) 
        { 
            if (traits_t::compare_key(k, cell.key_[i])) 
            { 
                unsigned const state2 = state & ~state_lock; 
                cell.state_ = state2; // memory_order_release 
                assert(0 == (cell.state_ & state_lock)); 
                return false; 
            } 
        } 
        add_item_t* aitem = cell.head_; 
        while (aitem) 
        { 
            if (traits_t::compare_key(k, aitem->key_)) 
            { 
                unsigned const state2 = state & ~state_lock; 
                cell.state_ = state2; // memory_order_release 
                assert(0 == (cell.state_ & state_lock)); 
                return false; 
            } 
            aitem = aitem->next_; 
        } 
        if (item_count < cell_item_count) 
        { 
            traits_t::copy_key(cell.key_[item_count], k); 
            traits_t::copy_value(cell.value_[item_count], v); 
            unsigned const state2 = ((state & ~state_lock) 
                | (state_mask_bit << item_count)) 
                + state_count + state_ver; 
            cell.state_ = state2; // memory_order_release 
            assert(0 == (cell.state_ & state_lock)); 
            return true; 
        } 
        else 
        { 
            add_item_t* aitem = alloc_add_item(b, hash); 
            if (0 == aitem) 
            { 
                grow(cell, state); 
                goto retry; 
            } 
            traits_t::copy_key(aitem->key_, k); 
            traits_t::copy_value(aitem->value_, v); 
            aitem->next_ = cell.head_; 
            cell.head_ = aitem; // memory_order_release 
            unsigned const state2 = ((state & ~state_lock) 
                | state_add_list) + state_ver; 
            cell.state_ = state2; // memory_order_release 
        } 
        assert(0 == (cell.state_ & state_lock)); 
        return true; 
    } 

    bool insert_or_update(key_in_t k, value_in_t v) 
    { 
        size_t const hash = traits_t::hash(k); 
retry: 
        block_hdr_t* b; 
        unsigned state; 
        cell_t& cell = lock_cell(hash, b, state); 
        unsigned const item_count = state & state_count_mask; 
        for (unsigned i = 0; i != item_count; ++i) 
        { 
            if (traits_t::compare_key(k, cell.key_[i])) 
            { 
                traits_t::atomic_copy_value(cell.value_[i], v); 
                unsigned const state2 = 
                    (state & ~state_lock) + state_ver; 
                cell.state_ = state2; // memory_order_release 
                return false; 
            } 
        } 
        add_item_t* aitem = cell.head_; 
        while (aitem) 
        { 
            if (traits_t::compare_key(k, aitem->key_)) 
            { 
                traits_t::atomic_copy_value(aitem->value_, v); 
                unsigned const state2 = 
                    (state & ~state_lock) + state_ver; 
                cell.state_ = state2; // memory_order_release 
                return false; 
            } 
            aitem = aitem->next_; 
        } 
        if (item_count < cell_item_count) 
        { 
            traits_t::copy_key(cell.key_[item_count], k); 
            traits_t::copy_value(cell.value_[item_count], v); 
            unsigned const state2 = ((state & ~state_lock) 
                | (state_mask_bit << item_count)) 
                + state_count + state_ver; 
            cell.state_ = state2; // memory_order_release 
            return true; 
        } 
        else 
        { 
            add_item_t* aitem = alloc_add_item(b, hash); 
            if (0 == aitem) 
            { 
                grow(cell, state); 
                goto retry; 
            } 
            traits_t::copy_key(aitem->key_, k); 
            traits_t::copy_value(aitem->value_, v); 
            aitem->next_ = cell.head_; 
            cell.head_ = aitem; // memory_order_release 
            if (aitem->next_) 
            { 
                unsigned const state2 = 
                    state & ~state_lock) + state_ver; 
                cell.state_ = state2; // memory_order_release 
            } 
            else 
            { 
                unsigned const state2 = 
                    ((state & ~state_lock) | state_add_list) 
                    + state_ver; 
                cell.state_ = state2; // memory_order_release 
            } 
        } 
        return true; 
    } 

    bool remove(key_in_t k) 
    { 
        size_t const hash = traits_t::hash(k); 
        block_hdr_t* b; 
        unsigned state; 
        cell_t& cell = lock_cell(hash, b, state); 
        unsigned const item_count = state & state_count_mask; 
        for (unsigned i = 0; i != item_count; ++i) 
        { 
            if (traits_t::compare_key(k, cell.key_[i])) 
            { 
                unsigned const state2 = 
                    (state & ~(state_mask_bit << i)) + state_ver; 
                cell.state_ = state2; // memory_order_release 
                if (cell.head_) 
                { 
                    add_item_t* aitem = cell.head_; 
                    traits_t::copy_key(cell.key_[i], aitem->key_); 
                    traits_t::copy_value 
                        (cell.value_[i], aitem->value_); 
                    unsigned const state3 = state + 2 * state_ver; 
                    cell.state_ = state3; // memory_order_release 
                    add_item_t* aitem_next = aitem->next_; 
                    cell.head_ = aitem_next; 
                    if (aitem_next) 
                    { 
                        unsigned const state4 = 
                            (state & ~state_lock) + 3 * state_ver; 
                        cell.state_ = state4; // memory_order_release 
                    } 
                    else 
                    { 
                        unsigned const state4 = 
                            ((state & ~state_lock) & ~state_add_list) 
                            + 3 * state_ver; 
                        cell.state_ = state4; // memory_order_release 
                    } 
                    free_add_item(aitem); 
                } 
                else if (i != item_count - 1) 
                { 
                    traits_t::copy_key(cell.key_[i], 
                        cell.key_[item_count - 1]); 
                    traits_t::copy_value(cell.value_[i], 
                        cell.value_[item_count - 1]); 
                    unsigned state3 = (state & ~state_lock) 
                        - state_count + state_ver; 
                    cell.state_ = state3; // memory_order_release 
                } 
                else 
                { 
                    unsigned state3 = 
                        (state & ~state_lock) 
                        - state_count + state_ver; 
                    cell.state_ = state3; // memory_order_release 
                } 
                assert(0 == (cell.state_ & state_lock)); 
                return true; 
            } 
        } 
        add_item_t* volatile* aitem_prev = &cell.head_; 
        add_item_t* aitem = *aitem_prev; 
        while (aitem) 
        { 
            if (traits_t::compare_key(k, aitem->key_)) 
            { 
                add_item_t* aitem_next = aitem->next_; 
                *aitem_prev = aitem_next; 
                if (cell.head_) 
                { 
                    unsigned state2 = 
                        (state & ~state_lock) + state_ver; 
                    cell.state_ = state2; // memory_order_release 
                } 
                else 
                { 
                    unsigned state2 = ((state & ~state_lock) 
                        & ~state_add_list) + state_ver; 
                    cell.state_ = state2; // memory_order_release 
                } 
                free_add_item(aitem); 
                assert(0 == (cell.state_ & state_lock)); 
                return true; 
            } 
            aitem_prev = &aitem->next_; 
            aitem = *aitem_prev; 
        } 
        unsigned const state2 = state & ~state_lock; 
        cell.state_ = state2; // memory_order_release 
        assert(0 == (cell.state_ & state_lock)); 
        return false; 
    } 

private: 
    block_hdr_t* volatile       anchor_; 
    unsigned volatile           resizing_; 

    block_hdr_t* create_block(size_t cell_count) 
    { 
        size_t add_cell_count = 
            cell_count / cell_to_add_cell_ratio; 
        size_t size = 
            sizeof(block_hdr_t) + sizeof(cell_t) * cell_count 
            + sizeof(add_cell_t) * add_cell_count; 
        void* mem = _aligned_offset_malloc 
            (size, cacheline_size, sizeof(block_hdr_t)); 
        block_hdr_t* b = new (mem) block_hdr_t; 
        b->cell_mask_ = cell_count - 1; 
        b->cell_count_ = cell_count; 
        b->add_cell_count_ = add_cell_count; 
        b->cell_ = (cell_t*)((char*)b + sizeof(block_hdr_t)); 
        b->add_cell_ = (add_cell_t*)((char*)b + sizeof(block_hdr_t) 
                + sizeof(cell_t) * cell_count); 
        b->mem_ = mem; 
        cell_t* cell = (cell_t*)((char*)mem + sizeof(block_hdr_t)); 
        // for now support only for pod types 
        // if block is very big than use non-temporal stores 
        memset(cell, 0, size - sizeof(block_hdr_t)); 
        return b; 
    } 

    static void block_dtor(pcx_node* n) 
    { 
        _aligned_free(static_cast<block_hdr_t*>(n)->mem_); 
    } 

    add_item_t* alloc_add_item(block_hdr_t* b, size_t hash) 
    { 
        size_t const add_cell_count = b->add_cell_count_; 
        for (size_t iter = 0; iter != 2; ++iter) 
        { 
            for (size_t idx = 0; idx != add_cell_count; ++idx) 
            { 
                size_t const add_cell_idx = 
                    (hash + idx) % add_cell_count; 
                add_cell_t& add_cell = b->add_cell_[add_cell_idx]; 
                for (;;) 
                { 
                    if (state_lock != _InterlockedExchange 
                        ((long*)&add_cell.lock_, state_lock)) 
                        break; 
                    SwitchToThread(); 
                } 
                if (add_cell.head_) 
                { 
                    add_item_t* aitem = add_cell.head_; 
                    add_cell.head_ = aitem->next_; 
                    add_cell.lock_ = 0; 
                    return aitem; 
                } 
                add_cell.lock_ = 0; 
            } 
        } 
        return 0; 
    } 

    void free_add_item(add_item_t* item) 
    { 
        add_cell_t& add_cell = 
            *(add_cell_t*)((intptr_t)item 
            - (intptr_t)item % sizeof(add_cell_t)); 
        for (;;) 
        { 
            if (state_lock != _InterlockedExchange 
                ((long*)add_cell.lock_, state_lock)) 
                break; 
            SwitchToThread(); 
        } 
        item->next_ = add_cell.head_; 
        add_cell.head_ = item; 
        add_cell.lock_ = 0; 
    } 

    __forceinline 
    cell_t& lock_cell 
        (hash_t hash, block_hdr_t*& block, unsigned& state) 
    { 
        for (;;) 
        { 
            block_hdr_t* b = anchor_; // memory_order_consume 
            size_t const cell_idx = hash & b->cell_mask_; 
            cell_t& cell = b->cell_[cell_idx]; 
            unsigned const st = cell.state_; 
            if (st & state_lock) 
            { 
                SwitchToThread(); 
                continue; 
            } 
            if (st == (unsigned)_InterlockedCompareExchange 
                ((long*)&cell.state_, st | state_lock, st)) 
            { 
                block = b; 
                state = st | state_lock; 
                return cell; 
            } 
        } 
    } 

    void grow(cell_t& cell, unsigned state) 
    { 
        unsigned const already_resizing = 
            (unsigned)_InterlockedExchange 
            (long*)&resizing_, state_lock); 
        unsigned const state2 = state & ~state_lock; 
        cell.state_ = state2; // memory_order_release 
        if (already_resizing) 
        { 
            while (resizing_) 
            { 
                SwitchToThread(); 
            } 
        } 
        else 
        { 
            block_hdr_t* b = anchor_; 
            size_t const cell_count = b->cell_count_; 
            block_hdr_t* new_block = create_block(cell_count * 4); 
            for (size_t i = 0; i != cell_count; ++i) 
            { 
                block_hdr_t* bb; 
                unsigned state; 
                lock_cell(i, bb, state); 
            } 
            for (size_t cell_idx = 0; cell_idx != cell_count; 
                ++cell_idx) 
            { 
                cell_t& cell = b->cell_[cell_idx]; 
                unsigned const state = cell.state_; 
                unsigned const item_count = state & state_count_mask; 
                for (unsigned i = 0; i != item_count; ++i) 
                { 
                    hash_t hash = traits_t::hash(cell.key_[i]); 
                    cell_t& new_cell = new_block->cell_ 
                        [hash & new_block->cell_mask_]; 
                    unsigned new_cell_count = 
                        new_cell.state_ & state_count_mask; 
                    traits_t::copy_key( 
                        new_cell.key_[new_cell_count], 
                        cell.key_[i]); 
                    traits_t::copy_value( 
                        new_cell.value_[new_cell_count], 
                        cell.value_[i]); 
                    new_cell.state_ = new_cell.state_ + state_count 
                        + (state_mask_bit << new_cell_count); 
                } 
                add_item_t* aitem = cell.head_; 
                while (aitem) 
                { 
                    hash_t hash = traits_t::hash(aitem->key_); 
                    cell_t& new_cell = new_block->cell_ 
                        [hash & new_block->cell_mask_]; 
                    unsigned new_cell_count = 
                        new_cell.state_ & state_count_mask; 
                    if (new_cell_count < cell_item_count) 
                    { 
                        traits_t::copy_key(new_cell.key_ 
                            [new_cell_count], aitem->key_); 
                        traits_t::copy_value(new_cell.value_ 
                            [new_cell_count], aitem->value_); 
                        new_cell.state_ = new_cell.state_ 
                            + state_count 
                            + (state_mask_bit << new_cell_count); 
                    } 
                    else 
                    { 
                        add_item_t* new_aitem = 
                            alloc_add_item(new_block, hash); 
                        assert(new_aitem); 
                        traits_t::copy_key 
                            (new_aitem->key_, aitem->key_); 
                        traits_t::copy_value 
                            (new_aitem->value_, aitem->value_); 
                        new_aitem->next_ = new_cell.head_; 
                        new_cell.head_ = new_aitem; 
                        new_cell.state_ = 
                            new_cell.state_ | state_add_list; 
                    } 
                    aitem = aitem->next_; 
                } 
            } 
            anchor_ = new_block; // memory_order_release 
            resizing_ = 0; 
            pcx_thread& th = pcx_thread::get(); 
            th.defer(b, &concurrent_hash_map::block_dtor); 
            th.promote(); 
        } 
    } 

    concurrent_hash_map(concurrent_hash_map const&); 
    concurrent_hash_map& operator = (concurrent_hash_map const&); 

}; 
} 

/************************* pcx.h *************************/ 

#pragma once 

#include <intrin.h> 
#pragma intrinsic (_InterlockedExchangeAdd) 
#pragma intrinsic (_InterlockedCompareExchange) 

namespace rl 
{ 

size_t const cacheline_size = 64; 

struct pcx_node 
{ 
    typedef void            (*pcx_dtor_t)(pcx_node*); 
    pcx_node*               pcx_next_; 
    pcx_dtor_t              pcx_dtor_; 

}; 

namespace pcx_int 
{ 
    unsigned const word_bits = 32; 
    unsigned const collector_bits = 4; 
    unsigned const collector_count = 1 << collector_bits; 
    unsigned const counter_inc = 1 << (collector_bits * 2); 
    unsigned const is_current_inc = 1; 
    unsigned const back_link_inc = 2; 

    struct master; 
    struct collector; 

    struct local_collector 
    { 
        pcx_node*               defer_head_; 
        pcx_node                defer_tail_; 
        unsigned                defer_size_; 
    }; 

    struct thread_int 
    { 
        pcx_int::master*        master_; 
        pcx_int::collector*     collectors_; 
        unsigned                recursion_count_; 
        unsigned                is_acquired_; 
        unsigned                collector_index_; 
        unsigned                last_seen_collector_index_; 
        unsigned                flush_tail_; 
        pcx_node*               defer_head_; 
        pcx_node                defer_tail_; 
        unsigned                defer_size_; 
        unsigned                promote_; 
        local_collector         local_collectors_ [collector_count]; 
    }; 

} 

class pcx_thread : private pcx_int::thread_int 
{ 
public: 
    static pcx_thread& get(); 

    void acquire(); 
    void release(); 
    void defer(pcx_node* node, pcx_node::pcx_dtor_t dtor); 
    void flush(); 
    void promote(); 
    void quiescent(); 

private: 
    void init(); 
    void deinit(); 
    unsigned acquire_impl(); 
    void release_impl(unsigned, unsigned); 
    void flush_impl(); 
    void local_flush(); 
    void quiescent_impl(); 
    friend void init(); 
    friend void deinit(); 
    friend void thread_callback(bool); 

}; 

namespace pcx_int 
{ 
    struct master 
    { 
        char                pad0_ [64]; 

        unsigned            garbage_threshold_; 

        char                pad1_ [64]; 

        struct state_part 
        { 
            unsigned        current_collector_ : collector_bits; 
            unsigned        collector_tail_ : collector_bits; 
            unsigned        outer_counter_ : 
                word_bits - 2 * collector_bits; 
        }; 

        union state 
        { 
             long           whole_; 
             state_part     part_; 
        }; 

        state               state_; 

        char                pad2_ [64]; 

        state               state_copy_; 

        char                pad3_ [64]; 
    }; 

    struct collector 
    { 
        char                pad0_ [64]; 

        pcx_node*           defer_list_head_; 
        unsigned            defer_list_size_; 

        char                pad1_ [64]; 

        struct state_part 
        { 
            unsigned        is_current_ : 1; 
            unsigned        back_link_ : 1; 
            unsigned        pad_ : collector_bits * 2 - 2; 
            unsigned        inner_counter_ : 
                word_bits - 2 * collector_bits; 
        }; 

        union state 
        { 
             long           whole_; 
             state_part     part_; 
        }; 

        state               state_; 

        char                pad2_ [64]; 
    }; 

    __declspec(selectany) 
    master                  g_master; 
    __declspec(selectany) 
    collector               g_collectors [collector_count]; 
    __declspec(selectany, thread) 
    thread_int              g_thread_instance; 

    typedef void (__stdcall nt_tls_cb_t)(void*, unsigned long, void*); 
    nt_tls_cb_t on_tls_callback; 

    #pragma data_seg(push, old_seg) 
    #pragma data_seg(".CRT$XLB") 
    __declspec(selectany, dllexport) 
    nt_tls_cb_t* volatile p_thread_callback = on_tls_callback; 
    #pragma data_seg(pop, old_seg) 

    inline void __stdcall on_tls_callback 
        (void*, unsigned long reason, void*) 
    { 
        if (1 == reason) 
        { 
            init(); 
            thread_callback(true); 
        } 
        else if (0 == reason) 
        { 
            thread_callback(false); 
            deinit(); 
        } 
        if (2 == reason) 
        { 
            thread_callback(true); 
        } 
        else if (3 == reason) 
        { 
            thread_callback(false); 
        } 
    } 

} 

inline void init() 
{ 
    using namespace pcx_int; 
    master& m = g_master; 
    m.garbage_threshold_ = 128; 
    m.state_.part_.current_collector_ = 0; 
    m.state_.part_.collector_tail_ = 0; 
    m.state_.part_.outer_counter_ = 0; 
    m.state_copy_.part_.current_collector_ = 0; 
    m.state_copy_.part_.collector_tail_ = 0; 
    m.state_copy_.part_.outer_counter_ = 0; 
    for (unsigned i = 0; i != collector_count; ++i) 
    { 
        collector& c = g_collectors[i]; 
        c.defer_list_head_ = 0; 
        c.defer_list_size_ = 0; 
        c.state_.part_.is_current_ = 1; 
        c.state_.part_.back_link_ = 1; 
        c.state_.part_.inner_counter_ = 0; 
    } 
    g_collectors[0].state_.part_.back_link_ = 0; 

} 

inline void deinit() 
{ 
    using namespace pcx_int; 
    pcx_thread::get().release_impl( 
        g_master.state_.part_.current_collector_, is_current_inc); 

} 

inline void thread_callback(bool init) 
{ 
    if (init) 
        pcx_thread::get().init(); 
    else 
        pcx_thread::get().deinit(); 

} 

inline pcx_thread& pcx_thread::get() 
{ 
    return static_cast<pcx_thread&>(pcx_int::g_thread_instance); 

} 

inline unsigned pcx_thread::acquire_impl() 
{ 
    using namespace pcx_int; 
    long const prev = 
        _InterlockedExchangeAdd( 
            &master_->state_.whole_, counter_inc); 
    master::state_part u = {prev}; 
    if (u.current_collector_ == flush_tail_ 
        && local_collectors_[flush_tail_].defer_size_) 
    { 
        local_flush(); 
    } 

    return u.current_collector_; 

} 

inline void pcx_thread::release_impl(unsigned index, unsigned count) 
{ 
    using namespace pcx_int; 
    collector& c = collectors_[index]; 
    unsigned const prev = 
        _InterlockedExchangeAdd( 
            &c.state_.whole_, (unsigned)-(int)count); 
    if (0 == prev - count) 
    { 
        pcx_node* curr = c.defer_list_head_; 
        while (curr) 
        { 
            pcx_node* next = curr->pcx_next_; 
            curr->pcx_dtor_(curr); 
            curr = next; 
        } 
        c.defer_list_head_ = 0; 
        c.defer_list_size_ = 0; 
        c.state_.part_.back_link_ = 1; 
        c.state_.part_.is_current_ = 1; 

        long u; 
        if (index != collector_count - 1) 
            u = collector_count; 
        else 
            u = -(long)(collector_count * (collector_count - 1)); 
        _InterlockedExchangeAdd(&master_->state_.whole_, u); 

        release_impl((index + 1) % collector_count, back_link_inc); 
    } 

} 

inline void pcx_thread::flush_impl() 
{ 
    using namespace pcx_int; 
    _mm_mfence(); 
    master::state state = master_->state_; 
    last_seen_collector_index_ = state.part_.current_collector_; 
    collector& gc = collectors_[state.part_.current_collector_]; 
    local_collector& lc = local_collectors_ 
        [state.part_.current_collector_]; 
    lc.defer_head_->pcx_next_ = defer_tail_.pcx_next_; 
    lc.defer_head_ = defer_tail_.pcx_next_; 
    lc.defer_size_ += defer_size_; 
    defer_head_ = &defer_tail_; 
    defer_tail_.pcx_next_ = 0; 
    defer_size_ = 0; 
    if (master_->garbage_threshold_ < lc.defer_size_ || promote_) 
    { 
        master::state cmp; 
        master::state val; 
        do 
        { 
            cmp = master_->state_; 
            if (cmp.part_.current_collector_ != 
                last_seen_collector_index_) 
            { 
                promote_ = 0; 
                return; 
            } 
            unsigned next_index = 
                (last_seen_collector_index_ + 1) % collector_count; 
            if (cmp.part_.collector_tail_ == next_index) 
                return; 
            val = cmp; 
            val.part_.current_collector_ += 1; 
            val.part_.outer_counter_ = 0; 
        } 
        while (cmp.whole_ != _InterlockedCompareExchange( 
          (long*)&master_->state_.whole_, val.whole_, cmp.whole_)); 
        last_seen_collector_index_ = val.part_.current_collector_; 
        promote_ = 0; 
        _InterlockedIncrement((long*)&master_->state_copy_.whole_); 
        _InterlockedExchangeAdd((long*)&gc.state_.whole_, 
            cmp.part_.outer_counter_ * counter_inc - is_current_inc); 
    } 

} 

__declspec(noinline) 
inline void pcx_thread::local_flush() 
{ 
    using namespace pcx_int; 
    if (flush_tail_ == master_->state_.part_.collector_tail_) 
        return; 
    local_collector& lc = local_collectors_[flush_tail_]; 
    pcx_node* curr = lc.defer_tail_.pcx_next_; 
    while (curr) 
    { 
        pcx_node* next = curr->pcx_next_; 
        curr->pcx_dtor_(curr); 
        curr = next; 
    } 
    lc.defer_head_ = &lc.defer_tail_; 
    lc.defer_tail_.pcx_next_ = 0; 
    lc.defer_size_ = 0; 
    flush_tail_ = (flush_tail_ + 1) % collector_count; 

} 

__declspec(noinline) 
inline void pcx_thread::quiescent_impl() 
{ 
    using namespace pcx_int; 
    if (defer_size_) 
        flush_impl(); 
    release_impl(collector_index_, counter_inc); 
    collector_index_ = acquire_impl(); 

} 

inline void pcx_thread::acquire() 
{ 
    using namespace pcx_int; 
    recursion_count_ += 1; 
    if (1 != recursion_count_) 
        return; 
    if (is_acquired_) 
        return; 
    collector_index_ = acquire_impl(); 
    last_seen_collector_index_ = collector_index_; 
    is_acquired_ = 1; 

} 

inline void pcx_thread::release() 
{ 
    using namespace pcx_int; 
    recursion_count_ -= 1; 
    if (0 == recursion_count_) 
    { 
        if (master_->state_copy_.part_.current_collector_ != 
            collector_index_ 
            || promote_) 
        { 
            if (defer_size_) 
                flush_impl(); 
            release_impl(collector_index_, counter_inc); 
            is_acquired_ = 0; 
        } 
    } 
    if (flush_tail_ != last_seen_collector_index_) 
    { 
        local_flush(); 
    } 

} 

inline void pcx_thread::quiescent() 
{ 
    if (master_->state_copy_.part_.current_collector_ != 
        collector_index_ 
        || promote_) 
    { 
        quiescent_impl(); 
    } 
    if (flush_tail_ != last_seen_collector_index_) 
    { 
        local_flush(); 
    } 

} 

inline void pcx_thread::defer 
    (pcx_node* node, pcx_node::pcx_dtor_t dtor) 
{ 
    using namespace pcx_int; 
    node->pcx_next_ = 0; 
    node->pcx_dtor_ = dtor; 
    defer_head_->pcx_next_ = node; 
    defer_head_ = node; 
    defer_size_ += 1; 

} 

inline void pcx_thread::flush() 
{ 
    using namespace pcx_int; 
    if (recursion_count_) 
        return; 
    if (0 == is_acquired_) 
        return; 
    if (defer_size_) 
        flush_impl(); 
    release_impl(collector_index_, counter_inc); 
    is_acquired_ = 0; 

} 

inline void pcx_thread::promote() 
{ 
    promote_ = 1; 

} 

inline void pcx_thread::init() 
{ 
    using namespace pcx_int; 
    master_ = &g_master; 
    collectors_ = g_collectors; 
    defer_head_ = &defer_tail_; 
    defer_tail_.pcx_next_ = 0; 
    for (unsigned i = 0; i != collector_count; ++i) 
    { 
        local_collectors_[i].defer_head_ = 
            &local_collectors_[i].defer_tail_; 
    } 

} 

inline void pcx_thread::deinit() 
{ 
    flush(); 

} 
} 

/* use:
#include "pcx.h" 
#include "hash_map.h" 

int main() 
{ 
    rl::concurrent_hash_map<int, int> m; 

    rl::pcx_thread& th = rl::pcx_thread::get(); 
    th.acquire(); 

    // insert 
    for (int i = 0; i != 3; ++i) 
    { 
        bool inserted = m.insert(i, i + 1); 
        th.quiescent(); 
    } 

    // find 
    for (int i = 0; i != 3; ++i) 
    { 
        int value; 
        bool found = m.find(i, value); 
        if (found) 
        { 
            // use value 
        } 
        th.quiescent(); 
    } 

    // update or insert 
    for (int i = 0; i != 3; ++i) 
    { 
        bool inserted = m.insert_or_update(i, i + 2); 
        th.quiescent(); 
    } 

    // update inplace 
    for (int i = 0; i != 3; ++i) 
    { 
        int value; 
        rl::concurrent_hash_map<int, int>::handle_t h; 
        bool found = m.find_and_lock(i, value, h); 
        if (found) 
        { 
            if (rand() % 2) 
                m.update_and_unlock(value + 100, h); 
            else 
                m.unlock(h); 
        } 
        th.quiescent(); 
    } 

    th.release(); 

} 
*/
#endif // NTL__PARALLEL_HASH_MAP
