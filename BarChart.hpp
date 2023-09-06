#ifndef BARCHART_HPP
#define BARCHART_HPP

#include "Starter.hpp"
#include "CSVReader.hpp"
#include "TextMaker.hpp"
#include "Hud.hpp"
#include "legend.hpp"

std::vector<SingleText> demoText;
std::string shaderDir;


class BarChart : public BaseProject {
    public:
        BarChart(std::string title, std::string shaderPath, const CSVReader& csv, float gridDim = 10000);

        ~BarChart();

        void pauseData() ;

        void playData();

        void toggleRotation();

    protected:
        char title[100]; // do not use std::string because text overlay wants a c_str but do not copy it (so can't use c_str() because temporary)
        Legend * legend;

        float minHeight,
            scalingFactor,
            gridDim,
            groundX,
            groundZ,
            gridLinesWidth;
        glm::vec3 gridColor;

        struct UniformBlock {
            alignas(16) glm::mat4 mvpMat;
        };

        struct GlobalUniformBlock {
            alignas(16) glm::vec3 DlightDir;
            alignas(16) glm::vec3 DlightColor;
            alignas(16) glm::vec3 AmbLightColor;
            alignas(16) glm::vec3 eyePos;
        };


        struct VertexColour {
            glm::vec3 pos;
            glm::vec3 normal;
            glm::vec3 colour;
        };

        struct VertexLine {
            glm::vec3 pos;
            glm::vec3 colour;
        };

        const char* name;
        CSVReader csv;
        float *visualizedValues;
        // Current aspect ratio (used by the callback that resized the window
        float Ar;

        // Descriptor Layouts ["classes" of what will be passed to the shaders]
        DescriptorSetLayout DSL_ground;
        DescriptorSetLayout DSL_bar;
        DescriptorSetLayout DSLGubo;
        DescriptorSetLayout DSL_grid;

        // Vertex formats
        VertexDescriptor VD_ground;
        VertexDescriptor VD_bar;
        VertexDescriptor VD_line;

        // Pipelines [Shader couples]
        Pipeline P_ground;
        Pipeline P_bar;
        Pipeline P_grid;

        // Models, textures and Descriptors (values assigned to the uniforms)
        // Please note that Model objects depends on the corresponding vertex structure
        // Models
        Model<VertexColour> M_ground;
        Model<VertexColour> * M_bars;
        Model<VertexLine> M_grid[2];


        // Descriptor sets
        DescriptorSet DS_ground;
        DescriptorSet * DS_bars;
        DescriptorSet DSGubo;
        DescriptorSet DS_grid[2];
        
        // C++ storage for uniform variables
        UniformBlock ubo_ground;
        UniformBlock* ubo_bars;
        UniformBlock ubo_grid[2];
        GlobalUniformBlock gubo;

	    TextMaker txt;
	    HudMaker hud;

        // Other application parameters
        float CamH, CamRadius, CamPitch, CamYaw, targtH;

        void setWindowParameters() override;

        void onWindowResize(int w, int h) override;

        void localInit() override;

        void pipelinesAndDescriptorSetsInit() override;

        void pipelinesAndDescriptorSetsCleanup() override;

        void localCleanup() override;

        void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) override;

        void updateUniformBuffer(uint32_t currentImage) override;

        glm::mat4 getWorldMatrixBar(float height);


};


BarChart *_BP_Ref;



void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		int height, width;
		glfwGetWindowSize(window, &width, &height);

		fflush(stdout);
        
		if (xpos >= width-100 && xpos <= width-70 && ypos >= height-35 && ypos <= height-10) {
			((BarChart *)_BP_Ref)->playData();
		}else if (xpos >= width-60 && xpos <= width-30 && ypos >= height-35 && ypos <= height-10) {
			((BarChart *)_BP_Ref)->pauseData();
		}else if (xpos >= width-30 && xpos <= width && ypos >= height-35 && ypos <= height-10) {
			((BarChart *)_BP_Ref)->toggleRotation();
		}
		
	}
}


/**************************************************
*****   TODO: Following code should have been in BarChart.cpp but that will cause multiple definition problem
*****   because Starter.hpp contains definitions
**************************************************/

