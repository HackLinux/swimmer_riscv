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

#include <stdint.h>

#include "./basic.h"
#include "./env.h"
#include "./inst_list.h"
#include "./dec_utils.h"

void RISCV_INST_LUI (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rd_addr = ExtractRDField (inst_hex);
    Word_t    imm     = ExtractBitField (inst_hex, 31, 12);
    Word_t    res     = (imm << 12) & 0xfffff000;

    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_AUIPC (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rd_addr = ExtractRDField (inst_hex);
    Word_t    imm     = ExtractBitField (inst_hex, 31, 12);
    Word_t    res     = ((imm << 12) & 0xfffff000) + env->pc;

    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_JAL (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rd_addr = ExtractRDField (inst_hex);
    Word_t    imm     = ExtractUJField (inst_hex);
    Addr_t    pc_addr = PCRead (env);
    Word_t    res_pc  = imm + pc_addr;

    GRegWrite (rd_addr, pc_addr + 4, env);
    PCWrite (res_pc, env);
}


void RISCV_INST_JALR (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rd_addr = ExtractRDField (inst_hex);
    Word_t    imm      = ExtractIField (inst_hex);
    Addr_t    pc_addr  = PCRead (env);
    Word_t    rs1_val  = GRegRead (rs1_addr, env);
    Word_t    res_pc   = rs1_val + imm;
    GRegWrite (rd_addr, pc_addr + 4, env);
    PCWrite (res_pc, env);
}



void RISCV_INST_BEQ (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  rs2_val  = GRegRead (rs2_addr, env);
    Word_t  imm      = ExtractSBField (inst_hex);

    bool taken = (rs1_val == rs2_val);
    if (taken) {
        Word_t res_pc = PCRead (env) + imm;
        PCWrite (res_pc, env);
    }
}


void RISCV_INST_BNE (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  rs2_val  = GRegRead (rs2_addr, env);
    Word_t  imm      = ExtractSBField (inst_hex);

    bool taken = (rs1_val != rs2_val);
    if (taken) {
        Word_t res_pc = PCRead (env) + imm;
        PCWrite (res_pc, env);
    }
}


void RISCV_INST_BLT (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  rs2_val  = GRegRead (rs2_addr, env);
    Word_t  imm      = ExtractSBField (inst_hex);

    bool taken = (rs1_val < rs2_val);
    if (taken) {
        Word_t res_pc = PCRead (env) + imm;
        PCWrite (res_pc, env);
    }
}


void RISCV_INST_BGE (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  rs2_val  = GRegRead (rs2_addr, env);
    Word_t  imm      = ExtractSBField (inst_hex);

    bool taken = (rs1_val >= rs2_val);
    if (taken) {
        Word_t res_pc = PCRead (env) + imm;
        PCWrite (res_pc, env);
    }
}


void RISCV_INST_BLTU (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    UWord_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t  imm      = ExtractSBField (inst_hex);

    bool taken = (rs1_val < rs2_val);
    if (taken) {
        Word_t res_pc = PCRead (env) + imm;
        PCWrite (res_pc, env);
    }
}


void RISCV_INST_BGEU (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    UWord_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t  imm      = ExtractSBField (inst_hex);

    bool taken = (rs1_val >= rs2_val);
    if (taken) {
        Word_t res_pc = PCRead (env) + imm;
        PCWrite (res_pc, env);
    }
}


void RISCV_INST_LB (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    Word_t  imm      = ExtractIField (inst_hex);
    Addr_t  mem_addr = rs1_val + imm;

    Word_t res       = ExtendSign (LoadMemory (mem_addr, Size_Byte, env) & 0x000000ff, 7);
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_LH (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    Word_t  imm      = ExtractIField (inst_hex);
    Addr_t  mem_addr = rs1_val + imm;

    Word_t res       = ExtendSign (LoadMemory (mem_addr, Size_HWord, env) & 0x0000ffff, 15);
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_LW (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    Word_t  imm      = ExtractIField (inst_hex);
    Addr_t  mem_addr = rs1_val + imm;

    Word_t res       = LoadMemory (mem_addr, Size_Word, env);
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_LBU (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    Word_t  imm      = ExtractIField (inst_hex);
    Addr_t  mem_addr = rs1_val + imm;

    Word_t res       = LoadMemory (mem_addr, Size_Byte, env) & 0x000000ff;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_LHU (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    Word_t  imm      = ExtractIField (inst_hex);
    Addr_t  mem_addr = rs1_val + imm;

    Word_t res       = LoadMemory (mem_addr, Size_HWord, env) & 0x0000ffff;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SB (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    UWord_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t  imm      = ExtractSBField (inst_hex);

    Addr_t  mem_addr = rs1_val + imm;

    StoreMemory (mem_addr, rs2_val & 0x000000ff, Size_Byte, env);
}


void RISCV_INST_SH (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    UWord_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t  imm      = ExtractSBField (inst_hex);

    Addr_t  mem_addr = rs1_val + imm;

    StoreMemory (mem_addr, rs2_val & 0x0000ffff, Size_HWord, env);
}


void RISCV_INST_SW (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    UWord_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t  imm      = ExtractSBField (inst_hex);

    Addr_t  mem_addr = rs1_val + imm;

    StoreMemory (mem_addr, rs2_val, Size_Word, env);
}


void RISCV_INST_ADDI (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  imm      = ExtractIField (inst_hex);

    Word_t  res      = rs1_val + imm;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SLTI (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  imm      = ExtendSign (ExtractBitField (inst_hex, 31, 20), 11);
    Word_t  res      = (rs1_val < imm) ? 0x1 : 0x0;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SLTIU (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  imm      = ExtractIField (inst_hex);
    Word_t  res      = (rs1_val < imm) ? 0x1 : 0x0;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_XORI (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  imm      = ExtractIField (inst_hex);
    Word_t  res      = rs1_val ^ imm;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_ORI (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  imm      = ExtractIField (inst_hex);
    Word_t  res      = rs1_val | imm;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_ANDI (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  imm      = ExtractIField (inst_hex);
    Word_t  res      = rs1_val & imm;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SLLI (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  shamt    = ExtractBitField (inst_hex, 24, 20);
    Word_t  res      = rs1_val << shamt;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SRLI (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    Word_t  shamt    = ExtractBitField (inst_hex, 24, 20);
    Word_t  res      = rs1_val >> shamt;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SRAI (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  shamt    = ExtractBitField (inst_hex, 24, 20);
    Word_t  res      = rs1_val >> shamt;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_ADD (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  rs2_val  = GRegRead (rs2_addr, env);
    Word_t  res      = rs1_val + rs2_val;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SUB (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  rs2_val  = GRegRead (rs2_addr, env);
    Word_t  res      = rs1_val + rs2_val;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SLL (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  rs2_val  = GRegRead (rs2_addr, env);
    Word_t  res      = rs1_val << rs2_val;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SLT (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  rs2_val  = GRegRead (rs2_addr, env);
    Word_t  res      = (rs1_val < rs2_val) ? 0x01 : 0x00;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SLTU (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    UWord_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t  res      = (rs1_val < rs2_val) ? 0x01 : 0x00;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_XOR (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    UWord_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t  res      = rs1_val ^ rs2_val;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SRL (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    UWord_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t  res      = rs1_val >> rs2_val;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_SRA (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  rs2_val  = GRegRead (rs2_addr, env);
    Word_t  res      = rs1_val >> rs2_val;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_OR (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    UWord_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t  res      = rs1_val | rs2_val;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_AND (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    UWord_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t  res      = rs1_val & rs2_val;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_FENCE (uint32_t inst_hex, riscvEnv env)
{
    /* As ISS, do nothing */
}


void RISCV_INST_FENCE_I (uint32_t inst_hex, riscvEnv env)
{
    /* As ISS, do nothing */
}


void RISCV_INST_SCALL (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_SBREAK (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_RDCYCLE (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_RDCYCLEH (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_RDTIME (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_RDTIMEH (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_RDINSTRET (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_RDINSTRETH (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_MUL (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t rs1_val  = GRegRead (rs1_addr, env);
    Word_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t res      = rs1_val * rs2_val;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_MULH (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val  = GRegRead (rs1_addr, env);
    Word_t  rs2_val  = GRegRead (rs2_addr, env);
    DWord_t res64    = (DWord_t)rs1_val * (DWord_t)rs2_val;
    Word_t  res      = (res64 >> 32) & 0x0ffffffffUL;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_MULHSU (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t  rs1_val = GRegRead (rs1_addr, env);
    UWord_t rs2_val = GRegRead (rs2_addr, env);
    DWord_t res64   = (DWord_t)rs1_val * (DWord_t)rs2_val;
    Word_t  res     = (res64 >> 32) & 0x0ffffffffUL;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_MULHU (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    UWord_t  rs1_val = GRegRead (rs1_addr, env);
    UWord_t  rs2_val = GRegRead (rs2_addr, env);
    UDWord_t res64   = (DWord_t)rs1_val * (DWord_t)rs2_val;
    UWord_t  res     = (res64 >> 32) & 0x0ffffffffUL;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_DIV (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    Word_t rs1_val  = GRegRead (rs1_addr, env);
    Word_t rs2_val  = GRegRead (rs2_addr, env);
    Word_t  res      = rs1_val / rs2_val;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_DIVU (uint32_t inst_hex, riscvEnv env)
{
    RegAddr_t rs1_addr = ExtractR1Field (inst_hex);
    RegAddr_t rs2_addr = ExtractR2Field (inst_hex);
    RegAddr_t rd_addr  = ExtractRDField (inst_hex);

    UWord_t rs1_val  = GRegRead (rs1_addr, env);
    UWord_t rs2_val  = GRegRead (rs2_addr, env);
    if (rs2_val == 0) {
        return;
    }
    UWord_t  res     = rs1_val / rs2_val;
    GRegWrite (rd_addr, res, env);
}


void RISCV_INST_REM (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_REMU (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_LR_W (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_SC_W (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_AMOSWAP_W (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_AMOADD_W (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_AMOXOR_W (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_AMOAND_W (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_AMOOR_W (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_AMOMIN_W (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_AMOMAX_W (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_AMOMINU_W (uint32_t inst_hex, riscvEnv env) {}
void RISCV_INST_AMOMAXU_W (uint32_t inst_hex, riscvEnv env) {}

void RISCV_INST_FLW (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSW (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMADD_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMSUB_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FNMSUB_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FNMADD_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FADD_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSUB_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMUL_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FDIV_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSQRT_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSGNJ_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSGNJN_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSGNJX_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMIN_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMAX_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCVT_W_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCVT_WU_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMV_X_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FEQ_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FLT_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FLE_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCLASS_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCVT_S_W (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCVT_S_WU (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMV_S_X (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FRCSR (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FRRM (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FRFLAGS (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSCSR (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSRM (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSFLAGS (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSRMI (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSFLAGSI (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FLD (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSD (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMADD_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMSUB_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FNMSUB_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FNMADD_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FADD_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSUB_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMUL_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FDIV_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSQRT_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSGNJ_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSGNJN_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FSGNJX_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMIN_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FMAX_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCVT_S_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCVT_D_S (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FEQ_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FLT_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FLE_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCLASS_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCVT_W_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCVT_WU_D (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCVT_D_W (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}


void RISCV_INST_FCVT_D_WU (uint32_t inst_hex, riscvEnv env)
{
    fprintf (env->dbgfp, "Sorry FPU instructions are not supported in this version.\n");
}
