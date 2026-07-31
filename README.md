# fluid-sim 

Working interactive 2D fluid sim — click and drag to inject velocity and
smoke into the field in real time.

Grid-based (Eulerian) incompressible fluid using the Stable Fluids
approach: a MAC-style staggered grid (velocities live on cell faces, not
centers), per-cell divergence solved and projected out each step to
enforce incompressibility, and semi-Lagrangian advection (backtracing
each point through the velocity field rather than tracking individual
particles). Same `Matrix<T>` grid and render pipeline as the base branch.

Space pauses, right-arrow steps one frame at a time for debugging.

**Working:** mouse-driven velocity/smoke injection, divergence solve,
advection, pause/step. **Rough edges:** fixed neighbor array size, some
dead/commented-out code, no runtime controls for sim parameters.