// This has been adapted from the Vulkan tutorial
// The uniform buffer objects data structures
// Remember to use the correct alignas(...) value
//        float : alignas(4)
//        vec2  : alignas(8)
//        vec3  : alignas(16)
//        vec4  : alignas(16)
//        mat3  : alignas(16)
//        mat4  : alignas(16)
// Example:

// MAIN ! 
BarChart::BarChart(std::string title, std::string shaderPath, const CSVReader& csv, float gridDim) : BaseProject(), csv(csv) {
    strcpy(this->title, title.c_str());
    shaderDir = shaderPath;
    name = "Bar Chart";
    M_bars = new Model<VertexColour>[csv.getNumVariables()-1];
    DS_bars = new DescriptorSet[csv.getNumVariables()-1];
    ubo_bars = new UniformBlock[csv.getNumVariables()-1];

    minHeight = 0.001f;
    int excludeCol[1] = {0}; 
    scalingFactor = 20/csv.getMaxValue(excludeCol, 1);//0.0001;
    printf("scaling: %f", scalingFactor);
    this->gridDim = gridDim;
    gridLinesWidth = 0.1f;
    gridColor = {0.5, 0.5, 0.5};

    groundZ = 1.5;
    groundX = csv.getNumVariables()/2.f+1;

    demoText = {
        {1, {this->title, "", "", ""}, 0, 0},
    };
}

BarChart::~BarChart() {
    // Deallocate the memory used by the models array
    delete[] M_bars;
    delete[] DS_bars;
    delete[] ubo_bars;
}


int height;
int width;
// Here you set the main application parameters
void BarChart::setWindowParameters() {
    // window size, titile and initial background
    windowWidth = 800;
    windowHeight = 600;
    windowTitle = name;
    windowResizable = GLFW_TRUE;
    initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
    
    // Descriptor pool sizes
    uniformBlocksInPool = 200;
    texturesInPool = 100;
    setsInPool = 200;
    
    Ar = (float)windowWidth / (float)windowHeight;
    height = windowHeight;
    width = windowWidth;    
}
	
// What to do when the window changes size

void BarChart::onWindowResize(int w, int h) {
    Ar = (float)w / (float)h;
    height = h;
    width = w;
}
	
