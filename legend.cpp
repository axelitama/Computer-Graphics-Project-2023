#include "legend.hpp"
#include <stdexcept>
#include <thread>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

Legend* Legend::instance = nullptr;
int Legend::cursorX = 0,
    Legend::cursorY = 0,
    Legend::deltaCursorX = 0,
    Legend::deltaCursorY = 0,
    Legend::windowPosX = 0,
    Legend::windowPosY = 0;
bool Legend::isMoved = false;

Legend::Legend(GLFWwindow* parentWindow)
{  
    this->parentWindow = parentWindow;
    

    glfwSetWindowFocusCallback(parentWindow, onParentFocusCallback);
    glfwSetWindowIconifyCallback(parentWindow, onParentIconifyCallback);

    // reset defaul api to opengl after disablig it for vulkan
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    childWindow = glfwCreateWindow(400, 300, "Legend Window", NULL, NULL);
    if (!childWindow) {
        fprintf(stderr, "Failed to create legend window\n");
        glfwTerminate();
        return;
    }

    glfwSetMouseButtonCallback(childWindow, onChildMouseButtonCallback);
    glfwSetCursorPosCallback(childWindow, onChildCursorPosCallback);
    
    glfwSetWindowAttrib(childWindow, GLFW_FLOATING, GLFW_TRUE);
    int parentX, parentY;
    glfwGetWindowPos(parentWindow, &parentX, &parentY);
    // Set child window position relative to the parent
    glfwSetWindowPos(childWindow, parentX + 10, parentY + 10);

    glfwMakeContextCurrent(childWindow);
    if (!glfwGetCurrentContext()) {
        // Handle OpenGL context creation failure
        fprintf(stderr, "Failed to create OpenGL context\n");
        return;
    }
    
    // Initialize ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL; // Disable imgui.ini file creation
    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddText(u8"█");
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.BuildRanges(&ranges);
    io.Fonts->AddFontFromFileTTF("fonts/Courier New Bold.ttf", 15, NULL, ranges.Data); // load a font that can print the █ character
    io.Fonts->Build();
    (void)io; // Prevents unused variable warning
    ImGui_ImplGlfw_InitForOpenGL(childWindow, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

Legend::~Legend() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(childWindow);
    // glfwTerminate(); // already called in Starter.hpp
}

Legend & Legend::getInstance(GLFWwindow* parentWindow) {
    if (!instance) {
        instance = new Legend(parentWindow);
    } else{
        throw std::logic_error("Singleton instance already initialized");
    }
    return *instance;
}

Legend & Legend::getInstance() {
    if (!instance) {
        throw std::logic_error("Singleton instance not yet initialized");
    }
    return *instance;
}

void Legend::setLegend(std::vector<std::string> names, std::vector<glm::vec3> colors) {
    assert(names.size() == colors.size() && "names and colors must be the same size");
    this->names = names;
    this->colors = colors;
}

void Legend::setValues(std::vector<float> values) {
    assert(names.size() == values.size() && "names and values must be the same size");
    this->values = values;
}

void Legend::setTime(std::string time) {
    this->time = time;
}

void Legend::mainLoop() {
    static bool isFirst = true;
    glfwMakeContextCurrent(childWindow);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    int window_width, window_height;
    int imgui_x, imgui_y, glfw_x, glfw_y;
    if(isFirst) {
        glfwGetWindowSize(instance->childWindow, &window_width, &window_height);
        ImGui::SetNextWindowSize(ImVec2(window_width, window_height)); // ensures ImGui fits the GLFW window
    }
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    

    ImGui::Begin("Legend", NULL, ImGuiWindowFlags_NoMove);

    glfwGetWindowPos(instance->childWindow, &windowPosX, &windowPosY);
    if(isMoved){
        // glfwSetWindowPos(instance->childWindow, windowPosX + deltaCursorX, windowPosY + deltaCursorY);
        // deltaCursorX = 0;
        // deltaCursorY = 0;
        // cursorX += deltaCursorX;
        // cursorY += deltaCursorY;

        double posx, posy;
        glfwGetCursorPos(instance->childWindow, &posx, &posy);
        deltaCursorX = posx - cursorX;
        deltaCursorY = posy - cursorY;

        int x, y;
        glfwGetWindowPos(instance->childWindow, &x, &y);
        glfwSetWindowPos(instance->childWindow, x + deltaCursorX, y + deltaCursorY);
    }

    if(!isMoved) {
        imgui_x = floor(ImGui::GetWindowPos().x);
        imgui_y = floor(ImGui::GetWindowPos().y);
        glfwGetWindowPos(instance->childWindow, &glfw_x, &glfw_y);
        glfwSetWindowPos(instance->childWindow, glfw_x + imgui_x, glfw_y + imgui_y);
    }
    window_width = ImGui::GetWindowSize().x;
    window_height = ImGui::GetWindowSize().y;
    glfwSetWindowSize(instance->childWindow, window_width, window_height);
    
    // imgui_x = floor(ImGui::GetWindowPos().x);
    // imgui_y = floor(ImGui::GetWindowPos().y);
    // glfwGetWindowPos(instance->childWindow, &glfw_x, &glfw_y);
    // glfwSetWindowPos(instance->childWindow, glfw_x + imgui_x, glfw_y + imgui_y);
    // ImGui::SetWindowPos(ImVec2(0, 0));

    ImGui::Text("Time: %s", time.c_str());
    ImGui::Separator();
    for(int i = 0; i < names.size(); i++) {
        ImGui::TextColored(ImVec4(colors[i].x, colors[i].y, colors[i].z, 1.0f), u8"██");
        ImGui::SameLine();
        ImGui::Text("  %s:  %f", names[i].c_str(), values[i]);
    }

    ImGui::End();

    
    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(childWindow, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(childWindow);

    // Poll events
    // glfwPollEvents(); // already called in Starter.hpp
    isFirst = false;
}

void Legend::onParentFocusCallback(GLFWwindow* window, int focused) {
    if (focused) {
        glfwFocusWindow(window);
        // set child window not floating
        glfwSetWindowAttrib(instance->childWindow, GLFW_FLOATING, GLFW_TRUE);
    } else {
        // set child window floating
        glfwSetWindowAttrib(instance->childWindow, GLFW_FLOATING, GLFW_FALSE);
        // TODO: set child window to go behind without reclicking the focused window
    }
}

void Legend::onParentIconifyCallback(GLFWwindow* window, int iconified) {
    if (iconified) {

        glfwIconifyWindow(window);
        // hide child window
        glfwHideWindow(instance->childWindow);

    } else {
        // show child window
        glfwShowWindow(instance->childWindow);
    }
}

// Mouse callback function to track mouse button events
void Legend::onChildMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
    //     double x, y;
    //     int width, height;
    //     glfwGetCursorPos(window, &x, &y);
    //     cursorX = floor(x);
    //     cursorY = floor(y);
    //     glfwGetFramebufferSize(window, &width, &height);
    //     if(cursorY >= 1 && cursorY <= 18 && cursorX >= 1 && cursorX <= width - 1)
    //         isMoved = true;
    // }
    // if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
    //     isMoved = false;
    //     cursorX = 0;
    //     cursorY = 0;
    // }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x, y;
        int width, height;
        glfwGetCursorPos(window, &x, &y);
        cursorX = floor(x);
        cursorY = floor(y);
        glfwGetFramebufferSize(window, &width, &height);
        if(cursorY >= 1 && cursorY <= 18 && cursorX >= 1 && cursorX <= width - 1)
            isMoved = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        isMoved = false;
    }
}

// Cursor position callback function to handle dragging
void Legend::onChildCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // if (isMoved) {
    //     deltaCursorX = xpos - cursorX;
    //     deltaCursorY = ypos - cursorY;
    // }

    if (isMoved) {
     int delta_x = xpos - cursorX;
     int delta_y = ypos - cursorY;
     int x,y;
     glfwGetWindowPos(window, &x, &y);
     glfwSetWindowPos(window, x + delta_x, y + delta_y);
  }
}

