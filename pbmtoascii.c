#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BUF_SIZE 4096

struct bwpixel {
	uint8_t dot;
};

struct pbmimage {
	int x, y;
	struct bwpixel *data;
};

static struct pbmimage *read_pbm(const char *filename)
{
	char buffer[BUF_SIZE] = {0};
	struct pbmimage *img;
	FILE *file;
	int c;

	file = fopen(filename, "rb");
	if (!file) {
		perror("fopen");
		return NULL;
	}

	if (!fgets(buffer, sizeof buffer, file)) {
		perror(filename);
		return NULL;
	}

	if (buffer[0] != 'P' || buffer[1] != '1') {
		fprintf(stderr, "Invalid image format (must be 'P1')\n");
		fclose(file);
		return NULL;
	}

	img = (struct pbmimage *) malloc(sizeof (struct pbmimage));
	if (!img) {
		perror("malloc");
		fclose(file);
		return NULL;
	}

	/* check for comments */
	c = getc(file);
	while (c == '#') {
		while (getc(file) != '\n') ;
		c = getc(file);
	}

	ungetc(c, file);

	/* read image size information */
	if (fscanf(file, "%d %d", &img->x, &img->y) != 2) {
		perror("fscanf");
		fclose(file);
		free(img);
		return NULL;
	}

	while (fgetc(file) != '\n') ;

	/* memory allocation for pixel data */
	img->data = (struct bwpixel *) malloc(img->x * img->y * sizeof(struct bwpixel));
	if (!img->data) {
		perror("malloc");
		free(img);
		fclose(file);
		return NULL;
	}

	/* read pixel data from file */
	if (fread(img->data, img->x, img->y, file) != img->y) {
		perror("fread");
		free(img->data);
		free(img);
		fclose(file);
		return NULL;
	}

	fclose(file);

	return img;
}

int main(int argc, char *argv[])
{
	struct pbmimage *image;
	int i;

	if (argc != 2) {
		fprintf(stderr, "Wrong arguments\n");
		return 0;
	}

	image = read_pbm(argv[1]);
	if (!image) {
		exit(EXIT_FAILURE);
	}

	printf("static const uint8_t %s[%d * %d] = {\n", argv[1], image->x / 8, image->y);
	for (i = 0; i < image->x * image->y; i += 8) {

		if (i % 8 == 0) {
			int b;
			uint8_t pixels = 0;
			for (b = 0; b < 8; b++) {
				/* pixels |= (image->data[i + b].dot - '0') << b; */
				uint8_t dot = ((image->data[i + b].dot - '0') == 0 ? 1 : 0);
				pixels |= dot << b;
			}

			if (i % image->x == 0) {
				putchar('\n');
				putchar('\t');
			}
			printf("0x%02x, ", pixels);
		}
	}
	printf("\n};\n");

	free(image->data);
	free(image);

	return 0;
}
