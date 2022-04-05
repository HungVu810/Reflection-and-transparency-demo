#include "../include/camera.h"

unsigned camera::win_width {1920};

unsigned camera::win_height {1080};

glm::mat4 camera::projection {1.0f};

glm::mat4 camera::view {1.0f};

float camera::yfov {60.0f};

glm::vec3 camera::position {0.0f};

double camera::prev_xpos {1920.0f / 2};

double camera::prev_ypos = {1080.0f / 2};

glm::vec3 camera::direction {0.0f, 0.0f, -1.0f};

glm::vec3 camera::up {0.0f, 1.0f, 0.0f};

glm::vec3 camera::side {1.0f, 0.0f, 0.0f};

float camera::pitch_angle {0.0f};

float camera::yaw_angle = 0.0f;

float camera::roll_angle = 0.0f;

bool camera::rotation_enabled = 0;

float camera::move_rate = 0.5f;

float camera::sensitivity = 1.0f;

bool camera::enable_debug_cam = 0;

bool camera::mouse_callback = 0;

glm::vec3 camera::prev_direction {0.0f, 0.0f, -1.0f};

glm::vec3 camera::prev_up {0.0f, 1.0f, 0.0f};

glm::vec3 camera::prev_side {1.0f, 0.0f, 0.0f};

float camera::prev_pitch_angle = 0.0f;

