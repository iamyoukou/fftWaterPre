# Pre-computed FFT Water

Usually, GPU-based parallelization is necessary to perform a high resolution [FFT water](https://github.com/iamyoukou/fftWater) simulation.
However, by using pre-computed height maps, xz-displacement maps and normal maps,
even a high resolution simulation can be performed in realtime.
This is a trade-off between performance and memory use.

# Result

All results are performed in realtime.

## Demo 1

`1` height map, `1` normal map and uv offsets

FFT resolution: `512x512`

![result](./output.gif)

## Demo 2

The pre-computed FFT water in this demo has a `512x512` resolution.
It also has a height map, a x-displacement map, a z-displacement map and a normal map per frame.

As the period is `20s` and the time step is `0.02s`,
each type of maps has an amount of `20 / 0.02 = 1000`.

All the maps are stored in `png` format.
The total memory for storing those maps is about `800MB`.

The result is same as [FFT water](https://github.com/iamyoukou/fftWater).
However, it can be performed in realtime without parallelization.
The price here is `800MB` memory.

![result2](./output2.gif)
