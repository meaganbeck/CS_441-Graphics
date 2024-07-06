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

double get_rand()
{
    #define GET_RAND_NVALS 300
    static int n = 0;
    static double vals[GET_RAND_NVALS] =
    {
         0.2425782988977518, 0.01346957451359815, 0.3831388500440581,
         0.4146526904844924, 0.06776897286426693, 0.993126929734427,
         1.565273851885122e-05, 0.2630755762863325, 0.5112106443900665,
         0.9173002638468986, 0.06553447482433844, 0.4379183726561807,
         3.130547703770244e-05, 0.526151152572665, 0.02242128878013291,
         0.8346005276937971, 0.1310689496486769, 0.8758367453123614,
         4.695821555655367e-05, 0.7892267288589975, 0.5336319331701994,
         0.7519007915406957, 0.1966034244730153, 0.3137551179685421,
         6.261095407540489e-05, 0.05230230514532994, 0.04484257756026581,
         0.6692010553875943, 0.2621378992973538, 0.7516734906247228,
         7.826369259425611e-05, 0.3153778814316624, 0.5560532219503322,
         0.5865013192344929, 0.3276723741216922, 0.1895918632809035,
         9.391643111310733e-05, 0.5784534577179949, 0.06726386634039873,
         0.5038015830813914, 0.3932068489460306, 0.6275102359370842,
         0.0001095691696319586, 0.8415290340043274, 0.5784745107304652,
         0.42110184692829, 0.4587413237703691, 0.06542860859326488,
         0.0001252219081508098, 0.1046046102906599, 0.08968515512053163,
         0.3384021107751886, 0.5242757985947075, 0.5033469812494455,
         0.000140874646669661, 0.3676801865769924, 0.6008957995105981,
         0.2557023746220872, 0.589810273419046, 0.9412653539056263,
         0.0001565273851885122, 0.6307557628633248, 0.1121064439006645,
         0.1730026384689858, 0.6553447482433844, 0.379183726561807,
         0.0001721801237073634, 0.8938313391496573, 0.623317088290731,
         0.09030290231588432, 0.7208792230677229, 0.8171020992179877,
         0.0001878328622262147, 0.1569069154359898, 0.1345277326807975,
         0.007603166162782892, 0.7864136978920613, 0.2550204718741684,
         0.0002034856007450659, 0.4199824917223223, 0.6457383770708639,
         0.9249034300096814, 0.8519481727163998, 0.692938844530349,
         0.0002191383392639171, 0.6830580680086548, 0.1569490214609304,
         0.84220369385658, 0.9174826475407382, 0.1308572171865298,
         0.0002347910777827683, 0.9461336442949873, 0.6681596658509968,
         0.7595039577034786, 0.9830171223650767, 0.5687755898427105,
         0.0002504438163016195, 0.2092092205813197, 0.1793703102410633,
         0.6768042215503772, 0.04855159718941506, 0.00669396249889115,
         0.0002660965548204708, 0.4722847968676522, 0.6905809546311297,
         0.5941044853972758, 0.1140860720137535, 0.4446123351550719,
         0.000281749293339322, 0.7353603731539847, 0.2017915990211962,
         0.5114047492441743, 0.1796205468380919, 0.8825307078112525,
         0.0002974020318581732, 0.9984359494403172, 0.7130022434112626,
         0.4287050130910729, 0.2451550216624304, 0.3204490804674333,
         0.0003130547703770244, 0.2615115257266497, 0.2242128878013291,
         0.3460052769379715, 0.3106894964867688, 0.758367453123614,
         0.0003287075088958757, 0.5245871020129822, 0.7354235321913956,
         0.2633055407848701, 0.3762239713111072, 0.1962858257797946,
         0.0003443602474147269, 0.7876626782993147, 0.246634176581462,
         0.1806058046317686, 0.4417584461354457, 0.6342041984359753,
         0.0003600129859335781, 0.05073825458564715, 0.7578448209715285,
         0.09790606847866722, 0.5072929209597842, 0.07212257109215603,
         0.0003756657244524293, 0.3138138308719796, 0.2690554653615949,
         0.01520633232556578, 0.5728273957841226, 0.5100409437483368,
         0.0003913184629712805, 0.5768894071583122, 0.7802661097516613,
         0.9325065961724643, 0.6383618706084611, 0.9479593164045175,
         0.0004069712014901318, 0.8399649834446445, 0.2914767541417278,
         0.8498068600193629, 0.7038963454327994, 0.3858776890606981,
         0.000422623940008983, 0.1030405597309771, 0.8026873985317943,
         0.7671071238662615, 0.7694308202571379, 0.8237960617168788,
         0.0004382766785278342, 0.3661161360173096, 0.3138980429218607,
         0.6844073877131601, 0.8349652950814763, 0.2617144343730595,
         0.0004539294170466854, 0.629191712303642, 0.8251086873119272,
         0.6017076515600587, 0.9004997699058148, 0.6996328070292402,
         0.0004695821555655367, 0.8922672885899745, 0.3363193317019936,
         0.5190079154069572, 0.9660342447301532, 0.1375511796854209,
         0.0004852348940843879, 0.155342864876307, 0.84752997609206,
         0.4363081792538558, 0.03156871955449168, 0.5754695523416016,
         0.0005008876326032391, 0.4184184411626395, 0.3587406204821265,
         0.3536084431007544, 0.09710319437883012, 0.0133879249977823,
         0.0005165403711220903, 0.681494017448972, 0.869951264872193,
         0.270908706947653, 0.1626376692031686, 0.451306297653963,
         0.0005321931096409415, 0.9445695937353045, 0.3811619092622595,
         0.1882089707945515, 0.228172144027507, 0.8892246703101437,
         0.0005478458481597928, 0.207645170021637, 0.8923725536523259,
         0.1055092346414501, 0.2937066188518455, 0.3271430429663244,
         0.000563498586678644, 0.4707207463079695, 0.4035831980423923,
         0.02280949848834867, 0.3592410936761839, 0.7650614156225051,
         0.0005791513251974952, 0.7337963225943019, 0.9147938424324588,
         0.9401097623352472, 0.4247755685005223, 0.2029797882786858,
         0.0005948040637163464, 0.9968718988806344, 0.4260044868225253,
         0.8574100261821458, 0.4903100433248608, 0.6408981609348665,
         0.0006104568022351976, 0.2599474751669669, 0.9372151312125917,
         0.7747102900290443, 0.5558445181491992, 0.07881653359104718,
         0.0006261095407540489, 0.5230230514532994, 0.4484257756026582,
         0.692010553875943, 0.6213789929735376, 0.5167349062472278,
         0.0006417622792729001, 0.7860986277396319, 0.9596364199927246,
         0.6093108177228416, 0.6869134677978761, 0.9546532789034086,
         0.0006574150177917513, 0.04917420402596435, 0.4708470643827911,
         0.5266110815697401, 0.7524479426222145, 0.3925716515595893,
         0.0006730677563106025, 0.3122497803122968, 0.9820577087728576,
         0.4439113454166387, 0.817982417446553, 0.83049002421577,
         0.0006887204948294538, 0.5753253565986293, 0.493268353162924,
         0.3612116092635373, 0.8835168922708914, 0.2684083968719507,
         0.000704373233348305, 0.8384009328849618, 0.004478997552990447,
         0.2785118731104358, 0.9490513670952299, 0.7063267695281313,
         0.0007200259718671562, 0.1014765091712943, 0.5156896419430569,
         0.1958121369573344, 0.01458584191956829, 0.1442451421843121,
         0.0007356787103860074, 0.3645520854576268, 0.02690028633312335,
         0.113112400804233, 0.08012031674390674, 0.5821635148404928,
         0.0007513314489048586, 0.6276276617439592, 0.5381109307231898,
         0.03041266465113157, 0.1456547915682452, 0.02008188749667345,
         0.0007669841874237099, 0.8907032380302917, 0.04932157511325626,
         0.9477129284980301, 0.2111892663925836, 0.4580002601528542
    };
    double rv=0;
    if (n < GET_RAND_NVALS)
    {
        rv=vals[n];
        n++;
    }
    else
    {
        if (n % 6 == 0)
            srand(2*n/6);
        n++;
        rv = (double)rand()/RAND_MAX;
    }
    return rv;
}

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
      CYLINDER
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
   GLuint mvploc;
   GLuint colorloc;
   GLuint camloc;
   GLuint ldirloc;
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

  // Get a handle for our MVP and color uniforms
  mvploc = glGetUniformLocation(shaderProgram, "MVP");
  colorloc = glGetUniformLocation(shaderProgram, "color");
  camloc = glGetUniformLocation(shaderProgram, "cameraloc");
  ldirloc = glGetUniformLocation(shaderProgram, "lightdir");

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
   glUniformMatrix4fv(mvploc, 1, GL_FALSE, &modelview[0][0]);
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

  GLuint vao[2];
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
}


