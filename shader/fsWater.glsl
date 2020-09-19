#version 330

in vec4 clipSpace;
in vec2 dudvCoord;
in vec3 toCamera;
in vec3 fromLightVector;
in vec3 worldPos;
in vec3 worldN;

uniform sampler2D texReflect;
uniform sampler2D texRefract;
uniform sampler2D texDudv;
uniform sampler2D texNormal;
uniform vec3 lightColor;

out vec4 fragColor;

const float alpha = 0.02;
const float shineDamper = 30.0;
const float reflectivity = 3.0;

// compute fragment normal from a normal map
// i.e. transform it from tangent space to world space
// the code is from https://github.com/JoeyDeVries/LearnOpenGL
// check the theory at https://learnopengl.com/Advanced-Lighting/Normal-Mapping
vec3 getNormalFromMap(vec2 distor) {
  vec3 tangentNormal = texture(texNormal, distor).xyz * 2.0 - 1.0;

  vec3 Q1 = dFdx(worldPos);
  vec3 Q2 = dFdy(worldPos);
  vec2 st1 = dFdx(dudvCoord);
  vec2 st2 = dFdy(dudvCoord);

  vec3 n = normalize(worldN);
  vec3 t = normalize(Q1 * st2.t - Q2 * st1.t);
  vec3 b = -normalize(cross(n, t));
  mat3 tbn = mat3(t, b, n);

  return normalize(tbn * tangentNormal);
}

void main() {
  vec2 ndc = vec2(clipSpace.x / clipSpace.w, clipSpace.y / clipSpace.w);
  ndc = ndc / 2.0 + 0.5;

  vec2 texCoordRefract = vec2(ndc.x, ndc.y);
  vec2 texCoordReflect = vec2(ndc.x, -ndc.y);

  texCoordReflect.x = clamp(texCoordReflect.x, 0.001, 0.999);
  texCoordReflect.y = clamp(texCoordReflect.y, -0.999, -0.001);

  texCoordRefract = clamp(texCoordRefract, 0.001, 0.999);

  vec4 colorReflection = texture(texReflect, texCoordReflect);
  vec4 colorRefraction = texture(texRefract, texCoordRefract);

  vec3 normal = vec3(0, 1, 0);

  vec3 viewVector = normalize(toCamera);
  float refractiveFactor = max(dot(viewVector, vec3(0, 1, 0)), 0.0);
  refractiveFactor = pow(refractiveFactor, 3.0);

  vec3 halfway = normalize(-fromLightVector + viewVector);
  float specular = max(dot(halfway, normal), 0.f);
  specular = pow(specular, shineDamper);
  vec3 specularHighlight = lightColor * specular * reflectivity;

  fragColor = mix(colorReflection, colorRefraction, refractiveFactor);
  fragColor = mix(fragColor,
                  vec4(0.0, 0.0, 0.1, 1.0) + vec4(specularHighlight, 0), 0.1);
}
