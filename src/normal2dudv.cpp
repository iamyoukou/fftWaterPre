// Based on this paper:
// Truelsen, Rene. "Real-time shallow water simulation and environment mapping
// and clouds." (2007).
// Given a normal (nx, ny, nz) from a normal map, compute
// the dudv map as following:
// (du, dv) = (dnx/dx, dny/dy)
#include "common.h"

const char *getFileDir(string prefix, int fNum) {
  // zero padding
  // e.g. "output0001.bmp"
  string num = to_string(fNum);
  num = string(4 - num.length(), '0') + num;
  string fileDir = prefix + num + ".png";

  return fileDir.c_str();
}

void computeAndSave(string inDir, string outDir) {
  // normal map can be generated using a height map
  // online generator: https://cpetry.github.io/NormalMap-Online/
  FIBITMAP *bitmap = FreeImage_Load(FIF_PNG, inDir.c_str());

  int w, h;
  w = FreeImage_GetWidth(bitmap);
  h = FreeImage_GetHeight(bitmap);

  FIBITMAP *dudvMap = FreeImage_Allocate(w, h, 24);

  RGBQUAD colorRight, colorLeft, colorDown, colorUp, dudvColor;

  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      // Due to the fact that
      // the dudv map will be used for a tiled surface,
      // pixels on the border are processed in a tiling style
      FreeImage_GetPixelColor(bitmap, (i - 1 < 0) ? w - 1 : i - 1, j,
                              &colorLeft);
      FreeImage_GetPixelColor(bitmap, i, (j - 1 < 0) ? h - 1 : j - 1, &colorUp);
      FreeImage_GetPixelColor(bitmap, (i + 1) % w, j, &colorRight);
      FreeImage_GetPixelColor(bitmap, i, (j + 1) % h, &colorDown);

      // to [0, 1]
      float nxLeft, nxRight, nyUp, nyDown;
      nxLeft = float(colorLeft.rgbRed) / 255.0;
      nyUp = float(colorUp.rgbGreen) / 255.0;
      nxRight = float(colorRight.rgbRed) / 255.0;
      nyDown = float(colorDown.rgbGreen) / 255.0;

      // to [-1, 1]
      nxLeft = nxLeft * 2.0 - 1.0;
      nyUp = nyUp * 2.0 - 1.0;
      nxRight = nxRight * 2.0 - 1.0;
      nyDown = nyDown * 2.0 - 1.0;

      // center finite difference
      float dnx, dny;
      dnx = (nxRight - nxLeft) / 2.0;
      dny = (nyDown - nyUp) / 2.0;

      // to [0, 1]
      dnx = (dnx + 1.0) / 2.0;
      dny = (dny + 1.0) / 2.0;

      // to [0, 255]
      dnx *= 255.0;
      dny *= 255.0;

      dudvColor.rgbRed = int(dnx);
      dudvColor.rgbGreen = int(dny);
      dudvColor.rgbBlue = 0;

      FreeImage_SetPixelColor(dudvMap, i, j, &dudvColor);
    }
  }

  if (FreeImage_Save(FIF_PNG, dudvMap, outDir.c_str(), 0))
    cout << outDir << " successfully saved!" << endl;
}

int main(int argc, char const *argv[]) {
  // for (size_t i = 0; i < 5000; i++) {
  //   string inDir = getFileDir("./normals/normal", i);
  //   string outDir = getFileDir("./dudvs/dudv", i);
  //
  //   computeAndSave(inDir, outDir);
  // }

  computeAndSave("./image/normal.png", "./image/dudv.png");

  return 0;
}
