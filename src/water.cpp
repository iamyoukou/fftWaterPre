#include "common.h"
#include "water.h"

const float Water::WATER_Y = 0.f;
vec2 Water::dudvMove = vec2(0.f, 0.f);

Water::Water(const string fileName) {
  // import mesh by assimp
  scene = importer.ReadFile(fileName, aiProcess_CalcTangentSpace);

  initShader();
  initBuffers();
  initTexture();
  initUniform();
  initReflect();
  initRefract();
}

Water::~Water() {}

void Water::draw(mat4 M, mat4 V, mat4 P, vec3 eyePoint, vec3 lightColor,
                 vec3 lightPosition) {
  glUseProgram(shader);

  glUniform3fv(uniEyePoint, 1, value_ptr(eyePoint));

  glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
  glUniform3fv(uniLightPos, 1, value_ptr(lightPosition));

  glUniformMatrix4fv(uniM, 1, GL_FALSE, value_ptr(M));
  glUniformMatrix4fv(uniV, 1, GL_FALSE, value_ptr(V));
  glUniformMatrix4fv(uniP, 1, GL_FALSE, value_ptr(P));

  glUniform2fv(uniDudvMove, 1, value_ptr(dudvMove));

  for (size_t i = 0; i < scene->mNumMeshes; i++) {
    int numVtxs = scene->mMeshes[i]->mNumVertices;

    glBindVertexArray(vaos[i]);
    glDrawArrays(GL_PATCHES, 0, numVtxs);
  }
}