// Here you load and setup all your Vulkan Models and Texutures.
// Here you also create your Descriptor set layouts and load the shaders for the pipelines
void BarChart::localInit() {
    legend = &Legend::getInstance(window);

    DSL_bar.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
            });
    DSL_grid.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
            });

    // Descriptor Layouts [what will be passed to the shaders]
    DSL_ground.init(this, {
                // this array contains the bindings:
                // first  element : the binding number
                // second element : the type of element (buffer or texture)
                //                  using the corresponding Vulkan constant
                // third  element : the pipeline stage where it will be used
                //                  using the corresponding Vulkan constant
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
            });
    DSLGubo.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
        });
    VD_bar.init(this, {
                {0, sizeof(VertexColour), VK_VERTEX_INPUT_RATE_VERTEX}
            }, {
                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexColour, pos), sizeof(glm::vec3), POSITION},
                {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexColour, normal), sizeof(glm::vec3), NORMAL},
                {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexColour, colour), sizeof(glm::vec3), COLOR}
            });
    VD_line.init(this, {
                {0, sizeof(VertexLine), VK_VERTEX_INPUT_RATE_VERTEX}
            }, {
                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexLine, pos), sizeof(glm::vec3), POSITION},
                {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexLine, colour), sizeof(glm::vec3), COLOR}
            });

    // Vertex descriptors
    VD_ground.init(this, {
                // this array contains the bindings
                // first  element : the binding number
                // second element : the stride of this binging
                // third  element : whether this parameter change per vertex or per instance
                //                  using the corresponding Vulkan constant
                {0, sizeof(VertexColour), VK_VERTEX_INPUT_RATE_VERTEX}
            }, {
                // this array contains the location
                // first  element : the binding number
                // second element : the location number
                // third  element : the offset of this element in the memory record
                // fourth element : the data type of the element
                //                  using the corresponding Vulkan constant
                // fifth  elmenet : the size in byte of the element
                // sixth  element : a constant defining the element usage
                //                   POSITION - a vec3 with the position
                //                   NORMAL   - a vec3 with the normal vector
                //                   UV       - a vec2 with a UV coordinate
                //                   COLOR    - a vec4 with a RGBA color
                //                   TANGENT  - a vec4 with the tangent vector
                //                   OTHER    - anything else
                //
                // ***************** DOUBLE CHECK ********************
                //    That the Vertex data structure you use in the "offsetoff" and
                //	in the "sizeof" in the previous array, refers to the correct one,
                //	if you have more than one vertex format!
                // ***************************************************
                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexColour, pos), sizeof(glm::vec3), POSITION},
                {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexColour, normal), sizeof(glm::vec3), NORMAL},
                {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexColour, colour), sizeof(glm::vec3), COLOR}
            });
		
    // Pipelines [Shader couples]
    // The second parameter is the pointer to the vertex definition
    // Third and fourth parameters are respectively the vertex and fragment shaders
    // The last array, is a vector of pointer to the layouts of the sets that will
    // be used in this pipeline. The first element will be set 0, and so on..

    //get executable path

    P_ground.init(this, &VD_ground, shaderDir + "ShaderBar.vert.spv", shaderDir + "ShaderBar.frag.spv", {&DSL_ground, &DSLGubo});

    P_bar.init(this, &VD_bar, shaderDir + "ShaderBar.vert.spv", shaderDir + "ShaderBar.frag.spv", {&DSL_bar, &DSLGubo});

    P_grid.init(this, &VD_line, shaderDir + "ShaderLine.vert.spv", shaderDir + "ShaderLine.frag.spv", {&DSL_grid});


    // Models, textures and Descriptors (values assigned to the uniforms)

    // Create models
    // The second parameter is the pointer to the vertex definition for this model
    // The third parameter is the file name
    // The last is a constant specifying the file type: currently only OBJ or GLTF

    float start = -groundX + 1;

    // create grid
    int tmp[1] = {0};
    int numLines = csv.getMaxValue(tmp, 1) / gridDim + 1;
    for(int i=0; i <= numLines; i++) {
        M_grid[0].vertices.push_back({{start-1, i*gridDim*scalingFactor+minHeight, 0}, {1, 1, 1}});
        M_grid[0].vertices.push_back({{-start+1, i*gridDim*scalingFactor+minHeight, 0}, {1, 1, 1}});
        printf("%f\n", i*gridDim*scalingFactor);
    }
    for(int i=0; i <= numLines*2; i++) {
        M_grid[0].indices.push_back(i);
    }
    M_grid[0].initMesh(this, &VD_line);

    for(int i=0; i <= numLines; i++) {
        M_grid[1].vertices.push_back({{0, i*gridDim*scalingFactor+minHeight, -1.5}, {1, 1, 1}});
        M_grid[1].vertices.push_back({{0, i*gridDim*scalingFactor+minHeight, 1.5}, {1, 1, 1}});
        printf("%f\n", i*gridDim*scalingFactor);
    }
    for(int i=0; i <= numLines*2; i++) {
        M_grid[1].indices.push_back(i);
    }
    M_grid[1].initMesh(this, &VD_line);
    
    // Creates a mesh with direct enumeration of vertices and indices
    
    M_ground.vertices = {
                    {{-groundX,-0.1,-groundZ}, {0.f, 1.f, 0.f}, {1.0f,1.0f, 1.0f}},
                    {{-groundX,-0.1,groundZ}, {0.f, 1.f, 0.f}, {1.0f,1.0f, 1.0f}},
                    {{groundX,-0.1,-groundZ}, {0.f, 1.f, 0.f}, {1.0f,1.0f, 1.0f}},
                    {{groundX,-0.1,groundZ}, {0.f, 1.f, 0.f}, {1.0f,1.0f, 1.0f}}
    };
    M_ground.indices = {0, 1, 2, 1, 3, 2};
    M_ground.initMesh(this, &VD_ground);

    //create parallelepipeds for bars

    std::vector<std::string> names;
    std::vector<glm::vec3> colors;
    
    for (int i = 0; i < csv.getNumVariables()-1; i++) {
        names.push_back(csv.getVariableNames()[i+1]);
        //create a parallelepiped of a random color
        float r = (float)rand() / (float)RAND_MAX;
        float g = (float)rand() / (float)RAND_MAX;
        float b = (float)rand() / (float)RAND_MAX;
        colors.push_back(glm::vec3(r, g, b));
        // add the vertices putting position, normal (replicated vertices) and color
        M_bars[i].vertices = {
            // bottom face
            {{start+i,0,-0.5}, {0, -1, 0}, {r,g,b}},
            {{start+i,0,0.5}, {0, -1, 0}, {r,g,b}},
            {{start+i+1,0,-0.5}, {0, -1, 0}, {r,g,b}},
            {{start+i+1,0,0.5}, {0, -1, 0}, {r,g,b}},
            // top face
            {{start+i,1,-0.5}, {0, 1, 0}, {r,g,b}},
            {{start+i,1,0.5}, {0, 1, 0}, {r,g,b}},
            {{start+i+1,1,-0.5}, {0, 1, 0}, {r,g,b}},
            {{start+i+1,1,0.5}, {0, 1, 0}, {r,g,b}},
            // left face
            {{start+i,0,-0.5}, {-1, 0, 0}, {r,g,b}},
            {{start+i,0,0.5}, {-1, 0, 0}, {r,g,b}},
            {{start+i,1,-0.5}, {-1, 0, 0}, {r,g,b}},
            {{start+i,1,0.5}, {-1, 0, 0}, {r,g,b}},
            // right face
            {{start+i+1,0,-0.5}, {1, 0, 0}, {r,g,b}},
            {{start+i+1,0,0.5}, {1, 0, 0}, {r,g,b}},
            {{start+i+1,1,-0.5}, {1, 0, 0}, {r,g,b}},
            {{start+i+1,1,0.5}, {1, 0, 0}, {r,g,b}},
            // front face
            {{start+i,0,0.5}, {0, 0, 1}, {r,g,b}},
            {{start+i+1,0,0.5}, {0, 0, 1}, {r,g,b}},
            {{start+i,1,0.5}, {0, 0, 1}, {r,g,b}},
            {{start+i+1,1,0.5}, {0, 0, 1}, {r,g,b}},
            // back face
            {{start+i,0,-0.5}, {0, 0, -1}, {r,g,b}},
            {{start+i+1,0,-0.5}, {0, 0, -1}, {r,g,b}},
            {{start+i,1,-0.5}, {0, 0, -1}, {r,g,b}},
            {{start+i+1,1,-0.5}, {0, 0, -1}, {r,g,b}}
        };

        // add the indices
        M_bars[i].indices = {
            0, 1, 2, 1, 3, 2, // bottom
            4, 5, 6, 5, 7, 6, // top
            8, 9, 10, 9, 11, 10, // left
            12, 13, 14, 13, 15, 14, // right
            16, 17, 18, 17, 19, 18, // front
            20, 21, 22, 21, 23, 22 // back
        };
        M_bars[i].initMesh(this, &VD_bar);
    }

	txt.init(this, &demoText);
	hud.init(this);
    
    // Create the textures
    // The second parameter is the file name
    
    // Init local variables
    CamH = 0.0f;
    targtH = 0.0f;
    CamRadius = 13.0f;
    CamPitch = 0.53f;
    CamYaw = 2.7f;

    visualizedValues = (float *)malloc((csv.getNumVariables()-1)*sizeof(float));
    legend->setLegend(names, colors);

    _BP_Ref = this;
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
}
	
