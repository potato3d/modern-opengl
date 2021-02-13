# modern-opengl
Modern OpenGL 4.5 rendering techniques

Demonstrates several OpenGL concepts using modern API: direct state access (DSA), frame buffer objects (FBO), shader storage buffer objects (SSBO), texture sampler objects (TSO), uniform buffer objects (UBO), geometry instancing, indirect drawing, bindless textures, multidrawelementsindirect, compute shaders, CPU/GPU synchronization with fences, approaching zero driver overhead (AZDO), etc.

The code was meant to be used as a hands-on training material. The teacher directory contains the full implementation, while the student directory has // TODO comments where new code must be inserted to complete each example scene.

# Description

* Scene 0: framebuffer and textures
* Scene 1: vertex specification, vertex and fragment shaders, drawing commands
* Scene 2: per-vertex lighting
* Scene 3: per-fragment lighting
* Scene 4: 2D texturing
* Scene 5: individually draw many instances of same geometry
* Scene 6: geometry instancing to improve performance when drawing many instances of same geometrry
* Scene 6b: same as scene 6, but using a per-instance vertex attribute
* Scene 7: individually draw many instances with texture
* Scene 8: use bindless textures with geometry instances to improve performance
* Scene 9: individually draw many instances of different geometries
* Scene 10: multi draw indirect
* Scene 11: multi draw indirect applied to a 3D CAD model
* Scene 12: frustum culling on the CPU using explicit synchronization
* Scene 13: frustum culling on the GPU using compute shaders
