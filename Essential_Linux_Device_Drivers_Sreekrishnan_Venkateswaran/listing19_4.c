#include <fcntl.h>

#include <sys/stat.h>

#include <sys/mman.h> /* For definition of mmap() */

#include <linux/fb.h> /* For frame buffer structures and ioctls */


int
main(int argc, char *argv[])
{
    int imagefd, fbfd;              /* File descriptors */
    char *imagebuf, *fbbuf;         /* mmap buffers */
    struct fb_var_screeninfo vinfo; /* Variable Screen info */
    struct stat statbuf;            /* Image info */
    int fbsize;                     /* Frame buffer size */

    /* Open image file */
    if ((imagefd = open(argv[1], O_RDONLY)) < 0) {
        perror("Bad image open\n");
        exit(1);
    }

    /* Get the size of the image file */
    if (fstat(imagefd, &statbuf) < 0) {
        perror("Bad fstat\n");
        exit(1);
    }

    /* mmap the image file */
    if ((imagebuf = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED,
                         imagefd, 0)) == (char *) -1){
        perror("Bad image mmap\n");
        exit(1);
    }

    /* Open video memory */
    if ((fbfd = open("/dev/fb0", O_RDWR)) < 0) {
        perror("Bad frame buffer open\n");
        exit(1);
    }

    /* Get screen attributes such as resolution and depth */
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Bad vscreeninfo ioctl\n");
        exit(1);
    }

    /* Size of video memory =
       (X-resolution * Y-resolution * Bytes per pixel) */
    fbsize = (vinfo.xres * vinfo.yres * vinfo.bits_per_pixel)/8;

    /* mmap the video memory */
    if ((fbbuf = mmap(0, fbsize, PROT_WRITE, MAP_SHARED, fbfd, 0))
        == (char *) -1){
        perror("Bad frame buffer mmap\n");
        exit(1);
    }
    
    /* Transfer imagebuf to fbbuf after applying transformations
       dependent on the format, resolution, depth, data offset,
       and other properties of the image file. Not implemented in
       this Listing */
    copy_image_to_fb();
    msync(fbbuf, fbsize, MS_SYNC); /* Flush changes to device */

    /* ... */

    /* Unmap frame buffer memory */
    munmap(fbbuf, fbsize);
    close(fbfd);

    /* Unmap image file */
    munmap(imagebuf, statbuf.st_size);
    close(imagefd);
}
