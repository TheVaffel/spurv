#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <Winval.hpp>
#include <Wingine.hpp>
#include <iostream>

#include <spurv.hpp>

using namespace std;

static const float test_vertices[] =
  { -1.0f, -1.0f, 0.5f, 1.0f,
    1.0f, -1.0f, 0.5f, 1.0f,
    -1.0f, 1.0f, 0.5f, 1.0f,
    1.0f, 1.0f, 0.5f, 1.0f};

static const float test_colors[] =
  { 1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f};

static const uint32_t test_indices[] =
  { 0, 1, 3,
    0, 3, 2};


int main(){

  Winval win(1280, 720);
  Wingine wg(win);

  WgBuffer vertexBuffer = wg.createVertexBuffer(4*4*sizeof(float), test_vertices);

  WingineBuffer colorBuffer = wg.createVertexBuffer(4*4*sizeof(float), test_colors);

  WingineBuffer indexBuffer = wg.createIndexBuffer(3*2*sizeof(int32_t), test_indices);

  std::vector<uint32_t> spirv_vertex;
  std::vector<uint32_t> spirv_fragment;
  
  {
    using namespace spurv;
    
    SShader<SHADER_VERTEX, vec4_s, vec4_s> shader;
    vec4_v position = shader.input<0>();
    vec4_v color = shader.input<1>();

    SUniformBinding<float_s> un1 = shader.uniformBinding<float_s>(0, 0);
    float_v oscil = un1.member<0>();

    vec4_v color_prod = oscil * color;
    
    shader.setBuiltin<BUILTIN_POSITION>(position);
    shader.compile(spirv_vertex, color_prod);

  }
 
  
  WingineShader vertexShader = wg.createShader(spirv_vertex, WG_SHADER_STAGE_VERTEX);

  
  printf("Done with vertex shader\n");

  {
    using namespace spurv;
    
    SShader<SHADER_FRAGMENT, vec4_s> shader;

    vec4_v color = shader.input<0>();

    shader.compile(spirv_fragment, color);

  }

  
  printf("Printing spirv vertex\n");
  for(uint i = 0; i  < spirv_vertex.size(); i++) {
    printf("%d\n", spirv_vertex[i]);
  }

  WingineShader fragmentShader = wg.createShader(spirv_fragment, WG_SHADER_STAGE_FRAGMENT);

  float color = 0.2;
  float dc = 0.01;

  WingineResourceSetLayout layout = wg.createResourceSetLayout({WG_RESOURCE_TYPE_UNIFORM},
							       {WG_SHADER_STAGE_VERTEX});

  WgUniform colorUniform = wg.createUniform(sizeof(float));

  WingineResourceSet colorSet = wg.createResourceSet(layout);
  wg.updateResourceSet(colorSet, {&colorUniform});

  WinginePipeline colorPipeline = wg.createPipeline({layout},
						    {vertexShader, fragmentShader},
						    {WG_ATTRIB_FORMAT_4, WG_ATTRIB_FORMAT_4},
						    true);
  
  // WinginePipeline colorPipeline = wg.createPipeline({},
  // {vertexShader, fragmentShader},
  // {WG_ATTRIB_FORMAT_4, WG_ATTRIB_FORMAT_4},
  // true);

  
  WingineObjectGroup colorGroup(wg, colorPipeline);

  WingineRenderObject object1(6, {vertexBuffer, colorBuffer}, indexBuffer);

  
  clock_t start_time = clock();
  int count = 0;

  while(win.isOpen()){
    count++;

    color += dc;
    if(color > 1 || color < 0) {
      dc *= -1;
    }

    wg.setUniform(colorUniform, &color, sizeof(float));
    
    colorGroup.startRecording();
    // colorGroup.recordRendering(object1, {});
    colorGroup.recordRendering(object1, {colorSet});
    colorGroup.endRecording();


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

  wg.destroyPipeline(colorPipeline);

  wg.destroyBuffer(vertexBuffer);
  wg.destroyBuffer(colorBuffer);
  wg.destroyBuffer(indexBuffer);

  wg.destroyUniform(colorUniform);
  wg.destroyResourceSet(colorSet);
  wg.destroyResourceSetLayout(layout);
  
  spirv_vertex.clear();
  spirv_fragment.clear();
  
  return 0;
}