/*
// ============
// MATRIX TYPES
// ============
*/
// Rotation matrix
glm::mat4 RotateMatrix(float degrees, float x, float y, float z)
{
   glm::mat4 identity(1.0f);
   glm::mat4 rotation = glm::rotate(identity, 
                                    glm::radians(degrees), 
                                    glm::vec3(x, y, z));
   return rotation;
}
// Scale matrix
glm::mat4 ScaleMatrix(double x, double y, double z)
{
   glm::mat4 identity(1.0f);
   glm::vec3 scale(x, y, z);
   return glm::scale(identity, scale);
}
// Translation matrix
glm::mat4 TranslateMatrix(double x, double y, double z)
{
   glm::mat4 identity(1.0f);
   glm::vec3 translate(x, y, z);
   return glm::translate(identity, translate);
}


/*
// ============
//   SHADERS
// ============
*/
// Vertex shader
const char *GetVertexShader()
{
   static char vertexShader[4096];
   strcpy(vertexShader, 
           "#version 400\n"
           "layout (location = 0) in vec3 vertex_position;\n"
           "layout (location = 1) in vec3 vertex_normal;\n"
           "uniform mat4 MVP;\n"
           "uniform vec3 cameraloc;  // Camera position \n"
           "uniform vec3 lightdir;   // Lighting direction \n"
           "uniform vec4 lightcoeff; // Lighting coeff, Ka, Kd, Ks, alpha\n"
           "out float shading_amount;\n"
           "void main() {\n"
           "  gl_Position = MVP*vec4(vertex_position, 1.0);\n"
// SHADING CODE GOES HERE
  	"vec3 R;\n" //good
 	 "float LdotN = lightdir[0]*vertex_normal[0] + lightdir[1]*vertex_normal[1] + lightdir[2]*vertex_normal[2];\n" //good
 	 "float diffuse = max(0,LdotN);\n" //good
 	 "R[0] = 2 * LdotN * vertex_normal[0] - lightdir[0];\n" //good
 	 "R[1] = 2 * LdotN * vertex_normal[1] - lightdir[1];\n" //good
 	 "R[2] = 2 * LdotN * vertex_normal[2] - lightdir[2];\n" //good
 	//probably still normalize V
 	 "vec3 V = vec3(cameraloc[0] - vertex_position[0], cameraloc[1] - vertex_position[1], cameraloc[2] - vertex_position[2]);\n"
 	 "float normalize = sqrt(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]);\n"
 	 "V[0] = V[0] / normalize;\n"
 	 "V[1] = V[1] / normalize;\n"
 	 "V[2] = V[2] / normalize;\n"
 	 "float specular = pow(max(0, R[0]*V[0] + R[1] * V[1] + R[2] * V[2]),lightcoeff[3]);\n"
 	 "shading_amount = lightcoeff[0] + lightcoeff[1]*diffuse + lightcoeff[2]*specular;\n"

  	"}\n"
	);
   return vertexShader;
}
// Fragment shader
const char *GetFragmentShader()
{
   static char fragmentShader[1024];
   strcpy(fragmentShader, 
           "#version 400\n"
           "in float shading_amount;\n"
           "uniform vec3 color;\n"
           "out vec4 frag_color;\n"
           "void main() {\n"
           "  frag_color = vec4(color, 1.0);\n"
           "  frag_color[0] = min(1.0,frag_color[0]*shading_amount);\n"
           "  frag_color[1] = min(1.0,frag_color[1]*shading_amount);\n"
           "  frag_color[2] = min(1.0,frag_color[2]*shading_amount);\n"
           "}\n"
         );
   return fragmentShader;
}




