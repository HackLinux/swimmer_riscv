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

#include <stdio.h>
#include <stdint.h>
#include "./basic.h"
#include "./env.h"
#include "./trace.h"

static binder Binder (void *, void *, binder, void *);

static Byte_t  LoadMemByte   (Addr_t, riscvEnv);
static HWord_t LoadMemHWord  (Addr_t, riscvEnv);
static Word_t  LoadMemWord   (Addr_t, riscvEnv);
static void    StoreMemByte  (Addr_t, Byte_t , riscvEnv);
static void    StoreMemHWord (Addr_t, HWord_t, riscvEnv);
static void    StoreMemWord  (Addr_t, Word_t , riscvEnv);

static uint32_t htoi (uint8_t);


void *checked_malloc (size_t size)
{
    void *mem;
    if ((mem = malloc (size)) == NULL) {
        perror ("malloc");
        exit (EXIT_FAILURE);
    }
    return mem;
}


/*!
 * extract operand from inst from left to right
 */
uint32_t ExtractBitField (uint32_t hex, uint32_t left, uint32_t right)
{
    return (hex >> right) & ((1 << (left - right + 1)) - 1);
}


uint32_t ExtractIField (uint32_t hex)
{
    uint32_t uimm32 = ExtractBitField (hex, 31, 20);
    return ExtendSign (uimm32, 11);
}


uint32_t ExtractSBField (uint32_t hex)
{
    uint32_t i07_07 = ExtractBitField (hex,  7,  7) & 0x01UL;
    uint32_t i11_08 = ExtractBitField (hex, 11,  8) & 0x0fUL;
    uint32_t i30_25 = ExtractBitField (hex, 30, 25) & 0x03fUL;
    uint32_t i31_31 = ExtractBitField (hex, 31, 31) & 0x01UL;

    uint32_t u_res = (i31_31 << 12) |
                     (i07_07 << 11) |
                     (i30_25 <<  5) |
                     (i11_08 <<  1);
    return ExtendSign (u_res, 12);
}


uint32_t ExtractUJField (uint32_t hex)
{
    uint32_t i24_21 = ExtractBitField (hex, 24, 21) & 0x0fUL;
    uint32_t i30_25 = ExtractBitField (hex, 30, 25) & 0x03fUL;
    uint32_t i20_20 = ExtractBitField (hex, 20, 20) & 0x01UL;
    uint32_t i19_12 = ExtractBitField (hex, 19, 12) & 0x0ffUL;
    uint32_t i31_31 = ExtractBitField (hex, 31, 31) & 0x01UL;

    uint32_t u_res = (i31_31 << 20) |
                     (i19_12 << 12) |
                     (i20_20 << 11) |
                     (i30_25 <<  5) |
                     (i24_21 <<  1);
    return ExtendSign (u_res, 20);
}


/*!
 * sign extension
 */
uint32_t ExtendSign (uint32_t data, uint32_t msb)
{
    uint32_t mask = 0x0UL;
    if (((data >> msb) & 0x01) == 0x01) {
        mask = 0xFFFFFFFFUL & (~((1 << msb) - 1));
    }
    return data | mask;
}


//*
//* === Basic Table Operations ===
//*

/*!
 * Binder
 * bind key to value
 * \param key     key of value
 * \param value   input value
 * \param next    next element
 */
static binder Binder (void *key, void *value, binder next, void *prevtop)
{
    binder b = (binder) checked_malloc (sizeof (*b));
    b->key = key;
    b->value = value;
    b->next=next;
    b->prevtop = prevtop;

    return b;
}


/*!
 * Table operations
 */
TAB_table TAB_empty(void)
{
    TAB_table t = (TAB_table) checked_malloc (sizeof (*t));
    uint32_t i;
    t->top = NULL;
    for (i = 0; i < TABSIZE; i++) {
        t->table[i] = NULL;
    }

    return t;
}

void TAB_enter (TAB_table t, void *key, void *value)
{
    uint32_t index;
    index = ((unsigned)key) % TABSIZE;
    t->table[index] = Binder (key, value, t->table[index], t->top) ;
    t->top = key;
}

void *TAB_look (TAB_table t, void *key)
{
    uint32_t index;
    binder b;

    index = ((unsigned)key) % TABSIZE;
    for (b = t->table[index]; b; b = NEXT (b))  {
        if  (b->key == key)
            return b->value;
    }
    return NULL;
}

//*
//* === Memory Operations ===
//*

/*!
 * create instruction table
 */
MemTable CreateMemTable (void)
{
    MemTable table = (MemTable)TAB_empty ();
    return table;
}

/*!
 * insert instruction into table
 * \param table   table to be inserted info
 * \param key     key used in search
 * \param info    info structure to be inserted
 */
void InsertMemTable (MemTable table, Addr_t addr, Word_t info)
{
    TAB_enter ((TAB_table)table, (void *)addr, (void *)info);
    return;
}

