#include "screenshot_sample.h"
#include <screen/screen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void write_bitmap_header(int nbytes, int fd, const int size[]);

void SampleScreenshot::run() {

	// Create a variable for context and window
	screen_context_t screenshot_ctx = 0;
	screen_window_t screenshot_win = 0;
	char window_group_name[64];

	// Create screen context 创建上下文
	screen_create_context(&screenshot_ctx, SCREEN_APPLICATION_CONTEXT);
	// Create your render targets. 创建父窗口
	screen_create_window(&screenshot_win, screenshot_ctx);
	// Retrieve the window ID 获取窗口组标示
	screen_get_window_property_cv(screenshot_win, SCREEN_PROPERTY_ID,
			sizeof(window_group_name), window_group_name);

	// set screen window property 设置父窗口属性
	int usage = SCREEN_USAGE_WRITE | SCREEN_USAGE_NATIVE;
	screen_set_window_property_iv(screenshot_win, SCREEN_PROPERTY_USAGE, &usage);
	// Create screen window buffers 创建父窗口缓冲区
	screen_create_window_buffers(screenshot_win, 1);

	//Get access to your buffers. 父窗口绑定父窗口的缓冲区
	screen_buffer_t screen_buf[2];
	screen_get_window_property_pv(screenshot_win, SCREEN_PROPERTY_RENDER_BUFFERS,
			(void **) screen_buf);

	//Draw to your buffer. 向父窗口的缓冲区中填充内容
	int color = 0xffffff00;
	int bg[] = { SCREEN_BLIT_COLOR, color, SCREEN_BLIT_END };
	screen_fill(screenshot_ctx, screen_buf[0], bg);

	screen_post_window(screenshot_win, screen_buf[0], 0, NULL, 0); //显示

	// Create variables for the pixmap, the pixmap buffer, the pixmap buffer pointer,
	screen_pixmap_t screen_pix;
	screen_buffer_t screenshot_buf;
	char *screentshot_ptr = NULL;
	int screenshot_stride = 0;

	// Create other variables necessary to support the Screen API calls and the writing of your screenshot to a bitmap
	char header[54];
	char *fname = "hourglass_window_screenshot.bmp";
	int nbytes;
	int fd;
	int i;
	int usage2, format;
	int size[2];

	// Create the pixmap for the screenshot and set the usage flag and format properties
	screen_create_pixmap(&screen_pix, screenshot_ctx);

	usage2 = SCREEN_USAGE_READ | SCREEN_USAGE_NATIVE;
	screen_set_pixmap_property_iv(screen_pix, SCREEN_PROPERTY_USAGE, &usage2);
	format = SCREEN_FORMAT_RGBA8888;
	screen_set_pixmap_property_iv(screen_pix, SCREEN_PROPERTY_FORMAT, &format);

	// Set the buffer size of the pixmap for the screenshot; the pixmap buffer size doesn't have to necessarily match the size of the source.
	size[0] = 200;
	size[1] = 200;

	screen_set_pixmap_property_iv(screen_pix, SCREEN_PROPERTY_BUFFER_SIZE, size);


	// Create the pixmap buffer for the screenshot and get the buffer handle, the pointer, and the stride
	screen_create_pixmap_buffer(screen_pix);
	screen_get_pixmap_property_pv(screen_pix, SCREEN_PROPERTY_RENDER_BUFFERS,
	                              (void**)&screenshot_buf);
	screen_get_buffer_property_pv(screenshot_buf, SCREEN_PROPERTY_POINTER,
	                              (void**)&screentshot_ptr);
	screen_get_buffer_property_iv(screenshot_buf, SCREEN_PROPERTY_STRIDE,
	                              &screenshot_stride);

	// Take the window screenshot
	screen_read_window(screenshot_win, screenshot_buf, 0, NULL ,0);

	// Create the bitmap file with appropriate permissions; prepare the header and write the buffer contents to the file
	fd = creat(fname, S_IRUSR | S_IWUSR);

	nbytes = size[0] * size[1] * 4;
	write_bitmap_header(nbytes, fd, size);

	for (i = 0; i < size[1]; i++) {
		write(fd, screentshot_ptr + i * screenshot_stride, size[0] * 4);
	}

	close(fd);

	screen_destroy_pixmap(screen_pix);
}

void write_bitmap_header(int nbytes, int fd, const int size[])
{
	char header[54];

	/* Set standard bitmap header */
	header[0] = 'B';
	header[1] = 'M';
	header[2] = nbytes & 0xff;
	header[3] = (nbytes >> 8) & 0xff;
	header[4] = (nbytes >> 16) & 0xff;
	header[5] = (nbytes >> 24) & 0xff;
	header[6] = 0;
	header[7] = 0;
	header[8] = 0;
	header[9] = 0;
	header[10] = 54;
	header[11] = 0;
	header[12] = 0;
	header[13] = 0;
	header[14] = 40;
	header[15] = 0;
	header[16] = 0;
	header[17] = 0;
	header[18] = size[0] & 0xff;
	header[19] = (size[0] >> 8) & 0xff;
	header[20] = (size[0] >> 16) & 0xff;
	header[21] = (size[0] >> 24) & 0xff;
	header[22] = -size[1] & 0xff;
	header[23] = (-size[1] >> 8) & 0xff;
	header[24] = (-size[1] >> 16) & 0xff;
	header[25] = (-size[1] >> 24) & 0xff;
	header[26] = 1;
	header[27] = 0;
	header[28] = 32;
	header[29] = 0;
	header[30] = 0;
	header[31] = 0;
	header[32] = 0;
	header[33] = 0;
	header[34] = 0; /* image size*/
	header[35] = 0;
	header[36] = 0;
	header[37] = 0;
	header[38] = 0x9;
	header[39] = 0x88;
	header[40] = 0;
	header[41] = 0;
	header[42] = 0x9l;
	header[43] = 0x88;
	header[44] = 0;
	header[45] = 0;
	header[46] = 0;
	header[47] = 0;
	header[48] = 0;
	header[49] = 0;
	header[50] = 0;
	header[51] = 0;
	header[52] = 0;
	header[53] = 0;

	/* Write bitmap header to file */
	write(fd, header, sizeof(header));
}