// ========================================================================
//
// PART3: main functions
//
/*
// ============
// SPRITE CLASS
// ============
*/
const double boundingBox[] = {-5,5, -5,1, -5,5};
class Ball
{
  public:
    // ==================
    // DON'T CHANGE THESE
    // ==================
    Ball() 
    {
  	// No collisions yet
  	tSinceAccident = 0;
  	color[0] = 0;
  	color[1] = 1;
  	color[2] = 1;
  	radius = 0.3;
    }; 
    // Initializer
    void initialize(int id)
    {
	    /*
    	srand(id*2);
    	// Set position and dir to random values
    	pos[0] = (double)rand()/RAND_MAX*6.0-3.0;
  	pos[1] = (double)rand()/RAND_MAX*6.0-5.0;
  	pos[2] = (double)rand()/RAND_MAX*6.0-3.0;
  	dir[0] = (double)rand()/RAND_MAX*0.4-0.2;
  	dir[1] = (double)rand()/RAND_MAX*0.4-0.2;
  	dir[2] = (double)rand()/RAND_MAX*0.4-0.2;
	 */
        pos[0] = get_rand()*6.0-3.0;
        pos[1] = get_rand()*6.0-5.0;
        pos[2] = get_rand()*6.0-3.0;
        dir[0] = get_rand()*0.4-0.2;
        dir[1] = get_rand()*0.4-0.2;
        dir[2] = get_rand()*0.4-0.2;
    };
    
