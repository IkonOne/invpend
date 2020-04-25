#include <iostream>
#include <chrono>
#include <cmath>
#include <ratio>

#include <imgui.h>
#include <iplib/GuiWindow.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <box2d/b2_body.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_draw.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_math.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>

#include "draw.h"

using namespace std;
using namespace iplib;

gui::GuiWindow window(800, 600, "Pendulum Sim");

struct pid_s {
    float kp, ki, kd;
    float setpoint;
    float integral;
    float prev_error;
};

float pid(pid_s &pid, float measured, float dt) {
    // https://en.wikipedia.org/wiki/PID_controller#Pseudocode
    float error = pid.setpoint - measured;
    pid.integral = pid.integral + error * dt;
    float d = (error - pid.prev_error) / dt;
    pid.prev_error = error;

    return pid.kp * error + pid.ki * pid.integral + pid.kd * d;
}

int main() {
    DebugDraw debugDraw;
    b2Vec2 gravity(0.0f, -9.81f);
    b2World world(gravity);
    world.SetDebugDraw(&debugDraw);

    // FIXME: These pid values are bad.
    // They should be a really high p, low i and d is probably fine.
    // I have these so that I could see the controller was working.
    pid_s pendPid;
    pendPid.setpoint = 0;
    pendPid.kp = 0.1f;
    pendPid.ki = 1.0f;
    pendPid.kd = 0.01f;

    b2Body *cart = nullptr;
    {
        // create the cart
        b2BodyDef bd;
        bd.type = b2BodyType::b2_kinematicBody;
        bd.position.Set(0, 20.0f);
        cart = world.CreateBody(&bd);

        b2PolygonShape shape;
        shape.SetAsBox(0.15f, 0.1f);
        
        b2FixtureDef fd;
        fd.shape = &shape;

        cart->CreateFixture(&fd);
    }


    b2Body *pend = nullptr;
    {
        // Create pendulum
        b2BodyDef bd;
        bd.type = b2BodyType::b2_dynamicBody;
        bd.position.Set(0.1f, 20.9f);
        pend = world.CreateBody(&bd);

        pend->SetSleepingAllowed(false);

        b2CircleShape shape;
        shape.m_radius = 0.1f;

        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = 1;

        pend->CreateFixture(&fd);
    }

    b2RevoluteJoint *revoluteJoint = nullptr;
    {
        // create revolute joint of pendulum
        b2RevoluteJointDef rjd;
        rjd.bodyA = cart;
        rjd.localAnchorA.SetZero();
        rjd.bodyB = pend;
        rjd.localAnchorB.Set(0.0f, -0.9f); // 22 cm
        rjd.enableLimit = false;
        rjd.enableMotor = false;

        revoluteJoint = (b2RevoluteJoint*)world.CreateJoint(&rjd);

        // create friction joint since the bearings in the pendulum are trash
        b2FrictionJointDef fjd;
        fjd.bodyA = cart;
        fjd.localAnchorA.SetZero();
        fjd.bodyB = pend;
        fjd.localAnchorB.Set(0, -0.9f);
        fjd.maxForce = 0;
        fjd.maxTorque = 0.005f;

        world.CreateJoint(&fjd);
    }

    g_camera.m_zoom = 0.1f;

    window.Open();
    debugDraw.Create();
    debugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit);

    glfwSwapInterval(1);

    // run our physics sim at 60fps;
    static const float timeStep = 1.0f / 60.0f;
    chrono::duration<float, chrono::seconds::period> target(timeStep);
    chrono::duration<float, chrono::seconds::period> accum(0);
    chrono::duration<float, chrono::seconds::period> waitTime(2);
    auto prevTime = chrono::steady_clock::now();
    auto startTime = chrono::steady_clock::now();

    while (!window.GetShouldClose()) {
        // run our physics sim at 60fps;
        auto now = chrono::steady_clock::now();
        if (now < startTime + waitTime) {
            prevTime = now;
            pendPid.prev_error = revoluteJoint->GetJointAngle();
        }
        else {
            auto elapsed = chrono::duration<float, chrono::seconds::period>(now - prevTime);
            prevTime = now;

            accum += elapsed;
            while (accum >= target) {

                {   // pid control

                    // joint angle is measured in cartesian coords from y+
                    float theta = revoluteJoint->GetJointAngle();
                    while (theta > b2_pi) theta -= 2 * b2_pi;
                    while (theta < -b2_pi) theta += 2 * b2_pi;

                    // don't bother trying to control unless pendulum is above the cart
                    if (abs(theta) < b2_pi * 0.4f) {
                        float val = pid(pendPid, theta, timeStep);
                        auto pos = cart->GetPosition();
                        pos.x += val;
                        cart->SetTransform(pos, 0);
                    }
                }

                accum -= target;
                world.Step(timeStep, 5, 5);
            }
        }

        window.BeginRender();

        world.DebugDraw();
        debugDraw.Flush();

        // bool show_demo = true;
        // ImGui::ShowDemoWindow(&show_demo);

        window.EndRender();
    }

    debugDraw.Destroy();
    window.Close();
    return 0;
}