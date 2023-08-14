// This has been adapted from the Vulkan tutorial

#include "Starter.hpp"
#include "CSVReader.hpp"

struct coordinates {
	float x;
	float z;
};

struct coordinates * bar_coordinates;
float up_coordinates = 47.2f, sx_coordinates = 6.0f, dx_coordinates = 19.0f, down_coordinates = 35.0f;
float zoom = 5.f;

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
	alignas(16) glm::mat4 mvpMat;
};

struct GlobalUniformBlock {
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};

struct OverlayUniformBlock {
	alignas(4) float visible;
};


// The vertices data structures
// Example
struct VertexGround {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 UV;
};

struct VertexBar {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 colour;
};

struct VertexOverlay {
	glm::vec2 pos;
	glm::vec2 UV;
};

// MAIN ! 
class BarChart : public BaseProject {
	public:
	BarChart(const CSVReader& csv) : BaseProject(), csv(csv) {
		M_bars = new Model<VertexBar>[csv.getNumVariables()];
		DS_bars = new DescriptorSet[csv.getNumVariables()];
		ubo_bars = new UniformBlock[csv.getNumVariables()];
	}

	~BarChart() {
        // Deallocate the memory used by the models array
        delete[] M_bars;
		delete[] DS_bars;
		delete[] ubo_bars;
    }

	protected:
	CSVReader csv;
	float *visualizedValues;
	// Current aspect ratio (used by the callback that resized the window
	float Ar;

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSL_ground;
	DescriptorSetLayout DSL_bar;
	DescriptorSetLayout DSLGubo, DSLOverlay;



	// Vertex formats
	VertexDescriptor VD_ground;
	VertexDescriptor VD_bar;
	VertexDescriptor VOverlay;

	// Pipelines [Shader couples]
	Pipeline P_ground;
	Pipeline P_bar;
	Pipeline POverlay;

	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	// Models
	Model<VertexGround> M_ground;
	Model<VertexBar> * M_bars;
	Model<VertexOverlay> MOverlay;


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
	OverlayUniformBlock uboOverlay;

	// Other application parameters
	float CamH, CamRadius, CamPitch, CamYaw, targtH;


	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Bar Chart";
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
		DSL_ground.init(this, {
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
		DSLOverlay.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});

