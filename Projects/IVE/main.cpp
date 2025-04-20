#include "ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <chrono>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Add image file path");
        return CVI_FAILURE;
    }

    const char *filename = argv[1];
    
    IVE_HANDLE handle = CVI_IVE_CreateHandle();
    printf("Init\n");

    printf("Read src image\n");
    IVE_IMAGE_S src = CVI_IVE_ReadImage(handle, filename, IVE_IMAGE_TYPE_U8C3_PLANAR);
    int width = src.u16Width;
    int height = src.u16Height;

    IVE_DST_IMAGE_S dst;
    printf("create dst image\n");
    CVI_IVE_CreateImage(handle, &dst, IVE_IMAGE_TYPE_U8C3_PLANAR, width, height);

    // 3x3 filter
    CVI_S8 arr[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    IVE_FILTER_CTRL_S iveFltCtrl;
    iveFltCtrl.u8MaskSize = 3; // mask h/w
    memcpy(iveFltCtrl.as8Mask, arr, iveFltCtrl.u8MaskSize * iveFltCtrl.u8MaskSize * sizeof(CVI_S8));
    iveFltCtrl.u32Norm = 1; // filter per-element divider 

    printf("Run %dx%d filter on img %dx%d\n", iveFltCtrl.u8MaskSize, iveFltCtrl.u8MaskSize, width, height);
    
    auto begin = std::chrono::steady_clock::now();
    
    int ret = CVI_IVE_Filter(handle, &src, &dst, &iveFltCtrl, 0);
    if (ret != CVI_SUCCESS)
    {
        printf("Failed to run CVI_IVE_Filter\n");
    }

    auto end = std::chrono::steady_clock::now();
    double fps = 1 / std::chrono::duration<double>(end - begin).count();

    printf("FPS: %lf\n", fps);

    // Save
    printf("Saving img...\n");
    CVI_IVE_WriteImage(handle, "filter_res.png", &dst);

    // Free memory, instance
    CVI_SYS_FreeI(handle, &src);
    CVI_SYS_FreeI(handle, &dst);
    CVI_IVE_DestroyHandle(handle);

    
    return ret;
}