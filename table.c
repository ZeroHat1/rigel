#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <simde/x86/sse2.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"
#include "vm_value.h"
#include "vm_heap_obj.h"

#define KEY_CMP(val0, val1) strcmp(AS_STRING(val0)->chars, AS_STRING(val1)->chars)

static inline void table_paste_to(Entry *slots, uint8_t* ctrl, size_t pos, TValue key, TValue value, uint8_t h2)
{
    slots[pos].key = key;
    slots[pos].value = value;
    ctrl [pos] = h2;
}

static inline void table_size_inc(Table *t)
{
    ++t->size;
}

static void table_set_internal(Table *t, TValue key, TValue value)
{
    uint32_t hash = get_hash(AS_STRING(key));

    uint32_t h1 = hash >> 7;
    uint8_t  h2 = hash & 0x7F;

    size_t pos = h1 & (t->num_groups - 1);
    size_t deleted_pos = (size_t)-1;

    for (size_t i = 0; i <= t->num_groups; i++)
    {
        simde__m128i group = simde_mm_load_si128(&((simde__m128i*)t->ctrl)[pos]);

        simde__m128i key_needle = simde_mm_set1_epi8(h2);
        simde__m128i key_cmp = simde_mm_cmpeq_epi8(group, key_needle);

        uint16_t key_mask = simde_mm_movemask_epi8(key_cmp);

        while (key_mask != 0) 
        {
            size_t absolute_pos = pos * GROUP_SIZE + __builtin_ctz(key_mask);

            if (KEY_CMP(t->slots[absolute_pos].key, key) == 0)
            {
                t->slots[absolute_pos].value = value;
                return;
            }

            key_mask &= key_mask - 1;
        }

        uint16_t free_mask = simde_mm_movemask_epi8(group); // free in the sense of a noun, not a verb

        while (free_mask != 0) 
        {
            size_t absolute_pos = pos * GROUP_SIZE + __builtin_ctz(free_mask);

            // printf("%b %ld ", t->ctrl[absolute_pos] == SLOT_EMPTY, pos);

            if (deleted_pos == (size_t)-1 && t->ctrl[absolute_pos] == SLOT_DELETED)
            {
                deleted_pos = absolute_pos;
            }

            if (t->ctrl[absolute_pos] == SLOT_EMPTY)
            {
                if (deleted_pos == (size_t)-1)
                {
                    table_paste_to(t->slots, t->ctrl, absolute_pos, key, value, h2);
                }
                else
                {
                    table_paste_to(t->slots, t->ctrl, deleted_pos, key, value, h2);
                    --t->num_deleted;
                }

                ++t->size;
                
                return;
            }

            free_mask &= free_mask - 1;
        }

        pos = (pos + 1) & (t->num_groups - 1);
    }

    if (deleted_pos != (size_t)-1)
    {
        table_paste_to(t->slots, t->ctrl, deleted_pos, key, value, h2);
        ++t->size;
        --t->num_deleted;

        return;
    }
}

int table_init(Table *t)
{
    t->ctrl = aligned_alloc(16, GROUP_SIZE * sizeof(uint8_t));
    t->slots = aligned_alloc(16, GROUP_SIZE * sizeof(Entry));

    if (!(t->ctrl && t->slots))
    {
        return 1;
    }

    t->num_groups = 1;
    t->num_deleted = 0;
    t->size = 0;
    t->capacity = GROUP_SIZE;

    memset(t->ctrl, SLOT_EMPTY, GROUP_SIZE * sizeof(uint8_t));

    return 0;
}

