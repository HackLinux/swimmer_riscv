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

require './riscv_arch_table.rb'

##=== displaying headers ===

def gen_header(fp)
  fp.puts("/*")
  fp.puts(" * Copyright (c) 2015, Masayuki Kimura")
  fp.puts(" * All rights reserved.")
  fp.puts(" *")
  fp.puts(" *     Redistribution and use in source and binary forms, with or without")
  fp.puts(" *     modification, are permitted provided that the following conditions are met:")
  fp.puts(" *     * Redistributions of source code must retain the above copyright")
  fp.puts(" *     notice, this list of conditions and the following disclaimer.")
  fp.puts(" *     * Redistributions in binary form must reproduce the above copyright")
  fp.puts(" *     notice, this list of conditions and the following disclaimer in the")
  fp.puts(" *       documentation and/or other materials provided with the distribution.")
  fp.puts(" *     * Neither the name of the Masayuki Kimura nor the")
  fp.puts(" *       names of its contributors may be used to endorse or promote products")
  fp.puts(" *       derived from this software without specific prior written permission.")
  fp.puts(" *")
  fp.puts(" * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND")
  fp.puts(" *     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED")
  fp.puts(" * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE")
  fp.puts(" * DISCLAIMED. IN NO EVENT SHALL MASAYUKI KIMURA BE LIABLE FOR ANY")
  fp.puts(" *     DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES")
  fp.puts(" *     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;")
  fp.puts(" *      LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND")
  fp.puts(" *     ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT")
  fp.puts(" *     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS")
  fp.puts(" *     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.")
  fp.puts(" */")
  fp.puts("")
  fp.puts("")
end


def get_key_idx(key)
  case key
  when 'R3'
    return ARCH::R3
  when 'F2'
    return ARCH::F2
  when 'R2'
    return ARCH::R2
  when 'R1'
    return ARCH::R1
  when 'F3'
    return ARCH::F3
  when 'RD'
    return ARCH::RD
  when 'OP'
    return ARCH::OP
  else
    printf("ERROR: can't find key %s\n", key)
    return -1
  end
end


module DEC
  FUNC_STR = ARCH::TAIL
  CURR_DEC = ARCH::TAIL + 1
  INST_NAME = ARCH::TAIL + 2
end

func_str = "Dec_RISCV"
inst_name = Array[]
temp_arch_table = Array[]

##
##=== making instruction define list ===
##

inst_define_fp = File.open('inst_list.h', 'w')

gen_header(inst_define_fp) # making header
inst_define_fp.puts("#pragma once")
inst_define_fp.puts("")
inst_define_fp.puts("")

# generate instruction list
$arch_table.each_with_index {|inst_info, index|
  mnemonic = "INST_%s"%([inst_info[ARCH::NAME].split(" ")[0].gsub(/\./,'_').upcase])
  $arch_table[index][DEC::INST_NAME] = mnemonic
  $arch_table[index][DEC::FUNC_STR] = "RISCV_DEC"
  mne_str = "#define %s\t\t%d"%([mnemonic, index])
  inst_define_fp.puts(mne_str)
}


##
##=== generate decode table ===
##
inst_decoder_c_fp = File.open('inst_decoder.c', 'w')
inst_decoder_h_fp = File.open('inst_decoder.h', 'w')

gen_header(inst_decoder_c_fp) # making header
gen_header(inst_decoder_h_fp) # making header

inst_decoder_c_fp.puts("#include <stdint.h>")
inst_decoder_c_fp.puts("#include \"./inst_list.h\"")
inst_decoder_c_fp.puts("#include \"./inst_decoder.h\"")
inst_decoder_c_fp.puts("#include \"./dec_utils.h\"\n\n\n")

inst_decoder_h_fp.puts("#include <stdint.h>")
inst_decoder_h_fp.puts("#include \"./inst_list.h\"")
inst_decoder_h_fp.puts("#include \"./dec_utils.h\"\n\n\n")

$arch_table.each {|inst_info| temp_arch_table.push(inst_info) }

#== generate decode table ==

