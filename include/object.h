#ifndef OBJECT_H
#define OBJECT_H

// abstract base class object
// derive class: shader, vao, buffer object, program
// buffer object has a print function (custom file output location, print the buffer data) for debugging purpose 
// derive class shader: vertex_shader, frag_shader
// derive class buffer object: vbo, vao

// construct (enum ?)
// dtor (delete object)
// buffer object has a struct that store the attribute index, the stride value and other parameter when associate with an attribute index for a member function (updateBuffer) that allow modifying of component in the buffer with a custome time function inside the that member function parameter
// 2d access type, override operator[][] ? ([i][j] = [i * num_component + j])

// delete, detach (object from program)
// print

#endif OBJECT_H