/*!
 * search info from pc location
 * \param table  target table to be searched
 * \param pc     target pc address
 * \return       info structure to be searched
 */
Word_t SearchMemTable (MemTable table, Word_t addr)
{
    return (Word_t) TAB_look (table, (void *)addr);
}


/*!
 * create new RISCV simulation environment
 * \return RiscvEnv structure (not formatted)
 */
riscvEnv CreateNewRISCVEnv (FILE *fp)
{
    riscvEnv env = (riscvEnv) checked_malloc (sizeof (*env));
    env->trace   = (traceInfo) checked_malloc (sizeof (*(env->trace)));
    env->memory  = CreateMemTable ();
    env->dbgfp   = fp;

    return env;
}


/*!
 * Read from General Register
 * \param reg register address to read
 * \param env RISC-V environment
 */
Word_t GRegRead (RegAddr_t reg, riscvEnv env)
{
    Word_t value = env->regs [reg];
    RecordTraceGRegRead (env->trace, reg, value);

    return value;
}


/*!
 * Write Data General Register
 * \param reg  register address to write
 * \param daat write data
 * \param env  RISC-V environment
 */
void GRegWrite (RegAddr_t reg, Word_t data, riscvEnv env)
{
    RecordTraceGRegWrite (env->trace, reg, data);
    env->regs [reg] = data;
}


/*!
 * Write new Addr to PC
 * \param reg  register address to write
 * \param daat write data
 * \param env  RISC-V environment
 */
void PCWrite (Addr_t addr, riscvEnv env)
{
    RecordTraceGRegWrite (env->trace, REG_PC, addr);
    env->pc = addr;
}


/*!
 * Read current PC
 * \param reg  register address to write
 * \param daat write data
 * \param env  RISC-V environment
 */
Addr_t PCRead (riscvEnv env)
{
    return env->pc;
}


/*!
 * Reference from Memory
 * \param addr address
 * \param env RISCV environment
 */
static Byte_t LoadMemByte (Addr_t addr, riscvEnv env)
{
    return SearchMemTable (env->memory, addr);
}


/*!
 * Reference from Memory
 * \param addr address
 * \param env RISCV environment
 */
static HWord_t LoadMemHWord (Addr_t addr, riscvEnv env)
{
    Word_t byte1 = LoadMemByte (addr + 1, env);
    Word_t byte0 = LoadMemByte (addr + 0, env);
    HWord_t res;
    if ((addr & 0x01) == 0) {
        res = (byte1 << 8) + byte0;
        return res;
    } else {
        fprintf (env->dbgfp, "<Error: Address Misalign: HalfWord Addr = %08x>\n", addr);
        return 0x0;
    }

}


/*!
 * Reference from Memory
 * \param addr address
 * \param env RISCV environment
 */
static Word_t LoadMemWord (Addr_t addr, riscvEnv env)
{
    Word_t byte3 = LoadMemByte (addr + 3, env) & 0x00FFUL;
    Word_t byte2 = LoadMemByte (addr + 2, env) & 0x00FFUL;
    Word_t byte1 = LoadMemByte (addr + 1, env) & 0x00FFUL;
    Word_t byte0 = LoadMemByte (addr + 0, env) & 0x00FFUL;
    Word_t res = 0;
    if ((addr & 0x03) == 0) {
        res = (byte3 << 24) + (byte2 << 16)
            + (byte1 << 8) + byte0;
    } else {
        fprintf (env->dbgfp, "<Error: Address Misalign: Byte Addr = %08x>\n", addr);
    }

    return res;
}


/*!
 * Store Data to Memory (Byte)
 * \param addr address
 * \param env RISCV environment
 */
static void StoreMemByte (Addr_t addr, Byte_t data, riscvEnv env)
{
    InsertMemTable (env->memory, addr, data);
    return;
}


/*!
 * Store Data to Memory (HWord)
 * \param addr address
 * \param env RISCV environment
 */
static void StoreMemHWord (Addr_t addr, HWord_t data, riscvEnv env)
{
    if ((addr & 0x01) == 0) {
        StoreMemByte (addr + 0, (data >>  0) & 0x00ff, env);
        StoreMemByte (addr + 1, (data >>  8) & 0x00ff, env);
    } else {
        fprintf (env->dbgfp, "<Address Misalign Error: Half Word Addr = %08x>\n", addr);
    }

    return;
}


/*!
 * Store Data to Memory (Word)
 * \param addr address
 * \param env RISCV environment
 */
static void StoreMemWord (Addr_t addr, Word_t data, riscvEnv env)
{
    if ((addr & 0x03) == 0) {
        StoreMemByte (addr + 0, (data >>  0) & 0x00ff, env);
        StoreMemByte (addr + 1, (data >>  8) & 0x00ff, env);
        StoreMemByte (addr + 2, (data >> 16) & 0x00ff, env);
        StoreMemByte (addr + 3, (data >> 24) & 0x00ff, env);
    } else {
        fprintf (env->dbgfp, "<Address Misalign Error: Word Addr = %08x>\n", addr);
    }

    return;
}


