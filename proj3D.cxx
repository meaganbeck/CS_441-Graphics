#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using std::endl;
using std::cerr;

#include <GL/glew.h>    // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>   // glm::vec3
#include <glm/vec4.hpp>   // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>  // glm::translate, glm::rotate, glm::scale

class RenderManager;

void        SetUpDog(int, RenderManager &);
const char *GetVertexShader();
const char *GetFragmentShader();

// This file is split into four parts:
// - Part 1: code to set up spheres and cylinders
// - Part 2: a "RenderManager" module
// - Part 3: main function
// - Part 4: SetUpDog and the shader programs -- things you modify
//
// It is intended that you will only need to modify code in Part 4.
// That said, you will need functions in Part 2 and should review
// those functions.
// Further, you are encouraged to look through the entire code base.
//


//
//
// PART 1: code to set up spheres and cylinders
//
//

class Triangle
{
  public:
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
};

std::vector<Triangle> SplitTriangle(std::vector<Triangle> &list)
{
    std::vector<Triangle> output(4*list.size());
    output.resize(4*list.size());
    for (unsigned int i = 0 ; i < list.size() ; i++)
    {
        Triangle t = list[i];
        glm::vec3 vmid1, vmid2, vmid3;
        vmid1 = (t.v0 + t.v1) / 2.0f;
        vmid2 = (t.v1 + t.v2) / 2.0f;
        vmid3 = (t.v0 + t.v2) / 2.0f;
        output[4*i+0].v0 = t.v0;
        output[4*i+0].v1 = vmid1;
        output[4*i+0].v2 = vmid3;
        output[4*i+1].v0 = t.v1;
        output[4*i+1].v1 = vmid2;
        output[4*i+1].v2 = vmid1;
        output[4*i+2].v0 = t.v2;
        output[4*i+2].v1 = vmid3;
        output[4*i+2].v2 = vmid2;
        output[4*i+3].v0 = vmid1;
        output[4*i+3].v1 = vmid2;
        output[4*i+3].v2 = vmid3;
    }
    return output;
}

void PushVertex(std::vector<float>& coords,
                const glm::vec3& v)
{
  coords.push_back(v.x);
  coords.push_back(v.y);
  coords.push_back(v.z);
}

//
// Sets up a cylinder that is the circle x^2+y^2=1 extruded from
// Z=0 to Z=1.
//
void GetCylinderData(std::vector<float>& coords, std::vector<float>& normals)
{
  int nfacets = 30;
  for (int i = 0 ; i < nfacets ; i++)
  {
    double angle = 3.14159*2.0*i/nfacets;
    double nextAngle = (i == nfacets-1 ? 0 : 3.14159*2.0*(i+1)/nfacets);
    glm::vec3 fnormal(0.0f, 0.0f, 1.0f);
    glm::vec3 bnormal(0.0f, 0.0f, -1.0f);
    glm::vec3 fv0(0.0f, 0.0f, 1.0f);
    glm::vec3 fv1(cos(angle), sin(angle), 1);
    glm::vec3 fv2(cos(nextAngle), sin(nextAngle), 1);
    glm::vec3 bv0(0.0f, 0.0f, 0.0f);
    glm::vec3 bv1(cos(angle), sin(angle), 0);
    glm::vec3 bv2(cos(nextAngle), sin(nextAngle), 0);
    // top and bottom circle vertices
    PushVertex(coords, fv0);
    PushVertex(normals, fnormal);
    PushVertex(coords, fv1);
    PushVertex(normals, fnormal);
    PushVertex(coords, fv2);
    PushVertex(normals, fnormal);
    PushVertex(coords, bv0);
    PushVertex(normals, bnormal);
    PushVertex(coords, bv1);
    PushVertex(normals, bnormal);
    PushVertex(coords, bv2);
    PushVertex(normals, bnormal);
    // curves surface vertices
    glm::vec3 v1normal(cos(angle), sin(angle), 0);
    glm::vec3 v2normal(cos(nextAngle), sin(nextAngle), 0);
    //fv1 fv2 bv1
    PushVertex(coords, fv1);
    PushVertex(normals, v1normal);
    PushVertex(coords, fv2);
    PushVertex(normals, v2normal);
    PushVertex(coords, bv1);
    PushVertex(normals, v1normal);
    //fv2 bv1 bv2
    PushVertex(coords, fv2);
    PushVertex(normals, v2normal);
    PushVertex(coords, bv1);
    PushVertex(normals, v1normal);
    PushVertex(coords, bv2);
    PushVertex(normals, v2normal);
  }
}