void Water::initBuffers() {
  // for each mesh
  for (size_t i = 0; i < scene->mNumMeshes; i++) {
    const aiMesh *mesh = scene->mMeshes[i];
    int numVtxs = mesh->mNumVertices;

    // numVertices * numComponents
    GLfloat *aVtxCoords = new GLfloat[numVtxs * 3];
    GLfloat *aUvs = new GLfloat[numVtxs * 2];
    GLfloat *aNormals = new GLfloat[numVtxs * 3];

    for (size_t j = 0; j < numVtxs; j++) {
      aiVector3D &vtx = mesh->mVertices[j];
      aVtxCoords[j * 3 + 0] = vtx.x;
      aVtxCoords[j * 3 + 1] = vtx.y;
      aVtxCoords[j * 3 + 2] = vtx.z;

      aiVector3D &nml = mesh->mNormals[j];
      aNormals[j * 3 + 0] = nml.x;
      aNormals[j * 3 + 1] = nml.y;
      aNormals[j * 3 + 2] = nml.z;

      aiVector3D &uv = mesh->mTextureCoords[0][j];
      aUvs[j * 2 + 0] = uv.x;
      aUvs[j * 2 + 1] = uv.y;
    }

    // vao
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    vaos.push_back(vao);

    // vbo for vertex
    GLuint vboVtx;
    glGenBuffers(1, &vboVtx);
    glBindBuffer(GL_ARRAY_BUFFER, vboVtx);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVtxs * 3, aVtxCoords,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    vboVtxs.push_back(vboVtx);

    // vbo for uv
    GLuint vboUv;
    glGenBuffers(1, &vboUv);
    glBindBuffer(GL_ARRAY_BUFFER, vboUv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVtxs * 2, aUvs,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    vboUvs.push_back(vboUv);

    // vbo for normal
    GLuint vboNml;
    glGenBuffers(1, &vboNml);
    glBindBuffer(GL_ARRAY_BUFFER, vboNml);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVtxs * 3, aNormals,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    vboNmls.push_back(vboNml);

    // delete client data
    delete[] aVtxCoords;
    delete[] aUvs;
    delete[] aNormals;
  } // end for each mesh
}

void Water::initShader() {
  shader = buildShader("./shader/vsWater.glsl", "./shader/fsWater.glsl",
                       "./shader/tcsQuad.glsl", "./shader/tesQuad.glsl");
}

void Water::initTexture() {
  setTexture(tboHeight, 11, "./image/height.png", FIF_PNG);
  setTexture(tboNormal, 12, "./image/normal.png", FIF_PNG);
  setTexture(tboFresnel, 13, "./image/fresnel.png", FIF_PNG);
}

void Water::initUniform() {
  glUseProgram(shader);

  // transform
  uniM = myGetUniformLocation(shader, "M");
  uniV = myGetUniformLocation(shader, "V");
  uniP = myGetUniformLocation(shader, "P");

  // glUniformMatrix4fv(uniM, 1, GL_FALSE, value_ptr(meshM));
  // glUniformMatrix4fv(uniV, 1, GL_FALSE, value_ptr(meshV));
  // glUniformMatrix4fv(uniP, 1, GL_FALSE, value_ptr(meshP));

  // texture
  uniTexReflect = myGetUniformLocation(shader, "texReflect");
  uniTexRefract = myGetUniformLocation(shader, "texRefract");
  uniTexHeight = myGetUniformLocation(shader, "texHeight");
  uniTexNormal = myGetUniformLocation(shader, "texNormal");
  uniTexSkybox = myGetUniformLocation(shader, "texSkybox");
  uniTexFresnel = myGetUniformLocation(shader, "texFresnel");

  glUniform1i(uniTexHeight, 11);
  glUniform1i(uniTexNormal, 12);
  glUniform1i(uniTexFresnel, 13);
  glUniform1i(uniTexReflect, 3);
  glUniform1i(uniTexRefract, 2);

  // light
  uniLightColor = myGetUniformLocation(shader, "lightColor");
  uniLightPos = myGetUniformLocation(shader, "lightPos");

  // glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
  // glUniform3fv(uniLightPos, 1, value_ptr(lightPosition));

  // other
  uniDudvMove = myGetUniformLocation(shader, "dudvMove");
  uniEyePoint = myGetUniformLocation(shader, "eyePoint");
}

void Water::initReflect() {
  // framebuffer object
  glGenFramebuffers(1, &fboReflect);
  glBindFramebuffer(GL_FRAMEBUFFER, fboReflect);

  glActiveTexture(GL_TEXTURE0 + 3);
  glGenTextures(1, &tboReflect);
  glBindTexture(GL_TEXTURE_2D, tboReflect);

  // On OSX, must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2, don't know why
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, 0,
               GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, tboReflect, 0);

  // The depth buffer
  // User-defined framebuffer must have a depth buffer to enable depth test
  glGenRenderbuffers(1, &rboDepthReflect);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepthReflect);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH * 2,
                        WINDOW_HEIGHT * 2);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepthReflect);

  glDrawBuffer(GL_COLOR_ATTACHMENT2);
}

void Water::initRefract() {
  // framebuffer object
  glGenFramebuffers(1, &fboRefract);
  glBindFramebuffer(GL_FRAMEBUFFER, fboRefract);

  glActiveTexture(GL_TEXTURE0 + 2);
  glGenTextures(1, &tboRefract);
  glBindTexture(GL_TEXTURE_2D, tboRefract);

  // On OSX, must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2, don't know why
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, 0,
               GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, tboRefract, 0);

  // The depth buffer
  // User-defined framebuffer must have a depth buffer to enable depth test
  glGenRenderbuffers(1, &rboDepthRefract);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepthRefract);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH * 2,
                        WINDOW_HEIGHT * 2);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepthRefract);

  glDrawBuffer(GL_COLOR_ATTACHMENT1);
}

void Water::setTexture(GLuint &tbo, int texUnit, const string texDir,
                       FREE_IMAGE_FORMAT imgType) {
  glActiveTexture(GL_TEXTURE0 + texUnit);

  FIBITMAP *texImage =
      FreeImage_ConvertTo24Bits(FreeImage_Load(imgType, texDir.c_str()));

  glGenTextures(1, &tbo);
  glBindTexture(GL_TEXTURE_2D, tbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(texImage),
               FreeImage_GetHeight(texImage), 0, GL_BGR, GL_UNSIGNED_BYTE,
               (void *)FreeImage_GetBits(texImage));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // release
  FreeImage_Unload(texImage);
}
