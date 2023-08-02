// This has been adapted from the Vulkan tutorial

#include "Starter.hpp"
#include "CSVReader.hpp"

// The uniform buffer objects data structures
// Remember to use the correct alignas(...) value
//        float : alignas(4)
//        vec2  : alignas(8)
//        vec3  : alignas(16)
//        vec4  : alignas(16)
//        mat3  : alignas(16)
//        mat4  : alignas(16)
// Example:
struct UniformBlock {
	alignas(4) float height;
	alignas(16) glm::mat4 mvpMat;
};

struct OverlayUniformBlock {
	alignas(4) float visible;
};

struct GlobalUniformBlock {
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};


// The vertices data structures
// Example
struct Vertex {
	glm::vec3 pos;
	glm::vec2 UV;
};

struct VertexBar {
	glm::vec3 pos;
	glm::vec3 colour;
};


// MAIN ! 
class SimpleCube : public BaseProject {
	public:
	SimpleCube(const CSVReader& csv) : BaseProject(), csv(csv) {
		M_bars = new Model<VertexBar>[csv.getNumVariables()];
		DS_bars = new DescriptorSet[csv.getNumVariables()];
		ubo_bars = new UniformBlock[csv.getNumVariables()];
	}

	~SimpleCube() {
        // Deallocate the memory used by the models array
        delete[] M_bars;
		delete[] DS_bars;
		delete[] ubo_bars;
    }

	protected:
	CSVReader csv;
	// Current aspect ratio (used by the callback that resized the window
	float Ar;

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSL;
	DescriptorSetLayout DSL_bar;
	DescriptorSetLayout DSLGubo;



	// Vertex formats
	VertexDescriptor VD;
	VertexDescriptor VD_bar;

	// Pipelines [Shader couples]
	Pipeline P;
	Pipeline P_bar;

	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	// Models
	Model<Vertex> M_ground;
	Model<VertexBar> * M_bars;


	// Descriptor sets
	DescriptorSet DS_ground;
	DescriptorSet * DS_bars;
	DescriptorSet DSGubo;
	// Textures
	Texture T;
	
	// C++ storage for uniform variables
	UniformBlock ubo_ground;
	UniformBlock* ubo_bars;
	GlobalUniformBlock gubo;

