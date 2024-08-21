#ifndef WEB_COLORS_H
#define WEB_COLORS_H

#include <Arduino.h>

struct Color {
    uint8_t r, g, b;
};

const struct WebColor {
    const char* name;
    Color color;
} webColors[] = {
    {"black", {0, 0, 0}},
    {"white", {255, 255, 255}},
    {"red", {255, 0, 0}},
    {"green", {0, 255, 0}},
    {"blue", {0, 0, 255}},
    {"yellow", {255, 255, 0}},
    {"cyan", {0, 255, 255}},
    {"magenta", {255, 0, 255}},
    {"gray", {128, 128, 128}},
    {"orange", {255, 165, 0}},
    {"purple", {128, 0, 128}},
    {"pink", {255, 192, 203}},
    // Add more colors as needed
};

Color getWebColor(const char* name) {
    for (auto &webColor : webColors) {
        if (strcasecmp(name, webColor.name) == 0) {
            return webColor.color;
        }
    }
    return {0, 0, 0}; // Default to black if not found
}

#endif // WEB_COLORS_H
