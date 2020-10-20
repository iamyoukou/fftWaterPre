# Pre-computed FFT Water

Usually, GPU-based parallelization is necessary to perform a high resolution FFT water simulation.
However, by using pre-computed height maps, xz-displacement maps and normal maps,
even a high resolution simulation can be performed in realtime.
This is a trade-off between performance and memory use.

# Method

Pre-compute a displacement map and a height map [1] for each frame using [fftWater](https://github.com/iamyoukou/fftWater).
I use the following algorithm to compute and save those maps:

    // similar for xz-displacement and normal map
    height <- (originalValue + offset) / scale * 255;
    (R, G, B) <- (xDisplacement, height, zDisplacement);

![xyz-disp](./image/xyz-disp.png)

# Result

All results are performed in realtime.

The periodic artifacts at the far place is reduced by blending the FFT-based result with a Perlin noise map [2].

## Sample 1

`1` height map, `1` normal map and uv offsets

FFT resolution: `512x512`

This method is suitable when the memory is not enough.

![result](./output.gif)

## Sample 2

The pre-computed FFT water in this demo has a `512x512` resolution.
It also has a height map, a x-displacement map, a z-displacement map and a normal map per frame.

As the period is `50s` and the time step is `0.01s`,
each type of maps has an amount of `50 / 0.01 = 5000`.
All the maps are stored in `png` format.
The total memory for storing those maps is about `3.2GB`.

The result is same as [fftWater](https://github.com/iamyoukou/fftWater).
However, it can be performed in realtime without parallelization.

![result2](./output2.gif)

## Sample 3

Changing the way of computing the fresnel coefficient results in different effects.

The refraction effect is from [dudvWater](https://github.com/iamyoukou/dudvWater).

![result3](./output3.gif)

# Reference

[1] An introduction to Realistic Ocean Rendering through FFT - Fabio Suriano - Codemotion Rome 2017 ([slide](https://www.slideshare.net/Codemotion/an-introduction-to-realistic-ocean-rendering-through-fft-fabio-suriano-codemotion-rome-2017))

[2] Ocean Surface Simulation ([slide](http://www-evasion.imag.fr/~Fabrice.Neyret/images/fluids-nuages/waves/Jonathan/articlesCG/NV_OceanCS_Slides.pdf))
