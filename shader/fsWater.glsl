#version 330

in vec4 clipSpace;
in vec2 uv;
in vec3 worldPos;
in vec3 worldN;

uniform sampler2D texReflect;
uniform sampler2D texRefract;
uniform sampler2D texNormal, texHeight, texFresnel;
uniform samplerCube texSkybox;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 eyePoint;

out vec4 fragColor;

void main() {
  vec2 ndc = vec2(clipSpace.x / clipSpace.w, clipSpace.y / clipSpace.w);
  ndc = ndc / 2.0 + 0.5;

  vec2 texCoordRefract = vec2(ndc.x, ndc.y);
  vec2 texCoordReflect = vec2(ndc.x, -ndc.y);

  texCoordReflect.x = clamp(texCoordReflect.x, 0.001, 0.999);
  texCoordReflect.y = clamp(texCoordReflect.y, -0.999, -0.001);

  texCoordRefract = clamp(texCoordRefract, 0.001, 0.999);

  float dist = length(lightPos - worldPos);
  // float sunAtten = 1.0 / (dist * dist);

  // farther: N more close to vec3(0, 1, 0)
  // this can significantly reduce artifacts due to the normal map at far place
  float nFrac = exp(0.075 * dist) - 1.0;
  vec3 warpN = vec3(0, 1.0, 0) * nFrac;
  vec3 N = texture(texNormal, mod(uv, 1.0)).rgb * 2.0 - 1.0;
  N = normalize(N + warpN);

  vec3 L = normalize(lightPos - worldPos);
  vec3 V = normalize(eyePoint - worldPos);
  vec3 H = normalize(L + V);
  vec3 R = reflect(-L, N);

  vec2 fresUv = vec2(1.0 - max(dot(N, V), 0), 0.0);
  float fresnel = texture(texFresnel, fresUv).r;

  vec4 sunColor = vec4(1.0, 1.0, 1.0, 1.0);
  float sunFactor = 20.0;

  vec4 refl = texture(texReflect, texCoordReflect);
  vec4 refr = mix(texture(texRefract, texCoordRefract),
                  vec4(0.168, 0.267, 0.255, 0), 0.9);
  // vec4 refl = vec4(0.69, 0.84, 1, 0);
  // vec4 refr = vec4(0.168, 0.267, 0.255, 0);

  vec4 sun = sunColor * sunFactor * max(pow(dot(N, H), 300.0), 0.0);
  vec4 sky = texture(texSkybox, R);

  fragColor = mix(sky, refl, 0.5);
  fragColor = mix(refr, fragColor, fresnel);
  // float temp = min(exp(0.01 * dist) - 1.0, 0.999);
  // fragColor = mix(refr, fragColor, temp) * fresnel;
  fragColor += sun;

  // farther the ocean, flatter the appearance
  // float dist2 = max(length(eyePoint - worldPos), 0.01);
  // dist2 = exp(-0.1 * dist2);
  //
  // fragColor = vec4(fresnel);
}
