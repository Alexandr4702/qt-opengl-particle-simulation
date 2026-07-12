#version 430 core

layout(std430, binding = 0) readonly buffer PositionBuffer
{
    vec4 positions[];
};

layout(std430, binding = 4) readonly buffer ParticleProperties
{
    vec4 properties[];
};

uniform mat4 view_projection;

in vec4 a_position;

out vec4 color;
void main()
{
    vec4 body = positions[gl_InstanceID];
    vec3 world_position = body.xyz + a_position.xyz * body.w;
    gl_Position = view_projection * vec4(world_position, 1.0);
    float charge = properties[gl_InstanceID].y;
    if (charge > 0.0)
        color = vec4(1.0, 0.15, 0.1, 1.0);
    else if (charge < 0.0)
        color = vec4(0.1, 0.35, 1.0, 1.0);
    else
        color = vec4(1.0, 0.75, 0.2, 1.0);
}