    void draw(RenderManager &rm)
    {
      glm::mat4 identity(1.0f);
      glm::mat4 translate = TranslateMatrix(pos[0], pos[1], pos[2]);
      glm::mat4 scale = ScaleMatrix(radius, radius, radius);
      rm.SetColor(color[0],color[1],color[2]);
      rm.Render(RenderManager::SPHERE,identity*translate*scale);
    } 
    // =================
    
    
    // ============
    // CHANGE THESE
    // ============
    void  UpdatePosition()
    {
       //STEP 3: Uncomment and run
       // Keep moving in the direction we should go
       
       pos[0] += dir[0]*0.1;
       pos[1] += dir[1]*0.1;
       pos[2] += dir[2]*0.1;
       
       // Don't escape the box
       if (pos[0] < boundingBox[0])
           dir[0] *= -1;
       if (pos[0] > boundingBox[1])
           dir[0] *= -1;
       if (pos[1] < boundingBox[2])
           dir[1] *= -1;
       if (pos[1] > boundingBox[3])
           dir[1] *= -1;
       if (pos[2] < boundingBox[4])
           dir[2] *= -1;
       if (pos[2] > boundingBox[5])
           dir[2] *= -1;  
       
    }
    
    double getDistance(Ball &s)
    {
	
	double distance = sqrt(((pos[0]-s.pos[0])*(pos[0] - s.pos[0])) + ((pos[1] - s.pos[1])*(pos[1] - s.pos[1])) + ((pos[2] - s.pos[2])*(pos[2] - s.pos[2])));
    	// Return the distance between these two balls
	return distance;
    }

    void LookForCollision(Ball &s)
    {
	   if (getDistance(s) <= radius + s.radius) {
		tSinceAccident = 0;
		CollisionDetected(s);
	   }
	   else{
		  tSinceAccident++;
	   }	  

       // Check if we overlap with another sphere
       // If we do, call CollisionDetected
    }
    
    void CollisionDetected(Ball &s)
    {
    	collideDir(pos, dir, s.pos, s.dir);
	radius -= 0.25 * radius;
	color[0] += 0.7 * (1-color[0]);
	color[1] -= 0.1 * color[1];
	color[2] -= 0.1 * color[2];	
        // update the ball's directions and maybe do other stuff
    }

