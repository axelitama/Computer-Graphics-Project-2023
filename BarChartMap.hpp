#ifndef BARCHARTMAP_HPP
#define BARCHARTMAP_HPP

#include "BarChart.hpp"
#include "CSVReader.hpp"

class BarChartMap : public BarChart {
    public:

        BarChartMap(const CSVReader& csv, const CSVReader& csv_coordinates, float up, float sx, float dx, float down, const float zoom);

    protected:

		struct VertexTexture {
			glm::vec3 pos;
			glm::vec3 normal;
			glm::vec2 UV;
		};

		// Models, textures and Descriptors (values assigned to the uniforms)
		// Please note that Model objects depends on the corresponding vertex structure
		// Models
		Model<VertexTexture> M_ground;

		// Textures
		Texture T;

        struct coordinates * bar_coordinates;
        float zoom;
        float latDim, lonDim;

        void localInit() override;

        void pipelinesAndDescriptorSetsInit() override;

        void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) override;

		void localCleanup() override;
};

/**************************************************
*****   NOTE: Following code should have been in BarChartMap.cpp but that will cause multiple definition problem
*****         because Starter.hpp contains definitions
**************************************************/

extern "C" {
	#include "mercator.h"
}


BarChartMap::BarChartMap(const CSVReader& csv, const CSVReader& csv_coordinates, float up, float sx, float dx, float down, const float zoom) : BarChart(csv){
    up = degreeLatitudeToY(up);
    sx = degreeLongitudeToX(sx);
    dx = degreeLongitudeToX(dx);
    down = degreeLatitudeToY(down);

    this->latDim = up - down;
    this->lonDim = dx - sx;
    this->zoom = zoom;

    bar_coordinates = new coordinates[csv_coordinates.getNumLines()];

	for (int i = 0; i < csv_coordinates.getNumLines(); i++) {
		// Converting latitude and longitude to mercator cartesian coordinates	
		bar_coordinates[i].x = degreeLatitudeToY(std::stof(csv_coordinates.getLine(i)[2]));
		bar_coordinates[i].z = degreeLongitudeToX(std::stof(csv_coordinates.getLine(i)[3]));
		// Scaling and translating the coordinates
		bar_coordinates[i].z = -zoom * (bar_coordinates[i].z - sx - (dx - sx) / 2.f);
		bar_coordinates[i].x = zoom * (up - bar_coordinates[i].x - (up - down) / 2.f);
    }
}
	
