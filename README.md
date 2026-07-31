# fluid-sim 

SDL2 scaffold: window/event loop, mouse tracking, and the render
pipeline shared by both branches.

Grid uses a flat-array-backed `Matrix<T>` instead of nested
pointers/vectors — the whole grid gets swept every frame, so contiguous
memory keeps that cache-friendly. Particle colors are written straight
into an off-screen surface, then blitted and scaled up in one call.

This branch has no real physics — just a placeholder color gradient to
prove out the rendering pipeline. See the `fluid` branch for the actual
sim.
