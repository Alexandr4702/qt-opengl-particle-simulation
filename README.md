# Qt OpenGL Particle Simulation

An experimental GPU-accelerated simulation of gravitational bodies rendered with Qt 6 and OpenGL.

## Build

The project requires Qt 6 with the `Core`, `Gui`, `Widgets`, `OpenGL`, and `OpenGLWidgets` modules, OpenGL 4.3, CMake 3.16 or newer, and a compiler supporting C++11.

```sh
cmake -S . -B build
cmake --build build
```

## Camera controls

- Hold the left mouse button and drag to orbit around the scene.
- Use the mouse wheel to zoom.
- Use `W` and `S` to move along the camera axis.
- Use `A` and `D` to move sideways.
- Use `Space` and `Shift` to move up and down.

## Simulation controls

- `Start/Pause` toggles simulation playback.
- `Reset` restores the initial scene and pauses playback.
- `Time scale` controls simulation speed from `0.1x` to `10x` without changing the integration step.
- Select a particle count and press `Spawn` to create a new paused scene.
- Select `Gravity`, `Coulomb`, `Gravity + Coulomb`, `Harmonic attraction`,
  `Lennard-Jones`, or `Yukawa` to switch the interaction potential and reset
  the scene.
- Coulomb-based scenes contain equal numbers of red positive and blue negative charges.

When Qt is not available on the default CMake search path, specify its installation prefix:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt
cmake --build build
```

## Mathematical model

Each body has a mass $m$, position $\mathbf r$, linear velocity $\mathbf v$,
and optional charge $q$. Positions and velocities are stored in ping-pong
shader storage buffers and updated by an OpenGL compute shader. Potential
constants, the softening term, integration step, and compute substeps per
rendered frame are configured by public `World` fields.

The gravitational acceleration exerted by body $j$ on body $i$ is calculated with gravitational constant $G=1$:

$$
\mathbf a_i = \sum_{j\ne i}
G\frac{m_j(\mathbf r_j-\mathbf r_i)}
{(\lVert \mathbf r_j-\mathbf r_i\rVert^2+\varepsilon)^{3/2}},
\qquad \varepsilon=\texttt{World::softening}.
$$

The Coulomb mode assigns each body a charge $q_i\in\{-1,1\}$ and adds electric acceleration with constant $K=10$:

$$
\mathbf a_i = -\sum_{j\ne i}
K\frac{q_iq_j(\mathbf r_j-\mathbf r_i)}
{m_i(\lVert \mathbf r_j-\mathbf r_i\rVert^2+\varepsilon)^{3/2}}.
$$

`Gravity + Coulomb` sums both acceleration terms before integration.

The harmonic attraction mode uses the pair potential:

$$
U(r)=\frac{k r^2}{2}.
$$

Its acceleration pulls each body toward every other body:

$$
\mathbf a_i = \sum_{j\ne i}
k\frac{\mathbf r_j-\mathbf r_i}{m_i}.
$$

The Lennard-Jones mode uses:

$$
U(r)=4\epsilon\left[
\left(\frac{\sigma}{r}\right)^{12}
-\left(\frac{\sigma}{r}\right)^6
\right].
$$

It repels strongly at short range and attracts weakly past the equilibrium
distance:

$$
\mathbf a_i = -\sum_{j\ne i}
\frac{24\epsilon}{m_i r^2}
\left[
2\left(\frac{\sigma}{r}\right)^{12}
-\left(\frac{\sigma}{r}\right)^6
\right](\mathbf r_j-\mathbf r_i).
$$

The Yukawa mode uses a finite-range attractive potential:

$$
U(r)=-Gm_im_j\frac{e^{-r/\lambda}}{r}.
$$

The resulting acceleration decays exponentially with distance:

$$
\mathbf a_i = \sum_{j\ne i}
Gm_j e^{-r/\lambda}
\left(\frac{1}{r^3}+\frac{1}{\lambda r^2}\right)
(\mathbf r_j-\mathbf r_i).
$$

The softening term $\varepsilon$ prevents singular acceleration at very small distances. Velocity and position use semi-implicit Euler integration:

$$
\mathbf v_i^{n+1}=\mathbf v_i^n+\mathbf a_i\Delta t,
\qquad
\mathbf r_i^{n+1}=\mathbf r_i^n+\mathbf v_i^{n+1}\Delta t.
$$

The algorithm evaluates every body pair and therefore has $O(N^2)$ work. The updated position buffer is consumed directly by the vertex shader for instanced rendering, without copying simulation data back to the CPU. The camera uses a perspective projection with a $90^\circ$ field of view.