int table_init_with_groups(Table *t, size_t num_groups)
{
    if (num_groups > 1)
    {
        num_groups = 32 - __builtin_clz(num_groups - 1);
        num_groups = 1u << num_groups;
    }
    else if (num_groups < 1)
    {
        return 2;
    }

    t->ctrl = aligned_alloc(16, num_groups * GROUP_SIZE * sizeof(uint8_t)); // align by 16 bytes
    t->slots = aligned_alloc(16, num_groups * GROUP_SIZE * sizeof(Entry));

    if (!(t->ctrl && t->slots))
    {
        return 1;
    }

    t->num_groups = num_groups;
    t->num_deleted = 0;
    t->size = 0;
    t->capacity = GROUP_SIZE * num_groups;

    memset(t->ctrl, SLOT_EMPTY, num_groups * GROUP_SIZE * sizeof(uint8_t));

    return 0;
}

void table_extend(Table *t)
{
    size_t old_num_groups = t->num_groups;
    size_t old_capacity   = t->capacity;

    uint8_t *old_ctrl  = t->ctrl;
    Entry   *old_slots = t->slots;

    size_t new_num_groups = old_num_groups * 2;
    size_t new_capacity = old_capacity  * 2;

    uint8_t *new_ctrl  = aligned_alloc(16, new_num_groups * GROUP_SIZE * sizeof(uint8_t));
    Entry   *new_slots = aligned_alloc(16, new_capacity   * sizeof(Entry));

    if (!new_ctrl || !new_slots)
    {
        free(new_ctrl);
        free(new_slots);

        return;
    }

    memset(new_ctrl, SLOT_EMPTY, new_num_groups * GROUP_SIZE * sizeof(uint8_t));

    t->ctrl = new_ctrl;
    t->slots = new_slots;
    t->num_groups = new_num_groups;
    t->capacity = new_capacity;
    t->size = 0;
    t->num_deleted = 0;

    for (size_t i = 0; i < old_capacity; i++) // same speed as the code below
    {
        if (old_ctrl[i] != SLOT_EMPTY && old_ctrl[i] != SLOT_DELETED)
        {
            table_set_internal(t, old_slots[i].key, old_slots[i].value);
        }
    }

    // for (size_t pos = 0; pos < old_num_groups; ++pos)
    // {
    //     simde__m128i group = simde_mm_load_si128(&((simde__m128i*)old_ctrl)[pos]);

    //     uint16_t mask = simde_mm_movemask_epi8(group);
    //     uint16_t taken_mask = (~mask) & 0xFFFF;

    //     while (taken_mask != 0) 
    //     {
    //         size_t absolute_pos = pos * GROUP_SIZE + __builtin_ctz(taken_mask);

    //         table_set_internal(t, old_slots[absolute_pos].key, old_slots[absolute_pos].value);

    //         taken_mask &= taken_mask - 1;
    //     }
    // }

    free(old_ctrl);
    free(old_slots);
}

int table_set(Table *t, TValue key, TValue value)
{
    if (t->size * 4 >= t->capacity * 3) // 75% load factor
    {
        table_extend(t);
    }

    uint32_t hash = get_hash(AS_STRING(key));

    uint32_t h1 = hash >> 7;
    uint8_t  h2 = hash & 0x7F;

    size_t pos = h1 & (t->num_groups - 1);
    size_t deleted_pos = -1;

    for (size_t i = 0; i < t->num_groups; i++)
    {
        simde__m128i group = simde_mm_load_si128(&((simde__m128i*)t->ctrl)[pos]);

        simde__m128i key_needle = simde_mm_set1_epi8(h2);
        simde__m128i key_cmp = simde_mm_cmpeq_epi8(group, key_needle);

        uint16_t key_mask = simde_mm_movemask_epi8(key_cmp);

        while (key_mask != 0) 
        {
            size_t absolute_pos = pos * GROUP_SIZE + __builtin_ctz(key_mask);

            if (KEY_CMP(t->slots[absolute_pos].key, key) == 0)
            {
                t->slots[absolute_pos].value = value;

                return 0;
            }

            key_mask &= key_mask - 1;
        }

        uint16_t free_mask = simde_mm_movemask_epi8(group); // free in the sense of a noun, not a verb

        while (free_mask != 0)
        {
            size_t absolute_pos = pos * GROUP_SIZE + __builtin_ctz(free_mask);

            if (deleted_pos == (size_t)-1 && t->ctrl[absolute_pos] == SLOT_DELETED)
            {
                deleted_pos = absolute_pos;
            }

            if (t->ctrl[absolute_pos] == SLOT_EMPTY) // if we find an empty slot, it means the key doesn't exist yet
            {
                if (deleted_pos == (size_t)-1)
                {
                    table_paste_to(t->slots, t->ctrl, absolute_pos, key, value, h2);
                }
                else
                {
                    table_paste_to(t->slots, t->ctrl, deleted_pos, key, value, h2);

                    --t->num_deleted;
                }

                table_size_inc(t);

                return 0;
            }

            free_mask &= free_mask - 1;
        }

        pos = (pos + 1) & (t->num_groups - 1);
    }

    if (deleted_pos != (size_t)-1)
    {
        table_paste_to(t->slots, t->ctrl, deleted_pos, key, value, h2);

        table_size_inc(t);

        --t->num_deleted;

        return 0;
    }

    return 1;
}

