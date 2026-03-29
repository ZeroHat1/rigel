#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "vm_types.h"

typedef struct Table Table;

#define AS_OBJ(val) ((Obj*)(val).as.pointer)

#define AS_STRING(val) ((ObjString*)(val).as.pointer)
#define AS_FUNCTION(val) ((ObjFunction*)(val).as.pointer)

typedef enum
{
	TypeString,
	TypeFunction,
    TypeBuiltinFunction
} ObjType;

typedef struct 
{
    void *add;
    void *sub;
    void *mul;
    void *div;

    void *eq; // ==
    void *lt; // >=
    void *le; // <=

    void *init;

    void *call;
} Dunder;

typedef struct 
{
    ObjType type;
    size_t refcount;

    Table *dict;
    Dunder *dunder;
} Obj;

typedef struct 
{
    Obj base;
    size_t length;
    uint32_t hash;
    char* chars;
} ObjString;

typedef struct 
{
    Obj base;
    size_t arity;
    ObjString *name;
    Bytecode *bytecode;
} ObjFunction;

typedef struct 
{
    Obj base;
    size_t arity;
    void *pointer;
    ObjString *name;
} ObjBuiltinFunction;

static inline void init_obj(Obj *obj, ObjType type)
{
    obj->type = type;
    obj->refcount = 0;
    obj->dict = NULL;
}

static inline ObjString* new_string(char *chars)
{
    ObjString *string = malloc(sizeof(ObjString));

    if (!string)
    {
        return NULL;
    }

    init_obj((Obj*)string, TypeString);

    string->chars = chars;
    string->hash = (uint32_t)-1;
    string->length = (size_t)-1;

    return string;
}

static inline uint32_t get_hash(ObjString *str)
{
    if (str->hash == -1)
    {
        char* chars = str->chars;

        uint32_t hash = 2166136261u;

        while (*chars)
        {
            hash ^= (uint32_t)*chars++;
            hash *= 16777619u;
        }

        if (hash == (uint32_t)-1)
        {
            hash = (uint32_t)-2;
        }

        str->hash = hash;
    }

    return str->hash;
}

static inline ObjFunction* new_function(Bytecode *bytecode, size_t arity, char *name)
{
    ObjFunction *func = malloc(sizeof(ObjFunction));

    if (!func)
    {
        return NULL;
    }

    init_obj((Obj*)func, TypeString);

    func->name = new_string(name);
    func->arity = arity;
    func->bytecode = bytecode;

    return func;
}