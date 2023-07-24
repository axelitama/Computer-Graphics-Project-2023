You need:

[] – 3D assets, in either OBJ or GLTF form

[] – 3D assets dynamically generated in the code

[] – Textures associated with the models


Then you decide:

[] – the illumination for the scene:

`	`[] – which type of direct light? How many ?

`	`[] – Ambient light type?

`	`[] – Any object having emission?

- These terms might be enclosed in a scene-wide DescriptorSetLayout

- For each asset
  - [] – Define which vertex format it uses
  - [] – Select a BRDF approximation and shading technique, and depending on the scene illumination, define the corresponding Vertex / Fragment shader couple
  - [] – Decide which texture it requires
  - [] – Decide which data sent from the CPP code the shaders need
    - The last two point determines the DescriptorSetLayout for the shader couple

You then:

[] – Examine how many different formats have been used by the assets

[] – How many different DescriptorSetLayout are needed

[] – How many different vertex and fragment shaders are needed

- This will also determine how many pipelines are needed

You can then:

[] – Create the Vertex formats

[] – Define the models and load them

[] – Define the texture and load them

[] – Create a DescriptorSetLayout for the scene-wide and pipeline specific uniform

[] – Create the pipelines needed

[] – For each scene-wide DescriptorSetLayout, create the corresponding DescriptorSet instance

[] – Count the required number of:

- DescriptorSets
- UniformBlocks elements of the DescriptorSets
- Texture elements of the DescriptorSets

[] – For each 3D asset, create its specific DescriptorSet according to the corresponding DescriptorSetLayout. Here is where you will define the size of the corresponding uniform, and assign the textures.

[] – In the procedure that populates the command buffer, enter the command to draw all the primitives:

`	`[] – first bind the scene-wide DescriptorSets

`	`[] – for each different pipeline:

- [] - Bind the pipeline
- [] - For each object belonging to that pipeline:
  - [] – Bind the corresponding DescriptorSet
  - [] – Bind the vertex and index buffers
  - [] – call the draw command for the corresponding mesh
- Remember: it is always easier to load all the 3D objects at the beginning, and then “hide” the ones you do not need by either giving them a zero scale, or by moving them far away from the far plane of the camera.

[] – initialize all the variables for the game logic

[] – in the procedure that handles the user interaction:

`	`[] – Read the user input (from the keyboard, the mouse or the Joystick)

`	`[] – update the camera position and direction (if needed), and the corresponding view / projection matrix

`	`[] – update the variable with the position of the objects

`	`[] – determine the new values of the uniform variable and map them




**1 - Vertex formats (C++)**

|**Name**|**Data structure**|
| :- | :- |
|||
|||



**2 - Data structures for Uniform Block Objects (C++)**

|**Name**|**Data structure**|
| :- | :- |
|||
|||


**3 - Descriptor Set Layouts**

<table><tr><th valign="top"><b>Variable</b></th><th valign="top"><b>Binding</b></th><th valign="top"><b>Type</b></th><th valign="top"><b>Which shader</b></th></tr>
<tr><td rowspan="3" valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td rowspan="3" valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
</table>


**4 - Vertex Descriptors**

<table><tr><th valign="top"><b>Variable</b></th><th valign="top"><b>Format (C++)</b></th><th valign="top"><b>Location</b></th><th valign="top"><b>Type</b></th><th valign="top"><b>Usage</b></th></tr>
<tr><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
</table>



**5 - Pipelines**

<table><tr><th valign="top"><b>Variable</b></th><th valign="top"><b>Vertex Shader</b></th><th valign="top"><b>Fragment Shader</b></th><th valign="top"><b>Vertex format (C++)</b></th><th valign="top"><b>Vertex descriptor</b></th><th valign="top"><b>Set ID</b></th><th valign="top"><b>Desriptor set Layout</b></th></tr>
<tr><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td></tr>
<tr><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td></tr>
</table>


**6 - Mesh objects**

|**Variable**|**Vertex Format (C++)**|**Vertex descriptor**|**Type**|**Model File**|
| :- | :- | :- | :- | :- |
||||||
||||||



**7 - Textures**

|**Variable**|**File**|**Sampler**|
| :- | :- | :- |
||||
||||


**8 - Uniform Blocks Objects, C++ sides**

|**Type**|**Variable**|
| :- | :- |
|||
|||


**9 - Descriptor Sets**

<table><tr><th valign="top"><b>Variable</b></th><th valign="top"><b>Descriptor Set Layout</b></th><th valign="top"><b>Binding</b></th><th valign="top"><b>Type</b></th><th valign="top"><b>C++ data structure</b></th><th valign="top"><b>Variable with values</b></th><th valign="top"><b>Texture</b></th></tr>
<tr><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td rowspan="3" valign="top"></td><td rowspan="3" valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td><td valign="top"></td></tr>
</table>


**10 - Scene Objects**

<table><tr><th valign="top"><b>ID</b></th><th valign="top"><b>Pipeline</b></th><th valign="top"><b>Mesh</b></th><th valign="top"><b>Descriptor Sets</b></th></tr>
<tr><td rowspan="2" valign="top"></td><td rowspan="2" valign="top"></td><td rowspan="2" valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td></tr>
<tr><td rowspan="2" valign="top"></td><td rowspan="2" valign="top"></td><td rowspan="2" valign="top"></td><td valign="top"></td></tr>
<tr><td valign="top"></td></tr>
</table>

