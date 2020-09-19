#version 330

in vec4 clipSpace;
in vec2 uv;
in vec3 worldPos;
in vec3 worldN;

uniform sampler2D texReflect;
uniform sampler2D texRefract;
uniform sampler2D texNormal, texHeight;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 eyePoint;
uniform vec2 dudvMove;

out vec4 fragColor;

const float alpha = 0.02;
const float shineDamper = 30.0;
const float reflectivity = 3.0;

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

  vec3 N = texture(texNormal, mod(uv + dudvMove, 1.0)).rgb * 2.0 - 1.0;
  vec3 L = normalize(lightPosition - worldPos);
  vec3 V = normalize(eyePoint - worldPos);
  vec3 H = normalize(L + V);

  float refractiveFactor = max(dot(V, vec3(0, 1, 0)), 0.0);
  refractiveFactor = pow(refractiveFactor, 3.0);

  float specular = max(dot(H, N), 0.f);
  specular = pow(specular, shineDamper);
  vec3 specularHighlight = lightColor * specular * reflectivity;

  fragColor = mix(colorReflection, colorRefraction, refractiveFactor);
  fragColor = mix(fragColor,
                  vec4(0.0, 0.0, 0.1, 1.0) + vec4(specularHighlight, 0), 0.1);

  // fragColor = texture(texHeight, uv);
}
