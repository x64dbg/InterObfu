# InterObfu - Intermediate x86 Instruction Representation

## Project Overview

**InterObfu** is a C++ library that provides an intermediate representation for x86 assembly instructions, designed specifically for obfuscation and deobfuscation applications. This code is intended to be part of an academic paper and focuses on pattern matching, peephole optimization, and instruction transformation capabilities.

## Core Architecture

### 1. Instruction Representation System

#### Instruction Structure (`Instruction.h`)
- **Primary Component**: Central data structure representing x86 instructions
- **Key Features**:
  - Opcode representation with mnemonic support
  - Variable operand count (0-4 operands)
  - Metadata storage (address, size, bytes, flags)
  - Register read/write tracking
  - Custom comparison function support for pattern matching
  - String representation via `ToString()`

#### Operand System (`Operand.h`)
- **Types Supported**:
  - Register operands (`Reg`)
  - Immediate values (`Imm`) 
  - Memory references (`Mem`)
- **Advanced Features**:
  - Size information
  - Access flags (read/write)
  - Custom comparison functions for wildcards

#### Register Management (`Register.h`)
- **Comprehensive Coverage**: All x86/x64 registers including:
  - General purpose (8-bit, 16-bit, 32-bit, 64-bit)
  - Segment registers
  - Control registers (CR0-CR15)
  - Debug registers (DR0-DR15)
  - FPU registers (FP0-FP7, ST0-ST7)
  - MMX/SSE/AVX registers (MM0-MM7, XMM0-XMM31, YMM0-YMM31, ZMM0-ZMM31)
  - Mask registers (K0-K7)
- **Size Calculation**: Automatic register size determination
- **Offset Support**: High byte register offset handling (AH, BH, etc.)

#### Memory Representation (`Memory.h`)
- **Components**:
  - Segment prefix
  - Base register
  - Index register  
  - Scale factor (1, 2, 4, 8)
  - Displacement value
- **Flexible Construction**: Multiple constructor overloads for different addressing modes

### 2. Pattern Matching Engine

#### Pattern System (`Pattern.h`)
- **Core Functionality**:
  - Multi-instruction pattern definition
  - Wildcard support through state variables
  - Pattern searching with Boyer-Moore-like efficiency
  - State-based matching with backtracking
  - Replacement rule definitions

#### State Management (`State.h`)
- **Wildcard Variables**: Up to 4 instances each of:
  - Operands (`operand1`, `operand2`, etc.)
  - Registers (`reg1`, `reg2`, etc.)
  - Values (`val1`, `val2`, etc.)
  - Memory references (`memory1`, `memory2`, etc.)
  - Opcodes (`opcode1`, `opcode2`, etc.)
- **Factory Methods**: 
  - `MakeRegisterN()` - Create register wildcards
  - `MakeValueN()` - Create value wildcards
  - `MakeMemoryN()` - Create memory wildcards
  - `MakeOperandN()` - Create operand wildcards
- **Special Predicates**:
  - `MakeRegisterSize()` - Match registers by size
  - `MakeOpcodeList()` - Match from opcode lists

### 3. Peephole Optimization Framework

#### Peephole Engine (`Peephole.h`)
- **Aho-Corasick Integration**: Efficient multi-pattern string matching
- **Two-Phase Matching**:
  1. Fast mnemonic-only matching using Aho-Corasick trie
  2. Full operand matching for valid candidates
- **Conflict Resolution**:
  - Longer patterns preferred over shorter ones
  - Earlier patterns preferred for same-length matches
  - Priority-based selection for equal patterns
  - Shorter replacement sequences preferred
- **Pattern Replacement**: Lambda-based replacement functions

#### Optimization Examples (from `main.cpp`)
1. **Dead Code Elimination**: `shl reg1, 0` → *(remove)*
2. **Redundant Push/Pop**: `push reg1; pop reg1` → *(remove)*
3. **Register Transfer**: `push reg1; pop reg2` → `mov reg2, reg1`
4. **Stack-based XOR**: `push reg1; xor [esp], reg2; pop reg1` → `xor reg1, reg2`

### 4. Assembly Integration

#### Assembly Parser (`Assemble.h`)
- **XEDParse Integration**: Text-to-binary assembly compilation
- **Capstone Integration**: Binary-to-instruction disassembly
- **Multi-Architecture**: Support for 32-bit and 64-bit modes
- **Address Resolution**: Configurable base addresses

#### String Conversion (`Convert.h`, `ToString.cpp`)
- **Bidirectional Mapping**:
  - Register names ↔ Register enums
  - Instruction mnemonics ↔ Opcode enums
- **Pretty Printing**: Human-readable instruction representation
- **Error Handling**: Invalid input detection and fallback

### 5. Advanced Features

