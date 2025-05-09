#ifndef SPKR_VERTS_H
#define SPKR_VERTS_H
#include <glm/glm.hpp>
#include <array>
#include <vector>

// Front face speakers.
const std::array<glm::vec3, 5> kfront_spkrs = {
    glm::vec3(-0.8f, 0.0f, -1.0f), // L
    glm::vec3( 0.8f, 0.0f, -1.0f), // R
    glm::vec3( 0.0f, 0.0f, -1.0f), // C
    glm::vec3(-0.8f, 0.8f, -1.0f), // Ltf
    glm::vec3( 0.8f, 0.8f, -1.0f), // Rtf
};

// Side wall speakers.
const std::array<glm::vec3, 4> kside_spkrs = {
    glm::vec3(-1.0f, 0.0f, 0.0f), // Ls
    glm::vec3( 1.0f, 0.0f, 0.0f), // Rs
    glm::vec3(-1.0f, 0.8f, 0.8f), // Lrs
    glm::vec3(1.0f, 0.8f, 0.8f),  // Rrs
};

// Rear face speakers.
const std::array<glm::vec3, 2> krear_spkrs = {
    glm::vec3(-0.8f, 0.8f, 1.0f), // Ltb
    glm::vec3(0.8f, 0.8f, 1.0f), // Rtb
};

inline std::vector<glm::vec3> spkr_from_coord(const glm::vec3* coords, const int num_verts, const glm::vec3 norm) {
    const float offset = 0.1f;
    std::vector<glm::vec3> ret;
    // From each center coord create 4 corner coords.
    for (int i = 0; i < num_verts; ++i) {
        // +Y
        ret.push_back(*(coords + i) + norm + glm::vec3(0.f, offset, 0.f));
        ret.push_back(*(coords + i) - norm + glm::vec3(0.f, offset, 0.f));
        // -Y
        ret.push_back(*(coords + i) + norm + glm::vec3(0.f, -offset, 0.f));
        ret.push_back(*(coords + i) - norm + glm::vec3(0.f, -offset, 0.f));
    }
    return ret;
}
#endif