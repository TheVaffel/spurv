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

- [x] If one instance of a struct appears as a member of a uniform buffer struct, while another instance of that same struct appears as another uniform buffer struct. Then the first instance will be Block-decorated, which it should not be. Solved by giving the SStruct-type a template parameter indicationg its decoration.