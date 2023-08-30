#include <GLFW/glfw3.h>
#include <string>

struct menuData {
    bool closed;
    std::string mode;
    std::string csv_data;
    std::string csv_coordinates;
    std::string map;
    int latitude_column;
    int longitude_column;
    float up;
    float down;
    float left;
    float right;
    float zoom;
};

/// TODO: maybe make this a class

menuData* menu();

void mainLoop(GLFWwindow* window);

void barChartMenu();

void barChartMapMenu();

void ok();