//
// Sets up a sphere with equation x^2+y^2+z^2=1
//
void
GetSphereData(std::vector<float>& coords, std::vector<float>& normals)
{
  int recursionLevel = 3;
  std::vector<Triangle> list;
  {
    Triangle t;
    t.v0 = glm::vec3(1.0f,0.0f,0.0f);
    t.v1 = glm::vec3(0.0f,1.0f,0.0f);
    t.v2 = glm::vec3(0.0f,0.0f,1.0f);
    list.push_back(t);
  }
  for (int r = 0 ; r < recursionLevel ; r++)
  {
      list = SplitTriangle(list);
  }

  for (int octant = 0 ; octant < 8 ; octant++)
  {
    glm::mat4 view(1.0f);
    float angle = 90.0f*(octant%4);
    if(angle != 0.0f)
      view = glm::rotate(view, glm::radians(angle), glm::vec3(1, 0, 0));
    if (octant >= 4)
      view = glm::rotate(view, glm::radians(180.0f), glm::vec3(0, 0, 1));
    for(int i = 0; i < list.size(); i++)
    {
      Triangle t = list[i];
      float mag_reci;
      glm::vec3 v0 = view*glm::vec4(t.v0, 1.0f);
      glm::vec3 v1 = view*glm::vec4(t.v1, 1.0f);
      glm::vec3 v2 = view*glm::vec4(t.v2, 1.0f);
      mag_reci = 1.0f / glm::length(v0);
      v0 = glm::vec3(v0.x * mag_reci, v0.y * mag_reci, v0.z * mag_reci);
      mag_reci = 1.0f / glm::length(v1);
      v1 = glm::vec3(v1.x * mag_reci, v1.y * mag_reci, v1.z * mag_reci);
      mag_reci = 1.0f / glm::length(v2);
      v2 = glm::vec3(v2.x * mag_reci, v2.y * mag_reci, v2.z * mag_reci);
      PushVertex(coords, v0);
      PushVertex(coords, v1);
      PushVertex(coords, v2);
      PushVertex(normals, v0);
      PushVertex(normals, v1);
      PushVertex(normals, v2);
    }
  }
}
//DO LATER FIXME
void GetCubeData(std::vector<float>& coords, std::vector<float>& normals)
{
  int nfacets = 6;
  for (int i = 0 ; i < nfacets ; i++)
  {
    glm::vec3 fnormal(0.0f, 0.0f, 1.0f);
    glm::vec3 bnormal(0.0f, 0.0f, -1.0f);
    glm::vec3 lnormal(-1.0f, 0.0f, 0.0f);
    glm::vec3 rnormal(1.0f, 0.0f, 1.0f);
    glm::vec3 tnormal(0.0f, 1.0f, 1.0f);
    glm::vec3 botnormal(0.0f, -1.0f, 1.0f);
   
    glm::vec3 fv0(0.0f, 0.0f, 0.0f);
    glm::vec3 fv1(1.0f, 0.0f, 0.0f);
    glm::vec3 fv2(1.0f, 1.0f, 0.0f);
    glm::vec3 fv3(0.0f, 1.0f, 0.0f);
    
    glm::vec3 bv0(0.0f, 0.0f, -1.0f);
    glm::vec3 bv1(1.0f, 0.0f, -1.0f);
    glm::vec3 bv2(1.0f, 1.0f, -1.0f);
    glm::vec3 bv3(0.0f, 1.0f, -1.0f);
    
    glm::vec3 lv0(0.0f, 0.0f, 0.0f);
    glm::vec3 lv1(0.0f, 1.0f, 0.0f);
    glm::vec3 lv2(0.0f, 1.0f, -1.0f);
    glm::vec3 lv3(0.0f, 0.0f, -1.0f);
    
    glm::vec3 rv0(1.0f, 0.0f, 0.0f);
    glm::vec3 rv1(1.0f, 1.0f, 0.0f);
    glm::vec3 rv2(1.0f, 1.0f, -1.0f);
    glm::vec3 rv3(1.0f, 0.0f, -1.0f);
    
    //idk if i need these
    glm::vec3 tv0(0.0f, 1.0f, 0.0f);
    glm::vec3 tv1(0.0f, 1.0f, -1.0f);
    glm::vec3 tv2(1.0f, 1.0f, -1.0f);
    glm::vec3 tv3(1.0f, 1.0f, 0.0f);
    
    glm::vec3 botv0(0.0f, 0.0f, 0.0f);
    glm::vec3 botv1(0.0f, 0.0f, -1.0f);
    glm::vec3 botv2(1.0f, 0.0f, -1.0f);
    glm::vec3 botv3(1.0f, 0.0f, 0.0f);
    // top and bottom circle vertices
    PushVertex(coords, fv0);
    PushVertex(normals, fnormal);
    PushVertex(coords, fv1);
    PushVertex(normals, fnormal);
    PushVertex(coords, fv2);
    PushVertex(normals, fnormal);
    PushVertex(coords, fv3);
    PushVertex(normals, fnormal);

    PushVertex(coords, bv0);
    PushVertex(normals, bnormal);
    PushVertex(coords, bv1);
    PushVertex(normals, bnormal);
    PushVertex(coords, bv2);
    PushVertex(normals, bnormal);
    PushVertex(coords, bv3);
    PushVertex(normals, bnormal);
    
    PushVertex(coords, lv0);
    PushVertex(normals, lnormal);
    PushVertex(coords, lv1);
    PushVertex(normals, lnormal);
    PushVertex(coords, lv2);
    PushVertex(normals, lnormal);
    PushVertex(coords, lv3);
    PushVertex(normals, lnormal);
    
    PushVertex(coords, rv0);
    PushVertex(normals, rnormal);
    PushVertex(coords, rv1);
    PushVertex(normals, rnormal);
    PushVertex(coords, rv2);
    PushVertex(normals, rnormal);
    PushVertex(coords, rv3);
    PushVertex(normals, rnormal);
    
    PushVertex(coords, tv0);
    PushVertex(normals, tnormal);
    PushVertex(coords, tv1);
    PushVertex(normals, tnormal);
    PushVertex(coords, tv2);
    PushVertex(normals, tnormal);
    PushVertex(coords, tv3);
    PushVertex(normals, tnormal);
    
    PushVertex(coords, botv0);
    PushVertex(normals, botnormal);
    PushVertex(coords, botv1);
    PushVertex(normals, botnormal);
    PushVertex(coords, botv2);
    PushVertex(normals, botnormal);
    PushVertex(coords, botv3);
    PushVertex(normals, botnormal);
  }
}
//
//
// PART 2: RenderManager module
//
//