// Here you load and setup all your Vulkan Models and Texutures.
// Here you also create your Descriptor set layouts and load the shaders for the pipelines
void BarChartMap::localInit() {
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
                {0, sizeof(VertexColour), VK_VERTEX_INPUT_RATE_VERTEX}
            }, {
                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexColour, pos), sizeof(glm::vec3), POSITION},
                {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexColour, normal), sizeof(glm::vec3), NORMAL},
                {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexColour, colour), sizeof(glm::vec3), COLOR}
            });

    // Vertex descriptors
    VD_ground.init(this, {
                // this array contains the bindings
                // first  element : the binding number
                // second element : the stride of this binging
                // third  element : whether this parameter change per vertex or per instance
                //                  using the corresponding Vulkan constant
                {0, sizeof(VertexTexture), VK_VERTEX_INPUT_RATE_VERTEX}
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
                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexTexture, pos), sizeof(glm::vec3), POSITION},
                {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexTexture, normal), sizeof(glm::vec3), NORMAL},
                {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexTexture, UV), sizeof(glm::vec2), UV}
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

    //POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", { &DSLOverlay });
    //POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
        //VK_CULL_MODE_NONE, false);

    // Models, textures and Descriptors (values assigned to the uniforms)

    // Create models
    // The second parameter is the pointer to the vertex definition for this model
    // The third parameter is the file name
    // The last is a constant specifying the file type: currently only OBJ or GLTF
    
    // Creates a mesh with direct enumeration of vertices and indices
    float xL = latDim * zoom;
    float zL = lonDim * zoom;
    M_ground.vertices = {
                    {{-xL/2,-0.1,-zL/2}, {0.f, 1.f, 0.f}, {1.0f,0.0f}},
                    {{-xL/2,-0.1,zL/2}, {0.f, 1.f, 0.f}, {0.0f,0.0f}},
                    {{xL/2,-0.1,-zL/2}, {0.f, 1.f, 0.f}, {1.0f,1.0f}},
                    {{xL/2,-0.1,zL/2}, {0.f, 1.f, 0.f}, {0.0f,1.0f}}
    };
    M_ground.indices = {0, 1, 2, 1, 3, 2};
    M_ground.initMesh(this, &VD_ground);

    //create parallelepipeds for bars
    
    for (int i = 0; i < csv.getNumVariables()-1; i++) {
        float west = bar_coordinates[i].z - 0.5;
        float east = bar_coordinates[i].z + 0.5;
        float north = bar_coordinates[i].x - 0.5;
        float south = bar_coordinates[i].x + 0.5;

        //create a parallelepiped of a random color
        float r = (float)rand() / (float)RAND_MAX;
        float g = (float)rand() / (float)RAND_MAX;
        float b = (float)rand() / (float)RAND_MAX;
        // add the vertices putting position, normal (replicated vertices) and color
        M_bars[i].vertices = {
            // bottom face				
            {{south,0,west}, {0, -1, 0}, {r,g,b}},
            {{south,0,east}, {0, -1, 0}, {r,g,b}},
            {{north,0,west}, {0, -1, 0}, {r,g,b}},
            {{north,0,east}, {0, -1, 0}, {r,g,b}},
            // top face
            {{south,1,west}, {0, 1, 0}, {r,g,b}},
            {{south,1,east}, {0, 1, 0}, {r,g,b}},
            {{north,1,west}, {0, 1, 0}, {r,g,b}},
            {{north,1,east}, {0, 1, 0}, {r,g,b}},
            // south face
            {{south,0,west}, {-1, 0, 0}, {r,g,b}},
            {{south,0,east}, {-1, 0, 0}, {r,g,b}},
            {{south,1,west}, {-1, 0, 0}, {r,g,b}},
            {{south,1,east}, {-1, 0, 0}, {r,g,b}},
            // north face
            {{north,0,west}, {1, 0, 0}, {r,g,b}},
            {{north,0,east}, {1, 0, 0}, {r,g,b}},
            {{north,1,west}, {1, 0, 0}, {r,g,b}},
            {{north,1,east}, {1, 0, 0}, {r,g,b}},
            // front face
            {{south,0,east}, {0, 0, 1}, {r,g,b}},
            {{north,0,east}, {0, 0, 1}, {r,g,b}},
            {{south,1,east}, {0, 0, 1}, {r,g,b}},
            {{north,1,east}, {0, 0, 1}, {r,g,b}},
            // back face
            {{south,0,west}, {0, 0, -1}, {r,g,b}},
            {{north,0,west}, {0, 0, -1}, {r,g,b}},
            {{south,1,west}, {0, 0, -1}, {r,g,b}},
            {{north,1,west}, {0, 0, -1}, {r,g,b}}
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
    T.init(this,   "textures/map-47.5-20-34.5-5.png");
	txt.init(this, &demoText);
    
    // Init local variables
    CamH = 0.0f;
    targtH = 0.0f;
    CamRadius = 13.0f;
    CamPitch = 0.53f;
    CamYaw = 2.7f;

    visualizedValues = (float *)malloc((csv.getNumVariables()-1)*sizeof(float));
}

// Here you create your pipelines and Descriptor Sets!
void BarChartMap::pipelinesAndDescriptorSetsInit() {
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
    for (int i = 0; i < csv.getNumVariables()-1; i++) {
        DS_bars[i].init(this, &DSL_bar, {
                {0, UNIFORM, sizeof(UniformBlock), nullptr}
            });
    }
    txt.pipelinesAndDescriptorSetsInit();
}

/// NOTE: need this because parent will try to use parent M_ground
void BarChartMap::populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
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
    for (int i = 0; i < csv.getNumVariables()-1; i++) {
        DS_bars[i].bind(commandBuffer, P_bar, 0, currentImage);
        M_bars[i].bind(commandBuffer);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(M_bars[i].indices.size()), 1, 0, 0, 0);
    }   
    txt.populateCommandBuffer(commandBuffer, currentImage, 0);


}

// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
// methods: .cleanup() recreates them, while .destroy() delete them completely
void BarChartMap::localCleanup() {
    /// NOTE: can't call parent's cleanup because it will try to use parent's M_ground
    // Cleanup models
    M_ground.cleanup();
    for (int i = 0; i < csv.getNumVariables()-1; i++) {
        M_bars[i].cleanup();
    }	
    
    // Cleanup descriptor set layouts
    DSL_ground.cleanup();
    DSL_bar.cleanup();
    DSLGubo.cleanup();
    
    // Destroies the pipelines
    P_ground.destroy();
    P_bar.destroy();
    // Cleanup textures
    T.cleanup();
	txt.localCleanup();
}


#endif // BARCHARTMAP_HPP