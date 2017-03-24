
typedef struct {
    float mass;
} RigidBodyProperties;

typedef struct {
    float3 position;
    float3 rotation;
    float3 velocity_linear;
    float3 velocity_angular;
} RigidBodyMotion;

typedef struct {
    float3 force;
    float3 torque;
} RigidBodyForces;



__kernel void SimpleMovement (
        __global RigidBodyProperties* rb_props,
        __global RigidBodyMotion* rb_motion,
        __global RigidBodyForces* rb_forces,
        __global float delta_time)
{
    const int i = get_global_id(0);

    rb_motion[i].velocity_linear += delta_time * rb_props[i].mass / rb_forces[i].force;

    // This needs to be changed to actual angular velocity formula
    rb_motion[i].velocity_angular += delta_time * rb_props[i].mass / rb_forces[i].torque;

    rb_motion[i].position = delta_time * rb_motion[i].velocity_linear;
    rb_motion[i].rotation = delta_time * rb_motion[i].velocity_angular;
}