		VD_bar.init(this, {
				  {0, sizeof(VertexBar), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexBar, pos), sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexBar, normal), sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexBar, colour), sizeof(glm::vec3), COLOR}
				});

		// Vertex descriptors
		VD_ground.init(this, {
				  // this array contains the bindings
				  // first  element : the binding number
				  // second element : the stride of this binging
				  // third  element : whether this parameter change per vertex or per instance
				  //                  using the corresponding Vulkan constant
				  {0, sizeof(VertexGround), VK_VERTEX_INPUT_RATE_VERTEX}
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
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexGround, pos), sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexGround, normal), sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexGround, UV), sizeof(glm::vec2), UV}
				});
		
		VOverlay.init(this, {
				  {0, sizeof(VertexOverlay), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, pos),
					 sizeof(glm::vec2), OTHER},
			  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, UV),
					 sizeof(glm::vec2), UV}
			});

		// Pipelines [Shader couples]
		// The second parameter is the pointer to the vertex definition
		// Third and fourth parameters are respectively the vertex and fragment shaders
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		P_ground.init(this, &VD_ground, "shaders/ShaderVert.spv", "shaders/ShaderFrag.spv", {&DSL_ground, &DSLGubo});

		P_bar.init(this, &VD_bar, "shaders/ShaderBarVert.spv", "shaders/ShaderBarFrag.spv", {&DSL_bar, &DSLGubo});

		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", { &DSLOverlay });
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		// The second parameter is the pointer to the vertex definition for this model
		// The third parameter is the file name
		// The last is a constant specifying the file type: currently only OBJ or GLTF
		
		// Creates a mesh with direct enumeration of vertices and indices
		float start = - csv.getNumVariables()/2.f;
		float xL = (dx_coordinates - sx_coordinates)*zoom;
		float zL = (down_coordinates - up_coordinates)*zoom;
		M_ground.vertices = {
						{{-xL/2,-0.5,-zL/2}, {0.f, -1.f, 0.f}, {0.0f,0.0f}},
						{{-xL/2,-0.5,zL/2}, {0.f, -1.f, 0.f}, {1.0f,0.0f}},
					    {{xL/2,-0.5,-zL/2}, {0.f, -1.f, 0.f}, {0.0f,1.0f}},
						{{xL/2,-0.5,zL/2}, {0.f, -1.f, 0.f}, {1.0f,1.0f}}
		};
		M_ground.indices = {0, 1, 2, 1, 3, 2};
		M_ground.initMesh(this, &VD_ground);

		//create parallelepipeds for bars
		
		for (int i = 0; i < csv.getNumVariables(); i++) {
			float left = bar_coordinates[i].x - 0.5;
			float right = bar_coordinates[i].x + 0.5;
			float up = bar_coordinates[i].z - 0.5;
			float down = bar_coordinates[i].z + 0.5;

			//create a parallelepiped of a random color
			float r = (float)rand() / (float)RAND_MAX;
			float g = (float)rand() / (float)RAND_MAX;
			float b = (float)rand() / (float)RAND_MAX;
			// add the vertices putting position, normal (replicated vertices) and color
			M_bars[i].vertices = {
				// bottom face
				{{left,0,up}, {0, -1, 0}, {r,g,b}},
				{{left,0,down}, {0, -1, 0}, {r,g,b}},
				{{right,0,up}, {0, -1, 0}, {r,g,b}},
				{{right,0,down}, {0, -1, 0}, {r,g,b}},
				// top face
				{{left,1,up}, {0, 1, 0}, {r,g,b}},
				{{left,1,down}, {0, 1, 0}, {r,g,b}},
				{{right,1,up}, {0, 1, 0}, {r,g,b}},
				{{right,1,down}, {0, 1, 0}, {r,g,b}},
				// left face
				{{left,0,up}, {-1, 0, 0}, {r,g,b}},
				{{left,0,down}, {-1, 0, 0}, {r,g,b}},
				{{left,1,up}, {-1, 0, 0}, {r,g,b}},
				{{left,1,down}, {-1, 0, 0}, {r,g,b}},
				// right face
				{{right,0,up}, {1, 0, 0}, {r,g,b}},
				{{right,0,down}, {1, 0, 0}, {r,g,b}},
				{{right,1,up}, {1, 0, 0}, {r,g,b}},
				{{right,1,down}, {1, 0, 0}, {r,g,b}},
				// front face
				{{left,0,down}, {0, 0, 1}, {r,g,b}},
				{{right,0,down}, {0, 0, 1}, {r,g,b}},
				{{left,1,down}, {0, 0, 1}, {r,g,b}},
				{{right,1,down}, {0, 0, 1}, {r,g,b}},
				// back face
				{{left,0,up}, {0, 0, -1}, {r,g,b}},
				{{right,0,up}, {0, 0, -1}, {r,g,b}},
				{{left,1,up}, {0, 0, -1}, {r,g,b}},
				{{right,1,up}, {0, 0, -1}, {r,g,b}}
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

		
		// Create the textures
		// The second parameter is the file name
		T.init(this,   "textures/map - up sx 47.2,6.0.png");
		
		// Init local variables
		CamH = 0.0f;
		targtH = 0.0f;
		CamRadius = 13.0f;
		CamPitch = 0.53f;
		CamYaw = 2.7f;

		visualizedValues = (float *)malloc(csv.getNumVariables()*sizeof(float));
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
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
		P_ground.cleanup();

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
		DSL_ground.cleanup();
		DSL_bar.cleanup();
		DSLGubo.cleanup();
		
		// Destroies the pipelines
		P_ground.destroy();
		P_bar.destroy();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		DSGubo.bind(commandBuffer, P_bar, 1, currentImage);
		// binds the pipeline
		P_ground.bind(commandBuffer);
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
		for (int i = 0; i < csv.getNumVariables(); i++) {
			DS_bars[i].bind(commandBuffer, P_bar, 0, currentImage);
			M_bars[i].bind(commandBuffer);
			vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(M_bars[i].indices.size()), 1, 0, 0, 0);
		}
	
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

		static bool isAutoRotationEnabled = false;
		static bool wasAutoRotationPressed = false;
		bool isAutoRotationPressed = false; 
		
		getSixAxis(deltaT, m, r, isAutoRotationPressed);
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

		
        // print the controls (m variable values)
		std::cout << m.x << " " << m.y << " " << m.z << std::endl;
		std::cout << r.x << " " << r.y << " " << r.z << std::endl;


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
        std::cout << CamPitch << " " << CamYaw << std::endl;



		glm::mat4 View = glm::lookAt(camPos, camTarget, glm::vec3(0,1,0));

		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = camPos;

		// Writes value to the GPU
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);


		glm::mat4 World = glm::mat4(1);		

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

		float valueTime = 0.1f;

		for (int i = 0; i < csv.getNumVariables(); i++) {
			float prevValue = line==0?0:std::stof(csv.getLine(line-1)[i]);
			float value = std::stof(csv.getLine(line)[i]);

			visualizedValues[i] = prevValue +
				(value - prevValue) * (animationTime / valueTime);
		}
		
		
		// every valueTime seconds, we change the line of the csv file to be read and therefore update the bars
		if(time >= valueTime) {
			time = 0;
			line++;
			animationTime = 0;
			if(line >= csv.getNumLines())
				line = 0; //qua potremmo mostrare un messaggio sull'overlay che dice premi "tasto" per ricominciare
		}

		for (int i = 0; i < csv.getNumVariables(); i++) {
			World = getWorldMatrixBar(visualizedValues[i]);
			ubo_bars[i].mvpMat = Prj * View * World;
			DS_bars[i].map(currentImage, &ubo_bars[i], sizeof(ubo_bars[i]), 0);
		}
		printf("\ntime: %f\nline: %d\n", time, line);
		printf("cam pitch: %f\ncam yaw: %f\n", CamPitch, CamYaw);
	}

	glm::mat4 getWorldMatrixBar(float height) {
		height = height * 0.0001f;
		glm::mat4 World =  glm::scale(glm::mat4(1), glm::vec3(1.f, height, 1.f));
		return World;
	}	

};


// This is the main: probably you do not need to touch this!
int main() {
    CSVReader csv("data/cases_by_region.csv");
	CSVReader csv_coordinates("data/region_coordinates.csv");
	

	bar_coordinates = new coordinates[csv_coordinates.getNumLines()];
	for (int i = 0; i < csv_coordinates.getNumLines(); i++) {
		bar_coordinates[i].x = ((std::stof(csv_coordinates.getLine(i)[2]) - up_coordinates - (down_coordinates - up_coordinates) / 2))*zoom;
		bar_coordinates[i].z = ((std::stof(csv_coordinates.getLine(i)[3]) - sx_coordinates - (dx_coordinates - sx_coordinates) / 2))*zoom;
		printf("x: %f, z: %f\n", bar_coordinates[i].x, bar_coordinates[i].z);
	}
	getchar();

    BarChart app(csv);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
