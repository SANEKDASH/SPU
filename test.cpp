#include <stdio.h>
#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <share.h>

int main()
{

    struct _stat data = {};
    FILE *fp = fopen("output.bin", "rb");

    int fd = fileno(fp);

    printf("discriptor = %d\n", fd);

    _fstat(fd, &data);

    printf("size = %ld", data.st_size);

    fclose(fp);
}
