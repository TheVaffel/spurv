#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <Winval.hpp>
#include <Wingine.hpp>
#include <WgUtils.hpp>
#include <iostream>

#include <../include/spurv.hpp>

using namespace std;

float test_vertices[] =
  { -1.0f, -1.0f, 0.5f, 1.0f,
     1.0f, -1.0f, 0.5f, 1.0f,
    -1.0f,  1.0f, 0.5f, 1.0f,
     1.0f,  1.0f, 0.5f, 1.0f };

float test_colors[] =
  { 1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f};

uint32_t test_indices[] =
  { 0, 1, 3,
    0, 3, 2};


int main(){

  Winval win(1280, 720);
  wg::Wingine wg(win);

  wg::VertexBuffer<float>* vertexBuffer = wg.createVertexBuffer<float>(4*4);
  vertexBuffer->set(test_vertices, 4 * 4);

  wg::VertexBuffer<float>* colorBuffer = wg.createVertexBuffer<float>(4*4);
  colorBuffer->set(test_colors, 4 * 4);

  wg::IndexBuffer* indexBuffer = wg.createIndexBuffer(3*2);
  indexBuffer->set(test_indices, 3 * 2);

  wgut::Model model({vertexBuffer, colorBuffer}, indexBuffer);

  std::vector<uint32_t> spirv_vertex;
  std::vector<uint32_t> spirv_fragment;

  {
    using namespace spurv;

    SShader<SShaderType::SHADER_VERTEX, vec4_s, vec4_s> shader;
    vec4_v position = shader.input<0>();
    vec4_v color = shader.input<1>();

    auto un1 = shader.uniformBinding<float_s>(0, 0);

    float_v oscil = un1.member<0>().load();

    vec4_v color_prod = oscil * color;

    shader.setBuiltin<BUILTIN_POSITION>(position);
    shader.compile(spirv_vertex, color_prod);

  }

  wg::Shader* vertexShader = wg.createShader(wg::shaVertex, spirv_vertex);


  printf("Done with vertex shader\n");

  {
    using namespace spurv;

    SShader<SShaderType::SHADER_FRAGMENT, vec4_s> shader;

    vec4_v color = shader.input<0>();

    shader.compile(spirv_fragment, color);

  }

  float color = 0.2;
  float dc = 0.01;

  wg::Uniform<float>* colorUniform = wg.createUniform<float>();

  std::vector<uint64_t> resourceSetLayout  = {wg::resUniform | wg::shaVertex};

  wg::ResourceSet* colorSet = wg.createResourceSet(resourceSetLayout);

  colorSet->set({colorUniform});


  wg::Shader* fragmentShader = wg.createShader(wg::shaFragment, spirv_fragment);

  std::vector<wg::VertexAttribDesc> vertAttrDesc =
    std::vector<wg::VertexAttribDesc> {{wg::tFloat32, // Component type
					0, // Binding no.
					4, // Number of elements
					4 * sizeof(float), // Stride (in bytes)
					0}, // Offset (bytes)
				       {wg::tFloat32, 1, 4, 4 * sizeof(float), 0}};

  wg::Pipeline* colorPipeline = wg.createPipeline(vertAttrDesc,
						 {resourceSetLayout},
						 {vertexShader, fragmentShader});

  wg::RenderFamily* family = wg.createRenderFamily(colorPipeline, true);

  clock_t start_time = clock();
  int count = 0;

  family->startRecording();
  family->recordDraw(model.getVertexBuffers(), model.getIndexBuffer(), {colorSet});
  family->endRecording();


  wg::SemaphoreChain* main_chain = wg.createSemaphoreChain();

  while(win.isOpen()){
    count++;

    color += dc;
    if(color > 1 || color < 0) {
      dc *= -1;
    }

    colorUniform->set(color);

    family->submit();

    wg.present({ main_chain });

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

  wg.destroy(main_chain);

  wg.destroy(vertexShader);
  wg.destroy(fragmentShader);

  wg.destroy(colorPipeline);

  wg.destroy(vertexBuffer);
  wg.destroy(colorBuffer);
  wg.destroy(indexBuffer);

  wg.destroy(colorUniform);
  wg.destroy(family);

  spirv_vertex.clear();
  spirv_fragment.clear();

  return 0;
}