// Here you create your pipelines and Descriptor Sets!
void BarChart::pipelinesAndDescriptorSetsInit() {
    // This creates a new pipeline (with the current surface), using its shaders
    P_ground.create();

    // Here you define the data set
    DS_ground.init(this, &DSL_ground, {
    // the second parameter, is a pointer to the Uniform Set Layout of this set
    // the last parameter is an array, with one element per binding of the set.
    // first  elmenet : the binding number
    // second element : UNIFORM or TEXTURE (an enum) depending on the type
    // third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
    // fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
                {0, UNIFORM, sizeof(UniformBlock), nullptr},
            });
    DSGubo.init(this, &DSLGubo, {
                {0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
        });
    

    P_bar.create();
    for (int i = 0; i < csv.getNumVariables()-1; i++) {
        DS_bars[i].init(this, &DSL_bar, {
                {0, UNIFORM, sizeof(UniformBlock), nullptr}
            });
    }

	P_grid.create(VK_PRIMITIVE_TOPOLOGY_LINE_LIST, gridLinesWidth);
    DS_grid[0].init(this, &DSL_grid, {
                {0, UNIFORM, sizeof(UniformBlock), nullptr}
            });
    DS_grid[1].init(this, &DSL_grid, {
                {0, UNIFORM, sizeof(UniformBlock), nullptr}
            });

    txt.pipelinesAndDescriptorSetsInit();
    hud.pipelinesAndDescriptorSetsInit();
}

// Here you destroy your pipelines and Descriptor Sets!
// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
void BarChart::pipelinesAndDescriptorSetsCleanup() {
    // Cleanup pipelines
    P_ground.cleanup();
    P_grid.cleanup();

    // Cleanup datasets
    DS_ground.cleanup();
    DS_grid[0].cleanup();
    DS_grid[1].cleanup();

    P_bar.cleanup();
    for (int i = 0; i < csv.getNumVariables()-1; i++) {
        DS_bars[i].cleanup();
    }
    DSGubo.cleanup();

	txt.pipelinesAndDescriptorSetsCleanup();
	hud.pipelinesAndDescriptorSetsCleanup();
}

// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
// methods: .cleanup() recreates them, while .destroy() delete them completely
void BarChart::localCleanup() {
    // Cleanup models
    M_ground.cleanup();
    for (int i = 0; i < csv.getNumVariables()-1; i++) {
        M_bars[i].cleanup();
    }
    M_grid[0].cleanup();
    M_grid[1].cleanup();
    
    // Cleanup descriptor set layouts
    DSL_ground.cleanup();
    DSL_bar.cleanup();
    DSLGubo.cleanup();
    DSL_grid.cleanup();
    
    // Destroies the pipelines
    P_ground.destroy();
    P_bar.destroy();
    P_grid.destroy();

	txt.localCleanup();
	hud.localCleanup();
}
	
// Here it is the creation of the command buffer:
// You send to the GPU all the objects you want to draw,
// with their buffers and textures
void BarChart::populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
    
    // binds the pipeline
    P_ground.bind(commandBuffer);
    DSGubo.bind(commandBuffer, P_ground, 1, currentImage);
    // For a pipeline object, this command binds the corresponing pipeline to the command buffer passed in its parameter

    // binds the data set
    DS_ground.bind(commandBuffer, P_ground, 0, currentImage);
    // For a Dataset object, this command binds the corresponing dataset
    // to the command buffer and pipeline passed in its first and second parameters.
    // The third parameter is the number of the set being bound
    // As described in the Vulkan tutorial, a different dataset is required for each image in the swap chain.
    // This is done automatically in file Starter.hpp, however the command here needs also the index
    // of the current image in the swap chain, passed in its last parameter

    // binds the model
    M_ground.bind(commandBuffer);
    // For a Model object, this command binds the corresponing index and vertex buffer
    // to the command buffer passed in its parameter

    // record the drawing command in the command buffer
    vkCmdDrawIndexed(commandBuffer,
            static_cast<uint32_t>(M_ground.indices.size()), 1, 0, 0, 0);
    // the second parameter is the number of indexes to be drawn. For a Model object,
    // this can be retrieved with the .indices.size() method.


    P_bar.bind(commandBuffer);
    DSGubo.bind(commandBuffer, P_bar, 1, currentImage);
    for (int i = 0; i < csv.getNumVariables()-1; i++) {
        DS_bars[i].bind(commandBuffer, P_bar, 0, currentImage);
        M_bars[i].bind(commandBuffer);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(M_bars[i].indices.size()), 1, 0, 0, 0);
    }

    P_grid.bind(commandBuffer);
    DS_grid[0].bind(commandBuffer, P_grid, 0, currentImage);
    M_grid[0].bind(commandBuffer);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_grid[0].indices.size()), 1, 0, 0, 0);

    DS_grid[1].bind(commandBuffer, P_grid, 0, currentImage);
    M_grid[1].bind(commandBuffer);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_grid[1].indices.size()), 1, 0, 0, 0);
	
    txt.populateCommandBuffer(commandBuffer, currentImage, 0);
    hud.populateCommandBuffer(commandBuffer, currentImage, 0);
}

