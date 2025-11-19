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

// 修改 uploadData：返回 Buffer 指针，并修正字节大小
SDL_GPUBuffer* uploadData(SDL_GPUDevice* device) {
    float datas[] = {
        // x, y, r, g, b
        0.0f,  0.5f, 1.0f, 0.0f, 0.0f, // 顶点 1
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 顶点 2
       -0.5f, -0.5f, 0.0f, 0.0f, 1.0f  // 顶点 3
    };
    Uint32 dataSize = sizeof(datas); // 修正：使用字节大小

    // 1. 创建传输缓冲 (CPU -> GPU)
    SDL_GPUTransferBufferCreateInfo tinfo;
    SDL_zero(tinfo);
    tinfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    tinfo.size = dataSize;
    SDL_GPUTransferBuffer* staging = SDL_CreateGPUTransferBuffer(device, &tinfo);

    // 2. 映射内存并拷贝数据
    void* ptr = SDL_MapGPUTransferBuffer(device, staging, false);
    memcpy(ptr, datas, dataSize);
    SDL_UnmapGPUTransferBuffer(device, staging);

    // 3. 创建顶点缓冲 (GPU VRAM)
    SDL_GPUBufferCreateInfo vbinfo;
    SDL_zero(vbinfo);
    vbinfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vbinfo.size = dataSize;
    SDL_GPUBuffer* vertexBuffer = SDL_CreateGPUBuffer(device, &vbinfo);

    // 4. 发送拷贝命令
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);

    SDL_GPUTransferBufferLocation src;
    SDL_zero(src);
    src.transfer_buffer = staging;

    SDL_GPUBufferRegion dst;
    SDL_zero(dst);
    dst.buffer = vertexBuffer;
    dst.size = dataSize;

    SDL_UploadToGPUBuffer(copy, &src, &dst, false);
    SDL_EndGPUCopyPass(copy);
    SDL_SubmitGPUCommandBuffer(cmd);

    // 5. 释放临时传输缓冲
    SDL_ReleaseGPUTransferBuffer(device, staging);

    return vertexBuffer;
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) { return -1; }

    SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
    SDL_Window* window = SDL_CreateWindow("SDL3 Triangle", WIDTH, HEIGHT, 0);
    SDL_ClaimWindowForGPUDevice(device, window);

    // 1. 加载着色器
    SDL_GPUShader* shaderVert = buildShader(device, "glsl/triangle.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_GPUShader* shaderFrag = buildShader(device, "glsl/triangle.frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT);

    // 2. 创建并上传顶点数据
    SDL_GPUBuffer* vertexBuffer = uploadData(device);

    // 3. 创建图形管线 (最重要的部分)
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo;
    SDL_zero(pipelineInfo);

    // 指定着色器
    pipelineInfo.vertex_shader = shaderVert;
    pipelineInfo.fragment_shader = shaderFrag;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    // 配置颜色输出目标 (Color Target)
    // 必须获取窗口交换链的格式，否则管线不匹配会报错
    SDL_GPUColorTargetDescription color_target_desc;
    SDL_zero(color_target_desc);
    color_target_desc.format = SDL_GetGPUSwapchainTextureFormat(device, window);

    pipelineInfo.target_info.color_target_descriptions = &color_target_desc;
    pipelineInfo.target_info.num_color_targets = 1;

    // 配置顶点输入布局 (Vertex Input State)
    SDL_GPUVertexAttribute attributes[2];
    SDL_zero(attributes);

    // 属性 0: 位置 (Location 0 in shader), float2, 偏移 0
    attributes[0].location = 0;
    attributes[0].buffer_slot = 0;
    attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[0].offset = 0;

    // 属性 1: 颜色 (Location 1 in shader), float3, 偏移 2*float
    attributes[1].location = 1;
    attributes[1].buffer_slot = 0;
    attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    attributes[1].offset = sizeof(float) * 2;

    SDL_GPUVertexBufferDescription vertex_buffer_desc;
    SDL_zero(vertex_buffer_desc);
    vertex_buffer_desc.slot = 0;
    vertex_buffer_desc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertex_buffer_desc.pitch = sizeof(float) * 5; // 步长: 每个顶点 5 个 float

    pipelineInfo.vertex_input_state.vertex_attributes = attributes;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &vertex_buffer_desc;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

    // 着色器创建完管线后就可以释放了
    SDL_ReleaseGPUShader(device, shaderVert);
    SDL_ReleaseGPUShader(device, shaderFrag);

    // 4. 主循环
    bool quit = false;
    while (!quit) {
        quit = handleEvent();

        // 获取命令缓冲
        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
        SDL_GPUTexture* swapchainTexture;

        // 获取屏幕纹理 (如果获取失败，比如窗口最小化，则跳过绘制)
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchainTexture, NULL, NULL)) {
            SDL_SubmitGPUCommandBuffer(cmd);
            continue;
        }

        // 开始渲染通道
        SDL_GPUColorTargetInfo color_target;
        SDL_zero(color_target);
        color_target.texture = swapchainTexture;
        color_target.clear_color = { 0.1f, 0.1f, 0.1f, 1.0f }; // 深灰色背景
        color_target.load_op = SDL_GPU_LOADOP_CLEAR;
        color_target.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, NULL);

        // 绑定管线和数据并绘制
        SDL_BindGPUGraphicsPipeline(pass, pipeline);

        SDL_GPUBufferBinding binding;
        binding.buffer = vertexBuffer;
        binding.offset = 0;
        SDL_BindGPUVertexBuffers(pass, 0, &binding, 1);

        SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0); // 画3个顶点

        SDL_EndGPURenderPass(pass);
        SDL_SubmitGPUCommandBuffer(cmd);
    }

    // 5. 清理资源
    SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
    SDL_ReleaseGPUBuffer(device, vertexBuffer);
    SDL_ReleaseWindowFromGPUDevice(device, window);
    SDL_DestroyWindow(window);
    SDL_DestroyGPUDevice(device);
    SDL_Quit();

    return 0;
}