# Spurv

Spurv is a shading language embedded in C++. It supports static type checking.

## Example

```
std::vector<uint32_t> fragment_spirv;

{
  using namespace spurv;

  SShader<SHADER_FRAGMENT, vec2_s> shader;
  vec2_v coord = shader.input<0>();

  texture2D_v texture = shader.uniformBinding<texture2D_s>(0, 0);

  vec4_v color = texture[coord];
    
  shader.compile(fragment_spirv, color);
}

// The SPIR-V bytecode is now in fragment_spirv
```

This is a simple fragment shader that takes in a texture coordinate as an attribute and a texture as a uniform binding, and returns the sampled color of the texture.

A few things to note:
- The most convenient way of specifying the shader is within its own scope, as above
- The shader class is declared with its type (vertex, fragment etc..) and the types of its inputs, just a `vec2` in this case. 
- The type `vec2_v` is an alias representing a concrete value to be computed with. `vec2_s` is the type of the value and cannot be instantiated.
- Uniform bindings are accessed using their set number and binding number (in that order)
- Texture sampling can be done using the \[\]-operator with an argument of applicable type. 
- The shader is compiled given a vector that is to be filled with the bytecode, and then the output values (only fragment color in this case). In the end, the `compile` call performs cleanup, so that no excess memory is used when exiting scope

## Warning

The code is __HIGHLY THREAD-UNSAFE__ because it relies heavily on storing information about types statically in the structs representing types of the language. The `compile` call will clean up all such information, so that a new shader can be compiled after this one has finished, but you __must not__ start a new compilation in another thread before the current one has finished. This will result in undefined behavior.

## Etymology

Spurv means sparrow in Norwegian, so... Yeah
