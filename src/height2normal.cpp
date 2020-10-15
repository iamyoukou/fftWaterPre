// Given a height map, compute a corresponding normal map
// https://stackoverflow.com/questions/5281261/generating-a-normal-map-from-a-height-map
#include "common.h"

int main(int argc, char const *argv[]) {

  FIBITMAP *bitmap = FreeImage_Load(FIF_PNG, "./image/noiseTexture.png");

  int w, h;
  w = FreeImage_GetWidth(bitmap);
  h = FreeImage_GetHeight(bitmap);

  FIBITMAP *normalMap = FreeImage_Allocate(w, h, 24);

  RGBQUAD colorRight, colorLeft, colorDown, colorUp, normalColor;

  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      // Due to the fact that
      // the map will be used for a tiled surface,
      // pixels on the border are processed in a tiling style
      FreeImage_GetPixelColor(bitmap, (i - 1 < 0) ? w - 1 : i, j, &colorLeft);
      FreeImage_GetPixelColor(bitmap, i, (j - 1 < 0) ? h - 1 : j, &colorUp);
      FreeImage_GetPixelColor(bitmap, (i + 1) % w, j, &colorRight);
      FreeImage_GetPixelColor(bitmap, i, (j + 1) % h, &colorDown);

      // to [0, 1]
      float hLeft, hRight, hUp, hDown;
      hLeft = float(colorLeft.rgbRed) / 255.0;
      hUp = float(colorUp.rgbRed) / 255.0;
      hRight = float(colorRight.rgbRed) / 255.0;
      hDown = float(colorDown.rgbRed) / 255.0;

      // to [-1, 1]
      hLeft = hLeft * 2.0 - 1.0;
      hUp = hUp * 2.0 - 1.0;
      hRight = hRight * 2.0 - 1.0;
      hDown = hDown * 2.0 - 1.0;

      // center finite difference
      float nx, ny, nz;
      float scale = 50.0;
      nx = (hRight - hLeft) * scale;
      ny = 1.0;
      nz = (hDown - hUp) * scale;
      vec3 n = normalize(vec3(nx, ny, nz));

      // to [0, 1]
      n = (n + vec3(1.0)) / 2.f;

      // to [0, 255]
      n *= 255.f;

      normalColor.rgbRed = int(n.x);
      normalColor.rgbGreen = int(n.y);
      normalColor.rgbBlue = int(n.z);

      FreeImage_SetPixelColor(normalMap, i, j, &normalColor);
    }
  }

  if (FreeImage_Save(FIF_PNG, normalMap, "./image/perlinNormal.png", 0))
    cout << "Image successfully saved!" << endl;

  return 0;
}
