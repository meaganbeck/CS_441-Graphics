#include <GL/glew.h>    // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <stdlib.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "proj2A_data.h"

#define PHASE3
#define PHASE4
#define PHASE5

void _print_shader_info_log(GLuint shader_index) {
  int max_len = 2048;
  int actual_len = 0;
  char shader_log[2048];
  glGetShaderInfoLog(shader_index, max_len, &actual_len, shader_log);
  printf("shader info log for GL index %u:\n%s\n", shader_index, shader_log);
}

unsigned char *
GetColorMap(int &textureSize)
{
    unsigned char controlPts[8][3] =
    {
        {  71,  71, 219 },
        {   0,   0,  91 },
        {   0, 255, 255 },
        {   0, 127,   0 },
        { 255, 255,   0 },
        { 255,  96,   0 },
        { 107,   0,   0 },
        { 224,  76,  76 },
    };
    textureSize = 256;
    unsigned char *ptr = new unsigned char[textureSize*3];
    int nControlPts = 8;
    double amountPerPair = ((double)textureSize-1.0)/(nControlPts-1.0);
    for (int i = 0 ; i < textureSize ; i++)
    {
	    //somewhere, maybe here, sed up a 1D texture (an array)
	    //texture function(set up texture function) assumes coordinates between 0 and 1 (1d coords) so just int array[i] = (0-1). 
	    //data values are 1-6. so data-1/5 is the texture coordinate in fragment shader. 
	    //
	    //Use texture coordinate to look up pixel color in texture data. then assigne frag that color
	    //Take coordinate (will be one number) find texture data at that point
        int lowerControlPt = (int)(i/amountPerPair);
        int upperControlPt = lowerControlPt+1;
        if (upperControlPt >= nControlPts)
            upperControlPt = lowerControlPt; // happens for i == textureSize-1

        double proportion = (i/amountPerPair)-lowerControlPt;
        for (int j = 0 ; j < 3 ; j++)
            ptr[3*i+j] = controlPts[lowerControlPt][j]
                       + proportion*(controlPts[upperControlPt][j]-
                                     controlPts[lowerControlPt][j]);
    }

    return ptr;
}

unsigned char *
GetTigerStripes(int &textureSize)
{
    textureSize = 2048;
    unsigned char *ptr = new unsigned char[textureSize];
    int numStripes = 20;
    int valsPerStripe = textureSize/numStripes;
    for (int i = 0 ; i < numStripes ; i++)
    {
        for (int j = 0 ; j < valsPerStripe ; j++)
        {
           int idx = i*valsPerStripe+j;
           if (j < valsPerStripe / 3)
               ptr[idx] = 152;
           else
               ptr[idx] = 255;
        }
    }
    for (int i = numStripes*valsPerStripe ; i < textureSize ; i++)
    {
        ptr[i] = 0;
    }
    return ptr;
}




GLuint SetupPhase345DataForRendering()
{
  printf("Getting data for Phase 3\n");

  // Add data to VBOs and VAO for phase 3 here
  // TODO: GET texture data in
  float points[77535];
  float data[25845];
  float normals[77535]; 
  GLuint indices[44106];
			
  for (int i = 0; i < 77535; i++)
  {
	points[i] = tri_points[i];
	normals[i] = tri_normals[i];
  }
  for (int i = 0; i < 25845; i++)
  {
	data[i] = tri_data[i];
  }
  for (int i =0; i < 44106; i++)
  {
	indices[i] = tri_indices[i];
  }
 
  GLuint points_vbo =0;
  glGenBuffers(1, &points_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glBufferData(GL_ARRAY_BUFFER, 77535 * sizeof(float), points, GL_STATIC_DRAW);

  GLuint data_vbo = 0;
  glGenBuffers(1, &data_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, data_vbo);
  glBufferData(GL_ARRAY_BUFFER, 25845 * sizeof(float), data, GL_STATIC_DRAW);

  GLuint index_vbo;    // Index buffer object
  glGenBuffers( 1, &index_vbo);
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_vbo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, 44106 * sizeof(GLuint), indices, GL_STATIC_DRAW );
//new
  GLuint normals_vbo;
  glGenBuffers(1, &normals_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
  glBufferData(GL_ARRAY_BUFFER, 77535 * sizeof(float), normals, GL_STATIC_DRAW);
//Dont think i need this
  
  
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao); //makes the buffer active
  //makes vao and puts vbos in it
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//0th vbo goes in location 0, and 3 values per vertex, floats, and not normalized
  glBindBuffer(GL_ARRAY_BUFFER, data_vbo);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
  
  glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  return vao;
}


