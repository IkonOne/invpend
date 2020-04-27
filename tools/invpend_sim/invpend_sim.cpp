#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include <iplib/Address.h>
#include <iplib/MathHelpers.h>
#include <iplib/PacketBuilder.h>
#include <iplib/Protocol.h>
#include <iplib/SocketUDP.h>
#include <iplib/Peer.h>

#include <box2d/b2_body.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_math.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>
#include <box2d/b2_friction_joint.h>
#include <box2d/b2_revolute_joint.h>

using namespace std;
using namespace iplib;

constexpr int PORT_SERVER = 30000;
// constexpr int PORT_CLIENT = 30001;
constexpr int PORT_SIM = 30002;
const net::Address ADDRESS_SERVER(127,0,0,1, PORT_SERVER);

/**********************
 * Simulation
 **********************/

using Clock = chrono::steady_clock;
using Duration = chrono::duration<float, chrono::seconds::period>;
using TimePoint = Clock::time_point;

static const float G = -9.81f;

// FIXME: time resolution representative of the Arduino???
// 120 fps
static const Duration timeStep(1.0f / 120.0f);

class Simulation {
  public:
    struct {
        // since the b2World owns the bodies and joints, it will delete them when destructed
        unique_ptr<b2World> world;
        b2Body *cart;
        b2Body *pend;
        b2RevoluteJoint *revoluteJoint;
    } phys;

    struct {
        Duration accum;
        TimePoint prev;
    } time;

    Simulation() {
        // FIXME: Magic numbers describing the inverted pendulum...
        phys.world = make_unique<b2World>(b2Vec2(0.0f, G));

        // cart
        {
            b2BodyDef bd;
            bd.type = b2BodyType::b2_kinematicBody;
            bd.position.Set(0, 20.0f);
            phys.cart = phys.world->CreateBody(&bd);

            b2PolygonShape shape;
            shape.SetAsBox(0.15f, 0.1f);

            b2FixtureDef fd;
            fd.shape = &shape;

            phys.cart->CreateFixture(&fd);
        }

        // pendulum
        {
            b2BodyDef bd;
            bd.type = b2BodyType::b2_dynamicBody;
            bd.position.Set(0.1f, 10.9f);
            phys.pend = phys.world->CreateBody(&bd);

            b2CircleShape shape;
            shape.m_radius = 0.1f;

            b2FixtureDef fd;
            fd.shape = &shape;
            fd.density = 1;

            phys.pend->CreateFixture(&fd);
        }

        // revolute joint - attatches pendulum to cart
        {
            b2RevoluteJointDef rjd;
            rjd.bodyA = phys.cart;
            rjd.localAnchorA.SetZero();
            rjd.bodyB = phys.pend;
            rjd.localAnchorB.Set(0.0f, -0.9f);
            rjd.enableLimit = false;
            rjd.enableMotor = false;

            phys.revoluteJoint = dynamic_cast<b2RevoluteJoint*>(phys.world->CreateJoint(&rjd));
        }

        // friction joint - the bearings I am using are trash salvaged from fidget spinners
        {
            b2FrictionJointDef fjd;
            fjd.bodyA = phys.cart;
            fjd.localAnchorA.SetZero();
            fjd.bodyB = phys.pend;
            fjd.localAnchorB.Set(0, -0.9f);
            fjd.maxForce = 0;
            fjd.maxTorque = 0.005f;

            phys.world->CreateJoint(&fjd);
        }


    }

    void InitTime() {
        // total hack to reset accumulator to 0...
        time.accum -= time.accum;
        time.prev = Clock::now();
    }

    // Returns true if a physics step was executed.
    bool Step() {
        auto now = Clock::now();
        auto elapsed = Duration(now - time.prev);
        time.prev = now;

        time.accum += elapsed;
        if (time.accum >= timeStep) {
            time.accum -= timeStep;
            phys.world->Step(timeStep.count(), 5, 5);
            return true;
        }

        return false;
    }
};

int main() {
    net::Peer<net::SocketUDP> peer(IPLIB_MAX_PACKET_SIZE, IPLIB_PROTOCOL_ID, isLittleEndian());
    peer.GetConnection().SetTransmitAddress(ADDRESS_SERVER);
    peer.GetConnection().Open(PORT_SIM);

    union {
        net::ipsrv_pos_t ipsrv_pos;
    } tx;

    union {
        net::ipsrv_pos_t ipsrv_pos;
    } rx;

    Simulation sim;
    sim.InitTime();

    // LOL threads.
    while (true) {
        // Receive and process packets.
        switch(peer.GetPacketType()) {
            case net::PacketType::IPSRV_POS:
                peer.Receive(rx.ipsrv_pos);
                break;
        }

        // if there is new info to send.
        if (sim.Step()) {
            tx.ipsrv_pos.pend_theta = sim.phys.revoluteJoint->GetJointAngle();
            peer.GetConnection().SetTransmitAddress(ADDRESS_SERVER);
            peer.Transmit(&tx.ipsrv_pos);
        }
    }

    peer.GetConnection().Close();

    return 0;
}