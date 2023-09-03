#include "menu.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <imgui/ImGuiFileDialog.h>

int selected_radio_button = 2;
std::string title="Cases by region" ,mode="barChartMap", csv_data = "data/cases_by_region.csv", csv_coordinates = "data/region_coordinates.csv", map = "textures/map-47.5-20-34.5-5.png";
int latitude_column=2, longitude_column=3;
float up=47.5f, down=34.5f, left=5.f, right=20.f, zoom=0.00002f;
float gridDim = 10000;

bool isOk = false;
int window_width, window_height;

menuData* menu() {
    bool closed = false;
    // Initialize GLFW
    if (!glfwInit())
    {
        return NULL;
    }

	// makes the window not closable using callbacks

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Menu", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return NULL;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL; // Disable imgui.ini file creation
    (void)io; // Prevents unused variable warning
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Main loop
	while (!isOk) {
        if(glfwWindowShouldClose(window)) {
            closed = true;
            goto endMainLoop;
        }
        mainLoop(window);
    }

    endMainLoop:

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    isOk = false;

    struct menuData * data = new menuData;
    data->closed = closed;
    data->title = title;
    data->mode = mode;
    data->csv_data = csv_data;
    data->csv_coordinates = csv_coordinates;
    data->map = map;
    data->latitude_column = latitude_column;
    data->longitude_column = longitude_column;
    data->up = up;
    data->down = down;
    data->left = left;
    data->right = right;
    data->zoom = zoom;
    data->gridDim = gridDim;

    return data;

}

void mainLoop(GLFWwindow *window) {
    // Start a new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glfwGetWindowSize(window, &window_width, &window_height);
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height)); // ensures ImGui fits the GLFW window
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    // Start a new ImGui window
    ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowFontScale(1.3);
    ImGui::Spacing();
    char buf[100];
    strcpy(buf, title.c_str());
    ImGui::InputText("Title", buf, 100);
    title = buf;
    ImGui::Spacing();
    ImGui::Text("Which version do you want to run?");
    ImGui::RadioButton("Bar chart", &selected_radio_button, 1);
    ImGui::RadioButton("Bar chart with map", &selected_radio_button, 2);
    ImGui::Spacing();
    
    switch(selected_radio_button) {
        case 1:
            mode = "barChart";
            barChartMenu();
            break;
        
        case 2:
            mode = "barChartMap";
            barChartMapMenu();
            break;
        
        default:
            break;
    }

    if(selected_radio_button != 0) {
        if(ImGui::Button("OK")) {
            ok();
        }
    }
    

    // End the ImGui window
    ImGui::End();

    // Render the ImGui frame
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(window);

    // Poll events
    glfwPollEvents();
}

void barChartMenu() {
    char button_text[2048];
    sprintf(button_text, "data: %s", csv_data == "" ? "Select the data file" : csv_data.c_str());
    // open Dialog Simple
    if (ImGui::Button(button_text))
        ImGuiFileDialog::Instance()->OpenDialog("data_csv", "Select a CSV file containing the data", ".csv", ".");

    // display
    if (ImGuiFileDialog::Instance()->Display("data_csv", ImGuiWindowFlags_NoCollapse, ImVec2(400.f, 300.f))) {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk()) {
            // action
            csv_data = ImGuiFileDialog::Instance()->GetFilePathName();
        }
        // close
        ImGuiFileDialog::Instance()->Close();
    }
    ImGui::Spacing();

    ImGui::InputFloat("Grid dimension", &gridDim, 0.0f, 0.0f, "%.6f");
    ImGui::Spacing();
}

void barChartMapMenu() {
    barChartMenu();

    char button_text[2048];
    sprintf(button_text, "coordinates: %s", csv_coordinates == "" ? "Select the coordinates file" : csv_coordinates.c_str());
    // open Dialog Simple
    if (ImGui::Button(button_text))
        ImGuiFileDialog::Instance()->OpenDialog("coordinates_csv", "Select a CSV file containing the coordinates", ".csv", ".");

    // display
    if (ImGuiFileDialog::Instance()->Display("coordinates_csv", ImGuiWindowFlags_NoCollapse, ImVec2(400.f, 300.f))) {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk()) {
            // action
            csv_coordinates = ImGuiFileDialog::Instance()->GetFilePathName();
        }
        // close
        ImGuiFileDialog::Instance()->Close();
    }
    ImGui::Spacing();

    ImGui::InputInt("Latitude column", &latitude_column);
    ImGui::Spacing();
    ImGui::InputInt("Longitude column", &longitude_column);
    ImGui::Spacing();

    sprintf(button_text, "map: %s", map == "" ? "Select the map file" : map.c_str());
    // open Dialog Simple
    if (ImGui::Button(button_text))
        ImGuiFileDialog::Instance()->OpenDialog("map_png", "Select a PNG file containing the map", ".png", ".");

    // display
    if (ImGuiFileDialog::Instance()->Display("map_png", ImGuiWindowFlags_NoCollapse, ImVec2(400.f, 300.f))) {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk()) {
            // action
            map = ImGuiFileDialog::Instance()->GetFilePathName();
        }
        // close
        ImGuiFileDialog::Instance()->Close();
    }
    ImGui::Spacing();

    ImGui::Text("Map coordinates bounds:");
    ImGui::Spacing();
    ImGui::InputFloat("North latitude [degree]", &up, 0.0f, 0.0f, "%.6f");
    ImGui::Spacing();
    ImGui::InputFloat("South latitude [degree]", &down, 0.0f, 0.0f, "%.6f");
    ImGui::Spacing();
    ImGui::InputFloat("West longitude [degree]", &left, 0.0f, 0.0f, "%.6f");
    ImGui::Spacing();
    ImGui::InputFloat("East longitude [degree]", &right, 0.0f, 0.0f, "%.6f");
    ImGui::Spacing();
    ImGui::InputFloat("Map scale", &zoom, 0.0f, 0.0f, "%.6f");
    ImGui::Spacing();

}

void ok() {
    isOk = true;
}