const char *phase345VertexShader =
  "#version 400\n"
  "layout (location = 0) in vec3 vertex_position;\n"
  "layout (location = 1) in float vertex_data;\n"
  "layout (location = 2) in vec3 vertex_normal;\n"
  "uniform mat4 MVP;\n"
  "uniform vec3 cameraloc;  // Camera position \n"
  "uniform vec3 lightdir;   // Lighting direction \n"
  "uniform vec4 lightcoeff; // Lighting coeff, Ka, Kd, Ks, alpha\n"
  "out float depth;\n" //found from gl_Position. divide by W??
  "out float data;\n"
  "out float shading_amount;\n"
  "void main() {\n"
  "  vec4 position = vec4(vertex_position, 1.0);\n"
  "  gl_Position = MVP*position;\n"
  "  //vec3 normalized = gl_Position.xyz/ gl_Position.w;\n"
  "  //depth = normalized[2];\n"
  "  //depth = gl_Position.z/gl_Position.w;\n"
  " // gl_Position/=gl_Position.w;\n"
  "  depth = gl_Position.z/gl_Position.w;\n"

  
  "  data = vertex_data;\n"
  "vec3 R;\n" //good
  "float LdotN = lightdir[0]*vertex_normal[0] + lightdir[1]*vertex_normal[1] + lightdir[2]*vertex_normal[2];\n" //good
  "float diffuse = max(0,LdotN);\n" //good
  "R[0] = 2 * LdotN * vertex_normal[0] - lightdir[0];\n" //good
  "R[1] = 2 * LdotN * vertex_normal[1] - lightdir[1];\n" //good
  "R[2] = 2 * LdotN * vertex_normal[2] - lightdir[2];\n" //good
  
  "vec3 V = vec3(cameraloc[0] - position[0], cameraloc[1] - position[1], cameraloc[2] - position[2]);\n"
  "float normalize = sqrt(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]);\n"
  "V[0] = V[0] / normalize;\n"
  "V[1] = V[1] / normalize;\n"
  "V[2] = V[2] / normalize;\n"
  "float specular = pow(max(0, R[0]*V[0] + R[1] * V[1] + R[2] * V[2]),lightcoeff[3]);\n"
  "shading_amount = lightcoeff[0] + lightcoeff[1]*diffuse + lightcoeff[2]*specular;\n"

  "}\n";

const char *phase345FragmentShader =


  "#version 400\n" 
  "uniform sampler1D texture1;\n"
  "uniform sampler1D texture2;\n"
  "in float shading_amount;\n"
  "in float data;\n" //gets texture data
  "in float depth;\n"
  "out vec4 frag_color;\n" //send frag co
  "void main() {\n"
  "  float tex_coord = (data-1)/5.0;\n"
  "  vec4 tiger_stripe = texture(texture2, depth);\n" //wrong
  "  frag_color = texture(texture1, tex_coord);\n"  //idk how to apply second texture
  "   frag_color = frag_color*shading_amount*tiger_stripe.x;\n"
  "}\n";

