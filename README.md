# Pre-computed FFT Water

Usually, GPU-based parallelization is necessary to perform a high resolution [FFT water](https://github.com/iamyoukou/fftWater) simulation.
However, by using pre-computed height maps, xz-displacement maps and normal maps,
even a high resolution simulation can be performed in realtime.
This is a trade-off between performance and memory use.

# Result

All results are performed in realtime.

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
The total memory for storing those maps is about `4.1GB`.

The result is same as [FFT water](https://github.com/iamyoukou/fftWater).
However, it can be performed in realtime without parallelization.
The price here is about `4.1GB` memory.

The periodic artifacts at the far place is reduced by blending an FFT-based method with a Perlin-noise-based method.

![result2](./output2.gif)

## Sample 3

Changing the way of computing the fresnel coefficient results in different effects.

![result3](./output3.gif)
