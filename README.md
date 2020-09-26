# Pre-computed FFT Water

Usually, GPU-based parallelization is necessary to perform a high resolution [FFT water](https://github.com/iamyoukou/fftWater) simulation.
However, by using pre-computed height maps, xz-displacement maps and normal maps,
even a high resolution simulation can be performed in realtime.
This is a trade-off between performance and memory use.

# Result

With height maps, normal maps and uv offsets:

![result](./output.gif)
