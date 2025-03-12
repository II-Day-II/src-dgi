#include <cstdlib> /* EXIT_SUCCESS */

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <wyre/core/ecs.h> 
#include <wyre/core/system/input.h>
#include <wyre/core/system/log.h>
#include <wyre/core/components/transform.h>
#include <wyre/core/components/camera.h>
#include <wyre/core/components/mesh.h>
#include <wyre/core/scene/triangle.h>

wyre::Entity cube {};

/* Demos */
#define LIMITS 1
#define DRAGON 2
#define MITSUBA 3 
#define CUBES 4
#define SPONZA 5
#define TEST 6

#define DEMO DRAGON

/**
 * @brief My game system.
 */
class MySystem : wyre::System {
    float time = 0.0f;

    /* Camera angles */
    float phi = 3.14f, theta = -0.15f;

    glm::ivec2 old_mouse_pos = glm::ivec2(0);

    public:
    MySystem() = default;
    ~MySystem() override = default;

    void update(wyre::WyreEngine& engine, const float dt) override {
        time += dt;
        
#if DEMO == CUBES
        wyre::Transform& cube_transform = engine.ecs.get_component<wyre::Transform>(cube);
        wyre::Mesh& cube_mesh = engine.ecs.get_component<wyre::Mesh>(cube);
        // cube_transform.rotation *= glm::angleAxis(glm::radians(1.0f), glm::vec3(0, 1, 0));
        // cube_transform.position.z = -1.0f + cos(time) * 0.5f;
        // cube_transform.position.y = 2.0f + sin(time) * 1.0f;
        // cube_mesh.material = glm::vec3(0.5f * cos(6.283f * (time * 0.2f + glm::vec3(0.0f, -0.33333f, 0.33333f))) + 0.5f) * 8.0f;
#endif

#if DEMO == TEST
        wyre::Transform& cube_transform = engine.ecs.get_component<wyre::Transform>(cube);
        wyre::Mesh& cube_mesh = engine.ecs.get_component<wyre::Mesh>(cube);

        cube_transform.position.z = -6.75f + sin(time);
#endif

        wyre::Transform& camera_transform = engine.ecs.get_component<wyre::Transform>(engine.active_camera);
        
        /* Rotate the camera */
        const float rotate_speed = dt * 1.0f;
        if (engine.input.is_key_held(wyre::KEY_LEFT)) phi -= rotate_speed;
        if (engine.input.is_key_held(wyre::KEY_RIGHT)) phi += rotate_speed;
        if (engine.input.is_key_held(wyre::KEY_UP)) theta += rotate_speed;
        if (engine.input.is_key_held(wyre::KEY_DOWN)) theta -= rotate_speed;

        /* Rotate camera using mouse movement if clicked */
        glm::ivec2 mouse_delta = glm::ivec2(0);
        glm::ivec2 new_mouse_pos = engine.input.get_mouse_pos();
        if (engine.input.is_mouse_held(wyre::MOUSE_LEFT)) {
            mouse_delta = new_mouse_pos - old_mouse_pos;
            theta -= mouse_delta.y * dt * 0.1f;
            phi += mouse_delta.x * dt * 0.1f;
        }
        old_mouse_pos = new_mouse_pos;

        /* Update camera rotation */
        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(1, 0, 0));
        rot *= glm::rotate(glm::mat4(1.0f), phi, glm::vec3(0, 1, 0));
        camera_transform.rotation = rot;

        /* Get forward and right vectors */
        const glm::vec3 forward = glm::vec4(0, 0, 1, 1) * rot;
        const glm::vec3 up = glm::vec3(0, 1, 0);
        const glm::vec3 right = glm::cross(forward, up);

        /* Move the camera */
        const float move_speed = dt * 2.0f;
        if (engine.input.is_key_held(wyre::KEY_W)) camera_transform.position += forward * move_speed;
        if (engine.input.is_key_held(wyre::KEY_A)) camera_transform.position -= right * move_speed;
        if (engine.input.is_key_held(wyre::KEY_S)) camera_transform.position -= forward * move_speed;
        if (engine.input.is_key_held(wyre::KEY_D)) camera_transform.position += right * move_speed;
        if (engine.input.is_key_held(wyre::KEY_SPACE)) camera_transform.position += up * move_speed;
        if (engine.input.is_key_held(wyre::KEY_LSHIFT)) camera_transform.position -= up * move_speed;
        if (engine.input.is_key_held(wyre::KEY_E)) camera_transform.position += up * move_speed;
        if (engine.input.is_key_held(wyre::KEY_Q)) camera_transform.position -= up * move_speed;

