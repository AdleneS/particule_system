__kernel void sine_wave(__global float4* pos, unsigned int width, unsigned int height, float time)
{
    unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);

    float3 location = float3(0.0f,0.0f,0.0f);
    float mass = 10;
    float pmass = 3;
    float G = 0.4;
    float2 acc = float2(0,0);
    float2 vel = float2(0,0);

    // calculate uv coordinates
    //sqrt(x*x + y*y + z*z) MAG
    if (time < 1)
    {
        float u = x / (float)width;
        float v = y / (float)height;
        float w = 0;
        u = u*2.0f - 1.0f;
        v = v*2.0f - 1.0f;
        pos[y*width+x] = (float4)(u, v, w, 1.0f);
    }
    float3 force;
    force.x = location.x - pos[y*width+x].x;
    force.y = location.y - pos[y*width+x].y;
    force.z = 0;//location.z - pos[y*width+x].z;
    float distance = sqrt(force.x*force.x + force.y*force.y + force.z*force.z);
    if (distance < 100)
    {
        distance = 100;
    }
    if (distance > 1000)
    {
        distance = 1000;
    }

    force.x /= distance; 
    force.y /= distance; 
    //force.z /= distance; 
    float strenght = (G * mass * pmass) / (distance);
    force.x *= strenght;
    force.y *= strenght;
    //force.z *= strenght;
    float3 f;
    f.x = force.x / pmass;
    f.y = force.y / pmass;
    f.z = force.z / pmass;
    acc.x += f.x;
    acc.y += f.y;
    vel.x += acc.x;
    vel.y += acc.y;

    pos[y*width+x].x += (vel.x);
    pos[y*width+x].y += (vel.y);
    //pos[y*width+x].z += (force.z);
    // write output vertex
}
