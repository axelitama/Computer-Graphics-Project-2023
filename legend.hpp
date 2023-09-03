#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Legend {
public:
    static Legend & getInstance(GLFWwindow* parentWindow);
    static Legend & getInstance();
    void mainLoop();
    void setLegend(std::vector<std::string> names, std::vector<glm::vec3> colors);
    void setValues(std::vector<float> values);
    void setTime(std::string time);
protected:
    Legend(GLFWwindow* parentWindow);
    ~Legend();
    static Legend* instance;
    GLFWwindow* parentWindow;
    GLFWwindow* childWindow;
    std::vector<std::string> names;
    std::vector<glm::vec3> colors;
    std::vector<float> values;
    std::string time;

    static int cursorX, cursorY, deltaCursorX, deltaCursorY, windowPosX, windowPosY;
    static bool isMoved;
    

    static void onParentFocusCallback(GLFWwindow* window, int focused);
    static void onParentIconifyCallback(GLFWwindow* window, int focused);
    static void onChildMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void onChildCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};


