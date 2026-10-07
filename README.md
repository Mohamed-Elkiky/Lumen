# Lumen

**A physically based path tracer written from scratch in modern C++.**

No game engine, no graphics API. Every pixel is computed by simulating how light bounces around a scene.

![Cornell box](renders/06-cornell-box.png)

*Cornell box, 600x600, 500 samples per pixel. Note the red/green colour bleeding on the white surfaces and the caustic under the glass sphere.*

---

## Features

- **Monte Carlo path tracing** with recursive bounces and Russian-roulette-free depth limiting
- **Materials**
  - Lambertian (matte)
  - Metal with adjustable roughness (fuzz)
  - Dielectric (glass, water, diamond) with Snell's law, total internal reflection and Schlick's approximation
  - Emissive area lights
- **Geometry:** spheres and quads (parallelograms)
- **Positionable camera** with configurable field of view, look-at and up vector
- **Antialiasing** via jittered supersampling
- **Gamma correction** and PNG output
- **Unit tests** with CTest

## Progress

Lumen is being built in public, one sprint at a time. Each render below was a milestone.

| | | |
|:---:|:---:|:---:|
| ![](renders/00-sky-gradient.png) | ![](renders/01-first-light.png) | ![](renders/02-diffuse.png) |
| Camera and rays | Ray-sphere intersection | Diffuse bounces and gamma |
| ![](renders/03-metal.png) | ![](renders/04-glass.png) | ![](renders/05-area-light.png) |
| Matte and metal | Glass with refraction | Area lights |

## Performance

| Scene | Resolution | Samples | Threads | Acceleration | Time |
|---|---|---|---|---|---|
| Cornell box | 600x600 | 500 | 1 | None | 135.4 s |

Next up: BVH acceleration and multithreading. Before/after numbers will go here.

## Build and run

Requires CMake 3.20+ and a C++17 compiler (MSVC, GCC or Clang).

**Linux / macOS**
```bash
cmake -B build
cmake --build build
./build/lumen
```

**Windows (PowerShell)**
```powershell
cmake -B build
cmake --build build --config Release
.\build\Release\lumen.exe
```

The render is written to `output.png`. Run the tests with:
```bash
ctest --test-dir build -C Release
```

## How it works

1. The **camera** fires rays through each pixel, many per pixel with random jitter (antialiasing).
2. Each ray is tested against every object to find the **closest hit**.
3. The hit surface's **material** decides what happens next: scatter randomly (matte), reflect (metal), refract or reflect (glass), or emit light (lights).
4. The ray **bounces** recursively, picking up colour from each surface, until it escapes the scene or hits the bounce limit.
5. All samples for a pixel are **averaged**, gamma corrected and written to PNG.

Because light paths are simulated physically, effects like soft shadows, colour bleeding and caustics appear naturally, without being programmed in.

## Project structure

```
lumen/
├── include/lumen/   Headers: vec3, ray, camera, materials, shapes
├── src/             Implementation and main
├── tests/           Unit tests
├── external/        Third-party single-header libraries (stb_image_write)
├── renders/         Milestone renders
└── docs/            Documentation and benchmarks
```

## Roadmap

- [x] **Sprint 1:** Foundations (vectors, rays, camera, spheres, PNG output)
- [x] **Sprint 2:** Materials and lighting (matte, metal, glass, area lights, Cornell box)
- [ ] **Sprint 3:** Performance (triangle meshes, `.obj` loading, BVH, multithreading, benchmarks)
- [ ] **Sprint 4:** Polish (JSON scene files, CLI, live preview window, CI)

## References

- Peter Shirley, Trevor David Black, Steve Hollasch: [*Ray Tracing in One Weekend* series](https://raytracing.github.io/)
- Matt Pharr, Wenzel Jakob, Greg Humphreys: [*Physically Based Rendering*](https://pbr-book.org/)
- [The Cornell Box](https://www.graphics.cornell.edu/online/box/), Cornell University Program of Computer Graphics