bool isAutoRotationEnabled = false;
bool isPauseEnabled = true;

void BarChart::pauseData(){
    isPauseEnabled = true;
}

void BarChart::playData(){
    isPauseEnabled = false;
}

void BarChart::toggleRotation(){
    isAutoRotationEnabled = !isAutoRotationEnabled;
}

// Here is where you update the uniforms.
// Very likely this will be where you will be writing the logic of your application.
void BarChart::updateUniformBuffer(uint32_t currentImage) {
    // Standard procedure to quit when the ESC key is pressed
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    // Integration with the timers and the controllers
    float deltaT;
    glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);

    static bool wasAutoRotationPressed = false;
    bool isAutoRotationPressed = false; 


    static bool wasPausePressed = false;
    bool isPausePressed = false;
    
    getSixAxis(deltaT, m, r, isAutoRotationPressed, isPausePressed);
    // getSixAxis() is defined in Starter.hpp in the base class.
    // It fills the float point variable passed in its first parameter with the time
    // since the last call to the procedure.
    // It fills vec3 in the second parameters, with three values in the -1,1 range corresponding
    // to motion (with left stick of the gamepad, or ASWD + RF keys on the keyboard)
    // It fills vec3 in the third parameters, with three values in the -1,1 range corresponding
    // to motion (with right stick of the gamepad, or Arrow keys + QE keys on the keyboard, or mouse)
    // If fills the last boolean variable with true if fire has been pressed:
    //          SPACE on the keyboard, A or B button on the Gamepad, Right mouse button
    if(isAutoRotationPressed) {
        if(!wasAutoRotationPressed) {
            isAutoRotationEnabled = !isAutoRotationEnabled;
            wasAutoRotationPressed = true;
        }
    } else {
        wasAutoRotationPressed = false;
    }

    if (isPausePressed) {
        if (!wasPausePressed) {
            isPauseEnabled = !isPauseEnabled;
            wasPausePressed = true;
        }
    }
    else {
        wasPausePressed = false;
    }

    // Parameters
    // Camera FOV-y, Near Plane and Far Plane
    const float FOVy = glm::radians(90.0f);
    const float nearPlane = 0.1f;
    const float farPlane = 100.0f;
    const float rotSpeed = glm::radians(90.0f);
    const float movSpeed = 10.0f;
    const float cameraSpeed = 0.5f;     // Adjust the speed as needed

    const float minPitch = glm::radians(-30.f);
    const float maxPitch = glm::radians(89.0f);

    // CamH += r.x * movSpeed * deltaT;
    CamRadius += -m.z * movSpeed * deltaT * 3.0f;
    if(CamRadius < 5) CamRadius = 5;
    CamPitch += r.x * rotSpeed * deltaT;
    if(CamPitch < minPitch) CamPitch = minPitch;
    if(CamPitch > maxPitch) CamPitch = maxPitch;
    
    if (isAutoRotationEnabled) {
        CamYaw += cameraSpeed * deltaT;
    }
    else {
        CamYaw -= r.y * rotSpeed * deltaT;
    }

    if (CamRadius>50.0f)
        CamRadius = 50.0f;
    // print the controls (m variable values)
        // std::cout << m.x << " " << m.y << " " << m.z << std::endl;
        // std::cout << r.x << " " << r.y << " " << r.z << std::endl;


    glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
    Prj[1][1] *= -1;
    glm::vec3 camTarget = glm::vec3(0, targtH, 0);

    glm::mat4 camMw = glm::rotate(glm::mat4(1), CamYaw, glm::vec3(0, 1, 0));
    
    glm::vec3 camPos = glm::vec3(camMw * glm::vec4(
        0,
        CamH + CamRadius * sin(CamPitch),
        CamRadius * cos(CamPitch),
        1
    ));

    // print campitch and camyaw
    // std::cout << CamPitch << " " << CamYaw << std::endl;



    glm::mat4 View = glm::lookAt(camPos, camTarget, glm::vec3(0,1,0));

    gubo.DlightDir = glm::normalize(glm::vec3(1, 3, 1));
    gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    gubo.AmbLightColor = glm::vec3(0.1f);
    gubo.eyePos = camPos;

    // Writes value to the GPU
    DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);


    //glm::mat4 World = glm::mat4(1);		
    glm::mat4 World = glm::mat4(1.f);//glm::scale(glm::mat4(1.0), glm::vec3(1.05f, 1.0f, 0.97f));


    ubo_ground.mvpMat = Prj * View * World;
    DS_ground.map(currentImage, &ubo_ground, sizeof(ubo_ground), 0);
    // the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
    // the second parameter is the pointer to the C++ data structure to transfer to the GPU
    // the third parameter is its size
    // the fourth parameter is the location inside the descriptor set of this uniform block
    static float time = 0, animationTime = 0;
    animationTime += deltaT;
    time += deltaT;
    static int line = 0;

    float valueTime = 0.5f;
    std::vector<float> values;

    for (int i = 0; i < csv.getNumVariables()-1; i++) {
        float prevValue = line==0?0:std::stof(csv.getLine(line-1)[i+1]);
        float value = std::stof(csv.getLine(line)[i+1]);
        values.push_back(visualizedValues[i]);

        if (!isPauseEnabled){
            visualizedValues[i] = prevValue +
            (value - prevValue) * (animationTime / valueTime);
        }
        else
        {
            visualizedValues[i] = prevValue;
        }
    }
    
    
    // every valueTime seconds, we change the line of the csv file to be read and therefore update the bars
    if(time >= valueTime) {
        time = 0;
        if(!isPauseEnabled)
            line++;
        animationTime = 0;
        if(line >= csv.getNumLines())
            line = 0; //qua potremmo mostrare un messaggio sull'overlay che dice premi "tasto" per ricominciare
    }

    for (int i = 0; i < csv.getNumVariables()-1; i++) {
        World = getWorldMatrixBar(visualizedValues[i]);
        ubo_bars[i].mvpMat = Prj * View * World;
        DS_bars[i].map(currentImage, &ubo_bars[i], sizeof(ubo_bars[i]), 0);
    }
    // printf("\ntime: %f\nline: %d\n", time, line);
    // printf("cam pitch: %f\ncam yaw: %f\n", CamPitch, CamYaw);

    txt.update(currentImage, height, width);
    hud.update(currentImage, height, width);

    if(camPos[2] > 0)
        World = glm::translate(glm::mat4(1), glm::vec3(0, 0, -groundZ)) * glm::mat4(1);
    else
        World = glm::translate(glm::mat4(1), glm::vec3(0, 0, groundZ)) * glm::mat4(1);
    ubo_grid[0].mvpMat = Prj * View * World;
    DS_grid[0].map(currentImage, &ubo_grid[0], sizeof(ubo_grid[0]), 0);

    if(camPos[0] > 0)
        World = glm::translate(glm::mat4(1), glm::vec3(-groundX, 0, 0)) * glm::mat4(1);
    else
        World = glm::translate(glm::mat4(1), glm::vec3(groundX, 0, 0)) * glm::mat4(1);
    ubo_grid[0].mvpMat = Prj * View * World;
    DS_grid[1].map(currentImage, &ubo_grid[0], sizeof(ubo_grid[0]), 0);

    char str[100];
    sprintf(str, "line: %d; time: %s", line, csv.getLine(line)[0].c_str());
    legend->setTime(str);
    legend->setValues(values);
    legend->mainLoop();
}

glm::mat4 BarChart::getWorldMatrixBar(float height) {
    height = height * scalingFactor + minHeight;
    glm::mat4 World =  glm::scale(glm::mat4(1), glm::vec3(1.f, height, 1.f));
    return World;
}




#endif // BARCHART_HPP
