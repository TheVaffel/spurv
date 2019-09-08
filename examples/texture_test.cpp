#include <Winval.hpp>
#include <Wingine.hpp>
#include <iostream>

#include "../include/spurv.hpp"

using namespace std;

static const float test_vertices[] =
  { -1.0f, -1.0f, 0.5f, 1.0f,
    1.0f, -1.0f, 0.5f, 1.0f,
    -1.0f, 1.0f, 0.5f, 1.0f,
    1.0f, 1.0f, 0.5f, 1.0f};

static const float texture_coords[] =
  {0.0f, 0.0f,
   1.0f, 0.0f,
   0.0f, 1.0f,
   1.0f, 1.0f};

static const uint32_t test_indices[] =
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
  Wingine wg(win);

  WgBuffer vertexBuffer = wg.createVertexBuffer(4*4*sizeof(float), test_vertices);

  WgBuffer textureCoordBuffer = wg.createVertexBuffer(4 * 2 * sizeof(float), texture_coords);

  WgBuffer indexBuffer = wg.createIndexBuffer(3*2*sizeof(int32_t), test_indices);

  std::vector<uint32_t> spirv_vertex;
  std::vector<uint32_t> spirv_fragment;
  
  {
    using namespace spurv;
    
    SShader<SHADER_VERTEX, vec4_s, vec2_s> shader;
    vec4_v position = shader.input<0>();
    vec2_v tex_coord = shader.input<1>();
    
    shader.setBuiltin<BUILTIN_POSITION>(position);
    shader.compile(spirv_vertex, tex_coord);

  }
 
  
  WingineShader vertexShader = wg.createShader(spirv_vertex, WG_SHADER_STAGE_VERTEX);

  {
    using namespace spurv;
    
    SShader<SHADER_FRAGMENT, vec2_s> shader;

    SUniformBinding<float_s> b0 = shader.uniformBinding<float_s>(0, 0);
    float_v oscil = b0.member<0>();

    texture2D_v tex = shader.uniformBinding<texture2D_s>(0, 1);

    vec2_v coord = shader.input<0>();

    vec2_v displacement = vec2_c::get(0.5f, 0.5f);

    vec4_v color = tex.lookup<vec4_s, vec2_s>(coord + displacement);
    
    
    shader.compile(spirv_fragment, oscil * color);

  }

  
  // printf("Printing spirv vertex\n");
  //   for(uint i = 0; i  < spirv_vertex.size(); i++) {
  // printf("%d\n", spirv_vertex[i]);
  // }

  WingineShader fragmentShader = wg.createShader(spirv_fragment, WG_SHADER_STAGE_FRAGMENT);

  float color = 0.2;
  float dc = 0.01;

  WingineResourceSetLayout layout = wg.createResourceSetLayout({WG_RESOURCE_TYPE_UNIFORM, WG_RESOURCE_TYPE_TEXTURE},
							       {WG_SHADER_STAGE_FRAGMENT, WG_SHADER_STAGE_FRAGMENT});

  WgUniform oscilUniform = wg.createUniform(sizeof(float));
  
  WingineResourceSet oscilSet = wg.createResourceSet(layout);
  
  WingineTexture texture = wg.createTexture(texWidth, texHeight, generic_pattern);
  
  wg.updateResourceSet(oscilSet, {&oscilUniform, &texture});

  
  WinginePipeline texPipeline = wg.createPipeline({layout},
						  {vertexShader, fragmentShader},
						  {WG_ATTRIB_FORMAT_4, WG_ATTRIB_FORMAT_2},
						  true);
  
  // WinginePipeline colorPipeline = wg.createPipeline({},
  // {vertexShader, fragmentShader},
  // {WG_ATTRIB_FORMAT_4, WG_ATTRIB_FORMAT_4},
  // true);

  
  WingineObjectGroup oscilGroup(wg, texPipeline);

  WingineRenderObject object1(6, {vertexBuffer, textureCoordBuffer}, indexBuffer);

  
  clock_t start_time = clock();
  int count = 0;

  while(win.isOpen()){
    count++;

    color += dc;
    if(color > 1 || color < 0) {
      dc *= -1;
    }

    wg.setUniform(oscilUniform, &color, sizeof(float));
    
    oscilGroup.startRecording();
    oscilGroup.recordRendering(object1, {oscilSet});
    oscilGroup.endRecording();


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

  wg.destroyShader(vertexShader);
  wg.destroyShader(fragmentShader);

  wg.destroyPipeline(texPipeline);

  wg.destroyBuffer(vertexBuffer);
  wg.destroyBuffer(textureCoordBuffer);
  wg.destroyBuffer(indexBuffer);

  wg.destroyUniform(oscilUniform);
  wg.destroyTexture(texture);
  wg.destroyResourceSet(oscilSet);
  wg.destroyResourceSetLayout(layout);
  
  spirv_vertex.clear();
  spirv_fragment.clear();
  
  return 0;
}
