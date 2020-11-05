# Pre-computed FFT Water

Usually, GPU-based parallelization is necessary to perform a high resolution FFT water simulation.
However, by using pre-computed height maps, xz-displacement maps and normal maps,
even a high resolution simulation can be performed in realtime.
This is a trade-off between performance and memory use.

![result2](./demo.gif)

# Method

Pre-compute a displacement map and a height map [1] for each frame using [fftWater](https://github.com/iamyoukou/fftWater).
I use the following algorithm to compute and save those maps:

    // similar for xz-displacement and normal map
    height <- (originalValue + offset) / scale * 255;
    (R, G, B) <- (xDisplacement, height, zDisplacement);

![xyz-disp](./image/xyz-disp.png)

# Result

The result in the demo video is performed in realtime.

The pre-computed FFT water in the demo has a `512x512` resolution.
It uses a displacement map and a normal map for each frame.
Each type of maps has an amount of `5000`.
All the maps are stored in `png` format.
The total memory for storing those maps is about `3.2GB`.
