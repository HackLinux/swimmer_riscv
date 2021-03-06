# Swimmer-RISCV
Functional Simulator for RISC-V instruction sets

## Motolora S-Record format file should be needed

```
  usage : swimmer_riscv -h <s-record file>

Options
    -c <int>   : simulation step
    -o <log>   : log file name
```

## sample of instruction simulator log:

```
...
        No : PC-Addres  Inst-Hex : [         mnemonic         ]    [Behavior of instructions ]
----------------------------------------------------------------------------------------------------------------------------
        36 : [00001674] 04112e23 : sw         r02,r01,0x02|r28     (00005f94)<=00000080
        37 : [00001678] 04812c23 : sw         r02,r08,0x02|r24     (00005f90)<=00000000
        38 : [0000167c] 00012223 : sw         r02,r00,0x00|r04     (00005f3c)<=00000000
        39 : [00001680] fd1ff0ef : jal        r01,0xfd1ff          r01<=00001684 pc<=00001650
        40 : [00001650] 00100793 : addi       r15,r00,0x001        r15<=00000001
        41 : [00001654] 00f50023 : sb         r10,r15,0x00|r00     (00005f86)<=00000001
        42 : [00001658] 00008067 : jalr       r00,r01,r103         r00<=0000165c pc<=00001684
        43 : [00001684] 00100513 : addi       r10,r00,0x001        r10<=00000001
        44 : [00001688] dc9ff0ef : jal        r01,0xdc9ff          r01<=0000168c pc<=00001450
        45 : [00001450] 00500793 : addi       r15,r00,0x005        r15<=00000005
        46 : [00001454] 04a7e063 : bltu       r15,r10,0x02
        47 : [00001458] f0c18793 : addi       r15,r03,0xf0c        r15<=00001eac
        48 : [0000145c] 00251513 : slli       r10,r10,r02          r10<=00000004
        49 : [00001460] 00f50533 : add        r10,r10,r15          r10<=00001eb0
        50 : [00001464] 00052783 : lw         r15,r10,0x000        r15<=00001474
        51 : [00001468] 00078067 : jalr       r00,r15,r103         r00<=0000146c pc<=00001474
        52 : [00001474] 7dc1a503 : lw         r10,r03,0x7dc        r10<=00000000
        53 : [00001478] 00008067 : jalr       r00,r01,r103         r00<=0000147c pc<=0000168c
        54 : [0000168c] 00a11623 : sh         r02,r10,0x00|r12     (00005f44)<=00000000
        55 : [00001690] 00200513 : addi       r10,r00,0x002        r10<=00000002
        56 : [00001694] dbdff0ef : jal        r01,0xdbdff          r01<=00001698 pc<=00001450
        57 : [00001450] 00500793 : addi       r15,r00,0x005        r15<=00000005
        58 : [00001454] 04a7e063 : bltu       r15,r10,0x02
        59 : [00001458] f0c18793 : addi       r15,r03,0xf0c        r15<=00001eac
        60 : [0000145c] 00251513 : slli       r10,r10,r02          r10<=00000008
        61 : [00001460] 00f50533 : add        r10,r10,r15          r10<=00001eb4
        62 : [00001464] 00052783 : lw         r15,r10,0x000        r15<=0000147c
        63 : [00001468] 00078067 : jalr       r00,r15,r103         r00<=0000146c pc<=0000147c
...
```
