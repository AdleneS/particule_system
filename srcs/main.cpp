#include "../headers/vox.hpp"

// const char *cSourceCL = "__kernel void sine_wave(__global float4* pos, unsigned int width, unsigned int height, float time)\n"
//						"{\n"
//						"unsigned int x = get_global_id(0);\n"
//						"unsigned int y = get_global_id(1);\n"
//						"float u = x / (float) width;\n"
//						"float v = y / (float) height;\n"
//						"u = u*2.0f - 1.0f;\n"
//						"v = v*2.0f - 1.0f;\n"
//						"float freq = 4.0f;\n"
//						"float w = sin(u*freq + time) * cos(v*freq + time) * 0.5f;\n"
//						"pos[y*width+x] = (float4)(u, w, v, 1.0f);\n"
//						"}\n";

static void error_callback(int error, const char *description)
{
	(void)error;
	fputs(description, stderr);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(void)
{
	GLFWwindow *window;
	GLFWmonitor *primary;

	unsigned int nr_particles = 500;

	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwSetErrorCallback(error_callback);
	primary = glfwGetPrimaryMonitor();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	const GLFWvidmode *mode = glfwGetVideoMode(primary);
	window = glfwCreateWindow(mode->width, mode->height, "ft_vox", primary, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	gl3wInit();
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	Shader shader("./shaders/vertex.glsl", "./shaders/fragment.glsl");

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);

	shader.use();

	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_BLEND);
	//  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//   Get the NVIDIA platform
	ciErrNum = getPlatformID(&cpPlatform);

	// Get the number of GPU devices available to the platform
	ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiDevCount);

	// Create the device list
	cdDevices = new cl_device_id[uiDevCount];
	ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, uiDevCount, cdDevices, NULL);

	// Get device requested on command line, if any
	unsigned int uiDeviceUsed = 0;
	unsigned int uiEndDev = uiDevCount - 1;

	bool bSharingSupported = false;
	for (unsigned int i = uiDeviceUsed; (!bSharingSupported && (i <= uiEndDev)); ++i)
	{
		size_t extensionSize;
		ciErrNum = clGetDeviceInfo(cdDevices[i], CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize);
		if (extensionSize > 0)
		{
			char *extensions = (char *)malloc(extensionSize);
			ciErrNum = clGetDeviceInfo(cdDevices[i], CL_DEVICE_EXTENSIONS, extensionSize, extensions, &extensionSize);
			std::string stdDevString(extensions);
			free(extensions);

			size_t szOldPos = 0;
			size_t szSpacePos = stdDevString.find(' ', szOldPos); // extensions string is space delimited
			while (szSpacePos != stdDevString.npos)
			{
				if (strcmp(GL_SHARING_EXTENSION, stdDevString.substr(szOldPos, szSpacePos - szOldPos).c_str()) == 0)
				{
					// Device supports context sharing with OpenGL
					uiDeviceUsed = i;
					bSharingSupported = true;
					break;
				}
				do
				{
					szOldPos = szSpacePos + 1;
					szSpacePos = stdDevString.find(' ', szOldPos);
				} while (szSpacePos == szOldPos);
			}
		}
	}

// Define OS-specific context properties and create the OpenCL context
#if defined(__APPLE__)
	CGLContextObj kCGLContext = CGLGetCurrentContext();
	CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
	cl_context_properties props[] =
		{
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
			0};
	cxGPUContext = clCreateContext(props, 0, 0, NULL, NULL, &ciErrNum);
#else
#ifdef UNIX
	cl_context_properties props[] =
		{
			CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
			CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
			CL_CONTEXT_PLATFORM, (cl_context_properties)cpPlatform,
			0};
	cxGPUContext = clCreateContext(props, 1, &cdDevices[uiDeviceUsed], NULL, NULL, &ciErrNum);
#else // Win32
	cl_context_properties props[] =
		{
			CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
			CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
			CL_CONTEXT_PLATFORM, (cl_context_properties)cpPlatform,
			0};
	cxGPUContext = clCreateContext(props, 1, &cdDevices[uiDeviceUsed], NULL, NULL, &ciErrNum);
#endif
#endif

	size_t program_length;
	cqCommandQueue = clCreateCommandQueue(cxGPUContext, cdDevices[uiDeviceUsed], 0, &ciErrNum);
	cSourceCL = oclLoadProgSource("./resources/simple.cl", "", &program_length);

	// create the program
	cpProgram = clCreateProgramWithSource(cxGPUContext, 1,
										  (const char **)&cSourceCL, &program_length, &ciErrNum);

	// build the program
	ciErrNum = clBuildProgram(cpProgram, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);

	// create the kernel
	ckKernel = clCreateKernel(cpProgram, "sine_wave", &ciErrNum);

	// create VBO (if using standard GL or CL-GL interop), otherwise create Cl buffer
	unsigned int size = mesh_width * mesh_height * 4 * sizeof(float);

	// create buffer object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	// initialize buffer object
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);

	// create OpenCL buffer from GL VBO
	vbo_cl = clCreateFromGLBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, vbo, NULL);
	// vbo_cl = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, size, NULL, &ciErrNum);

	// set the args values
	ciErrNum = clSetKernelArg(ckKernel, 0, sizeof(cl_mem), (void *)&vbo_cl);
	ciErrNum |= clSetKernelArg(ckKernel, 1, sizeof(unsigned int), &mesh_width);
	ciErrNum |= clSetKernelArg(ckKernel, 2, sizeof(unsigned int), &mesh_height);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		runKernel();
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPointSize(1);

		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		shader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
		// shader.setVec3("viewPos", camera.Position);
		//  shader.setMat4("model", buf.mat);

		// glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * mesh_width * mesh_height, NULL, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		//   printf("%d\n", vbo);
		//  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &pos[0], GL_STATIC_DRAW);
		//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		//  shader.setMat4("model", buf.mat);
		glBindVertexArray(vao);
		//  glDrawArrays(GL_POINTS, 0, 1);
		// glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, mesh_width * mesh_height);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	stop = true;

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.MovementSpeed = 20;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		camera.MovementSpeed = 10;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		camera.MovementSpeed = 50;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
		camera.MovementSpeed = 10;
	static int oldState = GLFW_PRESS;
	int newState = glfwGetKey(window, GLFW_KEY_E);
	if (newState == GLFW_PRESS && oldState == GLFW_RELEASE)
	{
		light = light ? false : true;
	}
	oldState = newState;
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	(void)window;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	(void)window;
	(void)xoffset;
	camera.ProcessMouseScroll(yoffset);
}

