/*
 * Copyright (c) 2015, Masayuki Kimura
 * All rights reserved.
 *
 *     Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Masayuki Kimura nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL MASAYUKI KIMURA BE LIABLE FOR ANY
 *     DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *      LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *     ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <stdio.h>
#include "./basic.h"
#include "./trace.h"

typedef struct TAB_table_ *TAB_table;
typedef        TAB_table   MemTable;

#define TABSIZE 124147 // random factor value

/*!
 * Basic Table structures
 */
typedef struct binder_ *binder;
struct binder_ {
    void *key;
    void *value;
    binder next;
    void *prevtop;
};

struct TAB_table_ {
    binder table[TABSIZE];
    void *top;
};


/*!
 * Architecture Environments
 */
typedef struct __riscvEnv *riscvEnv;

#define REG_R0  0
#define REG_R31 31
#define REG_PC  32

struct __riscvEnv {
    /*!
     * Architecture Implementations
     */
    Word_t     regs[32];     // general register
    Word_t     pc;           // program counter
    MemTable   memory;       // memory table

    /*!
     * debug information
     */
    FILE     *dbgfp;        // file pointer for debugging output
    uint32_t  start_time,   // for debug: performance counter start and stop
              stop_time;
    uint32_t  max_cycle;    // limit of simulation cycle
    uint32_t  step;         // no of simulation step
    traceInfo trace;        // trace information
};


/*!
 * === Basic Table Operations ===
 */
TAB_table TAB_empty(void);
void      TAB_enter(TAB_table t, void *key, void *value);
void     *TAB_look(TAB_table t, void *key);


/*!
 * === Memory Operations ===
 */
MemTable CreateMemTable (void);
void     InsertMemTable (MemTable, Addr_t, Word_t);
Word_t   SearchMemTable (MemTable, Word_t);


/*!
 * === Architecture Environments ===
 */
riscvEnv CreateNewRISCVEnv (FILE *fp);
Word_t   GRegRead  (RegAddr_t, riscvEnv);
void     GRegWrite (RegAddr_t, Word_t, riscvEnv);
void     PCWrite (Addr_t, riscvEnv);
Addr_t   PCRead (riscvEnv);
Word_t   LoadMemory  (Addr_t, Size_t, riscvEnv);
void     StoreMemory (Addr_t, Word_t, Size_t, riscvEnv);
void     AdvanceStep (riscvEnv);
uint32_t LoadSrec (FILE *, riscvEnv);


/*!
 * === Utilities
 */
void    *checked_malloc (size_t);
uint32_t ExtractBitField (uint32_t, uint32_t, uint32_t);
uint32_t ExtendSign (uint32_t, uint32_t);
