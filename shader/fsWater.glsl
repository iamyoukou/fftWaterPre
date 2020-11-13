#version 330

in vec4 clipSpace;
in vec2 uv;
in vec3 worldPos;
in vec3 worldN;

uniform sampler2D texReflect;
uniform sampler2D texRefract;
uniform sampler2D texNormal, texHeight, texFresnel, texPerlinN;
uniform sampler2D texNormalDudv, texPerlinDudv;
uniform samplerCube texSkybox;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 eyePoint;
uniform vec2 dudvMove;

out vec4 fragColor;

const float alpha = 0.02;

float fresnelSchlick(float cosTheta, float F0) {
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
  vec2 ndc = vec2(clipSpace.x / clipSpace.w, clipSpace.y / clipSpace.w);
  ndc = ndc / 2.0 + 0.5;

  vec2 texCoordRefract = vec2(ndc.x, ndc.y);
  vec2 texCoordReflect = vec2(ndc.x, -ndc.y);

  // Distorting the reflection or refraction texture
  // produces a better effect.
  // This method is from the dudvWater project.
  // Note: the difference between only using texPerlinDudv
  // and blending texPerlinDudv with texNormalDudv is very small.
  vec2 distortion1 =
      mix(texture(texPerlinDudv, vec2(uv.x + dudvMove.x, uv.y)).rg * 2.0 - 1.0,
          texture(texNormalDudv, vec2(uv.x + dudvMove.x, uv.y)).rg * 2.0 - 1.0,
          0.5);
  distortion1 *= alpha;

  vec2 distortion2 =
      mix(texture(texPerlinDudv, vec2(-uv.x, uv.y + dudvMove.y)).rg * 2.0 - 1.0,
          texture(texNormalDudv, vec2(-uv.x, uv.y + dudvMove.y)).rg * 2.0 - 1.0,
          0.5);
  distortion2 *= alpha;

  vec2 distortion = distortion1 + distortion2;

  texCoordReflect += distortion;
  texCoordReflect.x = clamp(texCoordReflect.x, 0.001, 0.999);
  texCoordReflect.y = clamp(texCoordReflect.y, -0.999, -0.001);

  texCoordRefract += distortion;
  texCoordRefract = clamp(texCoordRefract, 0.001, 0.999);

  float distLP = length(lightPos - worldPos);
  float distEP = length(eyePoint - worldPos);
  // float sunAtten = 1.0 / (distLP * distLP);

  // farther: N more close to vec3(0, 1, 0)
  // this can significantly reduce artifacts due to the normal map at far place
  float warpFrac = min(exp(0.08 * distEP) - 1.0, 0.999);
  vec3 up = vec3(0, 1.0, 0);
  vec3 N = texture(texNormal, mod(uv + dudvMove, 1.0)).rgb * 2.0 - 1.0;

  // float pFrac = min(exp(0.03 * distLP) - 1.0, 1.0);
  float pFrac = 0.3;
  vec3 perlinN = texture(texPerlinN, (uv + dudvMove) / 16.0).rgb * 2.0 - 1.0;
  perlinN *= pFrac;

  // N = normalize(N + up + perlinN);
  N = normalize(mix(N + perlinN, up, warpFrac));
  // end warping normal

  vec3 L = normalize(lightPos - worldPos);
  // vec3 L = normalize(vec3(1, 1, 0));
  vec3 V = normalize(eyePoint - worldPos);
  vec3 H = normalize(L + V);
  vec3 R = reflect(-L, N);

  // two kinds of fresnel effect
  // vec2 fresUv = vec2(1.0 - max(dot(N, V), 0), 0.0);
  // vec2 fresUv = vec2(max(dot(N, R), 0), 0.0);
  // float fresnel = texture(texFresnel, fresUv).r;
  float fresnel = fresnelSchlick(max(dot(N, V), 0), 0.02);

  vec4 sunColor = vec4(1.0, 1.0, 1.0, 1.0);
  float sunFactor = 20.0;

  vec4 refl = texture(texReflect, texCoordReflect);
  vec4 refr = mix(texture(texRefract, texCoordRefract),
                  vec4(0.168, 0.267, 0.255, 0), 0.9);
  // vec4 refl = vec4(0.69, 0.84, 1, 0);
  // vec4 refr = vec4(0.168, 0.267, 0.255, 0);

  vec4 sun = sunColor * sunFactor * max(pow(dot(N, H), 600.0), 0.0);
  vec4 sky = texture(texSkybox, -R);

  // fragColor = mix(sky, refl, 0.5) * 0.7;
  fragColor = mix(refr, refl * 0.7, fresnel);
  fragColor += sun;

  // compensation for far place
  // can slightly reduce periodic artifacts at far place
  // float cFrac = min(exp(0.1 * distEP) - 1.0, 0.999);
  // vec4 compen = mix(sky, refl, 0.5);
  // fragColor = mix(fragColor, compen, cFrac);
}