void _print_shader_info_log(GLuint shader_index) {
  int max_length = 2048;
  int actual_length = 0;
  char shader_log[2048];
  glGetShaderInfoLog(shader_index, max_length, &actual_length, shader_log);
  printf("shader info log for GL index %u:\n%s\n", shader_index, shader_log);
}

class RenderManager
{
  public:
   enum ShapeType
   {
      SPHERE,
      CYLINDER,
      CUBE
   };

                 RenderManager();
   void          SetView(glm::vec3 &c, glm::vec3 &, glm::vec3 &);
   void          SetUpGeometry();
   void          SetColor(double r, double g, double b);
   void          Render(ShapeType, glm::mat4 model);
   GLFWwindow   *GetWindow() { return window; };

  private:
   glm::vec3 color;
   GLuint sphereVAO;
   GLuint sphereNumPrimitives;
   GLuint cylinderVAO;
   GLuint cylinderNumPrimitives;
   GLuint cubeVAO;
   GLuint cubeNumPrimitives;
   GLuint mvploc;
   GLuint colorloc;
   GLuint camloc;
   GLuint ldirloc;
   GLuint reflectionloc; //added
   glm::mat4 reflection;
   glm::mat4 projection;
   glm::mat4 view;
   GLuint shaderProgram;
   GLFWwindow *window;

   void SetUpWindowAndShaders();
   void MakeModelView(glm::mat4 &);
};

RenderManager::RenderManager()
{
  SetUpWindowAndShaders();
  SetUpGeometry();
  projection = glm::perspective(
        glm::radians(45.0f), (float)1000 / (float)1000,  5.0f, 100.0f);
  //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
  //view = glm::scale(view, glm::vec3(1.0f, -1.0f, 1.0f));
  //glm::mat4 uCameraView = projection * view;
  //add here?
  //reflection = glm::perspective
  // Get a handle for our MVP and color uniforms
  mvploc = glGetUniformLocation(shaderProgram, "MVP");
  colorloc = glGetUniformLocation(shaderProgram, "color");
  camloc = glGetUniformLocation(shaderProgram, "cameraloc");
  ldirloc = glGetUniformLocation(shaderProgram, "lightdir");
  //added
  reflectionloc = glGetUniformLocation(shaderProgram, "reflection");
  //
  glm::vec4 lightcoeff(0.3, 0.7, 2.8, 50.5); // Lighting coeff, Ka, Kd, Ks, alpha
  GLuint lcoeloc = glGetUniformLocation(shaderProgram, "lightcoeff");
  glUniform4fv(lcoeloc, 1, &lightcoeff[0]);
}

