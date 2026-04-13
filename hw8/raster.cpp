#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <algorithm>
#include "common.h"

// Helper to calculate barycentric coordinates
void barycentric(vec2 p, vec2 a, vec2 b, vec2 c, double &alpha, double &beta, double &gamma) {
    vec2 v0 = b - a, v1 = c - a, v2 = p - a;
    double den = v0[0] * v1[1] - v1[0] * v0[1];
    beta = (v2[0] * v1[1] - v1[0] * v2[1]) / den;
    gamma = (v0[0] * v2[1] - v2[0] * v0[1]) / den;
    alpha = 1.0 - beta - gamma;
}

void Rasterize(Pixel* pixels, int width, int height, const std::vector<Triangle>& tris)
{
    std::vector<double> z_buffer(width * height, 1.0);

    for (const auto& tri : tris) {
        vec3 pts[3];
        vec4 clip_pts[3] = {tri.A, tri.B, tri.C};
        double w_inv[3];

        for (int i = 0; i < 3; ++i) {
            w_inv[i] = 1.0 / clip_pts[i][3];
            
            pts[i][0] = (clip_pts[i][0] * w_inv[i] + 1.0) * 0.5 * width;
            pts[i][1] = (clip_pts[i][1] * w_inv[i] + 1.0) * 0.5 * height;
            pts[i][2] = (clip_pts[i][2] * w_inv[i] + 1.0) * 0.5; // Depth [0,1]
        }

        int min_x = std::max(0, (int)std::floor(std::min({pts[0][0], pts[1][0], pts[2][0]})));
        int max_x = std::min(width - 1, (int)std::ceil(std::max({pts[0][0], pts[1][0], pts[2][0]})));
        int min_y = std::max(0, (int)std::floor(std::min({pts[0][1], pts[1][1], pts[2][1]})));
        int max_y = std::min(height - 1, (int)std::ceil(std::max({pts[0][1], pts[1][1], pts[2][1]})));

        vec3 Ca_w = tri.Ca * w_inv[0];
        vec3 Cb_w = tri.Cb * w_inv[1];
        vec3 Cc_w = tri.Cc * w_inv[2];

        for (int y = min_y; y <= max_y; ++y) {
            for (int x = min_x; x <= max_x; ++x) {
                double alpha, beta, gamma;
                barycentric(vec2(x + 0.5, y + 0.5), 
                            vec2(pts[0][0], pts[0][1]), 
                            vec2(pts[1][0], pts[1][1]), 
                            vec2(pts[2][0], pts[2][1]), 
                            alpha, beta, gamma);

                // Check if pixel is inside triangle
                if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                    double z = alpha * pts[0][2] + beta * pts[1][2] + gamma * pts[2][2];

                    if (z < z_buffer[y * width + x]) {
                        double interp_w_inv = alpha * w_inv[0] + beta * w_inv[1] + gamma * w_inv[2];
                        vec3 interp_color = (alpha * Ca_w + beta * Cb_w + gamma * Cc_w) / interp_w_inv;

                        z_buffer[y * width + x] = z;
                        set_pixel(pixels, width, height, x, y, interp_color);
                    }
                }
            }
        }
    }
}