        // if (time > 5.0f) {
        //     engine.logger.info("five seconds passed.");
        //     time = 0.0f;
        // }
    };
};

wyre::Entity add_cube(wyre::WyreEngine& engine, glm::vec3 pos, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 mat = glm::vec3(1.0f), float yangle = 0.0f, float xangle = 0.0f, float zangle = 0.0f) {
    wyre::Entity mesh = engine.ecs.create_entity();
    wyre::Transform& mesh_transform = engine.ecs.add_component<wyre::Transform>(mesh);
    mesh_transform.position = pos;
    mesh_transform.rotation = glm::angleAxis(glm::radians(yangle), glm::vec3(0, 1, 0)) * glm::angleAxis(glm::radians(xangle), glm::vec3(1, 0, 0)) * glm::angleAxis(glm::radians(zangle), glm::vec3(0, 0, 1));
    mesh_transform.scale = scale;
    wyre::Mesh& mesh_mesh = engine.ecs.add_component<wyre::Mesh>(mesh, engine.files, "assets/models/box.glb", mat);
    return mesh;
}

wyre::Entity add_sphere(wyre::WyreEngine& engine, glm::vec3 pos, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 mat = glm::vec3(1.0f)) {
    wyre::Entity mesh = engine.ecs.create_entity();
    wyre::Transform& mesh_transform = engine.ecs.add_component<wyre::Transform>(mesh);
    mesh_transform.position = pos;
    mesh_transform.scale = scale;
    wyre::Mesh& mesh_mesh = engine.ecs.add_component<wyre::Mesh>(mesh, engine.files, "assets/models/sphere.glb", mat);
    return mesh;
}

void add_tree(wyre::WyreEngine& engine, glm::vec3 pos, glm::vec3 mat = glm::vec3(1.0f)) {
    wyre::Entity mesh = engine.ecs.create_entity();
    wyre::Transform& mesh_transform = engine.ecs.add_component<wyre::Transform>(mesh);
    mesh_transform.position = pos;
    mesh_transform.rotation = glm::angleAxis(glm::radians(180.0f), glm::vec3(1, 0, 0));
    mesh_transform.scale = glm::vec3(0.02f);
    wyre::Mesh& mesh_mesh = engine.ecs.add_component<wyre::Mesh>(mesh, engine.files, "assets/models/tree.glb");
}

void add_model(wyre::WyreEngine& engine, const char* path, glm::vec3 pos, glm::vec3 mat = glm::vec3(1.0f), size_t mesh_idx = 0u) {
    wyre::Entity mesh = engine.ecs.create_entity();
    wyre::Transform& mesh_transform = engine.ecs.add_component<wyre::Transform>(mesh);
    mesh_transform.position = pos;
    mesh_transform.rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));
    wyre::Mesh& mesh_mesh = engine.ecs.add_component<wyre::Mesh>(mesh, engine.files, path, mat, mesh_idx);
}

void add_dragon_big(wyre::WyreEngine& engine, glm::vec3 pos, const float angle = 90.0f) {
    wyre::Entity mesh = engine.ecs.create_entity();
    wyre::Transform& mesh_transform = engine.ecs.add_component<wyre::Transform>(mesh);
    mesh_transform.position = pos;
    mesh_transform.rotation = glm::angleAxis(glm::radians(angle), glm::vec3(0, 1, 0)) * glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));
    mesh_transform.scale = glm::vec3(4.0f);
    wyre::Mesh& mesh_mesh = engine.ecs.add_component<wyre::Mesh>(mesh, engine.files, "assets/models/dragon_800k.glb");
}

int main(int argc, char* argv[]) {
    wyre::WyreEngine engine(wyre::LogLevel::INFO);

    /* Init engine resources */
    if (engine.init() == false) return EXIT_FAILURE;

    /* Register my game system */
    engine.ecs.register_system<MySystem>();

    /* Create a camera */
    engine.active_camera = engine.ecs.create_entity();
    wyre::Transform& camera_transform = engine.ecs.add_component<wyre::Transform>(engine.active_camera);
    engine.ecs.add_component<wyre::Camera>(engine.active_camera, 50.0f);
    camera_transform.position = glm::vec3(0.0f, 2.0f, 4.0f);

    /* Create some cubes */
    const glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f) * 4.0f;
    const glm::vec3 red = glm::vec3(1.0f, 0.2f, 0.2f) * 6.0f;
    const glm::vec3 yellow = glm::vec3(1.0f, 0.7f, 0.1f) * 4.0f;
    const glm::vec3 green = glm::vec3(0.1f, 1.0f, 0.2f) * 4.0f;
    const glm::vec3 purple = glm::vec3(0.8f, 0.3f, 1.0f) * 3.0f;
    add_cube(engine, {0.0f, -0.5f, 0.0f}, {128.0f, 1.0f, 128.0f}, {-1.0f, -1.0f, -1.0f}, 0.0f); /* floor */
    
