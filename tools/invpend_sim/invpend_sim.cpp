#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

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
#include <box2d/b2_prismatic_joint.h>

using namespace std;
using namespace iplib;

constexpr int PORT_SERVER = 30000;
// constexpr int PORT_CLIENT = 30001;
constexpr int PORT_SIM = 30002;
net::Address ADDRESS_SERVER(127,0,0,1, PORT_SERVER);

struct dat_s {
    recursive_mutex rw_mtx;

    struct {
        float cart_x = 0; 
        float pend_d_theta = 0;
        float pend_theta = 0;
    } ground_truth;

    struct {
        float cart_x = 0;
    } control;

    void Reset() {
        lock_guard lock(rw_mtx);

        ground_truth.cart_x = 0;
        ground_truth.pend_d_theta = 0;
        ground_truth.pend_theta = 0;
        control.cart_x = 0;
    }
} dat;

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
    recursive_mutex mtx;

    struct {
        // since the b2World owns the bodies and joints, it will delete them when destructed
        unique_ptr<b2World> world;
        b2Body *cart;
        b2Body *pend;
        b2RevoluteJoint *revoluteJoint;
        b2PrismaticJoint *prismaticJoint;
        float prev_theta;
    } phys;

    struct {
        Duration accum;
        TimePoint prev;
    } time;

    pid_s cartPid;

    Simulation() : cartPid(0.0f, 12.0f, 1.0f, 0.0f) { }

    void Start(float initial_impulse = 0.1f) {
        lock_guard lock(mtx);

        // FIXME: Magic numbers describing the inverted pendulum...
        phys.world = make_unique<b2World>(b2Vec2(0.0f, G));

        // cart
        {
            b2BodyDef bd;
            bd.type = b2BodyType::b2_dynamicBody;
            bd.position.Set(0, 20.0f);
            bd.linearDamping = 30.0f;
            phys.cart = phys.world->CreateBody(&bd);

            b2PolygonShape shape;
            shape.SetAsBox(1.0f, 1.0f);

            b2FixtureDef fd;
            fd.shape = &shape;
            fd.density = 1.0f;

            phys.cart->CreateFixture(&fd);
        }

        // pendulum
        {
            b2BodyDef bd;
            bd.type = b2BodyType::b2_dynamicBody;
            bd.position.Set(0.0f, 20.9f);
            phys.pend = phys.world->CreateBody(&bd);

            b2CircleShape shape;
            shape.m_radius = 1.0f;

            b2FixtureDef fd;
            fd.shape = &shape;
            fd.density = 0.1f;

            phys.pend->CreateFixture(&fd);
        }

        {
            b2BodyDef bd;
            bd.type = b2BodyType::b2_kinematicBody;
            bd.position = phys.cart->GetPosition();
            auto body = phys.world->CreateBody(&bd);

            b2PrismaticJointDef pjd;
            b2Vec2 worldAxis(1.0f, 0.0f);
            pjd.Initialize(body, phys.cart, body->GetWorldCenter(), worldAxis);
            pjd.enableLimit = true;
            pjd.lowerTranslation = -15.0f;
            pjd.upperTranslation = 15.0f;
            pjd.enableMotor = false;
            // pjd.maxMotorForce = 2000.0f;
            pjd.collideConnected = false;

            phys.prismaticJoint = (b2PrismaticJoint*)phys.world->CreateJoint(&pjd);
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
            phys.prev_theta = phys.revoluteJoint->GetJointAngle();
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

        cout << "Initial Impulse: " << initial_impulse << endl;
        phys.cart->ApplyLinearImpulseToCenter(b2Vec2(initial_impulse, 0), true);
    }

    void Restart(float initial_impulse = 0.1f) {
        lock_guard lock(mtx);

        // phys.world is a unique_ptr that is replace in Start
        phys.cart = nullptr;
        phys.pend = nullptr;
        phys.prismaticJoint = nullptr;
        phys.revoluteJoint = nullptr;
        phys.prev_theta = 0;
        Start(initial_impulse);
    }

    void InitTime() {
        lock_guard lock(mtx);
        // total hack to reset accumulator to 0...
        time.accum -= time.accum;
        time.prev = Clock::now();
    }

    // Returns true if a physics step was executed.
    bool Step() {
        lock_guard lock(mtx);

        auto now = Clock::now();
        auto elapsed = Duration(now - time.prev);
        time.prev = now;

        time.accum += elapsed;
        if (time.accum >= timeStep) {
            time.accum -= timeStep;

            cartPid.setpoint = dat.control.cart_x;
            if (cartPid.setpoint < -15.0f) cartPid.setpoint = -15.0f;
            if (cartPid.setpoint > 15.0f) cartPid.setpoint = 15.0f;
            auto pos = phys.cart->GetPosition();
            float val = pid(cartPid, pos.x, timeStep.count());
            phys.cart->ApplyLinearImpulseToCenter(b2Vec2(val, 0), true);

            phys.world->Step(timeStep.count(), 5, 5);
            return true;
        }

        return false;
    }
};

