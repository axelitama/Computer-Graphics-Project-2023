struct HudVertex {
	glm::vec2 pos;
	glm::vec2 texCoord;
};

extern std::string shaderDir;

struct HudMaker {
	VertexDescriptor VD;	
	
	BaseProject *BP;

	DescriptorSetLayout DSL;
	Pipeline P;
	Model<HudVertex> M;
	Texture T;
	DescriptorSet DS;

	UniformBlock ubo_txt;
	
	void init(BaseProject *_BP) {
		BP = _BP;
		createHudDescriptorSetAndVertexLayout();

		createHudPipeline();
		createHudModelAndTexture();
	}

	void createHudDescriptorSetAndVertexLayout() {
		VD.init(BP, {
				  {0, sizeof(HudVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(HudVertex, pos),
				         sizeof(glm::vec2), OTHER},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(HudVertex, texCoord),
				         sizeof(glm::vec2), UV}
				});
		DSL.init(BP,
				{{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
				{1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},});
	}


 	void createHudPipeline() {
		P.init(BP, &VD, shaderDir + "Hud.vert.spv", shaderDir + "Hud.frag.spv", {&DSL});
		P.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, true);
 	}
	
	void createHudModelAndTexture() {
		M.BP = BP;
		M.VD = &VD;
		createHudMesh();
		M.createVertexBuffer();
		M.createIndexBuffer();

		T.init(BP, "textures/HudObj.png");
	}

	void createHudMesh() {
		// Create the vertices
		M.vertices.push_back({{0.0f, 0.0f}, {0.0f, 0.0f}});
		M.vertices.push_back({{1.0f, 0.0f}, {1.0f, 0.0f}});
		M.vertices.push_back({{1.0f, 1.0f}, {1.0f, 1.0f}});
		M.vertices.push_back({{0.0f, 1.0f}, {0.0f, 1.0f}});
		// Create the indices
		M.indices.push_back(0);
		M.indices.push_back(1);
		M.indices.push_back(2);
		M.indices.push_back(2);
		M.indices.push_back(3);
		M.indices.push_back(0);
	}

	void createHudDescriptorSets() {
		DS.init(BP, &DSL, {
					{0, TEXTURE, 0, &T},
					{1, UNIFORM, sizeof(UniformBlock), nullptr}
				});
	}

	void pipelinesAndDescriptorSetsInit() {
		P.create();
		createHudDescriptorSets();
	}
	
	void pipelinesAndDescriptorSetsCleanup() {
		P.cleanup();
		DS.cleanup();
	}

	void localCleanup() {
		T.cleanup();
		M.cleanup();
		DSL.cleanup();
		
		P.destroy();
	}
	
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage, int curText = 0) {
    	P.bind(commandBuffer);
		M.bind(commandBuffer);
		
		DS.bind(commandBuffer, P, 0, currentImage);

		vkCmdDrawIndexed(commandBuffer,
						static_cast<uint32_t>(M.indices.size()), 1, 0, 0, 0);
			
	}

	void update(uint32_t currentImage, int h, int w){
		ubo_txt.mvpMat = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(200.0f/w, 75.0f/h, 1.0f))*
			glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, -1.0f));


		DS.map(currentImage, &ubo_txt, sizeof(ubo_txt), 1);
	}
};
    