#if DEMO == TEST
    add_cube(engine, {0.0f, 2.5f, -2.0f}, {5.0f, 5.0f, 1.0f}, {-1.0f, -1.0f, -1.0f}, 0.0f);
    add_cube(engine, {0.0f, 2.5f, 2.0f}, {5.0f, 5.0f, 1.0f}, {-1.0f, -1.0f, -1.0f}, 0.0f);
    add_cube(engine, {-3.0f, 2.5f, 0.0f}, {1.0f, 5.0f, 5.0f}, {-1.0f, -1.0f, -1.0f}, 0.0f);
    // add_cube(engine, {8.0f, 2.5f, 0.0f}, {0.01f, 5.0f, 5.0f}, {-1.0f, -1.0f, -1.0f}, 0.0f);

    add_cube(engine, {0.0f, 2.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, red * 4.0f, 0.0f);

    add_cube(engine, {-6.6f, 1.75f, 3.5f}, {0.5f, 0.5f, 0.5f}, yellow * 4.0f, 0.0f);
    add_cube(engine, {-6.6f, 1.75f, -3.5f}, {0.5f, 0.5f, 0.5f}, yellow * 4.0f, 0.0f);
    add_cube(engine, {6.6f, 1.75f, 3.5f}, {0.5f, 0.5f, 0.5f}, yellow * 4.0f, 0.0f);
    add_cube(engine, {6.6f, 1.75f, -3.5f}, {0.5f, 0.5f, 0.5f}, yellow * 4.0f, 0.0f);

    add_cube(engine, {6.6f, 7.0f, -8.5f}, {5.0f, 5.0f, 1.0f}, {-1.0f, -1.0f, -1.0f}, 0.0f);
    cube = add_cube(engine, {6.6f, 7.0f, -6.5f}, {0.5f, 0.5f, 0.5f}, yellow * 4.0f);
    // add_dragon_big(engine, {0.0f, 1.1f, 0.0f}, 180.0f);
#endif

#if DEMO == SPONZA
    add_model(engine, "assets/models/sponza_66k.glb", {0.0f, 0.0f, 0.0f}, {-1.0f, -1.0f, -1.0f}, 0u);
    add_sphere(engine, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, red);

    add_cube(engine, {-6.6f, 2.0f, 3.5f}, {0.5f, 0.5f, 0.5f}, yellow * 4.0f, 0.0f);
    add_cube(engine, {-6.6f, 2.0f, -3.5f}, {0.5f, 0.5f, 0.5f}, yellow * 4.0f, 0.0f);
    add_cube(engine, {6.6f, 2.0f, 3.5f}, {0.5f, 0.5f, 0.5f}, yellow * 4.0f, 0.0f);
    add_cube(engine, {6.6f, 2.0f, -3.5f}, {0.5f, 0.5f, 0.5f}, yellow * 4.0f, 0.0f);

    add_sphere(engine, {6.6f, 7.0f, -7.0f}, {0.35f, 0.35f, 0.35f}, yellow * 4.0f);
    // add_sphere(engine, {6.6f, 7.0f, -6.5f}, {0.35f, 0.35f, 0.35f}, yellow * 4.0f);
    // add_dragon_big(engine, {-4.0f, 1.1f, 0.0f}, 180.0f);
#endif
    
#if DEMO == CUBES
    cube = add_cube(engine, {-1.25f, 0.2f, 0.2f}, {0.4f, 0.4f, 0.4f}, red, -10.0f);
    add_cube(engine, {0.0f, 0.75f, -1.5f}, {1.0f, 1.5f, 1.0f}, yellow, 0.0f);
    add_cube(engine, {-1.5f, 0.5f, -1.0f}, {0.2f, 1.0f, 1.0f}, {-1.0f, -1.0f, -1.0f}, -15.0f);
    add_cube(engine, {-0.5f, 0.25f, 0.0f}, {0.2f, 0.5f, 0.2f}, {-1.0f, -1.0f, -1.0f}, 0.0f);
#endif

#if DEMO == MITSUBA
    add_model(engine, "assets/models/mitsuba_knob.glb", {0.0f, 0.0f, 0.0f}, {-1.0f, -1.0f, -1.0f}, 0u);
    add_model(engine, "assets/models/mitsuba_knob.glb", {0.0f, 0.0f, 0.0f}, yellow, 1u);
    add_cube(engine, {-2.0f, 0.75f, 1.0f}, {1.0f, 1.5f, 1.0f}, red, 0.0f);
#endif

    // add_cube(engine, {-1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {5.0f, 0.0f, 0.0f}, 0.0f);
    // add_cube(engine, {0.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 5.0f, 0.0f}, 0.0f);
    // add_cube(engine, {1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 5.0f}, 0.0f);

#if DEMO == DRAGON
    /* Dragon */
    add_dragon_big(engine, {0.0f, 1.1f, 0.0f});
    add_cube(engine, {0.0f, 2.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, yellow, 0.0f);
    add_cube(engine, {-2.0f, 0.75f, 1.0f}, {1.0f, 1.5f, 1.0f}, red, 0.0f);
    add_cube(engine, {2.5f, 3.0f, -1.5f}, {1.5f, 1.5f, 0.1f}, green, -45.0f, 25.0f);
#endif

#if DEMO == LIMITS
    /* Limits DEMO */
    add_cube(engine, {-2.0f, 0.25f, 0.0f}, {0.5f, 0.5f, 0.5f}, red, 0.0f);
    add_cube(engine, {0.0f, 0.125f, 0.0f}, {0.25f, 0.25f, 0.25f}, {-1.0f, -1.0f, -1.0f}, 0.0f);
    add_cube(engine, {0.0f, 0.375f, 0.0f}, {0.25f, 0.25f, 0.25f}, yellow, 0.0f);
    add_cube(engine, {2.0f, 0.05f, 0.0f}, {0.1f, 0.1f, 0.1f}, purple, 0.0f);
    
    add_cube(engine, {-0.05f, 0.25f, -2.0f}, {0.1f, 0.5f, 0.5f}, {3.0f, 0.0f, 0.0f}, 0.0f);
    add_cube(engine, {0.05f, 0.25f, -2.0f}, {0.1f, 0.5f, 0.5f}, {0.0f, 3.0f, 0.0f}, 0.0f);
    
    add_cube(engine, {-1.0f, 0.25f, -0.5f}, {0.1f, 0.5f, 0.1f}, {-1.0f, -1.0f, -1.0f}, 0.0f);
    add_cube(engine, {-1.5f, 0.125f, 0.25f}, {0.1f, 0.25f, 0.1f}, {-1.0f, -1.0f, -1.0f}, 0.0f);
#endif

    // add_cube(engine, {-2.0f, 0.75f, 1.0f}, {1.0f, 1.5f, 1.0f}, red, 0.0f);
    // add_tree(engine, {0.0f, 1.5f, 0.0f}, {-1.0f, -1.0f, -1.0f});

    /* Rainbow */
    // add_cube(engine, {7.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.5f}, glm::vec3(1.0f, 0.0f, 0.0f) * 16.0f, 0.0f);
    // add_cube(engine, {7.0f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.5f}, glm::vec3(0.0f, 1.0f, 0.0f) * 16.0f, 0.0f);
    // add_cube(engine, {7.0f, 0.5f, 1.0f}, {1.0f, 1.0f, 0.5f}, glm::vec3(0.0f, 0.0f, 1.0f) * 16.0f, 0.0f);

    // glm::vec3 colors[3] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)};
    // for (int i = 0; i < 6; ++i) {
    //     add_cube(engine, {0.0f, 0.1f, 0.0f + i * 0.1f}, {1.0f, 1.0f, 0.1f}, colors[i % 3] * 3.0f, 0.0f);
    // }

    // for (int i = 0; i < 6; ++i) {
    //     add_cube(engine, {6.25f, 0.5f, 0.0f + i * 0.2f}, {0.1f, 1.0f, 0.1f}, glm::vec3(-1.0f, -1.0f, -1.0f), 0.0f);
    // }

    // add_cube(engine, {6.0f, 0.5f, 0.5f}, {0.1f, 1.0f, 0.1f}, glm::vec3(-1.0f, -1.0f, -1.0f), 0.0f);

    /* Run the engine, catch runtime errors */
    if (engine.run() == false) return EXIT_FAILURE;

    /* Cleanup engine resources */
    if (engine.destroy() == false) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