	// Other application parameters
	float CamH, CamRadius, CamPitch, CamYaw;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Simple Cube";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 200;
		texturesInPool = 100;
		setsInPool = 200;
		
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		DSL_bar.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
				});

		// Descriptor Layouts [what will be passed to the shaders]
		DSL.init(this, {
					// this array contains the bindings:
					// first  element : the binding number
					// second element : the type of element (buffer or texture)
					//                  using the corresponding Vulkan constant
					// third  element : the pipeline stage where it will be used
					//                  using the corresponding Vulkan constant
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});
		DSLGubo.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});

		VD_bar.init(this, {
				  {0, sizeof(VertexBar), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexBar, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexBar, colour),
				         sizeof(glm::vec3), COLOR}
				});

		// Vertex descriptors
		VD.init(this, {
				  // this array contains the bindings
				  // first  element : the binding number
				  // second element : the stride of this binging
				  // third  element : whether this parameter change per vertex or per instance
				  //                  using the corresponding Vulkan constant
				  {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
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
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, UV),
				         sizeof(glm::vec2), UV}
				});

		// Pipelines [Shader couples]
		// The second parameter is the pointer to the vertex definition
		// Third and fourth parameters are respectively the vertex and fragment shaders
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		P.init(this, &VD, "shaders/ShaderVert.spv", "shaders/ShaderFrag.spv", {&DSL, &DSLGubo});

		P_bar.init(this, &VD_bar, "shaders/ShaderBarVert.spv", "shaders/ShaderBarFrag.spv", {&DSL_bar, &DSLGubo});

		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		// The second parameter is the pointer to the vertex definition for this model
		// The third parameter is the file name
		// The last is a constant specifying the file type: currently only OBJ or GLTF
		
		// Creates a mesh with direct enumeration of vertices and indices
		M_ground.vertices = {{{-3,-1,-3}, {0.0f,0.0f}}, {{-3,-1,3}, {0.0f,1.0f}},
					    {{3,-1,-3}, {1.0f,0.0f}}, {{3,-1,3}, {1.0f,1.0f}}};
		M_ground.indices = {0, 1, 2,    1, 3, 2};
		M_ground.initMesh(this, &VD);

		//create a parallelepiped of a random color for each bar (separated by 1)
		for (int i = 0; i < csv.getNumVariables(); i++) {
			//create a parallelepiped of a random color
			float r = (float)rand() / (float)RAND_MAX;
			float g = (float)rand() / (float)RAND_MAX;
			float b = (float)rand() / (float)RAND_MAX;
			M_bars[i].vertices = {
				{{i,0,0}, {r,g,b}}, // bottom left
				{{i,0,1}, {r,g,b}}, // top left
				{{i+1,0,0}, {r,g,b}}, // bottom right
				{{i+1,0,1}, {r,g,b}}, // top right
				{{i,1,0}, {r,g,b}}, // bottom left
				{{i,1,1}, {r,g,b}}, // top left
				{{i+1,1,0}, {r,g,b}}, // bottom right
				{{i+1,1,1}, {r,g,b}} // top right
				
			};
			M_bars[i].indices = {
				0, 1, 2, 1, 3, 2, // bottom
				0, 1, 4, 1, 5, 4, // left
				2, 3, 6, 3, 7, 6, // right
				4, 5, 6, 5, 7, 6, // top
				1, 3, 5, 3, 7, 5, // front
				0, 2, 4, 2, 6, 4 // back
			};
			M_bars[i].initMesh(this, &VD_bar);
		}

		
		// Create the textures
		// The second parameter is the file name
		T.init(this,   "textures/Checker.png");
		
		// Init local variables
		CamH = 1.0f;
		CamRadius = 3.0f;
		CamPitch = glm::radians(15.f);
		CamYaw = glm::radians(30.f);
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		P.create();

		// Here you define the data set
		DS_ground.init(this, &DSL, {
		// the second parameter, is a pointer to the Uniform Set Layout of this set
		// the last parameter is an array, with one element per binding of the set.
		// first  elmenet : the binding number
		// second element : UNIFORM or TEXTURE (an enum) depending on the type
		// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
		// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
					{0, UNIFORM, sizeof(UniformBlock), nullptr},
					{1, TEXTURE, 0, &T}
				});
		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
			});
		

		P_bar.create();
		for (int i = 0; i < csv.getNumVariables(); i++) {
			DS_bars[i].init(this, &DSL_bar, {
					{0, UNIFORM, sizeof(UniformBlock), nullptr}
				});
		}
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		P.cleanup();

		// Cleanup datasets
		DS_ground.cleanup();

		P_bar.cleanup();
		for (int i = 0; i < csv.getNumVariables(); i++) {
			DS_bars[i].cleanup();
		}
		DSGubo.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		T.cleanup();
		
		// Cleanup models
		M_ground.cleanup();
		for (int i = 0; i < csv.getNumVariables(); i++) {
			M_bars[i].cleanup();
		}	
		
		// Cleanup descriptor set layouts
		DSL.cleanup();
		DSL_bar.cleanup();
		
		// Destroies the pipelines
		P.destroy();
		P_bar.destroy();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// binds the pipeline
		P.bind(commandBuffer);
		// For a pipeline object, this command binds the corresponing pipeline to the command buffer passed in its parameter

		// binds the data set
		DS_ground.bind(commandBuffer, P, 0, currentImage);
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
		for (int i = 0; i < csv.getNumVariables(); i++) {
			DS_bars[i].bind(commandBuffer, P_bar, 0, currentImage);
			M_bars[i].bind(commandBuffer);
			vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(M_bars[i].indices.size()), 1, 0, 0, 0);
		}

		DSGubo.bind(commandBuffer, P, 1, currentImage);
	
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		// Standard procedure to quit when the ESC key is pressed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		// Integration with the timers and the controllers
		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		// getSixAxis() is defined in Starter.hpp in the base class.
		// It fills the float point variable passed in its first parameter with the time
		// since the last call to the procedure.
		// It fills vec3 in the second parameters, with three values in the -1,1 range corresponding
		// to motion (with left stick of the gamepad, or ASWD + RF keys on the keyboard)
		// It fills vec3 in the third parameters, with three values in the -1,1 range corresponding
		// to motion (with right stick of the gamepad, or Arrow keys + QE keys on the keyboard, or mouse)
		// If fills the last boolean variable with true if fire has been pressed:
		//          SPACE on the keyboard, A or B button on the Gamepad, Right mouse button

		
		// Parameters
		// Camera FOV-y, Near Plane and Far Plane
		const float FOVy = glm::radians(90.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.0f;
		const float rotSpeed = glm::radians(90.0f);
		const float movSpeed = 1.0f;

		CamH += m.z * movSpeed * deltaT;
		CamRadius -= m.x * movSpeed * deltaT;
		CamPitch -= r.x * rotSpeed * deltaT;
		CamYaw += r.y * rotSpeed * deltaT;


		glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Prj[1][1] *= -1;
		glm::vec3 camTarget = glm::vec3(0, CamH, 0);
		glm::vec3 camPos = camTarget +
			CamRadius * glm::vec3(cos(CamPitch) * sin(CamYaw),
				sin(CamPitch),
				cos(CamPitch) * cos(CamYaw));
		glm::mat4 View = glm::lookAt(camPos, camTarget, glm::vec3(0,1,0));

		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = camPos;

		// Writes value to the GPU
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);

		

		static float L_time = 0.0;
		L_time += 0.001;

		glm::mat4 World = glm::mat4(1);		

		ubo_ground.mvpMat = Prj * View * World;
		ubo_ground.height=1;
		DS_ground.map(currentImage, &ubo_ground, sizeof(ubo_ground), 0);
		// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
		// the second parameter is the pointer to the C++ data structure to transfer to the GPU
		// the third parameter is its size
		// the fourth parameter is the location inside the descriptor set of this uniform block

		for (int i = 0; i < csv.getNumVariables(); i++) {
			ubo_bars[i].mvpMat = Prj * View * World;
			ubo_bars[i].height = 1+fmod(L_time*100,i)/10;
			DS_bars[i].map(currentImage, &ubo_bars[i], sizeof(ubo_bars[i]), 0);
		}
	}	
};


// This is the main: probably you do not need to touch this!
int main() {
    CSVReader csv("credit_evolution.csv");
    SimpleCube app(csv);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
