# SDL3

## 坐标系统

屏幕坐标采用`Direct`同款, 即`left-up`为原点

## SDL3的两套渲染管线

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

> 一改往期的拖沓，由于SDL3早已有了自己的wiki，且其`include`文件注释完整，故不再将大量时间耗费在感动自己的参数、功能笔记上。

## 一些常用函数

- `SDL_Delay` SDL版本的sleep，且**仅仅阻塞当前线程**。
- `SDL_Log` Log输出，接收一个`const char*`。
- `SDL_GetError` 获取最新的错误（不是队列也不是堆栈，近唯一空间）。
- `SDL_zero` 将某个对象对应空间置为全0，用于规避不同系统对于初始化的不同填充方式

## SDL3简单加载卸载

- `SDL_Init` 系统初始化
- `SDL_CreateWindow` 与SDL2不同，SDL3不再需要繁杂的初始化参数，名称，大小和功能标识就够了
- `SDL_CreateRenderer` 获取`Window`对应的`Render`。
- `SDL_PollEvent` 获取事件（使用时事件会先被底层系统处理）
- `SDL_SetRenderDrawColor` 设置清空所用颜色
- `SDL_RenderClear` 清空缓冲
- `SDL_RenderPresent` 交换缓冲
- `SDL_DestroyRenderer`
- `SDL_DestroyWindow`
- `SDL_Quit`

## 复杂图片加载

- `IMG_Load` 以`Surface`形式加载图片
- `SDL_CreateTextureFromSurface` 转化为`Texture`格式
- `SDL_DestroySurface`
- `SDL_RenderTexture` 以`Texture`填充
- `SDL_DestroyTexture`

**额外功能**

* `SDL_SetSurfaceColorKey` 设置颜色精灵（是对Surface进行操作）
* `SDL_RenderTextureRotated` 以旋转结果绘制在画板上，不是返回新的 `Texture`，先旋转再翻转，角度制