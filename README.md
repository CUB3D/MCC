# MCC
The microcode compiler*

![aa](https://api.travis-ci.com/CUB3D/MCC.svg?token=Bc8txnT1MmVtuHW2CHM3&branch=master)



\* Not actually a compiler, more of an assembler

MCC is a multi target NASM syntax assembler, that produces machine code for on a provided ISA

Output formats currently available
---
* logisim v2.0 raw roms
* Raw rom - WIP

Example program
---
```; Bootstrap instruction
mov PC, ML
mov RAM, IR

.fill 0xF
; Read RAM to A
mov IR, ML
mov RAM, A
inc PC
mov PC, ML
mov RAM, IR

.fill 0xB
; Read RAM to B
mov IR, ML
mov RAM, B
inc PC
mov PC, ML
mov RAM, IR

.fill 0xC
; Read ALU to A
mov ALU, A
inc PC
mov PC, ML
mov RAM, IR
```

Example output
---
```v2.0 raw
5 50 15*0 1004 90 2 5 50 11*0
1004 14 2 5 50 12*0 2080 2 5
50
```

ISA
---
Machine code generation requires a valid ISA configuration, which sepcifies information such as the available registers,
the control bits and instruction timing. ISAs are specified in JSON, like the following example.
```{
  "Registers": {
    "ALU": {
      "ReadBit": "00002000"
    },
    "A": {
      "ReadBit": "00000200",
      "WriteBit": "00000080"
    },
    "B": {
      "WriteBit": "00000004"
    },
    "ML": {
      "ReadBit": "00000008",
      "WriteBit": "00000004"
    },
    "PC": {
      "ReadBit": "00000001",
      "IncrementBit": "00000002"
    },
    "IR": {
      "ReadBit": "00001000",
      "WriteBit": "00000040"
    },
    "RAM": {
      "ReadBit": "00000010",
      "WriteBit": "00000020"
    }
  }
}
```

Building
---
* Clone repo

```
git clone --recursive https://github.com/CUB3D/MCC 
```
* Build

```
mkdir build
cd build
cmake ..
make -j 4
```

* Run
```
./mcc -i input.mcc -o out.rom --arch=cpu.arch
```
