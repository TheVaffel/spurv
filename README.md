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


Here follows a more convoluted shader pair for a rendering of the Mandelbrot Set

Vertex Shader:

```
  float scale = 0.001f;
  float offx = -0.77568377f;
  float offy = 0.13646737f;

  std::vector<uint32_t> vertex_spirv;
  {
    using namespace spurv;

    SShader<SShaderType::SHADER_VERTEX, vec4_s> shader;
    vec4_v s_pos = shader.input<0>();
    uint_v vi = shader.getBuiltin<BUILTIN_VERTEX_INDEX>();
    SUniformBinding<float_s> un1 = shader.uniformBinding<float_s>(0, 0);

    // Compute corners, (-1, -1) to (1, 1)
    float_v pv0 = cast<float_s>(vi % 2) * 2.f - 1.f;
    float_v pv1 = cast<float_s>(vi / 2) * 2.f - 1.f;

    vec2_v coord = vec2_s::cons(pv0, pv1) * scale + vec2_s::cons(offx, offy);

    shader.setBuiltin<BUILTIN_POSITION>(s_pos);
    shader.compile(vertex_spirv, coord);

  }
```

Fragment Shader:

```
  int mandelbrot_iterations = 1000;
  float max_rad = 4.f;
  
  std::vector<uint32_t> fragment_spirv;
  {
    using namespace spurv;

    SShader<SShaderType::SHADER_FRAGMENT, vec2_s> shader;
   
    vec2_v coord = shader.input<0>();

    vec2_lv z = shader.local<vec2_s>();
    z.store(coord);

    int_lv num_its = shader.local<int_s>();
    num_its.store(mandelbrot_iterations);

    int_v i = shader.forLoop(mandelbrot_iterations);
    {
      vec2_v zl = z.load();
      float_v a = zl[0];
      float_v b = zl[1];

      float_v r = a * a + b * b;
      
      shader.ifThen(r > max_rad);
      {
	num_its.store(i);
	shader.breakLoop();
      }
      shader.endIf();
      
      vec2_v new_z = vec2_s::cons(a * a - b * b, 2.f * a * b) + coord;
      z.store(new_z);
    }
    shader.endLoop();

    float_v itnum = cast<float_s>(num_its.load());

    float_v r = (sin(itnum * 0.143f) + 1.0f) / 2.0f;
    float_v g = (cos(itnum * 0.273f) + 1.0f) / 2.0f;
    float_v b = (sin(itnum * 0.352f) + 1.0f) / 2.0f;

    vec4_v black = vec4_s::cons(0.0f, 0.0f, 0.0f, 1.0f);
    
    vec4_v out_col = select(itnum < mandelbrot_iterations,
			    vec4_s::cons(r, g, b, 1.0f), black);
    
    shader.compile(fragment_spirv, out_col);
  }
```

Result:

![Rendering of an area of the MandelbrotSet](mandelbrot.png)


## Warning

The code is __HIGHLY THREAD-UNSAFE__ because it relies heavily on storing information about types statically in the structs representing types of the language. The `compile` call will clean up all such information, so that a new shader can be compiled after this one has finished, but you __must not__ start a new compilation in another thread before the current one has finished. This will result in undefined behavior.

## Etymology

Spurv means sparrow in Norwegian, so... Yeah
