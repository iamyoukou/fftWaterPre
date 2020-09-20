// pre-compute fresnel factors
// save into a 1D texture (i.e 1xN 2d texture)

#include "common.h"

int main(int argc, char const *argv[]) {
  int w, h;
  w = 512;
  h = 1;

  FIBITMAP *bitmap = FreeImage_Allocate(w, h, 24);
  RGBQUAD color;

  if (!bitmap) {
    std::cout << "FreeImage: Cannot allocate image." << '\n';
    exit(EXIT_FAILURE);
  }

  float incidence = 0.f;
  float step = (3.1415f * 0.5f) / float(w);

  for (size_t i = 0; i < w; i++) {
    for (size_t j = 0; j < h; j++) {
      float nSnell = 1.34f;
      float thetai = incidence;
      float sinthetat = sin(thetai) / nSnell;
      float thetat = asin(sinthetat);

      // [0, 1]
      float fresnel;
      if (thetai == 0.f) {
        fresnel = (nSnell - 1) / (nSnell + 1);
        fresnel = fresnel * fresnel;
      } else {
        float fs = sin(thetat - thetai) / sin(thetat + thetai);
        float ts = tan(thetat - thetai) / tan(thetat + thetai);
        fresnel = 0.5 * (fs * fs + ts * ts);
      }

      // to [0, 255]
      int iFres = int(fresnel * 255.f);

      color.rgbRed = iFres;
      color.rgbGreen = iFres;
      color.rgbBlue = iFres;
      FreeImage_SetPixelColor(bitmap, i, j, &color);

      incidence += step;
    }
  }

  if (FreeImage_Save(FIF_PNG, bitmap, "fresnel.png", 0))
    cout << "Fresnel map successfully saved!" << endl;

  return 0;
}
