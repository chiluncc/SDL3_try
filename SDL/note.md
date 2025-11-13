## SDL的两套渲染管线
```c
Surface 管线（CPU渲染）:
   Surface (RAM)
      ↓  SDL_UpdateWindowSurface
   Window (显示内容)

Renderer 管线（GPU渲染）:
   Texture (VRAM)
      ↓  SDL_RenderCopy
   Back Buffer (VRAM)
      ↓  SDL_RenderPresent
   Window (显示内容)
```

|函数|作用|
|:-:|:-:|
|`SDL_UpdateWindowSurface`|把CPU内存中的surface像素拷贝到窗口显示|
|`SDL_RenderPresent`|把GPU后台缓冲区显示到窗口|

> 前者是“CPU 贴图”，后者是“GPU 翻屏”

所以，后续基本使用`Render`系统，而至于SDL1时代的遗老`Surface`则基本不使用。虽然至今依然需要使用`Surface`实现加载资源转移到GPU。

每一个`window`都有一个前缓冲区和后缓冲区，而`Render`操作的是后缓冲区，且一个`Render`和一个缓冲区或者说`window`是绑定的。