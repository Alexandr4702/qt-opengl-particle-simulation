# Qt OpenGL Particle Simulation

An experimental GPU-accelerated simulation of gravitational bodies rendered with Qt 6 and OpenGL.

## Build

The project requires Qt 6 with the `Core`, `Gui`, `Widgets`, `OpenGL`, and `OpenGLWidgets` modules, OpenGL 4.3, CMake 3.16 or newer, and a compiler supporting C++11.

```sh
cmake -S . -B build
cmake --build build
```

When Qt is not available on the default CMake search path, specify its installation prefix:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt
cmake --build build
```

## Mathematical model

Each body has a mass $m$, position $\mathbf r$, and linear velocity $\mathbf v$. Positions and velocities are stored in ping-pong shader storage buffers and updated by an OpenGL compute shader.

The gravitational force exerted by body $j$ on body $i$ is calculated with gravitational constant $G=1$:

$$
\mathbf a_i = \sum_{j\ne i}
\frac{m_j(\mathbf r_j-\mathbf r_i)}
{(\lVert \mathbf r_j-\mathbf r_i\rVert^2+\varepsilon)^{3/2}},
\qquad \varepsilon=0.01.
$$

The softening term $\varepsilon$ prevents singular acceleration at very small distances. Velocity and position use semi-implicit Euler integration:

$$
\mathbf v_i^{n+1}=\mathbf v_i^n+\mathbf a_i\Delta t,
\qquad
\mathbf r_i^{n+1}=\mathbf r_i^n+\mathbf v_i^{n+1}\Delta t.
$$

The algorithm evaluates every body pair and therefore has $O(N^2)$ work. The updated position buffer is consumed directly by the vertex shader for instanced rendering, without copying simulation data back to the CPU. The camera uses a perspective projection with a $90^\circ$ field of view.