void
RenderManager::SetView(glm::vec3 &camera, glm::vec3 &origin, glm::vec3 &up)
{ 
   glm::mat4 v = glm::lookAt(
                       camera, // Camera in world space
                       origin, // looks at the origin
                       up      // and the head is up
                 );
   //added
   glm::mat4 r = {{1,0,0,0},{0,-1,0,0},{0,0,1,0},{0,0,0,1}}; //will "scale" by 1,-1,1 or smth
   reflection = r;
   view = v; 
   glUniform3fv(camloc, 1, &camera[0]);
   // Direction of light
   glm::vec3 lightdir = glm::normalize(camera - origin);   
   glUniform3fv(ldirloc, 1, &lightdir[0]);
};

void
RenderManager::SetUpWindowAndShaders()
{
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(700, 700, "CIS 441", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
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

//added
  
  const char* vertex_shader = GetVertexShader();
  const char* fragment_shader = GetFragmentShader();

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
  if (GL_TRUE != params) {
    fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
    _print_shader_info_log(fs);
    exit(EXIT_FAILURE);
  }

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, fs);
  glAttachShader(shaderProgram, vs);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);
}

void RenderManager::SetColor(double r, double g, double b)
{
   color[0] = r;
   color[1] = g;
   color[2] = b;
}

void RenderManager::MakeModelView(glm::mat4 &model)
{
   glm::mat4 modelview = projection * view * model;
   glm::mat4 reflec = projection* view * reflection;

   glUniformMatrix4fv(mvploc, 1, GL_FALSE, &modelview[0][0]);
   glUniformMatrix4fv(reflectionloc, 1, GL_FALSE, &reflec[0][0]);
}

void RenderManager::Render(ShapeType st, glm::mat4 model)
{
   int numPrimitives = 0;
   if (st == SPHERE)
   {
      glBindVertexArray(sphereVAO);
      numPrimitives = sphereNumPrimitives;
   }
   else if (st == CYLINDER)
   {
      glBindVertexArray(cylinderVAO);
      numPrimitives = cylinderNumPrimitives;
   }
   else if (st == CUBE)
   {
      glBindVertexArray(cubeVAO);
      numPrimitives = cubeNumPrimitives;
   }
   MakeModelView(model);
   glUniform3fv(colorloc, 1, &color[0]);
   glDrawElements(GL_TRIANGLES, numPrimitives, GL_UNSIGNED_INT, NULL);
}

