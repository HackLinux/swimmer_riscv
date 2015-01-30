#!/bin/ruby
#
# Copyright (c) 2015, Masayuki Kimura
# All rights reserved.
#
#     Redistribution and use in source and binary forms, with or without
#     modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Masayuki Kimura nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL MASAYUKI KIMURA BE LIABLE FOR ANY
#     DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#      LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#     ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

$arch_table = Array[]  # instruction table

module ARCH
  NAME      = 0
  R3        = 1
  F2        = 2
  R2        = 3
  R1        = 4
  F3        = 5
  RD        = 6
  OP        = 7
  KEY_TABLE = 8
  TAIL      = 9
end


## start of RISC-V instructions
#                       ['BITFIELD'                                       31-27,   26-25,    24-20    19-15    14-12     11-07    06-00    ]
#                       ['NAME',                                          'rs3',   'funct2', 'rs2',   'rs1',   'funct3', 'rd',    'opcode' , DecodeTable]
#                       ['DECODE-KEY',                                    'R3',    'F2'      'R2',    'R1',    'F3',     'RD'     'OP'     ]
$arch_table[  0] = Array['lui        d[11:7],h[31:12]',                   'XXXXX', 'XX',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '0110111', Array['OP']]
$arch_table[  1] = Array['auipc      d[11:7],h[31:12]',                   'XXXXX', 'XX',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '0010111', Array['OP']]
$arch_table[  2] = Array['jal        d[11:7],uj[31:12]',                  'XXXXX', 'XX',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1101111', Array['OP']]
$arch_table[  3] = Array['jalr       d[11:7],d[19:15],d[11:0]',           'XXXXX', 'XX',     'XXXXX', 'XXXXX', '000',    'XXXXX', '1100111', Array['OP']]
$arch_table[  4] = Array['beq        d[19:15],d[24:20],sb[31:25]',        'XXXXX', 'XX',     'XXXXX', 'XXXXX', '000',    'XXXXX', '1100011', Array['OP', 'F3']]
$arch_table[  5] = Array['bne        d[19:15],d[24:20],sb[31:25]',        'XXXXX', 'XX',     'XXXXX', 'XXXXX', '001',    'XXXXX', '1100011', Array['OP', 'F3']]
$arch_table[  6] = Array['blt        d[19:15],d[24:20],sb[31:25]',        'XXXXX', 'XX',     'XXXXX', 'XXXXX', '100',    'XXXXX', '1100011', Array['OP', 'F3']]
$arch_table[  7] = Array['bge        d[19:15],d[24:20],sb[31:25]',        'XXXXX', 'XX',     'XXXXX', 'XXXXX', '101',    'XXXXX', '1100011', Array['OP', 'F3']]
$arch_table[  8] = Array['bltu       d[19:15],d[24:20],sb[31:25]',        'XXXXX', 'XX',     'XXXXX', 'XXXXX', '110',    'XXXXX', '1100011', Array['OP', 'F3']]
$arch_table[  9] = Array['bgeu       d[19:15],d[24:20],sb[31:25]',        'XXXXX', 'XX',     'XXXXX', 'XXXXX', '111',    'XXXXX', '1100011', Array['OP', 'F3']]
$arch_table[ 10] = Array['lb         d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '000',    'XXXXX', '0000011', Array['OP', 'F3']]
$arch_table[ 11] = Array['lh         d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '001',    'XXXXX', '0000011', Array['OP', 'F3']]
$arch_table[ 12] = Array['lw         d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0000011', Array['OP', 'F3']]
$arch_table[ 13] = Array['lbu        d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '100',    'XXXXX', '0000011', Array['OP', 'F3']]
$arch_table[ 14] = Array['lhu        d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '101',    'XXXXX', '0000011', Array['OP', 'F3']]
$arch_table[ 15] = Array['sb         d[19:15],d[24:20],h[31:25]|d[11:7]', 'XXXXX', 'XX',     'XXXXX', 'XXXXX', '000',    'XXXXX', '0100011', Array['OP', 'F3']]
$arch_table[ 16] = Array['sh         d[19:15],d[24:20],h[31:25]|d[11:7]', 'XXXXX', 'XX',     'XXXXX', 'XXXXX', '001',    'XXXXX', '0100011', Array['OP', 'F3']]
$arch_table[ 17] = Array['sw         d[19:15],d[24:20],h[31:25]|d[11:7]', 'XXXXX', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0100011', Array['OP', 'F3']]
$arch_table[ 18] = Array['addi       d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '000',    'XXXXX', '0010011', Array['OP', 'F3']]
$arch_table[ 19] = Array['slti       d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0010011', Array['OP', 'F3']]
$arch_table[ 20] = Array['sltiu      d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '011',    'XXXXX', '0010011', Array['OP', 'F3']]
$arch_table[ 21] = Array['xori       d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '100',    'XXXXX', '0010011', Array['OP', 'F3']]
$arch_table[ 22] = Array['ori        d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '110',    'XXXXX', '0010011', Array['OP', 'F3']]
$arch_table[ 23] = Array['andi       d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '111',    'XXXXX', '0010011', Array['OP', 'F3']]
$arch_table[ 24] = Array['slli       d[11:7],d[19:15],d[24:20]',          '00000', '00',     'XXXXX', 'XXXXX', '001',    'XXXXX', '0010011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 25] = Array['srli       d[11:7],d[19:15],d[24:20]',          '00000', '00',     'XXXXX', 'XXXXX', '101',    'XXXXX', '0010011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 26] = Array['srai       d[11:7],d[19:15],d[24:20]',          '01000', '00',     'XXXXX', 'XXXXX', '101',    'XXXXX', '0010011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 27] = Array['add        d[11:7],d[19:15],d[24:20]',          '00000', '00',     'XXXXX', 'XXXXX', '000',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 28] = Array['sub        d[11:7],d[19:15],d[24:20]',          '01000', '00',     'XXXXX', 'XXXXX', '000',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 29] = Array['sll        d[11:7],d[19:15],d[24:20]',          '00000', '00',     'XXXXX', 'XXXXX', '001',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 30] = Array['slt        d[11:7],d[19:15],d[24:20]',          '00000', '00',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 31] = Array['sltu       d[11:7],d[19:15],d[24:20]',          '00000', '00',     'XXXXX', 'XXXXX', '011',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 32] = Array['xor        d[11:7],d[19:15],d[24:20]',          '00000', '00',     'XXXXX', 'XXXXX', '100',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 33] = Array['srl        d[11:7],d[19:15],d[24:20]',          '00000', '00',     'XXXXX', 'XXXXX', '101',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 34] = Array['sra        d[11:7],d[19:15],d[24:20]',          '01000', '00',     'XXXXX', 'XXXXX', '101',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 35] = Array['or         d[11:7],d[19:15],d[24:20]',          '00000', '00',     'XXXXX', 'XXXXX', '110',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 36] = Array['and        d[11:7],d[19:15],d[24:20]',          '00000', '00',     'XXXXX', 'XXXXX', '111',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 37] = Array['fence',                                         '0000X', 'XX',     'XXXXX', '00000', '000',    'XXXXX', '0001111', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 38] = Array['fence.i',                                       '00000', '00',     '00000', '00000', '001',    'XXXXX', '0001111', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 39] = Array['scall',                                         '00000', '00',     '00000', '00000', '000',    'XXXXX', '1110011', Array['OP', 'F3', 'F2', 'R3', 'R2']]
$arch_table[ 40] = Array['sbreak',                                        '00000', '00',     '00001', '00000', '000',    'XXXXX', '1110011', Array['OP', 'F3', 'F2', 'R3', 'R2']]
$arch_table[ 41] = Array['rdcycle    d[11:7]',                            '11000', '00',     '00000', '00000', '010',    'XXXXX', '1110011', Array['OP', 'F3', 'F2', 'R3', 'R2']]
$arch_table[ 42] = Array['rdcycleh   d[11:7]',                            '11001', '00',     '00000', '00000', '010',    'XXXXX', '1110011', Array['OP', 'F3', 'F2', 'R3', 'R2']]
$arch_table[ 43] = Array['rdtime     d[11:7]',                            '11000', '00',     '00001', '00000', '010',    'XXXXX', '1110011', Array['OP', 'F3', 'F2', 'R3', 'R2']]
$arch_table[ 44] = Array['rdtimeh    d[11:7]',                            '11001', '00',     '00001', '00000', '010',    'XXXXX', '1110011', Array['OP', 'F3', 'F2', 'R3', 'R2']]
$arch_table[ 45] = Array['rdinstret  d[11:7]',                            '11000', '00',     '00010', '00000', '010',    'XXXXX', '1110011', Array['OP', 'F3', 'F2', 'R3', 'R2']]
$arch_table[ 46] = Array['rdinstreth d[11:7]',                            '11001', '00',     '00010', '00000', '010',    'XXXXX', '1110011', Array['OP', 'F3', 'F2', 'R3', 'R2']]
$arch_table[ 47] = Array['mul        d[11:7],d[19:15],d[24:20]',          '00000', '01',     'XXXXX', 'XXXXX', '000',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 48] = Array['mulh       d[11:7],d[19:15],d[24:20]',          '00000', '01',     'XXXXX', 'XXXXX', '001',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 49] = Array['mulhsu     d[11:7],d[19:15],d[24:20]',          '00000', '01',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 50] = Array['mulhu      d[11:7],d[19:15],d[24:20]',          '00000', '01',     'XXXXX', 'XXXXX', '011',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 51] = Array['div        d[11:7],d[19:15],d[24:20]',          '00000', '01',     'XXXXX', 'XXXXX', '100',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 52] = Array['divu       d[11:7],d[19:15],d[24:20]',          '00000', '01',     'XXXXX', 'XXXXX', '101',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 53] = Array['rem        d[11:7],d[19:15],d[24:20]',          '00000', '01',     'XXXXX', 'XXXXX', '110',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 54] = Array['remu       d[11:7],d[19:15],d[24:20]',          '00000', '01',     'XXXXX', 'XXXXX', '111',    'XXXXX', '0110011', Array['OP', 'F3', 'F2', 'R3']]
$arch_table[ 55] = Array['lr.w       d[11:7],d[19:15]',                   '00010', 'XX',     '00000', 'XXXXX', '010',    'XXXXX', '0101111', Array['OP', 'F3', 'R3', 'R2']]
$arch_table[ 56] = Array['sc.w       d[11:7],d[19:15],d[24:20]',          '00011', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0101111', Array['OP', 'F3', 'R3']]
$arch_table[ 57] = Array['amoswap.w  d[11:7],d[19:15],d[24:20]',          '00001', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0101111', Array['OP', 'F3', 'R3']]
$arch_table[ 58] = Array['amoadd.w   d[11:7],d[19:15],d[24:20]',          '00000', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0101111', Array['OP', 'F3', 'R3']]
$arch_table[ 59] = Array['amoxor.w   d[11:7],d[19:15],d[24:20]',          '00100', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0101111', Array['OP', 'F3', 'R3']]
$arch_table[ 60] = Array['amoand.w   d[11:7],d[19:15],d[24:20]',          '01100', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0101111', Array['OP', 'F3', 'R3']]
$arch_table[ 61] = Array['amoor.w    d[11:7],d[19:15],d[24:20]',          '01000', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0101111', Array['OP', 'F3', 'R3']]
$arch_table[ 62] = Array['amomin.w   d[11:7],d[19:15],d[24:20]',          '10010', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0101111', Array['OP', 'F3', 'R3']]
$arch_table[ 63] = Array['amomax.w   d[11:7],d[19:15],d[24:20]',          '10100', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0101111', Array['OP', 'F3', 'R3']]
$arch_table[ 64] = Array['amominu.w  d[11:7],d[19:15],d[24:20]',          '11000', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0101111', Array['OP', 'F3', 'R3']]
$arch_table[ 65] = Array['amomaxu.w  d[11:7],d[19:15],d[24:20]',          '11100', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0101111', Array['OP', 'F3', 'R3']]
$arch_table[ 66] = Array['flw        d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0000111', Array['OP', 'F3']]
$arch_table[ 67] = Array['fsw        d[19:15],d[24:20],h[31:25]|d[11:7]', 'XXXXX', 'XX',     'XXXXX', 'XXXXX', '010',    'XXXXX', '0100111', Array['OP', 'F3']]
$arch_table[ 68] = Array['fmadd.s    d[11:7],d[19:15],d[24:20],h[31:27]', 'XXXXX', '00',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1000011', Array['OP', 'F2']]
$arch_table[ 69] = Array['fmsub.s    d[11:7],d[19:15],d[24:20],h[31:27]', 'XXXXX', '00',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1000111', Array['OP', 'F2']]
$arch_table[ 70] = Array['fnmsub.s   d[11:7],d[19:15],d[24:20],h[31:27]', 'XXXXX', '00',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1001011', Array['OP', 'F2']]
$arch_table[ 71] = Array['fnmadd.s   d[11:7],d[19:15],d[24:20],h[31:27]', 'XXXXX', '00',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1001111', Array['OP', 'F2']]
$arch_table[ 72] = Array['fadd.s     d[11:7],d[19:15],d[24:20]',          '00000', '00',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[ 73] = Array['fsub.s     d[11:7],d[19:15],d[24:20]',          '00001', '00',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[ 74] = Array['fmul.s     d[11:7],d[19:15],d[24:20]',          '00010', '00',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[ 75] = Array['fdiv.s     d[11:7],d[19:15],d[24:20]',          '00011', '00',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[ 76] = Array['fsqrt.s    d[11:7],d[19:15]',                   '01011', '00',     '00000', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[ 77] = Array['fsgnj.s    d[11:7],d[19:15],d[24:20]',          '00100', '00',     'XXXXX', 'XXXXX', '000',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'F3']]
$arch_table[ 78] = Array['fsgnjn.s   d[11:7],d[19:15],d[24:20]',          '00100', '00',     'XXXXX', 'XXXXX', '001',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'F3']]
$arch_table[ 79] = Array['fsgnjx.s   d[11:7],d[19:15],d[24:20]',          '00100', '00',     'XXXXX', 'XXXXX', '010',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'F3']]
$arch_table[ 80] = Array['fmin.s     d[11:7],d[19:15],d[24:20]',          '00101', '00',     'XXXXX', 'XXXXX', '000',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'F3']]
$arch_table[ 81] = Array['fmax.s     d[11:7],d[19:15],d[24:20]',          '00101', '00',     'XXXXX', 'XXXXX', '001',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'F3']]
$arch_table[ 82] = Array['fcvt.w.s   d[11:7],d[19:15]',                   '11000', '00',     '00000', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[ 83] = Array['fcvt.wu.s  d[11:7],d[19:15]',                   '11000', '00',     '00000', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[ 84] = Array['fmv.x.s    d[11:7],d[19:15]',                   '11100', '00',     '00000', 'XXXXX', '000',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2', 'F3']]
$arch_table[ 85] = Array['feq.s      d[11:7],d[19:15],d[24:20]',          '10100', '00',     'XXXXX', 'XXXXX', '010',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'F3']]
$arch_table[ 86] = Array['flt.s      d[11:7],d[19:15],d[24:20]',          '10100', '00',     'XXXXX', 'XXXXX', '001',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'F3']]
$arch_table[ 87] = Array['fle.s      d[11:7],d[19:15],d[24:20]',          '10100', '00',     'XXXXX', 'XXXXX', '000',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'F3']]
$arch_table[ 88] = Array['fclass.s   d[11:7],d[19:15]',                   '11100', '00',     '00000', 'XXXXX', '001',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2', 'F3']]
$arch_table[ 89] = Array['fcvt.s.w   d[11:7],d[19:15]',                   '11010', '00',     '00000', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[ 90] = Array['fcvt.s.wu  d[11:7],d[19:15]',                   '11010', '00',     '00001', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[ 91] = Array['fmv.s.x    d[11:7],d[19:15]',                   '11110', '00',     '00000', 'XXXXX', '000',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2', 'F3']]
$arch_table[ 92] = Array['frcsr      d[11:7]',                            '00000', '00',     '00011', '00000', '010',    'XXXXX', '1110011', Array['OP', 'F2', 'R3', 'R2', 'R1', 'F3']]
$arch_table[ 93] = Array['frrm       d[11:7]',                            '00000', '00',     '00010', '00000', '010',    'XXXXX', '1110011', Array['OP', 'F2', 'R3', 'R2', 'R1', 'F3']]
$arch_table[ 94] = Array['frflags    d[11:7]',                            '00000', '00',     '00001', '00000', '010',    'XXXXX', '1110011', Array['OP', 'F2', 'R3', 'R2', 'R1', 'F3']]
$arch_table[ 95] = Array['fscsr      d[11:7],d[19:15]',                   '00000', '00',     '00011', 'XXXXX', '001',    'XXXXX', '1110011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[ 96] = Array['fsrm       d[11:7],d[19:15]',                   '00000', '00',     '00010', 'XXXXX', '001',    'XXXXX', '1110011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[ 97] = Array['fsflags    d[11:7],d[19:15]',                   '00000', '00',     '00001', 'XXXXX', '001',    'XXXXX', '1110011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[ 98] = Array['fsrmi      d[11:7]',                            '00000', '00',     '00010', '00000', '101',    'XXXXX', '1110011', Array['OP', 'F2', 'R3', 'R2', 'R1', 'F3']]
$arch_table[ 99] = Array['fsflagsi   d[11:7]',                            '00000', '00',     '00001', '00000', '101',    'XXXXX', '1110011', Array['OP', 'F2', 'R3', 'R2', 'R1', 'F3']]
$arch_table[100] = Array['fld        d[11:7],d[19:15],h[31:20]',          'XXXXX', 'XX',     'XXXXX', 'XXXXX', '011',    'XXXXX', '0000111', Array['OP', 'F3']]
$arch_table[101] = Array['fsd        d[19:15],d[24:20],h[31:25]|d[11:7]', 'XXXXX', 'XX',     'XXXXX', 'XXXXX', '011',    'XXXXX', '0100111', Array['OP', 'F3']]
$arch_table[102] = Array['fmadd.d    d[11:7],d[19:15],d[24:20],h[31:27]', 'XXXXX', '01',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1000011', Array['OP', 'F2']]
$arch_table[103] = Array['fmsub.d    d[11:7],d[19:15],d[24:20],h[31:27]', 'XXXXX', '01',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1000111', Array['OP', 'F2']]
$arch_table[104] = Array['fnmsub.d   d[11:7],d[19:15],d[24:20],h[31:27]', 'XXXXX', '01',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1001011', Array['OP', 'F2']]
$arch_table[105] = Array['fnmadd.d   d[11:7],d[19:15],d[24:20],h[31:27]', 'XXXXX', '01',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1001111', Array['OP', 'F2']]
$arch_table[106] = Array['fadd.d     d[11:7],d[19:15],d[24:20]',          '00000', '01',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[107] = Array['fsub.d     d[11:7],d[19:15],d[24:20]',          '00001', '01',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[108] = Array['fmul.d     d[11:7],d[19:15],d[24:20]',          '00010', '01',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[109] = Array['fdiv.d     d[11:7],d[19:15],d[24:20]',          '00011', '01',     'XXXXX', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[110] = Array['fsqrt.d    d[11:7],d[19:15]',                   '01011', '01',     '00000', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[111] = Array['fsgnj.d    d[11:7],d[19:15],d[24:20]',          '00100', '01',     'XXXXX', 'XXXXX', '000',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[112] = Array['fsgnjn.d   d[11:7],d[19:15],d[24:20]',          '00100', '01',     'XXXXX', 'XXXXX', '001',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[113] = Array['fsgnjx.d   d[11:7],d[19:15],d[24:20]',          '00100', '01',     'XXXXX', 'XXXXX', '010',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[114] = Array['fmin.d     d[11:7],d[19:15],d[24:20]',          '00101', '01',     'XXXXX', 'XXXXX', '000',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[115] = Array['fmax.d     d[11:7],d[19:15],d[24:20]',          '00101', '01',     'XXXXX', 'XXXXX', '001',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[116] = Array['fcvt.s.d   d[11:7],d[19:15]',                   '01000', '00',     '00001', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[117] = Array['fcvt.d.s   d[11:7],d[19:15]',                   '01000', '01',     '00000', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[118] = Array['feq.d      d[11:7],d[19:15],d[24:20]',          '10100', '01',     'XXXXX', 'XXXXX', '010',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[119] = Array['flt.d      d[11:7],d[19:15],d[24:20]',          '10100', '01',     'XXXXX', 'XXXXX', '001',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[120] = Array['fle.d      d[11:7],d[19:15],d[24:20]',          '10100', '01',     'XXXXX', 'XXXXX', '000',    'XXXXX', '1010011', Array['OP', 'F2', 'R3']]
$arch_table[121] = Array['fclass.d   d[11:7],d[19:15]',                   '11100', '01',     '00000', 'XXXXX', '001',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2', 'F3']]
$arch_table[122] = Array['fcvt.w.d   d[11:7],d[19:15]',                   '11000', '01',     '00000', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[123] = Array['fcvt.wu.d  d[11:7],d[19:15]',                   '11000', '01',     '00001', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[124] = Array['fcvt.d.w   d[11:7],d[19:15]',                   '11010', '01',     '00000', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
$arch_table[125] = Array['fcvt.d.wu  d[11:7],d[19:15]',                   '11010', '01',     '00001', 'XXXXX', 'XXX',    'XXXXX', '1010011', Array['OP', 'F2', 'R3', 'R2']]