/*!
 * Load Data from Memory
 */
Word_t LoadMemory (Addr_t addr, Size_t size, riscvEnv env)
{
    Word_t res;
    switch (size) {
    case Size_Byte:
        res = LoadMemByte  (addr, env);
        RecordTraceMemRead (env->trace, addr, res, size);
        break;
    case Size_HWord:
        res = (Word_t)LoadMemHWord (addr, env);
        RecordTraceMemRead (env->trace, addr, res, size);
        break;
    case Size_Word:
        res = (Word_t)LoadMemWord (addr, env);
        RecordTraceMemRead (env->trace, addr, res, size);
        break;
    default:
        return 0;
        fprintf (env->dbgfp, "<Internal Error: Illegal size of LoadMemory : %d>\n", size);
        break;
    }
    return res;
}


/*!
 * Load Data from Memory
 * it is different from LoadMemory, FetchMemory doesn't recorded to trace
 */
Word_t FetchMemory (Addr_t addr, riscvEnv env)
{
    Word_t res = LoadMemWord (addr, env);
    return res;
}


void StoreMemory (Addr_t addr, Word_t data, Size_t size, riscvEnv env)
{
    switch (size) {
    case Size_Byte:
        StoreMemByte (addr, data, env);
        RecordTraceMemWrite (env->trace, addr, data, size);
        break;
    case Size_HWord:
        StoreMemHWord (addr, data, env);
        RecordTraceMemWrite (env->trace, addr, data, size);
        break;
    case Size_Word:
        StoreMemWord (addr, data, env);
        RecordTraceMemWrite (env->trace, addr, data, size);
        break;
    default:
        fprintf (env->dbgfp, "<Internal Error: Illegal size of StoreMem is %d>\n", size);
        exit (EXIT_FAILURE);
        break;
    }
    return;
}


void AdvanceStep (riscvEnv env)
{
    PCWrite (PCRead (env) + 4, env);
}


/*!
 * load s-rec motrola format
 * \param fp   file pointer to be loaded srec
 * \param env  environment to be load
 * \return     max memory address to be loaded
 */
uint32_t LoadSrec (FILE *fp, riscvEnv env)
{
    char buff[256 + 1];
    uint32_t i;
    char load_data = false;
    uint32_t pc_max = 0x00000000;

    while (fgets (buff, 256, fp) != NULL) {
        if (buff[0] == 'S') { // valid s-record
            uint8_t type = buff[1] - 0x30;
            uint8_t byte_length = (htoi (buff[2]) << 4) + htoi (buff[3]);
            uint32_t address = 0;
            uint8_t addr_len;   // length of address format
            switch (type) {
            case 0 : // Block header
                addr_len = 4; load_data = false; break;  // 2 byte
            case 1 : // Data sequence
                addr_len = 4; load_data = true;  break;  // 2 byte
            case 2 : // Data sequence
                addr_len = 6; load_data = true;  break;  // 3 byte
            case 3 : // Data sequence
                addr_len = 8; load_data = true;  break;  // 4 byte
            case 5 : // Record count
                addr_len = 4; load_data = false; break;  // 2 byte
            case 7 : // End of block
                addr_len = 8; load_data = false; break;  // 4 byte
            case 8 : // End of block
                addr_len = 6; load_data = false; break;  // 3 byte
            case 9 : // End of block
                addr_len = 4; load_data = false; break;  // 2 byte
            default : // illegal type
                addr_len = 0;
                load_data = false;
                fprintf (stderr, "<Loading Srecord File : type is illegal %d>\n", type);
                break;
            }

            if (load_data == true) {
                //** read address of S-Rec
                int index; // head of address
                const int addr_head = 4;
                for (index = 0; index < addr_len; index++) {
                    address = (address << 4) + htoi (buff[index + addr_head]);
                }

                //** read word
                int data_head = index + addr_head;   // head of data is tail of address information
                for (i = 0; i < byte_length - addr_len / 2 - 1; i++) {
                    uint8_t data = (htoi (buff[i * 2 + data_head]) << 4) + htoi (buff[i * 2 + 1 + data_head]);
                    StoreMemory (address, data, Size_Byte, env);
                    address++;
                }
                if (pc_max < address) {
                    pc_max = address;
                }
            }
        }
    }

    return pc_max;
}


static uint32_t htoi (uint8_t h)
{

    if (h >= '0' && h <= '9') {
        return h - '0';  // number
    } else if (h >= 'A' && h <= 'F') {
        return h - 'A' + 10; // 0xA - 0xF
    } else if (h >= 'a' && h <= 'f') {
        return h - 'a' + 10;
    } else {
        return -1;
    }
}