until temp_arch_table.size == 0
  target_func_str = temp_arch_table[0][DEC::FUNC_STR]

  # declare function
  inst_decoder_c_fp.printf("uint32_t %s (uint32_t inst_hex)\n", target_func_str)
  inst_decoder_c_fp.puts("{\n")
  # declare function headers
  inst_decoder_h_fp.printf("uint32_t %s (uint32_t inst_hex);\n", target_func_str)

  temp_arch_table.each_with_index {|inst_info, index|
    if inst_info[DEC::FUNC_STR] == target_func_str then
      key_table = inst_info[ARCH::KEY_TABLE]
      key_idx   = get_key_idx(key_table[0])
      dec       = inst_info[key_idx]

      if key_table.size == 1 then
        inst_decoder_c_fp.printf("    if (Extract%sField (inst_hex) == 0x%02x)\n", key_table[0], inst_info[key_idx].to_i(2))
        inst_decoder_c_fp.printf("        return %s;\n", inst_info[DEC::INST_NAME])
      else # need to more decode
        temp_arch_table[index][DEC::CURR_DEC] = dec
      end
    end
  }

  dec_table = Array[]
  # generate next decode table
  # shrinking same decode-table
  temp_arch_table.each_with_index {|inst_info, index|
    if inst_info[DEC::FUNC_STR] == target_func_str then
      key_table = inst_info[ARCH::KEY_TABLE]
      key_idx   = get_key_idx(key_table[0])
      func_str = "%s_%s_0x%x"%([inst_info[DEC::FUNC_STR], key_table[0], inst_info[key_idx].to_i(2)])

      if key_table.size != 1 then
        if not dec_table.include?(func_str) then
          dec_table.push (func_str)
          inst_decoder_c_fp.printf("    if (Extract%sField (inst_hex) == 0x%02x)\n", key_table[0], inst_info[key_idx].to_i(2))
          inst_decoder_c_fp.printf("        return %s (inst_hex);\n", func_str)
        end
        temp_arch_table[index][DEC::FUNC_STR] = func_str
        temp_arch_table[index][ARCH::KEY_TABLE].delete_at(0)
      end
    end
  }

  inst_decoder_c_fp.printf("    return -1;\n");

  # finally decode-finished entry is deleted.
  temp_arch_table.delete_if {|inst_info| inst_info[DEC::FUNC_STR] == target_func_str }

  inst_decoder_c_fp.puts("}\n\n\n")
end  # temp_arch_table.size == 0


##
##=== generate function table ===
##

inst_func_fp = File.open('inst_riscv.h', 'w')
gen_header(inst_func_fp) # making header
inst_func_fp.puts("#pragma once")
inst_func_fp.puts("")
inst_func_fp.puts("")

inst_func_fp.puts("#include <stdint.h>")
inst_func_fp.puts("#include \"./env.h\"\n")
inst_func_fp.puts("#include \"./inst_list.h\"\n")
inst_func_fp.puts("#include \"./dec_utils.h\"\n\n\n")

$arch_table.each {|inst_info|
  inst_func_fp.printf("void RISCV_%s (uint32_t inst_hex, riscvEnv env);\n", inst_info[DEC::INST_NAME]);
}


##
##=== generate function call array ===
##
inst_array_fp = File.open('inst_call.c', 'w')
gen_header(inst_array_fp) # making header
inst_array_fp.puts("")
inst_array_fp.puts("")

inst_array_fp.puts("#include <stdint.h>")
inst_array_fp.puts("#include \"./env.h\"\n")
inst_array_fp.puts("#include \"./inst_riscv.h\"\n")

inst_array_fp.puts("void (* const inst_exec_func[])(uint32_t, riscvEnv) = {\n");
$arch_table.each_with_index {|inst_info, index|
  inst_array_fp.printf("    RISCV_%s", inst_info[DEC::INST_NAME]);
  if (index == $arch_table.size-1) then
    inst_array_fp.puts("\n};");
  else
    inst_array_fp.puts(",\n");
  end
}

inst_array_fp.close()