#### Wildcard System
- **Value Wildcards**: Custom comparison functions for flexible matching
- **Segment Wildcards**: `SegWild()` for any segment register
- **Register Wildcards**: `RegWild()` for any register
- **Complete Flexibility**: User-defined comparison predicates

#### String Matching (`aho-corasick.h`)
- **Multi-Pattern Search**: Simultaneous search for multiple patterns
- **Linear Time Complexity**: O(n + m + z) where n=text length, m=pattern lengths, z=matches
- **Overlap Handling**: Configurable overlap resolution strategies

## Current Functionality

### ✅ Implemented Features

1. **Complete Instruction Representation**
   - All x86/x64 instruction components
   - Metadata preservation (addresses, sizes, flags)
   - Register tracking (read/written registers)

2. **Pattern Matching System**
   - Multi-instruction patterns
   - Wildcard variables with state management
   - Efficient search algorithms
   - Flexible comparison functions

3. **Peephole Optimization Framework**
   - Aho-Corasick pattern matching
   - Multi-pass optimization support
   - Conflict resolution strategies
   - Lambda-based replacements

4. **Assembly Integration**
   - Text assembly parsing via XEDParse
   - Binary disassembly via Capstone
   - Cross-platform library support

5. **Testing Framework**
   - Multiple test functions in `main.cpp`
   - Pattern matching validation
   - Optimization effectiveness testing
   - Converter validation

## Planned Functionality (Based on TODOs)

### 🔄 Identified Improvements

1. **Enhanced Pattern Validation** (`main.cpp:177`)
   - **Current**: Basic mnemonic matching in Aho-Corasick
   - **Planned**: Full pattern matching with wildcards during Aho-Corasick validation
   - **Impact**: Reduced false positives, improved performance

2. **Overlap Resolution** (`main.cpp:201`, `Peephole.h:91`)
   - **Current**: First pattern wins, overlaps ignored
   - **Planned**: Sophisticated overlap handling with better predicate
   - **Impact**: More optimal pattern selection in complex scenarios

3. **Memory Scale Validation** (`Memory.h:10`)
   - **Current**: Value type for scale factor
   - **Planned**: Proper validation for x86 scale factors (1, 2, 4, 8)
   - **Impact**: Better error detection and validation

## Technical Implementation Details

### Design Patterns Used

1. **Visitor Pattern**: Custom comparison functions for different matching strategies
2. **Factory Pattern**: State factory methods for wildcard creation
3. **Strategy Pattern**: Configurable comparison predicates
4. **Template Pattern**: Generic state value management

### Performance Characteristics

- **Pattern Search**: O(n + m + z) complexity via Aho-Corasick
- **State Management**: O(1) wildcard lookup and assignment
- **Memory Usage**: Efficient representation with minimal overhead
- **Optimization Passes**: Configurable iteration limits (default: 1000 passes)

### Integration Points

1. **XEDParse Library**: Assembly text parsing
2. **Capstone Engine**: Disassembly and metadata extraction  
3. **Standard Template Library**: Container and algorithm support
4. **Platform Libraries**: Windows-specific optimizations

## Development Infrastructure

### Code Quality
- **AStyle Integration**: Automated code formatting via git hooks
- **GPG Signing**: Verified binary integrity for development tools
- **Version Control**: Git with pre-commit hooks for consistency

### Build System
- **Visual Studio Solution**: Windows-native development
- **Multi-Platform**: 32-bit and 64-bit support
- **Library Linking**: Automatic linking of required dependencies

## Usage Examples

### Basic Pattern Definition
```cpp
// Pattern: push reg1; pop reg2
Pattern transferPattern;
transferPattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
transferPattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(2)))));

// Replacement: mov reg2, reg1
transferPattern.repls.push_back([](const State & state) {
    return Instruction(X86_INS_MOV, 
                      Operand(state.registers[2]()), 
                      Operand(state.registers[1]()));
});
```

### Optimization Application
```cpp
std::vector<Instruction> input = /* ... assembly instructions ... */;
std::vector<Pattern> patterns = /* ... optimization patterns ... */;

Peephole optimizer(patterns);
std::vector<Instruction> optimized;

// Apply optimizations until no more changes
while(optimizer.Optimize(input, optimized)) {
    input = optimized;
}
```

## Research Applications

This library is designed for academic research in:
- **Code Obfuscation**: Pattern-based transformation techniques
- **Deobfuscation**: Reverse engineering and simplification
- **Compiler Optimization**: Peephole optimization research
- **Malware Analysis**: Automated pattern recognition
- **Binary Analysis**: Instruction-level program understanding

## Future Research Directions

1. **Machine Learning Integration**: Pattern discovery through ML techniques
2. **Control Flow Analysis**: Integration with CFG reconstruction
3. **Semantic Preservation**: Verification of optimization correctness
4. **Scalability**: Large program analysis capabilities
5. **Cross-Architecture**: Extension to ARM, RISC-V, and other architectures