void SetUpVBOs(std::vector<float> &coords, std::vector<float> &normals,
               GLuint &points_vbo, GLuint &normals_vbo, GLuint &index_vbo)
{
  int numIndices = coords.size()/3;
  std::vector<GLuint> indices(numIndices);
  for(int i = 0; i < numIndices; i++)
    indices[i] = i;

  points_vbo = 0;
  glGenBuffers(1, &points_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(float), coords.data(), GL_STATIC_DRAW);

  normals_vbo = 0;
  glGenBuffers(1, &normals_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

  index_vbo = 0;    // Index buffer object
  glGenBuffers(1, &index_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void RenderManager::SetUpGeometry()
{
  std::vector<float> sphereCoords;
  std::vector<float> sphereNormals;
  GetSphereData(sphereCoords, sphereNormals);
  sphereNumPrimitives = sphereCoords.size() / 3;
  GLuint sphere_points_vbo, sphere_normals_vbo, sphere_indices_vbo;
  SetUpVBOs(sphereCoords, sphereNormals, 
            sphere_points_vbo, sphere_normals_vbo, sphere_indices_vbo);

  std::vector<float> cylCoords;
  std::vector<float> cylNormals;
  GetCylinderData(cylCoords, cylNormals);
  cylinderNumPrimitives = cylCoords.size() / 3;
  GLuint cyl_points_vbo, cyl_normals_vbo, cyl_indices_vbo;
  SetUpVBOs(cylCoords, cylNormals, 
            cyl_points_vbo, cyl_normals_vbo, cyl_indices_vbo);

  /*std::vector<float> cubeCoords;
  std::vector<float> cubeNormals;
  GetCubeData(cubeCoords, cubeNormals);
  cubeNumPrimitives = cubeCoords.size() / 3;
  GLuint cube_points_vbo, cube_normals_vbo, cube_indices_vbo;
  SetUpVBOs(cubeCoords, cubeNormals, 
            cube_points_vbo, cube_normals_vbo, cube_indices_vbo);
*/
  GLuint vao[2]; //or 2 idk FIXME
  glGenVertexArrays(2, vao);

  glBindVertexArray(vao[SPHERE]);
  sphereVAO = vao[SPHERE];
  glBindBuffer(GL_ARRAY_BUFFER, sphere_points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, sphere_normals_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_indices_vbo);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(vao[CYLINDER]);
  cylinderVAO = vao[CYLINDER];
  glBindBuffer(GL_ARRAY_BUFFER, cyl_points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, cyl_normals_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cyl_indices_vbo);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  

  glBindVertexArray(vao[CUBE]);
  cubeVAO = vao[CUBE];
  glBindBuffer(GL_ARRAY_BUFFER, cube_points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, cube_normals_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_indices_vbo);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

}

//
// PART3: main function
//
int main() 
{
  RenderManager rm;
  GLFWwindow *window = rm.GetWindow();

  glm::vec3 origin(0, 0, 0);
  glm::vec3 up(0, 1, 0);

  int counter=0;
  while (!glfwWindowShouldClose(window)) 
  { 

    double angle=counter/300.0*2*M_PI;
    counter++;

    glm::vec3 camera(10*sin(angle), 0, 10*cos(angle));
    rm.SetView(camera, origin, up);
    // wipe the drawing surface clear
    glClearColor(0.3, 0.3, 0.8, 1.0);

    glEnable(GL_DEPTH_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glStencilMask(0x00);
    //normalShader.use();
    SetUpFloor(); //doesn't update stencil buffer with floor. but does with normal dog and walls

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    SetUpWalls();
    
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    //shadersSingleColor.use();
    SetUpDog(counter, rm);

    rm.SetView(camera, origin, up);
    SetUpDog(counter, rm);

    //reflected dog1
    double camera_abs = sqrt(camera[0] * camera[0] + camera[1] * camera[1] + camera[2] * camera[2]);
    double n_abs = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    double var = 2*(camera_abs*n_abs*cos(angle))*n;
    glm::vec3 camera_reflect(camera[0] - var, camera[1] - var, camera[2] - var);
    rm.SetView(camera_reflect, origin, up);
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    SetUpDog(counter, rm);

    //different reflection for dog2

    //dog3

    //dog4
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);

   /* double camera_abs = sqrt(camera[0] * camera[0] + camera[1] * camera[1] + camera[2] * camera[2]);
    double n_abs = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    double var = 2*(camera_abs*n_abs*cos(angle))*n;
    glm::vec3 camera_reflec(camera[0] - var, camera[1] - var, camera[2] - var);

    double reflect
    */
    /*glm::vec3 camera_reflectX(-10*sin(angle), 0, 10*cos(angle));
    rm.SetView(camera_reflectX, origin, up);
    SetUpDog(counter, rm);


    glm::vec3 camera_reflectZ(10*sin(angle), 0, -10*cos(angle));
    rm.SetView(camera_reflectZ, origin, up);
    SetUpDog(counter, rm);
    
    //????
    glm::vec3 camera_reflectXZ(-10*sin(angle), 0, -10*cos(angle));
    rm.SetView(camera_reflectXZ, origin, up);
    SetUpDog(counter, rm);*/
    //SetUpRoom(counter, rm);

    // update other events like input handling
    glfwPollEvents();
    // put the stuff we've been drawing onto the display
    glfwSwapBuffers(window);
  }

  // close GL context and any other GLFW resources
  glfwTerminate();
  return 0;
}


glm::mat4 RotateMatrix(float degrees, float x, float y, float z)
{
   glm::mat4 identity(1.0f);
   glm::mat4 rotation = glm::rotate(identity, 
                                    glm::radians(degrees), 
                                    glm::vec3(x, y, z));
   return rotation;
}

glm::mat4 ScaleMatrix(double x, double y, double z)
{
   glm::mat4 identity(1.0f);
   glm::vec3 scale(x, y, z);
   return glm::scale(identity, scale);
}

glm::mat4 TranslateMatrix(double x, double y, double z)
{
   glm::mat4 identity(1.0f);
   glm::vec3 translate(x, y, z);
   return glm::translate(identity, translate);
}
//added

glm::mat4 ReflectMatrix(double x, double y, double z)
{
   glm::mat4 identity(1.0f);
   glm::vec3 reflect(x, y, z);
   return glm::reflect(identity, reflect);
}


//Part 4 setup dog
//
void SetUpEyeball(glm::mat4 modelSoFar, RenderManager &rm)
{
   glm::mat4 scaled10 = ScaleMatrix(0.25, 0.25, 0.25);
   rm.SetColor(1,1,1); 
   rm.Render(RenderManager::SPHERE, modelSoFar*scaled10);

   glm::mat4 translate = TranslateMatrix(0, 0, 0.95);
   glm::mat4 scaled30 = ScaleMatrix(0.3, 0.3, 0.3);
   rm.SetColor(0,0,0); 
   rm.Render(RenderManager::SPHERE, modelSoFar*scaled10*translate*scaled30);
}

void SetUpLeg(glm::mat4 modelSoFar, RenderManager &rm)
{
//LEG
   glm::mat4 scale_leg = ScaleMatrix(0.2, 1.2, 0.2);
   glm::mat4 rotate_leg = RotateMatrix(90, 1, 0,0);
   rm.SetColor(0.889,0.44,0.115); 
   rm.Render(RenderManager::CYLINDER, modelSoFar*scale_leg*rotate_leg);
//FOOT
   glm::mat4 translate_foot = TranslateMatrix(0, -1.2, .2);
   glm::mat4 scale_foot = ScaleMatrix(0.25, 0.2, .5);
   rm.SetColor(1,1,1);
   rm.Render(RenderManager::SPHERE, modelSoFar*translate_foot*scale_foot);
}


void SetUpBody(int counter, glm::mat4 modelSoFar, RenderManager &rm)
{
   //BODY
   glm::mat4 translate_body = TranslateMatrix(0, -1.2, -.5);
   glm::mat4 scale_body = ScaleMatrix(.75, .9, 1.4);
   rm.SetColor(0.77,0.53,0.37); 
   rm.Render(RenderManager::SPHERE, modelSoFar*translate_body*scale_body);
   
   //LEGS					   //
   glm::mat4 backRightTranslate = TranslateMatrix(-.4,-.2,-.55);
   glm::mat4 backLeftTranslate = TranslateMatrix(.4,-.2, -.55);
   glm::mat4 frontRightTranslate = TranslateMatrix(-.4,-.2,.55);
   glm::mat4 frontLeftTranslate = TranslateMatrix(.4,-.2,.55);
   
   SetUpLeg(modelSoFar*translate_body*backRightTranslate, rm);
   SetUpLeg(modelSoFar*translate_body*backLeftTranslate, rm);
   SetUpLeg(modelSoFar*translate_body*frontRightTranslate, rm);
   SetUpLeg(modelSoFar*translate_body*frontLeftTranslate, rm);
//TAIL
   double radians = counter%360 * 3.14/180;
   double rotate = 11*sin(radians);
   
   glm::mat4 translate_tail = TranslateMatrix(0, 0, -4);
   glm::mat4 scale_tail = ScaleMatrix(.15, .15, .8);//.65
   glm::mat4 rotate_tail = RotateMatrix(rotate, 0, 1, 1);
   rm.SetColor(0.77,0.53,0.37); 
   glm::mat4 translate_tail2 = TranslateMatrix(0, 0, 0.4);
   rm.Render(RenderManager::SPHERE, rotate_tail*translate_tail2*translate_body*translate_tail*scale_body*scale_tail);


} 

void SetUpLips(glm::mat4 modelSoFar, RenderManager &rm){

   glm::mat4 scaled10 = ScaleMatrix(.8, 0.5, 0.25);
   rm.SetColor(0.889, 0.44, 0.115); 
   rm.Render(RenderManager::SPHERE, modelSoFar*scaled10);
}

void SetUpSnout(glm::mat4 modelSoFar, RenderManager &rm)
{
   //SNOUT
   glm::mat4 translate_snout = TranslateMatrix(0,-.25, .75);
   glm::mat4 scale25 = ScaleMatrix(.35, 0.35, 0.45);
   rm.SetColor(0.77,0.53,0.37); 
   rm.Render(RenderManager::CYLINDER, modelSoFar*translate_snout*scale25);
   //TOP LIP
   glm::mat4 translate_top = TranslateMatrix(0,.1, .5);
   SetUpLips(modelSoFar*translate_top*translate_snout*scale25,rm);
   //BOTTOM LIP
   glm::mat4 translate_bot = TranslateMatrix(0,-.1, .5);
   SetUpLips(modelSoFar*translate_bot*translate_snout*scale25, rm);

   //TONGUE    
   glm::mat4 translate_tongue  = TranslateMatrix(0,0, 1);
   glm::mat4 scale_tongue = ScaleMatrix(0.6, 0.1, .8);
   rm.SetColor(0.76, 0.39, 0.51); 
   rm.Render(RenderManager::SPHERE, modelSoFar*translate_snout*scale25*translate_tongue*scale_tongue);
   
   //NOSE
   glm::mat4 translate_nose = TranslateMatrix(0, .8, 1.25);
   glm::mat4 scale_nose = ScaleMatrix(0.2, 0.2, 0.2);
   rm.SetColor(0,0,0); 
   rm.Render(RenderManager::SPHERE, modelSoFar*translate_snout*scale25*translate_nose*scale_nose);

}

void SetUpEar(glm::mat4 modelSoFar, RenderManager &rm){

   //LEFT EAR
   glm::mat4 scale = ScaleMatrix(0.2, 1.3, .6);
   rm.SetColor(0.889,0.44,0.115); 
   rm.Render(RenderManager::SPHERE, modelSoFar*scale);
}
   void SetUpHead(int counter, glm::mat4 modelSoFar, RenderManager &rm)
{
   //HEAD
   glm::mat4 translate = TranslateMatrix(0, 0, 0);
   glm::mat4 scale_head = ScaleMatrix(.9,.9,1);
   //LEFT EYE
   glm::mat4 leftEyeTranslate = TranslateMatrix(-0.25, .25, .95);
   glm::mat4 rotateInFromLeft = RotateMatrix(15, 0, 1, 0);
   SetUpEyeball(modelSoFar*translate*scale_head*leftEyeTranslate*rotateInFromLeft, rm);
   //RIGHT EYE
   glm::mat4 rightEyeTranslate = TranslateMatrix(0.25, 0.25, .95);
   glm::mat4 rotateInFromRight = RotateMatrix(-15, 0, 1, 0);
   SetUpEyeball(modelSoFar*translate*scale_head*rightEyeTranslate*rotateInFromRight, rm);


   //RIGHT EAR
   glm::mat4 rightEarTranslate = TranslateMatrix(1, -0.3, 0);
   SetUpEar(modelSoFar*translate*scale_head*rightEarTranslate, rm);

   //LEFT EAR
   glm::mat4 leftEarTranslate = TranslateMatrix(-1, -0.3, 0);
   SetUpEar(modelSoFar*translate*scale_head*leftEarTranslate, rm);

   //SNOUT
   SetUpSnout(modelSoFar*translate*scale_head, rm);

   //HEAD
   rm.SetColor(0.77,0.53,0.37); 
   rm.Render(RenderManager::SPHERE, modelSoFar*translate);
   //NECK-will be rotated

   glm::mat4 translate_neck = TranslateMatrix(0,-.2, -1.75);
   glm::mat4 scale_neck = ScaleMatrix(0.35, 0.35, .75);
   glm::mat4 rotate_neck = RotateMatrix(-40, 1, 0,0);
   rm.SetColor(0.889, 0.44, 0.115); 
   rm.Render(RenderManager::CYLINDER, modelSoFar*scale_head*translate*rotate_neck * translate_neck*scale_neck);
   //COLLAR
   glm::mat4 translate_collar = TranslateMatrix(0, 0, .5);
   glm::mat4 scale_collar = ScaleMatrix(1.1, 1.1, .2);
   rm.SetColor(0,0,0);

   rm.Render(RenderManager::CYLINDER, modelSoFar*translate*scale_head*rotate_neck *translate_collar* translate_neck*scale_neck*scale_collar);
   //BODY
   SetUpBody(counter, modelSoFar*translate*translate_neck, rm);
}

void
SetUpDog(int counter, RenderManager &rm)
{//big cylinder
    glm::mat4 identity(1.0f);
    //glm::mat4 reflect{{1,0,0,0},{0,-1,0,0},{0,0,1,0},{0,0,0,1}}
    double var=(counter%100)/99.0;
    if ((counter/100 % 2) == 1)
       var=1-var;
    
    SetUpHead(counter, identity, rm);
    //SetUpHead(counter, reflect, rm);
}
    
void SetUpWall(glm::mat4 modelSoFar, RenderManager &rm)
{
//LEG
   float vertices[] = {0.0f, 0.5f, 0.5f,
   			0.0f, 0.5f, 0.5f,
			0.0f, 0.5f, 0.5f,
			0.0f, 0.2f, 0.5f};
   glm::mat4 scale_wall = ScaleMatrix(.25, 1.5, 3);
   rm.SetColor(0,0,0); 
   rm.Render(RenderManager::CUBE, modelSoFar*scale_wall);
}

void SetUpFloor(glm::mat4 modelSoFar, RenderManager &rm)
{
//LEG
   glm::mat4 translate_f = TranslateMatrix(0,0,0);
   glm::mat4 scale_f = ScaleMatrix(3, .2, 3);
   rm.SetColor(0,0,0); 
   rm.Render(RenderManager::CUBE, modelSoFar*translate_f, scale_f);
}

void SetUpRoom(int counter, RenderManager &rm)
{
    glm::mat4 identity(1.0f);
    double var=(counter%100)/99.0;
    if ((counter/100 % 2) == 1)
       var=1-var;
    
    //TODO maybe set up 8 walls, with 4 gaps in between. 
   //WALLS
   glm::mat4 translate_1 = TranslateMatrix(0, 0, 1.5);
   glm::mat4 translate_2 = TranslateMatrix(-1.5, 0, 0);
   glm::mat4 translate_3 = TranslateMatrix(0, 0,-1.5);
   glm::mat4 translate_4 = TranslateMatrix(1.5, 0, 0);

   glm::mat4 rotation = RotateMatrix(90,0,0,1);

   glm::mat4 reflection1 = ReflectMatrix(0, 1, 0);
   //reflect matrices for the four walls. include translation for 2 walls
   glm::mat4 reflection1 = ReflectMatrix();
   glm::mat4 reflection1 = ReflectMatrix();
   glm::mat4 reflection1 = ReflectMatrix();




   
   SetUpWall(translate_1*rotation, rm);
   SetUpWall(translate_2, rm);
   SetUpWall(translate_3*rotation, rm);
   SetUpWall(translate_4, rm);

   //FLOOR
   SetUpFloor(identity, rm);
}

const char *GetVertexShader()
{
   static char vertexShader[4096];
   strcpy(vertexShader, 
            
  "#version 400\n"
  "layout (location = 0) in vec3 vertex_position;\n"
  "layout (location = 1) in vec3 vertex_normal;\n"
  "uniform mat4 MVP;\n"
  "uniform mat4 reflection;\n" //define elsewhere
  "uniform vec3 cameraloc;  // Camera position \n"
  "uniform vec3 lightdir;   // Lighting direction \n"
  "uniform vec4 lightcoeff; // Lighting coeff, Ka, Kd, Ks, alpha\n"
  "out float shading_amount;\n"
  "void main() {\n"
  "  gl_Position = MVP*vec4(vertex_position, 1.0);\n"
  " vertex_normal = reflection*vec4(vertex_normal, 1.0);\n" //multiply normal by reflection
  //Phone shading. 
  "vec3 R;\n"
  "float LdotN = lightdir[0]*vertex_normal[0] + lightdir[1]*vertex_normal[1] + lightdir[2]*vertex_normal[2];\n"
  "float diffuse = max(0,LdotN);\n" 
  "R[0] = 2 * LdotN * vertex_normal[0] - lightdir[0];\n" 
  "R[1] = 2 * LdotN * vertex_normal[1] - lightdir[1];\n" 
  "R[2] = 2 * LdotN * vertex_normal[2] - lightdir[2];\n" 
  "vec3 V = vec3(cameraloc[0] - vertex_position[0], cameraloc[1] - vertex_position[1], cameraloc[2] - vertex_position[2]);\n"
  "float normalize = sqrt(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]);\n"
  "V[0] = V[0] / normalize;\n"
  "V[1] = V[1] / normalize;\n"
  "V[2] = V[2] / normalize;\n"
  "float specular = pow(max(0, R[0]*V[0] + R[1] * V[1] + R[2] * V[2]),lightcoeff[3]);\n"
  "shading_amount = lightcoeff[0] + lightcoeff[1]*diffuse + lightcoeff[2]*specular;\n"
  "};\n"
         );
   return vertexShader;
}

const char *GetFragmentShader()
{
   static char fragmentShader[4096];
   strcpy(fragmentShader, 
           "#version 400\n"
	   "in float shading_amount;\n"
           "uniform vec3 color;\n"
           "out vec4 frag_color;\n"
           "void main() {\n"
           "  frag_color = vec4(color, 1.0);\n"
	   "  frag_color = vec4(shading_amount * frag_color[0], shading_amount * frag_color[1], shading_amount * frag_color[2], shading_amount * frag_color[3]);\n"
	   "}\n"
         );
   return fragmentShader;
}
