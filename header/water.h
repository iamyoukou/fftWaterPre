#ifndef WATER_H
#define WATER_H

#include "common.h"

class Water {
public:
  static const float WATER_Y;
  static vec2 dudvMove;

  // mesh data
  Assimp::Importer importer;
  const aiScene *scene;

  // opengl data
  vector<GLuint> vboVtxs, vboUvs, vboNmls;
  vector<GLuint> vaos;

  GLuint shader;
  GLuint tboDisp, tboNormal, tboFresnel, tboNormalDudv;
  GLuint tboPerlin, tboPerlinN, tboPerlinDudv;
  GLint uniM, uniV, uniP;
  GLint uniLightColor, uniLightPos;
  GLint uniTexReflect, uniTexRefract, uniTexDisp, uniTexNormal, uniTexSkybox;
  GLint uniTexFresnel, uniTexNormalDudv;
  GLint uniTexPerlin, uniTexPerlinN, uniTexPerlinDudv;
  GLint uniEyePoint;
  GLint uniDudvMove;
  GLuint tboRefract, tboReflect;
  GLuint fboRefract, fboReflect;
  GLuint rboDepthRefract, rboDepthReflect;

  Water(const string);
  ~Water();

  void draw(mat4, mat4, mat4, vec3, vec3, vec3, int);
  void initBuffers();
  void initShader();
  void initTexture();
  void initUniform();
  void initReflect();
  void initRefract();
  void setTexture(GLuint &, int, const string, FREE_IMAGE_FORMAT);
  const char *getFileDir(string, int);
};

#endif