void UpdateNet(dat_s *dat, Simulation *sim) {
    union {
        net::ipsrv_pos_t ipsrv_pos;
    } tx;

    union {
        net::sim_setup_t sim_setup;
        net::clisrv_cart_pos_t clisrv_cart_pos;
    } rx;
 
    net::Peer<net::SocketUDP> peer(IPLIB_MAX_PACKET_SIZE, IPLIB_PROTOCOL_ID, isLittleEndian());
    peer.GetConnection().SetTransmitAddress(ADDRESS_SERVER);
    peer.GetConnection().Open(PORT_SIM);
    peer.GetConnection().SetBlocking(true);

    while (true) {
        // Receive and process packets.
        while (peer.IsPacketReady()) {
            switch(peer.GetPacketType()) {
                case net::PacketType::CLISRV_CART_POS:
                    peer.Receive(rx.clisrv_cart_pos);
                    dat->control.cart_x = rx.clisrv_cart_pos.cart_x;
                    break;
                
                case net::PacketType::SIM_SETUP:
                    peer.Receive(rx.sim_setup);
                    sim->Restart(rx.sim_setup.initial_impulse);
            }
        }

        peer.GetConnection().SetTransmitAddress(ADDRESS_SERVER);
        tx.ipsrv_pos.pend_theta = dat->ground_truth.pend_theta;
        tx.ipsrv_pos.pend_d_theta = dat->ground_truth.pend_d_theta;
        tx.ipsrv_pos.cart_x = dat->ground_truth.cart_x;
        peer.Transmit(&tx.ipsrv_pos);

        this_thread::sleep_for(chrono::milliseconds(15));
    }
}

int main(int argc, char *argv[]) {
    string prog_name = argv[0];

    if (argc > 2) {
        cout << "Usage: " << prog_name << " <server ip>\n";
        throw new runtime_error("invalid number of arguments - "s + to_string(argc));
    }

    if (argc == 2)
        ADDRESS_SERVER = net::Address::fromString(argv[1]);
    
    cout << "Server IP: " << ADDRESS_SERVER << '\n';

    Simulation sim;
    sim.Start();
    sim.InitTime();

    thread net(UpdateNet, &dat, &sim);

    while (true) {
        if (sim.Step()) {
            lock_guard lock(dat.rw_mtx);

            dat.ground_truth.pend_theta = sim.phys.revoluteJoint->GetJointAngle();
            dat.ground_truth.pend_d_theta = dat.ground_truth.pend_theta - sim.phys.prev_theta;
            sim.phys.prev_theta = dat.ground_truth.pend_theta;
            dat.ground_truth.cart_x = sim.phys.cart->GetPosition().x;

            this_thread::sleep_for(chrono::milliseconds(5));
        }
    }

    return 0;
}