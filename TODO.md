# TODO

Somewhat in order of priority

### Datatype Features
- [x] Structs
- [x] 2D Texture
- [x] Functions for composing vectors and matrices
- [x] (At least ish) Functions for reordering vector components

### Structural Features
- [x] Conditionals
- [x] Loops
- [ ] Functions

### BIG features (probably)
- [x] Uniforms
- [ ] Computation kernel support
- [ ] Geometry shader support

### Operations
- [x] Arithmetics
- [x] Basic vector and matrix operations
- [x] Int `mod` and `rem`
- [x] Floating point `mod` and `rem`
- [x] Implement addition, subtraction, multiplication and the like for all combinations of matrices and scalars

### "Minor" Refactorizations
- [x] Simplify datatype names
- [x] Simplify exposed user functions
- [x] Make shortcuts for types of value nodes
- [x] Make conversion system that allows for simpler interaction where user can provide primitive datatypes (or matrices from FlatAlg) in place for Spurv value

### Cleanup

- [x] Make sure non-template member functions of non-template classes are declared in .cpp files (violators found in pointers_impl.hpp and control_flow_impl.hpp)

### Bugs

- [ ] Currently, only one instance of a struct with given members is created. This means that if there are multiple structs with the same member types in e.g. different uniforms, they will receieve the same decorations. A case where this will actually be a problem, is when one instance of the struct appears as a member of a uniform buffer struct, while another instance appears as another uniform buffer struct. Then the first instance will be Block-decorated, which it should not be. Possible solution: Give SStruct another template parameter, a "decoration parameter". E.g. 2 means block annotated, 0 means not annotated