# QNX Screen Rendering

QNX Screen的渲染简单说就是生成图像的过程.
App使用Screen API可以有如下四种方式进行渲染:
* [Software 软件方式](#Software-Rendering)
* Hardware acceleration 硬件加速
* Hybrid 混合方式
* Cloning 克隆

## Render Targets
Render target(渲染目标)是指可以被导向的一个平面.Screen API里有以下render target

|Render target|Output destination|Buffers|
|--|--|--|
|Window|On-screen|Multiple|
|Stream|Off-screen|Multiple|
|Pixmap|Off-screen|Single|

**On-screen** 
1. 在显示设备上渲染的内容可见
2. Window是惟一的on-screen render target
3. Window只有在screen运行时(screen服务进程)且至少有一个dispaly设备被配置,才能被创建

**Off-screen**
1. 渲染的内容不可见,在非屏下使用
2. Pixmap和Stream都是off-screen的render target
3. 在screen运行,但没有配置display的情况下,off-screen的render target可以被创建

## Software  Rendering
使用软件渲染其实就是利用CPU来读写buffers进行渲染.

软件渲染的基本步骤如下:
1. 创建render target(window,pixmap或stream)
2. 获取到buffer的访问通道
3. 写像素数据到buffer
4. 发布内容

### Create render target
```c++
screen_context_t screen_ctx;

screen_pixmap_t screen_pix;
// set properties on this target
// ...
screen_create_pixmap(&screen_pix, screen_ctx);

screen_window_t screen_win;
// set properties on this target
// ...
screen_create_window(&screen_win, screen_ctx);

screen_stream_t screen_stream;
// set properties on this target
// ...
screen_create_stream(&screen_stream, screen_ctx);
```
其中至少要设置render target的如下几类属性:
* usage 用途 只读(SCREEN_USAGE_READ),只写(SCREEN_USAGE_WRITE)或读写
* color format 色彩格式
* buffer size 缓冲区大小

```c++
...
int size[2];
...

/* Note that if you're using a pixmap as your off-screen render target and you eventually wish
   to make at least parts of the pixmap buffer visible, then you must set the usage, format,
   and buffer size to be compatible with those of the window on which you'll be using to
   display your pixmap-rendered content.
*/
screen_set_pixmap_property_iv(screen_pix, SCREEN_PROPERTY_FORMAT, (const int[]){ SCREEN_FORMAT_RGBX8888 });
screen_set_pixmap_property_iv(screen_pix, SCREEN_PROPERTY_USAGE, (const int[]){ SCREEN_USAGE_WRITE | SCREEN_USAGE_NATIVE });
screen_set_pixmap_property_cv(screen_pix, SCREEN_PROPERTY_ID_STRING, strlen("sw-vsync-pix"), "sw-vsync-pix");
screen_get_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
screen_set_pixmap_property_iv(screen_pix, SCREEN_PROPERTY_BUFFER_SIZE, size);
...
// and finally, you'll need to create a buffer for your render target
screen_create_pixmap_buffer(screen_pix);   
```

### Get access to your buffers

通过SCREEN_PROPERTY_RENDER_BUFFERS从render target获取buffer的引用
```c++
...
screen_buffer_t screen_pbuf;
...
screen_get_pixmap_property_pv(screen_pix, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&screen_pbuf);
...           
```
获得buffer的读/写指针,以及获取buffer的一些属性
```c++
...
void *pointer;
...
screen_get_buffer_property_pv(screen_pbuf, SCREEN_PROPERTY_POINTER, &pointer);
...
// the number of bytes allocated per line
int stride;
screen_get_buffer_property_iv(screen_pbuf, SCREEN_PROPERTY_STRIDE, &stride);

```

### Draw to buffers
利用buffer写指针进行buffer的更新.

### Post content
你的render内容已经准备好,通知screen需要被显示可见或者其他消费组件.

以window为例
```c++
...
int rects[8]; /* stores up to 2 dirty rectangles */
screen_buffer_t screen_wbuf = NULL;
...
screen_post_window(screen_win, screen_wbuf, 2, rects, 0);
```

Pixmap或stream需要额外的步骤使得buffer里的内容可见
```c++
...
int rects[8]; /* stores up to 2 dirty rectangles */
...
screen_buffer_t screen_wbuf = NULL;
screen_get_window_property_pv(screen_win, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&screen_wbuf);
int i;
    for (i = 0; i < 2; i++) {
        screen_blit(screen_ctx, screen_wbuf, screen_pbuf, (const int []){
                SCREEN_BLIT_SOURCE_X, rects[i*4+0],
                SCREEN_BLIT_SOURCE_Y, rects[i*4+1],
                SCREEN_BLIT_SOURCE_WIDTH, rects[i*4+2],
                SCREEN_BLIT_SOURCE_HEIGHT, rects[i*4+3],
                SCREEN_BLIT_DESTINATION_X, rects[i*4+0],
                SCREEN_BLIT_DESTINATION_Y, rects[i*4+1],
                SCREEN_BLIT_DESTINATION_WIDTH, rects[i*4+2],
                SCREEN_BLIT_DESTINATION_HEIGHT, rects[i*4+3],
                SCREEN_BLIT_END });
    }
...
screen_post_window(screen_win, screen_wbuf, 2, rects, 0);
...
```