void Cleanup(int iExitCode)
{
	// Cleanup allocated objects
	if (ckKernel)
		clReleaseKernel(ckKernel);
	if (cpProgram)
		clReleaseProgram(cpProgram);
	if (cqCommandQueue)
		clReleaseCommandQueue(cqCommandQueue);
	if (vbo)
	{
		glBindBuffer(1, vbo);
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
	if (vbo_cl)
		clReleaseMemObject(vbo_cl);
	if (cxGPUContext)
		clReleaseContext(cxGPUContext);
	if (cPathAndName)
		free(cPathAndName);
	// if (cSourceCL)
	//	free(cSourceCL);
	if (cdDevices)
		delete (cdDevices);

	exit(iExitCode);
}

cl_int getPlatformID(cl_platform_id *clSelectedPlatformID)
{
	char chBuffer[1024];
	cl_uint num_platforms;
	cl_platform_id *clPlatformIDs;
	cl_int ciErrNum;
	*clSelectedPlatformID = NULL;

	// Get OpenCL platform count
	ciErrNum = clGetPlatformIDs(0, NULL, &num_platforms);
	if (ciErrNum != CL_SUCCESS)
	{
		return -1000;
	}
	else
	{
		if (num_platforms == 0)
		{
			return -2000;
		}
		else
		{
			// if there's a platform or more, make space for ID's
			if ((clPlatformIDs = (cl_platform_id *)malloc(num_platforms * sizeof(cl_platform_id))) == NULL)
			{
				return -3000;
			}

			ciErrNum = clGetPlatformIDs(num_platforms, clPlatformIDs, NULL);
			for (cl_uint i = 0; i < num_platforms; ++i)
			{
				ciErrNum = clGetPlatformInfo(clPlatformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
				if (ciErrNum == CL_SUCCESS)
				{
					if (strstr(chBuffer, "NVIDIA") != NULL)
					{
						*clSelectedPlatformID = clPlatformIDs[i];
						break;
					}
				}
			}

			// default to zeroeth platform if NVIDIA not found
			if (*clSelectedPlatformID == NULL)
			{
				*clSelectedPlatformID = clPlatformIDs[0];
			}

			free(clPlatformIDs);
		}
	}

	return CL_SUCCESS;
}

void runKernel()
{
	ciErrNum = CL_SUCCESS;

	// map OpenGL buffer object for writing from OpenCL
	glFinish();
	ciErrNum = clEnqueueAcquireGLObjects(cqCommandQueue, 1, &vbo_cl, 0, 0, 0);

	// Set arg 3 and execute the kernel
	ciErrNum = clSetKernelArg(ckKernel, 3, sizeof(float), &lastFrame);
	ciErrNum |= clEnqueueNDRangeKernel(cqCommandQueue, ckKernel, 2, NULL, szGlobalWorkSize, NULL, 0, 0, 0);

	// unmap buffer object
	ciErrNum = clEnqueueReleaseGLObjects(cqCommandQueue, 1, &vbo_cl, 0, 0, 0);
	clFinish(cqCommandQueue);
}

char *oclLoadProgSource(const char *cFilename, const char *cPreamble, size_t *szFinalLength)
{
	// locals
	FILE *pFileStream = NULL;
	size_t szSourceLength;

// open the OpenCL source code file
#ifdef _WIN32 // Windows version
	if (fopen_s(&pFileStream, cFilename, "rb") != 0)
	{
		return NULL;
	}
#else // Linux version
	pFileStream = fopen(cFilename, "rb");
	if (pFileStream == 0)
	{
		return NULL;
	}
#endif

	size_t szPreambleLength = strlen(cPreamble);

	// get the length of the source code
	fseek(pFileStream, 0, SEEK_END);
	szSourceLength = ftell(pFileStream);
	fseek(pFileStream, 0, SEEK_SET);

	// allocate a buffer for the source code string and read it in
	char *cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1);
	memcpy(cSourceString, cPreamble, szPreambleLength);
	if (fread((cSourceString) + szPreambleLength, szSourceLength, 1, pFileStream) != 1)
	{
		fclose(pFileStream);
		free(cSourceString);
		return 0;
	}

	// close the file and return the total length of the combined (preamble + source) string
	fclose(pFileStream);
	if (szFinalLength != 0)
	{
		*szFinalLength = szSourceLength + szPreambleLength;
	}
	cSourceString[szSourceLength + szPreambleLength] = '\0';
	return cSourceString;
}