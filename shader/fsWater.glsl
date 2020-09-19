#version 330

in vec4 clipSpace;
in vec2 uv;
in vec3 worldPos;
in vec3 worldN;

uniform sampler2D texReflect;
uniform sampler2D texRefract;
uniform sampler2D texNormal, texHeight;
uniform samplerCube texSkybox;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 eyePoint;
uniform vec2 dudvMove;

out vec4 fragColor;

const float shineDamper = 30.0;

void main() {
  vec2 ndc = vec2(clipSpace.x / clipSpace.w, clipSpace.y / clipSpace.w);
  ndc = ndc / 2.0 + 0.5;

  vec2 texCoordRefract = vec2(ndc.x, ndc.y);
  vec2 texCoordReflect = vec2(ndc.x, -ndc.y);

  texCoordReflect.x = clamp(texCoordReflect.x, 0.001, 0.999);
  texCoordReflect.y = clamp(texCoordReflect.y, -0.999, -0.001);

  texCoordRefract = clamp(texCoordRefract, 0.001, 0.999);

  vec4 refl = texture(texReflect, texCoordReflect);
  // vec4 refr = texture(texRefract, texCoordRefract);
  vec4 refr = vec4(0.168, 0.267, 0.255, 0);

  vec3 N = texture(texNormal, mod(uv + dudvMove, 1.0)).rgb * 2.0 - 1.0;
  vec3 L = normalize(lightPosition - worldPos);
  vec3 V = normalize(eyePoint - worldPos);
  vec3 H = normalize(L + V);
  vec3 R = reflect(-L, N);

  float nSnell = 1.34;
  float fresnel;
  float costhetai = max(dot(N, V), 0.0);
  float thetai = acos(costhetai);
  float sinthetat = sin(thetai) / nSnell;
  float thetat = asin(sinthetat);

  if (thetai == 0.0) {
    fresnel = (nSnell - 1) / (nSnell + 1);
    fresnel = fresnel * fresnel;
  } else {
    float fs = sin(thetat - thetai) / sin(thetat + thetai);
    float ts = tan(thetat - thetai) / tan(thetat + thetai);
    fresnel = 0.5 * (fs * fs + ts * ts);
  }

  vec4 sky = texture(texSkybox, R);

  fragColor = mix(sky, refl, 0.25);
  fragColor = mix(refr, fragColor, fresnel);
}
