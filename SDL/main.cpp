#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

constexpr Uint64 WIDTH = 1600;
constexpr Uint64 HEIGHT = 1200;

SDL_GPUShader* buildShader(SDL_GPUDevice* device, const char* path, const SDL_GPUShaderStage stage) {
	SDL_IOStream* f = SDL_IOFromFile(path, "rb");
	if (!f) {
		SDL_Log("Failed to open %s", path);
		return {};
	}

	Sint64 size = SDL_GetIOSize(f);
	std::vector<Uint8> data(size);
	SDL_ReadIO(f, data.data(), size);
	SDL_CloseIO(f);
	SDL_GPUShaderCreateInfo info;
	SDL_zero(info);
	info.format = SDL_GPU_SHADERFORMAT_SPIRV;
	info.stage = stage;
	info.code = data.data();
	info.code_size = data.size();
	SDL_GPUShader* shader = nullptr;
	shader = SDL_CreateGPUShader(device, &info);
	if (!shader) {
		SDL_Log(SDL_GetError());
		return nullptr;
	}
	return shader;
}

SDL_GPUBuffer* uploadData(SDL_GPUDevice* device) {
	float datas[15] = {
	   0.0f,  0.5f, 1.0f, 0.0f, 0.0f,
	   0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	   -0.5f, -0.5f, 0.0f, 0.0f, 1.0f
	};

	SDL_GPUTransferBufferCreateInfo tinfo;
	tinfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	tinfo.size = 15 * sizeof(float);
	SDL_GPUTransferBuffer* staging = SDL_CreateGPUTransferBuffer(device, &tinfo);

	SDL_GPUBufferCreateInfo vbinfo;
	vbinfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
	vbinfo.size = 15 * sizeof(float);
	SDL_GPUBuffer* vertexBuffer = SDL_CreateGPUBuffer(device, &vbinfo);

	void* ptr = SDL_MapGPUTransferBuffer(device, staging, false);
	memcpy(ptr, datas, 15 * sizeof(float));
	SDL_UnmapGPUTransferBuffer(device, staging);

	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTransferBufferLocation src;
	src.transfer_buffer = staging;
	src.offset = 0;
	SDL_GPUBufferRegion dst;
	dst.buffer = vertexBuffer;
	dst.offset = 0;
	dst.size = 15 * sizeof(float);
	SDL_UploadToGPUBuffer(copy, &src, &dst, false);
	SDL_EndGPUCopyPass(copy);
	SDL_SubmitGPUCommandBuffer(cmd);
	return vertexBuffer;
}

bool handleEvent() {
	static SDL_Event event;
	static bool quit = false;
	SDL_zero(event);
	SDL_PollEvent(&event);
	switch (event.type) {
		case SDL_EVENT_QUIT:
			quit = true;
			break;
		default:
			break;
	}
	return quit;
}

int main(int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cout << "SDL init failed" << std::endl;
		exit(-1);
	}
	SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, "vulkan");
	if (!device) {
		SDL_Log(SDL_GetError());
		SDL_Quit();
		exit(-1);
	}
	SDL_Window* window = SDL_CreateWindow("SDL GPU Render", WIDTH, HEIGHT, NULL);
	SDL_ClaimWindowForGPUDevice(device, window);
	
	SDL_GPUShader* shaderVert = buildShader(device, "glsl/triangle.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX);
	SDL_GPUShader* shaderFrag = buildShader(device, "glsl/triangle.frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT);
	if (!shaderFrag || !shaderVert) {
		SDL_DestroyWindow(window);
		SDL_DestroyGPUDevice(device);
		SDL_Quit();
	}

	uploadData(device);



	SDL_Event event;
	SDL_zero(event);
	bool quit = false;
	while (!quit) {
		quit = handleEvent();
	}
	return 0;
}