TValue table_get(Table *t, TValue key)
{
    uint32_t hash = get_hash(AS_STRING(key));

    uint32_t h1 = hash >> 7;
    uint8_t  h2 = hash & 0x7F;

    size_t pos = h1 & (t->num_groups - 1);

    for (size_t i = 0; i < t->num_groups; i++)
    {
        simde__m128i group = simde_mm_load_si128(&((simde__m128i*)t->ctrl)[pos]);

        simde__m128i key_needle = simde_mm_set1_epi8(h2);
        simde__m128i key_cmp = simde_mm_cmpeq_epi8(group, key_needle);

        uint16_t key_mask = simde_mm_movemask_epi8(key_cmp);

        while (key_mask != 0) 
        {
            size_t absolute_pos = pos * GROUP_SIZE + __builtin_ctz(key_mask);

            if (KEY_CMP(t->slots[absolute_pos].key, key) == 0)
            {
                return t->slots[absolute_pos].value;
            }

            key_mask &= key_mask - 1;
        }

        simde__m128i empty_needle = simde_mm_set1_epi8(SLOT_EMPTY);
        simde__m128i empty_cmp = simde_mm_cmpeq_epi8(group, empty_needle);
        uint16_t empty_mask = simde_mm_movemask_epi8(empty_cmp);

        if (empty_mask)
        {
            break;
        }

        pos = (pos + 1) & (t->num_groups - 1);
    }

    TValue nil = new_value(VAL_NIL);
    return nil;
}

int table_delete(Table *t, TValue key)
{
    uint32_t hash = get_hash(AS_STRING(key));

    uint32_t h1 = hash >> 7;
    uint8_t  h2 = hash & 0x7F;

    size_t pos = h1 & (t->num_groups - 1);

    for (size_t i = 0; i < t->num_groups; i++)
    {
        simde__m128i group = simde_mm_load_si128(&((simde__m128i*)t->ctrl)[pos]);

        simde__m128i key_needle = simde_mm_set1_epi8(h2);
        simde__m128i key_cmp = simde_mm_cmpeq_epi8(group, key_needle);

        uint16_t key_mask = simde_mm_movemask_epi8(key_cmp);

        while (key_mask != 0) 
        {
            size_t absolute_pos = pos * GROUP_SIZE + __builtin_ctz(key_mask);

            if (KEY_CMP(t->slots[absolute_pos].key, key) == 0)
            {
                t->ctrl[absolute_pos] = SLOT_DELETED;

                --t->size;
                ++t->num_deleted;

                return 0;
            }

            key_mask &= key_mask - 1;
        }

        pos = (pos + 1) & (t->num_groups - 1);
    }

    return 1;
}

void table_free(Table *t)
{
    t->capacity = 0;
    t->size = 0;
    t->num_deleted = 0;
    t->num_groups = 0;

    free(t->ctrl);
    free(t->slots);
}