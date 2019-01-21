#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int is_file(const char* path) {
  return (access(path, F_OK) != -1);
}

int writePNG(const char* pmm, const char* outputPath) {
  FILE *pmmFile = fopen(pmm, "r");
  FILE *pngFile = fopen(outputPath, "w");
  int fileClosed;

  // Write the file
  // 0x0089
  // 0x0050, 0x004e, 0x0047 // PNG
  // 0x000d, 0x000a // Line ending
  // 0x001a, 0x001a // Line ending
  unsigned char pngHeader[8] = {0x0089, 0x0050, 0x004e, 0x0047, 0x000d, 0x000a, 0x001a, 0x001a};
  fwrite(pngHeader, 1, 8, pngFile);

  //  IHDR - 13 bytes
  // 4 bytes - width
  // 4 bytes - height
  // 1 bytes - bit depth
  // 1 bytes - color type
  // 1 bytes - compression method
  // 1 bytes - filter method
  // 1 bytes - interface method

  // 0x000c, 0x0049, 0x0048, 0x0044, 0x0052 // IDHR
  // 0x0, 0x0,
  // 0x0003, 0x0020, // 600
  // 0x0, 0x0,
  // 0x0002, 0x0058 // 800
  // 0x0008 // bit depth
  // 0x0006 // color type
  // 0x0 // compression method
  // 0x0 // filter method
  // 0x0 // interface method

  int w;
  int h;
  int line = 0;
  char *sep = " ";
  char buf[13];
  char *lEnd = "1\n";
  while (fgets(buf, sizeof(buf), pmmFile)) {
    if (line == 1) {
      char* wh;
      wh = strtok(buf, sep);
      w = atoi(wh);
      wh = strtok(NULL, sep);
      h = atoi(wh);

      // unsigned char idhr[13] = {0x000c, 0x0049, 0x0048, 0x0044, 0x0052, 0x0, 0x0, w, h, 8, };
      unsigned char idhr[18] = {0x000c, 0x0049, 0x0048, 0x0044, 0x0052, 0x0, 0x0, 0x0003, 0x0020, 0x0, 0x0, 0x0002, 0x0058, 0x0008, 0x0006, 0x0, 0x0, 0x0};
      fwrite(idhr, 1, 8, pngFile);

      break;
    }

    if (line > 1) {
      fwrite(buf, 1, 3, pngFile);
      fwrite(lEnd, 1, 2, pngFile);
    }

    line += 1;
  }

  printf("[Writer] Width: %i | Height: %i\n", w, h);

  unsigned char pngEnd[4] = {0x0049, 0x0045, 0x004f, 0x0044};
  fwrite(pngEnd, 1, 4, pngFile);

  // Close the files
  fileClosed = fclose(pmmFile);
  if (fileClosed != 0) {
    return fileClosed;
  }
  fileClosed = fclose(pngFile);
  if (fileClosed != 0) {
    return fileClosed;
  }

  return 0;
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    printf("[Encoder] Missing file path\nUsage:\n\t./run [filepath]\n\n");
    return 1;
  }

  if (is_file(argv[1]) != 1){
    printf("[Encoder] Cannot read from: %s\n", argv[1]);
    return 1;
  }

  const char* outputPath = argv[2];
  if (argv[2] == 0) {
    outputPath = "image.png";
  }

  int res = writePNG(argv[1], outputPath);

  return 0;
}
