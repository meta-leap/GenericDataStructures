#ifndef GENERIC_DATA_STRUCTURES_SET_H
#define GENERIC_DATA_STRUCTURES_SET_H

/*
    Defines a generic set of unique values.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "hash_utils.h"

#define SET_DEFINE_H(type_name, function_prefix, value_type) \
    typedef struct type_name ## Cell { \
        value_type value; \
        uint32_t hash; \
        bool active; \
    } type_name ## Cell; \
 \
    typedef struct type_name { \
        type_name ## Cell* cells; \
        uint32_t count; \
        uint32_t capacity; \
        uint32_t load_factor; \
        int shift; \
    } type_name; \
 \
    static inline uint32_t function_prefix ## _count(type_name* set) { return set->count; } \
    static inline void function_prefix ## _free(type_name* set) { free(set->cells); free(set); } \
    static inline void function_prefix ## _free_resources(type_name* set) { free(set->cells); } \
 \
    type_name* function_prefix ## _create(void); \
    bool function_prefix ## _init(type_name* set); \
    bool function_prefix ## _add(type_name* set, value_type value); \
    bool function_prefix ## _contains(type_name* set, value_type value); \
    bool function_prefix ## _remove(type_name* set, value_type value); \
    bool function_prefix ## _get(type_name* set, value_type value, value_type* out_value); \
    bool function_prefix ## _get_and_remove(type_name* set, value_type value, value_type* out_value); \


#define SET_DEFINE_C(type_name, function_prefix, value_type, hash_fn, compare_fn) \
    type_name* function_prefix ## _create(void) { \
        type_name* set = malloc(sizeof(type_name)); \
        if(!set) \
            return NULL; \
        function_prefix ## _init(set); \
        return set; \
    } \
 \
    bool function_prefix ## _init(type_name* set) { \
        set->shift = 29; \
        set->capacity = 8; \
        set->count = 0; \
        set->load_factor = 4; \
        return (set->cells = calloc(8, sizeof(type_name ## Cell))) != NULL; \
    } \
 \
    static void function_prefix ## _resize(type_name* set) { \
        int capacity = set->load_factor = set->capacity; \
        set->capacity = 1u << (32 - (--set->shift)); \
        type_name ## Cell* old = set->cells; \
        type_name ## Cell* current = calloc(set->capacity, sizeof(type_name ## Cell)); \
        assert(current); \
 \
        for(uint32_t i = 0; i < capacity; i++) { \
            if(old[i].active) { \
                uint32_t cell; \
                old[i].hash = cell = ___fib_hash(hash_fn(old[i].value), set->shift); \
                while(current[cell].active) { \
                    if(++cell > set->capacity) \
                        cell = 0; \
                } \
 \
                current[cell] = old[i]; \
            } \
        } \
        free(old); \
        set->cells = current; \
    } \
 \
    bool function_prefix ## _add(type_name* set, value_type value) { \
        uint32_t hash, cell; \
 \
        if(set->count == set->load_factor) \
            function_prefix ## _resize(set); \
 \
        hash = cell = ___fib_hash(hash_fn(value), set->shift); \
 \
        while(true) { \
            if(!set->cells[cell].active) { \
                set->cells[cell].active = true; \
                set->cells[cell].value = value; \
                set->cells[cell].hash = hash; \
                set->count++; \
                return true; \
            } else if(set->cells[cell].hash == hash && compare_fn(set->cells[cell].value, value) == 0) \
                return false; \
 \
            if(++cell == set->capacity) \
                cell = 0; \
        } \
 \
        return false; \
    } \
 \
    static inline bool function_prefix ## _find_cell(type_name* set, value_type value, uint32_t* out_hash, uint32_t* out_cell) { \
        uint32_t cell, hash; \
        hash = cell = ___fib_hash(hash_fn(value), set->shift); \
 \
        while(true) { \
            if(!set->cells[cell].active) \
                return false; \
 \
            if(set->cells[cell].hash == hash && compare_fn(set->cells[cell].value, value) == 0) { \
                *out_hash = hash; \
                *out_cell = cell; \
                return true; \
            } \
 \
            if(++cell == set->capacity) \
                cell = 0; \
        } \
    } \
 \
    static inline void function_prefix ## _replace_cell(type_name* set, uint32_t cell, uint32_t hash) { \
        uint32_t start = cell; \
        int64_t last = -1; \
 \
        while(true) { \
            if(++cell == set->capacity) \
                cell = 0; \
 \
            if(!set->cells[cell].active) \
                break; \
 \
            if(set->cells[cell].hash <= hash) \
                last = cell; \
        } \
 \
        if(last != -1) { \
            set->cells[start] = set->cells[last]; \
            set->cells[last].active = false; \
        } else \
            set->cells[start].active = false; \
    } \
 \
    bool function_prefix ## _contains(type_name* set, value_type value) { \
        uint32_t cell, hash; \
        return function_prefix ## _find_cell(set, value, &hash, &cell); \
    } \
 \
    bool function_prefix ## _remove(type_name* set, value_type value) { \
        uint32_t cell, hash; \
        if(!function_prefix ## _find_cell(set, value, &hash, &cell)) \
            return false; \
 \
        function_prefix ## _replace_cell(set, cell, hash); \
        set->count--; \
        return true; \
    } \
 \
    bool function_prefix ## _get(type_name* set, value_type value, value_type* out_value) { \
        uint32_t cell, hash; \
        if(!function_prefix ## _find_cell(set, value, &hash, &cell)) \
            return false; \
        *out_value = set->cells[cell].value; \
        return true; \
    } \
 \
    bool function_prefix ## _get_and_remove(type_name* set, value_type value, value_type* out_value) { \
        uint32_t cell, hash; \
        if(!function_prefix ## _find_cell(set, value, &hash, &cell)) \
            return false; \
        *out_value = set->cells[cell].value; \
 \
        function_prefix ## _replace_cell(set, cell, hash); \
        set->count--; \
        return true; \
    } \

#endif