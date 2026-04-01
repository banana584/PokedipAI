#version 430 core

layout(local_size_x = 10) in;

// buffer binding point 0
layout(std430, binding = 0) buffer Data
{
    float values[];
};

void main()
{
    uint id = gl_GlobalInvocationID.x;

    values[id] = values[id] * values[id];
}