    void collideDir(double pos1[3], double dir1[3], double pos2[3], double dir2[3])
    {
    	// Calculate the new collision speed and direction
    	// by editing the direction vector for each sphere
	
    	// 1. Find the normal vector between the two balls
	double x1[3] = {pos1[0] - pos2[0], pos1[1] - pos2[1], pos1[2] - pos2[2]};
    	double x_norm1 = sqrt(x1[0]*x1[0] + x1[1] * x1[1] + x1[2] * x1[2]);
    	double n[3] = {x1[0]/x_norm1, x1[1]/x_norm1, x1[2]/x_norm1};
	
    	// 2. Calculate the relative velocity

	double vRelative[3] = {dir1[0] - dir2[0], dir1[1] - dir2[1], dir1[2] - dir2[2]};
    	// 3. Calculate the relative along the normal
	double VdotN = (vRelative[0] * n[0] + vRelative[1] * n[1] + vRelative[2] * n[2]); 
	double vNormal[3] = {VdotN * n[0], VdotN * n[1], VdotN * n[2]}; 

    	// 4. Modify dir using the relative along the normal
	for (int i = 0; i < 3; i++){
		dir1[i] -= vNormal[i];
		dir2[i] += vNormal[i];
	}
	// Any collision between two spheres can be modeled as a collision between two circles on a 2d plane bisecting them both. 
    	// This is the intuition for deriving the above vector math. 
    	// However, all of the components (finding normals, dot products, normalizing, etc.) are things we have done before. 
    	// Feel free to write/copy your own helper methods to implement this clearly.	
    }
    
    
    // VARIABLES
    double pos[3];
    double dir[3]; 
    int tSinceAccident;
    double color[3];
    double radius;
};



/*
// ============
//     MAIN     (should only need to change numBalls)
// ============
*/

int main() 
{
  // Create the balls
  const int numBalls = 50;
  Ball *ballList = new Ball[numBalls];

  // Set this to -1 to run forever
  int TICK_LIMIT = 1000; //1000
  
  // Create the render manager, window, etc.
  RenderManager rm;
  GLFWwindow *window = rm.GetWindow();
  glm::vec3 origin(0, 0, 0);
  glm::vec3 up(0, 1, 0);
  
  // Initialize the balls (necessary to seed randomness)
  for (int i = 0; i < numBalls; i++) {ballList[i].initialize(i);}


  // MAIN LOOP
  int tick=0;
  while (!glfwWindowShouldClose(window)) 
  {
    if(TICK_LIMIT != 0)
    {
      // Set up the camera
      glm::vec3 camera(10, -10, -15);
      rm.SetView(camera, origin, up);
      // wipe the drawing surface clear
      glClearColor(0, 0, 0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
      // Each tick, check all the balls for collisions 
      for (int i = 0 ; i < numBalls ; i++)
      {
          // Look for a collision with every other ball
          for (int j = i+1 ; j < numBalls ; j++)
          {
              ballList[i].LookForCollision(ballList[j]);
          }
	   if (tick > 0 && tick % 50 == 0 && ballList[i].tSinceAccident > 50){	
		ballList[i].color[0] -= 0.05*ballList[i].color[0];
		ballList[i].color[1] += 0.35*(1-ballList[i].color[1]);
		ballList[i].color[2] += 0.35*(1-ballList[i].color[2]);
		ballList[i].radius += 0.01 *(1-ballList[i].radius);
	   }

          // Move the ball
          ballList[i].UpdatePosition();

          // Draw the ball
          ballList[i].draw(rm);
      }
      // put the stuff we've been drawing onto the display
      glfwSwapBuffers(window);
    
      // Make a "Tick" pass
      tick++;
      if(TICK_LIMIT > 0){TICK_LIMIT--;} 
    }

    // update other events like input handling
    glfwPollEvents();
  }

  // close GL context and any other GLFW resources
  glfwTerminate();
  return 0;
}
