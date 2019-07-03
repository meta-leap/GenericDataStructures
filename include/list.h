#ifndef GENERIC_DATA_STRUCTURES_LIST_H
#define GENERIC_DATA_STRUCTURES_LIST_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define LIST_DEFINE_H(type_name, function_prefix, value_type) \
    typedef struct type_name { \
        value_type* buffer; \
        unsigned int count; \
        unsigned int capacity; \
    } type_name; \
    \
    type_name* function_prefix ## _create(void); \
    void* function_prefix ## _init(type_name* list); \
    void* function_prefix ## _init_capacity(type_name* list, unsigned int capacity); \
    void function_prefix ## _add(type_name* list, value_type value); \
    void function_prefix ## _insert(type_name* list, unsigned int index, value_type value); \
    \
    static inline void function_prefix ## _clear(type_name* list) { list->count = 0; } \
    \
    static inline unsigned int function_prefix ## _count(type_name* list) { return list->count; } \
    \
    static inline void function_prefix ## _free(type_name* list) { free(list->buffer); } \
    \
    static inline value_type function_prefix ## _get(type_name* list, unsigned int index) {  \
        assert(index < list->count); \
        return list->buffer[index]; \
    } \
    \
    static inline void function_prefix ## _set(type_name* list, unsigned int index, value_type value) { \
        assert(index <= list->count); \
        if(index == list->count) \
            function_prefix ## _add(list, value); \
        else \
            list->buffer[index] = value; \
    } \
    \
    static inline void function_prefix ## _remove(type_name* list, unsigned int index) { \
        assert(index < list->count); \
        if(index == --list->count) \
            return; \
        else \
            memmove(list->buffer + index, list->buffer + index + 1, (list->count - index) * sizeof(value_type)); \
    } \
    \
    static inline value_type function_prefix ## _peek(type_name* list) { \
        assert(list->count); \
        return list->buffer[list->count - 1]; \
    } \
    \
    static inline value_type function_prefix ## _pop(type_name* list) {  \
        assert(list->count); \
        return list->buffer[--list->count]; \
    }

#define LIST_DEFINE_C(type_name, function_prefix, value_type) \
    type_name* function_prefix ## _create(void) { \
        type_name* list = malloc(sizeof(type_name)); \
        if(!list) \
            return list; \
        if(!function_prefix ## _init(list)) { \
            free(list); \
            return NULL; \
        } \
        return list; \
    } \
    \
    void* function_prefix ## _init(type_name* list) { \
        list->capacity = 4; \
        list->count = 0; \
        return list->buffer = malloc(4 * sizeof(value_type)); \
    } \
    \
    void* function_prefix ## _init_capacity(type_name* list, unsigned int capacity) { \
        assert(capacity); \
        list->capacity = capacity; \
        list->count = 0; \
        return list->buffer = malloc(capacity * sizeof(value_type)); \
    } \
    \
    void function_prefix ## _add(type_name* list, value_type value) { \
        if(list->count == list->capacity) { \
            list->capacity *= 2; \
            list->buffer = realloc(list->buffer, list->capacity * sizeof(value_type)); \
            assert(list->buffer); \
        } \
        list->buffer[list->count++] = value; \
    } \
    \
    void function_prefix ## _insert(type_name* list, unsigned int index, value_type value) { \
        assert(index <= list->count); \
        if(index == list->count) { \
            function_prefix ## _add(list, value); \
            return; \
        } \
        if(list->count == list->capacity) { \
            list->capacity *= 2; \
            list->buffer = realloc(list->buffer, list->capacity * sizeof(value_type)); \
            assert(list->buffer); \
        } \
        memmove(list->buffer + index + 1, list->buffer + index, (list->count++ - index) * sizeof(value_type)); \
        list->buffer[index] = value; \
    }



#endif