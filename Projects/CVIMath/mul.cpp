// Fast matrix multiplication on NPU

#include <assert.h>
#include <cvimath_internal.h>
#include <test_cvikernel_util.h>

#include <sys/time.h>
#include <time.h>


int gl_elapsed;

static void gen_matrix(uint16_t *matrix, size_t size)
{
    float t;
    for (size_t i = 0; i < size; i++)
    {
        float f;
        if (i % 2 == 0)
            t = i % 8;
        if (i % 2 == 1)
            t = -1 * (i % 8);
        f = t;
        matrix[i] = convert_fp32_bf16(f);
    }
}

static cvk_mg_t *alloc_on_npu(CVI_RT_HANDLE *rt_ctx, size_t row, size_t col,
                            cvk_fmt_t mg_data_format, uint8_t data[])
{
    cvk_mg_shape_t s;
    s.row = row;
    s.col = col;
    return test_put_matrix_g(rt_ctx, s, mg_data_format, data);
}

// BF16
static int test_gemm_bf16(size_t M, size_t N, size_t K)
{
    long elapsed;
    struct timeval t0, t1;
    int ret = 0;

    // alloc test data in host
    uint16_t *bf16_A = new uint16_t[M * K]; // left matrix
    uint16_t *bf16_B = new uint16_t[N * K]; // right matrix
    uint16_t *bf16_R = new uint16_t[2 * M * N]; // result matrix

    // assign data
    gen_matrix(bf16_A, M * K);
    gen_matrix(bf16_B, N * K);

    CVI_RT_HANDLE ctx;
    cvk_context_t *bk_ctx;

    test_init(&ctx, &bk_ctx);

    cvk_mg_t *mg_A = alloc_on_npu(&ctx, M, K, CVK_FMT_BF16, (uint8_t *)bf16_A);
    cvk_mg_t *mg_B = alloc_on_npu(&ctx, K, N, CVK_FMT_BF16, (uint8_t *)bf16_B);
    cvk_mg_t *mg_R = alloc_on_npu(&ctx, M * 2, N, CVK_FMT_BF16, (uint8_t *)bf16_R);

    gaddr_t gaddr_a = mg_A->start_address;
    gaddr_t gaddr_b = mg_B->start_address;
    gaddr_t gaddr_r = mg_R->start_address;

    size_t *slice_num =
        cvm_gemm((cvk_context_t *)bk_ctx, gaddr_a, gaddr_b, gaddr_r, M, K, N, CVK_FMT_BF16);

    gettimeofday(&t0, NULL);
    test_submit_comp(&ctx, bk_ctx); // mul
    gettimeofday(&t1, NULL);
    elapsed = (t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec;
    gl_elapsed += elapsed;

    // get result
    uint16_t *bf16_ref = (uint16_t *)test_get_mg_mem_comp(&ctx, mg_R);

    printf("Time: %ld us\n", elapsed);

    // free
    test_free_mg_mem_comp(&ctx, mg_A);
    test_free_mg_mem_comp(&ctx, mg_B);
    test_free_mg_mem_comp(&ctx, mg_R);

    test_exit(&ctx, bk_ctx); // NPU release

    delete[] bf16_A;
    delete[] bf16_B;
    delete[] bf16_R;
    free(bf16_ref);
    free(slice_num);

    return ret;
}

// INT8
// todo

int main()
{
    for (int i = 0; i < 1000; i++) test_gemm_bf16(100, 100, 100);
    double res = (double)gl_elapsed / (1000.0 * 1000.0); // convert to ms
    printf("AVG: %lf\n", res);
}