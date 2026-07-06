#version 430 core

layout(std430, binding = 0) readonly buffer PositionBuffer
{
    vec4 positions[];
};

uniform mat4 view_projection;

in vec4 a_position;

out vec4 color;
void main()
{
    vec4 body = positions[gl_InstanceID];
    vec3 world_position = body.xyz + a_position.xyz * body.w;
    gl_Position = view_projection * vec4(world_position, 1.0);
    color=vec4(clamp(a_position[0],0.1,1.0),clamp(a_position[1],0.1,1.0),clamp(a_position[2],0.1,1.0),1);
}
