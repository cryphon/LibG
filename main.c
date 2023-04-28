#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#define WIDTH 800
#define HEIGHT 600

void lib_fill(uint32_t *pixels, size_t width, size_t height, uint32_t color)
{
	for(size_t i = 0; i < width*height; ++i){
		pixels[i] = color;
	}
}

typedef int Errno;

#define return_defer(value) do { result = (value); goto defer;} while (0)

int lib_save_to_ppm_file(uint32_t *pixels, size_t width, size_t height, const char *file_path){
	int result = 0;
	FILE *f = NULL;

	{
		f = fopen(file_path , "wb");
		if(f == NULL) return_defer(errno);
		//0xBBRRGG
		fprintf(f, "P6\n%zu %zu 255\n", width, height);
		if(ferror(f)) return_defer(errno);

		for(size_t i = 0; i < width*height; ++i){
			// 0xAABBGGRR
			uint32_t pixel = pixels[i];
			uint8_t bytes[3] = {
				(pixel>>(8*0))&0xFF,
				(pixel>>(8*1))&0xFF,
				(pixel>>(8*2))&0xFF,
			};
			fwrite(bytes, sizeof(bytes), 1, f);
			if(ferror(f)) return_defer(errno);
		}
	}

defer:
	if(f) fclose(f);
	return result;
}




void lib_fill_rect(uint32_t *pixels, size_t pixels_width, size_t pixels_height, int x0, int y0, size_t w, size_t h, uint32_t color){
	uint32_t newCol = 0xFF00FFFF;
	for(int dy = 0; dy < (int) h; ++dy){
		int y = y0 + dy;
		if(0 <= y && y < (int) pixels_height){
			for(int dx = 0; dx <(int)  w; ++dx){
				int x = x0 + dx;
				if(0 <= x && x < (int) pixels_width){
					if(y > (pixels_height/2))
						pixels[y*pixels_width + x] = newCol;
					else
						pixels[y*pixels_width + x] = color;
				}
			}
		}	
	}	
}




static uint32_t pixels[HEIGHT*WIDTH];
int main(void)
{
	//0xFF0000FF
	lib_fill(pixels, WIDTH, HEIGHT, 0xFF202020);
	lib_fill_rect(pixels, WIDTH, HEIGHT, 100, 150, 250, 300, 0xFF0000FF); 

	const char *file_path = "output.ppm";
	Errno err = lib_save_to_ppm_file(pixels, WIDTH, HEIGHT, file_path);
	if(err){
		fprintf(stderr, "ERROR: could not save file %s, 5s\n", file_path, strerror(errno));
		return 1;
	}
	return 0;
}