int main() {
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(700, 700, "CIS 441", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);
  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();
  
  // get version info
  const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte *version = glGetString(GL_VERSION);   // version as a string
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable(GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

  GLuint vao = 0;
  vao = SetupPhase345DataForRendering();
  const char* vertex_shader = phase345VertexShader;
  const char* fragment_shader = phase345FragmentShader;



  GLuint textures[2]; //TODO change to 2
  glGenTextures(2, textures); //generates a handle to refer to textures with (just need once)
			  
  int textureSize = 0;
  unsigned char *map = GetColorMap(textureSize);
  
  glActiveTexture(GL_TEXTURE0); 
  glBindTexture(GL_TEXTURE_1D, textures[0]); 
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, textureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, map);
  glGenerateMipmap(GL_TEXTURE_1D);


  int tigerTextureSize = 0;
  unsigned char* tiger = GetTigerStripes(tigerTextureSize);

  glActiveTexture(GL_TEXTURE1); 
  glBindTexture(GL_TEXTURE_1D, textures[1]); 
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, tigerTextureSize, 0, GL_RED, GL_UNSIGNED_BYTE, tiger); //GL_RGB maybe fix
  glGenerateMipmap(GL_TEXTURE_1D);
  




  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);
  int params = -1;
  glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vs);
    _print_shader_info_log(vs);
    exit(EXIT_FAILURE);
  }

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);
  glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
  
  //TODO: read texture from somewhere
  
  if (GL_TRUE != params) {
    fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
    _print_shader_info_log(fs);
    exit(EXIT_FAILURE);
  }

  GLuint shader_programme = glCreateProgram();
  glAttachShader(shader_programme, fs);
  glAttachShader(shader_programme, vs);
  glLinkProgram(shader_programme);

  glUseProgram(shader_programme);

  GLuint texture1Location = glGetUniformLocation(shader_programme, "texture1");
  glUniform1i(texture1Location, 0);
  
  GLuint texture2Location = glGetUniformLocation(shader_programme, "texture2");
  glUniform1i(texture2Location, 1);

  
  glm::mat4 Projection = glm::perspective(
      glm::radians(30.0f), (float)1000 / (float)1000,  40.0f, 60.0f);
  glm::vec3 camera(0, 40, 40);
  glm::vec3 origin(0, 0, 0);
  glm::vec3 up(0, 1, 0);
  // Camera matrix
  glm::mat4 View = glm::lookAt(
    camera, // Camera in world space
    origin, // looks at the origin
    up      // and the head is up
  );
  // Model matrix : an identity matrix (model will be at the origin)
  glm::mat4 Model = glm::mat4(1.0f);
  // Our ModelViewProjection : multiplication of our 3 matrices
  glm::mat4 mvp = Projection * View * Model;

  // Get a handle for our "MVP" uniform
  // Only during the initialisation
  GLuint mvploc = glGetUniformLocation(shader_programme, "MVP");
  // Send our transformation to the currently bound shader, in the "MVP" uniform
  // This is done in the main loop since each model will have a different MVP matrix
  // (At least for the M part)
  glUniformMatrix4fv(mvploc, 1, GL_FALSE, &mvp[0][0]);

  GLuint camloc = glGetUniformLocation(shader_programme, "cameraloc");
  glUniform3fv(camloc, 1, &camera[0]);
  glm::vec3 lightdir = glm::normalize(camera - origin);   // Direction of light
  GLuint ldirloc = glGetUniformLocation(shader_programme, "lightdir");
  glUniform3fv(ldirloc, 1, &lightdir[0]);
  glm::vec4 lightcoeff(0.3, 0.7, 2.8, 50.5); // Lighting coeff, Ka, Kd, Ks, alpha
  GLuint lcoeloc = glGetUniformLocation(shader_programme, "lightcoeff");
  glUniform4fv(lcoeloc, 1, &lightcoeff[0]);

  while (!glfwWindowShouldClose(window)) {
    
    // wipe the drawing surface clear
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao);
    // Draw triangles

    // Add correct number of indices
    glDrawElements( GL_TRIANGLES, 44106, GL_UNSIGNED_INT, NULL );

    // update other events like input handling
    glfwPollEvents();
    // put the stuff we've been drawing onto the display
    glfwSwapBuffers(window);
  }

  // close GL context and any other GLFW resources
  glfwTerminate();
  return 0;
}
