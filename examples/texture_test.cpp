#include <Winval.hpp>
#include <Wingine.hpp>
#include <iostream>

#include "../include/spurv.hpp"

using namespace std;

float test_vertices[] =
  { -1.0f, -1.0f, 0.5f, 1.0f,
    1.0f, -1.0f, 0.5f, 1.0f,
    -1.0f, 1.0f, 0.5f, 1.0f,
    1.0f, 1.0f, 0.5f, 1.0f};

float texture_coords[] =
  {0.0f, 0.0f,
   1.0f, 0.0f,
   0.0f, 1.0f,
   1.0f, 1.0f};

uint32_t test_indices[] =
  { 0, 1, 3,
    0, 3, 2};


int main(){

  // Make checkerboard patterned texture
  const int texWidth = 1000;
  const int texHeight = 500;
  unsigned char* generic_pattern = new unsigned char[4*texWidth*texHeight];
  for(int i = 0; i < texHeight; i++){
    for(int j = 0; j < texWidth; j++){
      ((int*)generic_pattern)[i*texWidth + j] = (((i/8)%2 == 0) ^ ((j/8)%2 == 0)) ? 0xFFFFFFFF : 0xFF000000;
    }
  }


  Winval win(1280, 720);
  wg::Wingine wg(win);

  wg::VertexBuffer<float>* vertexBuffer = wg.createVertexBuffer<float>(4*4);
  vertexBuffer->set(test_vertices, 4 * 4);

  wg::VertexBuffer<float>* textureCoordBuffer = wg.createVertexBuffer<float>(4 * 2);
  textureCoordBuffer->set(texture_coords, 4 * 2);

  wg::IndexBuffer* indexBuffer = wg.createIndexBuffer(3*2);
  indexBuffer->set(test_indices, 3 * 2);

  
  std::vector<wg::VertexAttribDesc> vertAttrDesc =
    std::vector<wg::VertexAttribDesc> {{wg::tFloat32, // Component type
					0, // Binding no.
					4, // Number of elements
					4 * sizeof(float), // Stride (in bytes)
					0}, // Offset (bytes)
				       {wg::tFloat32, 1, 2, 2 * sizeof(float), 0}};
  
  std::vector<uint32_t> spirv_vertex;
  std::vector<uint32_t> spirv_fragment;
  
  {
    using namespace spurv;
    
    SShader<SShaderType::SHADER_VERTEX, vec4_s, vec2_s> shader;
    vec4_v position = shader.input<0>();
    vec2_v tex_coord = shader.input<1>();
    
    shader.setBuiltin<BUILTIN_POSITION>(position);
    shader.compile(spirv_vertex, tex_coord);

  }
 
  
  wg::Shader* vertexShader = wg.createShader(wg::shaVertex, spirv_vertex);

  {
    using namespace spurv;
    
    SShader<SShaderType::SHADER_FRAGMENT, vec2_s> shader;

    SUniformBinding<float_s> b0 = shader.uniformBinding<float_s>(0, 0);
    float_v oscil = b0.member<0>();

    float_v coo = float_s::cons(0.5f);
    float_v factor = select(coo < oscil, coo, oscil);
    
    texture2D_v tex = shader.uniformBinding<texture2D_s>(0, 1);

    vec2_v coord = shader.input<0>();

    vec2_v displacement = vec2_s::cons(0.2f, 0.2f);
    
    vec2_v dd = vec2_s::cons(displacement[0], 0.0f);
    
    vec4_v color = tex[coord + displacement + dd];
    
    
    shader.compile(spirv_fragment, factor * color);

  }

  
  // printf("Printing spirv vertex\n");
  //   for(uint i = 0; i  < spirv_vertex.size(); i++) {
  // printf("%d\n", spirv_vertex[i]);
  // }

  wg::Shader* fragmentShader = wg.createShader(wg::shaFragment, spirv_fragment);

  float color = 0.2;
  float dc = 0.01;

  std::vector<uint64_t> layout = { wg::resUniform | wg::shaFragment,
				   wg::resTexture | wg::shaFragment };

  wg::Uniform<float>* oscilUniform = wg.createUniform<float>();
  
  wg::ResourceSet* oscilSet = wg.createResourceSet(layout);
  
  wg::Texture* texture = wg.createTexture(texWidth, texHeight);
  texture->set(generic_pattern);
  
  oscilSet->set({oscilUniform, texture});

  
  wg::Pipeline* texPipeline = wg.createPipeline(vertAttrDesc,
					       {layout},
					       {vertexShader, fragmentShader});
  
  
  // WinginePipeline colorPipeline = wg.createPipeline({},
  // {vertexShader, fragmentShader},
  // {WG_ATTRIB_FORMAT_4, WG_ATTRIB_FORMAT_4},
  // true);

  wg::RenderFamily* family = wg.createRenderFamily(texPipeline, true);

  family->startRecording();
  family->recordDraw({vertexBuffer, textureCoordBuffer}, indexBuffer, {oscilSet});
  family->endRecording();
  
  clock_t start_time = clock();
  int count = 0;

  while(win.isOpen()){
    count++;

    color += dc;
    if(color > 1 || color < 0) {
      dc *= -1;
    }

    oscilUniform->set(color);

    family->submit();
    
    wg.present();
    
    clock_t current_time = clock();
    long long int diff = current_time - start_time;
    long long w = 1000/60 - 1000*diff/CLOCKS_PER_SEC;
    if(w > 0){
      win.sleepMilliseconds((int32_t)w);
    }
    start_time = current_time;
    win.flushEvents();
    if(win.isKeyPressed(WK_ESC)){
      break;
    }
  }

  wg.destroy(vertexShader);
  wg.destroy(fragmentShader);

  wg.destroy(texPipeline);

  wg.destroy(vertexBuffer);
  wg.destroy(textureCoordBuffer);
  wg.destroy(indexBuffer);

  wg.destroy(oscilUniform);
  wg.destroy(texture);
  wg.destroy(family);
  
  return 0;
}
