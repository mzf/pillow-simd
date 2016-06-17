void
ImagingResampleHorizontalConvolution8u4x(
    UINT32 *lineOut0, UINT32 *lineOut1, UINT32 *lineOut2, UINT32 *lineOut3,
    UINT32 *lineIn0, UINT32 *lineIn1, UINT32 *lineIn2, UINT32 *lineIn3,
    int xsize, int *xbounds, INT16 *kk, int kmax, int coefs_precision)
{
    int xmin, xmax, xx, x;
    INT16 *k;

    for (xx = 0; xx < xsize; xx++) {
        xmin = xbounds[xx * 2 + 0];
        xmax = xbounds[xx * 2 + 1];
        k = &kk[xx * kmax];
        x = 0;

#if defined(__AVX2__)

        __m256i sss0, sss1;
        __m256i zero = _mm256_setzero_si256();
        __m256i initial = _mm256_set1_epi32((1 << (coefs_precision -1)));
        sss0 = initial;
        sss1 = initial;

        for (; x < xmax - 3; x += 4) {
            __m256i pix, mmk0, mmk1, source, ksource;
            __m128i tmp = _mm_loadl_epi64((__m128i *) &k[x]);
            ksource = _mm256_insertf128_si256(
                _mm256_castsi128_si256(tmp), tmp, 1);

            mmk0 = _mm256_shuffle_epi8(ksource, _mm256_set_epi8(
                3,2, 1,0, 3,2, 1,0, 3,2, 1,0, 3,2, 1,0,
                3,2, 1,0, 3,2, 1,0, 3,2, 1,0, 3,2, 1,0));
            mmk1 = _mm256_shuffle_epi8(ksource, _mm256_set_epi8(
                7,6, 5,4, 7,6, 5,4, 7,6, 5,4, 7,6, 5,4,
                7,6, 5,4, 7,6, 5,4, 7,6, 5,4, 7,6, 5,4));

            source = _mm256_inserti128_si256(_mm256_castsi128_si256(
                _mm_loadu_si128((__m128i *) &lineIn0[x + xmin])),
                _mm_loadu_si128((__m128i *) &lineIn1[x + xmin]), 1);
            pix = _mm256_shuffle_epi8(source, _mm256_set_epi8(
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0,
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0));
            sss0 = _mm256_add_epi32(sss0, _mm256_madd_epi16(pix, mmk0));
            pix = _mm256_shuffle_epi8(source, _mm256_set_epi8(
                -1,15, -1,11, -1,14, -1,10, -1,13, -1,9, -1,12, -1,8,
                -1,15, -1,11, -1,14, -1,10, -1,13, -1,9, -1,12, -1,8));
            sss0 = _mm256_add_epi32(sss0, _mm256_madd_epi16(pix, mmk1));

            source = _mm256_inserti128_si256(_mm256_castsi128_si256(
                _mm_loadu_si128((__m128i *) &lineIn2[x + xmin])),
                _mm_loadu_si128((__m128i *) &lineIn3[x + xmin]), 1);
            pix = _mm256_shuffle_epi8(source, _mm256_set_epi8(
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0,
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0));
            sss1 = _mm256_add_epi32(sss1, _mm256_madd_epi16(pix, mmk0));
            pix = _mm256_shuffle_epi8(source, _mm256_set_epi8(
                -1,15, -1,11, -1,14, -1,10, -1,13, -1,9, -1,12, -1,8,
                -1,15, -1,11, -1,14, -1,10, -1,13, -1,9, -1,12, -1,8));
            sss1 = _mm256_add_epi32(sss1, _mm256_madd_epi16(pix, mmk1));
        }

        for (; x < xmax - 1; x += 2) {
            __m256i pix, mmk;
            __m128i ksource = _mm_cvtsi32_si128(*(int *) &k[x]);
            ksource = _mm_shuffle_epi8(ksource, _mm_set_epi8(
                3,2, 1,0, 3,2, 1,0, 3,2, 1,0, 3,2, 1,0));
            mmk = _mm256_inserti128_si256(_mm256_castsi128_si256(ksource), ksource, 1);

            pix = _mm256_inserti128_si256(_mm256_castsi128_si256(
                _mm_loadl_epi64((__m128i *) &lineIn0[x + xmin])),
                _mm_loadl_epi64((__m128i *) &lineIn1[x + xmin]), 1);
            pix = _mm256_shuffle_epi8(pix, _mm256_set_epi8(
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0,
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0));
            sss0 = _mm256_add_epi32(sss0, _mm256_madd_epi16(pix, mmk));

            pix = _mm256_inserti128_si256(_mm256_castsi128_si256(
                _mm_loadl_epi64((__m128i *) &lineIn2[x + xmin])),
                _mm_loadl_epi64((__m128i *) &lineIn3[x + xmin]), 1);
            pix = _mm256_shuffle_epi8(pix, _mm256_set_epi8(
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0,
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0));
            sss1 = _mm256_add_epi32(sss1, _mm256_madd_epi16(pix, mmk));
        }

        for (; x < xmax; x ++) {
            __m256i pix;
            __m256i mmk = _mm256_set1_epi32(k[x]);
            pix = _mm256_inserti128_si256(_mm256_castsi128_si256(
                _mm_cvtepu8_epi32(*(__m128i *) &lineIn0[x + xmin])),
                _mm_cvtepu8_epi32(*(__m128i *) &lineIn1[x + xmin]), 1);
            sss0 = _mm256_add_epi32(sss0, _mm256_madd_epi16(pix, mmk));
            pix = _mm256_inserti128_si256(_mm256_castsi128_si256(
                _mm_cvtepu8_epi32(*(__m128i *) &lineIn2[x + xmin])),
                _mm_cvtepu8_epi32(*(__m128i *) &lineIn3[x + xmin]), 1);
            sss1 = _mm256_add_epi32(sss1, _mm256_madd_epi16(pix, mmk));
        }

        sss0 = _mm256_srai_epi32(sss0, coefs_precision);
        sss1 = _mm256_srai_epi32(sss1, coefs_precision);
        sss0 = _mm256_packs_epi32(sss0, zero);
        sss1 = _mm256_packs_epi32(sss1, zero);
        sss0 = _mm256_packus_epi16(sss0, zero);
        sss1 = _mm256_packus_epi16(sss1, zero);
        lineOut0[xx] = _mm_cvtsi128_si32(_mm256_extracti128_si256(sss0, 0));
        lineOut1[xx] = _mm_cvtsi128_si32(_mm256_extracti128_si256(sss0, 1));
        lineOut2[xx] = _mm_cvtsi128_si32(_mm256_extracti128_si256(sss1, 0));
        lineOut3[xx] = _mm_cvtsi128_si32(_mm256_extracti128_si256(sss1, 1));

#else

        __m128i sss0, sss1, sss2, sss3;
        __m128i initial = _mm_set1_epi32((1 << (coefs_precision -1)));
        sss0 = initial;
        sss1 = initial;
        sss2 = initial;
        sss3 = initial;

        for (; x < xmax - 3; x += 4) {
            __m128i pix, mmk;
            __m128i source0 = _mm_loadu_si128((__m128i *) &lineIn0[x + xmin]);
            __m128i source1 = _mm_loadu_si128((__m128i *) &lineIn1[x + xmin]);
            __m128i source2 = _mm_loadu_si128((__m128i *) &lineIn2[x + xmin]);
            __m128i source3 = _mm_loadu_si128((__m128i *) &lineIn3[x + xmin]);
            __m128i ksource = _mm_loadl_epi64((__m128i *) &k[x]);

            pix = _mm_shuffle_epi8(source0, _mm_set_epi8(
                -1,11, -1,3, -1,10, -1,2, -1,9, -1,1, -1,8, -1,0));
            mmk = _mm_shuffle_epi8(ksource, _mm_set_epi8(
                5,4, 1,0, 5,4, 1,0, 5,4, 1,0, 5,4, 1,0));
            sss0 = _mm_add_epi32(sss0, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source1, _mm_set_epi8(
                -1,11, -1,3, -1,10, -1,2, -1,9, -1,1, -1,8, -1,0));
            sss1 = _mm_add_epi32(sss1, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source2, _mm_set_epi8(
                -1,11, -1,3, -1,10, -1,2, -1,9, -1,1, -1,8, -1,0));
            sss2 = _mm_add_epi32(sss2, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source3, _mm_set_epi8(
                -1,11, -1,3, -1,10, -1,2, -1,9, -1,1, -1,8, -1,0));
            sss3 = _mm_add_epi32(sss3, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source0, _mm_set_epi8(
                -1,15, -1,7, -1,14, -1,6, -1,13, -1,5, -1,12, -1,4));
            mmk = _mm_shuffle_epi8(ksource, _mm_set_epi8(
                7,6, 3,2, 7,6, 3,2, 7,6, 3,2, 7,6, 3,2));
            sss0 = _mm_add_epi32(sss0, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source1, _mm_set_epi8(
                -1,15, -1,7, -1,14, -1,6, -1,13, -1,5, -1,12, -1,4));
            sss1 = _mm_add_epi32(sss1, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source2, _mm_set_epi8(
                -1,15, -1,7, -1,14, -1,6, -1,13, -1,5, -1,12, -1,4));
            sss2 = _mm_add_epi32(sss2, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source3, _mm_set_epi8(
                -1,15, -1,7, -1,14, -1,6, -1,13, -1,5, -1,12, -1,4));
            sss3 = _mm_add_epi32(sss3, _mm_madd_epi16(pix, mmk));
        }

        for (; x < xmax - 1; x += 2) {
            __m128i pix, mmk;
            __m128i source0 = _mm_loadl_epi64((__m128i *) &lineIn0[x + xmin]);
            __m128i source1 = _mm_loadl_epi64((__m128i *) &lineIn1[x + xmin]);
            __m128i source2 = _mm_loadl_epi64((__m128i *) &lineIn2[x + xmin]);
            __m128i source3 = _mm_loadl_epi64((__m128i *) &lineIn3[x + xmin]);
            __m128i ksource = _mm_cvtsi32_si128(*(int *) &k[x]);
            mmk = _mm_shuffle_epi8(ksource, _mm_set_epi8(
                3,2, 1,0, 3,2, 1,0, 3,2, 1,0, 3,2, 1,0));

            pix = _mm_shuffle_epi8(source0, _mm_set_epi8(
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0));
            sss0 = _mm_add_epi32(sss0, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source1, _mm_set_epi8(
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0));
            sss1 = _mm_add_epi32(sss1, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source2, _mm_set_epi8(
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0));
            sss2 = _mm_add_epi32(sss2, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source3, _mm_set_epi8(
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0));
            sss3 = _mm_add_epi32(sss3, _mm_madd_epi16(pix, mmk));
        }

        for (; x < xmax; x ++) {
            __m128i pix;
            __m128i mmk = _mm_set1_epi32(k[x]);
            pix = _mm_cvtepu8_epi32(*(__m128i *) &lineIn0[x + xmin]);
            sss0 = _mm_add_epi32(sss0, _mm_madd_epi16(pix, mmk));
            pix = _mm_cvtepu8_epi32(*(__m128i *) &lineIn1[x + xmin]);
            sss1 = _mm_add_epi32(sss1, _mm_madd_epi16(pix, mmk));
            pix = _mm_cvtepu8_epi32(*(__m128i *) &lineIn2[x + xmin]);
            sss2 = _mm_add_epi32(sss2, _mm_madd_epi16(pix, mmk));
            pix = _mm_cvtepu8_epi32(*(__m128i *) &lineIn3[x + xmin]);
            sss3 = _mm_add_epi32(sss3, _mm_madd_epi16(pix, mmk));
        }

        sss0 = _mm_srai_epi32(sss0, coefs_precision);
        sss1 = _mm_srai_epi32(sss1, coefs_precision);
        sss2 = _mm_srai_epi32(sss2, coefs_precision);
        sss3 = _mm_srai_epi32(sss3, coefs_precision);
        sss0 = _mm_packs_epi32(sss0, sss0);
        sss1 = _mm_packs_epi32(sss1, sss1);
        sss2 = _mm_packs_epi32(sss2, sss2);
        sss3 = _mm_packs_epi32(sss3, sss3);
        lineOut0[xx] = _mm_cvtsi128_si32(_mm_packus_epi16(sss0, sss0));
        lineOut1[xx] = _mm_cvtsi128_si32(_mm_packus_epi16(sss1, sss1));
        lineOut2[xx] = _mm_cvtsi128_si32(_mm_packus_epi16(sss2, sss2));
        lineOut3[xx] = _mm_cvtsi128_si32(_mm_packus_epi16(sss3, sss3));

#endif

    }
}


void
ImagingResampleHorizontalConvolution8u(UINT32 *lineOut, UINT32 *lineIn,
    int xsize, int *xbounds, INT16 *kk, int kmax, int coefs_precision)
{
    int xmin, xmax, xx, x;
    INT16 *k;

    for (xx = 0; xx < xsize; xx++) {
        __m128i sss;
        xmin = xbounds[xx * 2 + 0];
        xmax = xbounds[xx * 2 + 1];
        k = &kk[xx * kmax];
        x = 0;

#if defined(__AVX2__)

    if (xmax < 8) {
        sss = _mm_set1_epi32((1 << (coefs_precision -1)) + xmax / 2);
    } else {

        __m256i sss256 = _mm256_set1_epi32((1 << (coefs_precision -2)) + xmax / 4);

        for (; x < xmax - 7; x += 8) {
            __m256i pix, mmk, source;
            __m128i tmp = _mm_loadu_si128((__m128i *) &k[x]);
            __m256i ksource = _mm256_insertf128_si256(
                _mm256_castsi128_si256(tmp), tmp, 1);

            source = _mm256_loadu_si256((__m256i *) &lineIn[x + xmin]);

            pix = _mm256_shuffle_epi8(source, _mm256_set_epi8(
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0,
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0));
            mmk = _mm256_shuffle_epi8(ksource, _mm256_set_epi8(
                11,10, 9,8, 11,10, 9,8, 11,10, 9,8, 11,10, 9,8,
                3,2, 1,0, 3,2, 1,0, 3,2, 1,0, 3,2, 1,0));
            sss256 = _mm256_add_epi32(sss256, _mm256_madd_epi16(pix, mmk));

            pix = _mm256_shuffle_epi8(source, _mm256_set_epi8(
                -1,15, -1,11, -1,14, -1,10, -1,13, -1,9, -1,12, -1,8,
                -1,15, -1,11, -1,14, -1,10, -1,13, -1,9, -1,12, -1,8));
            mmk = _mm256_shuffle_epi8(ksource, _mm256_set_epi8(
                15,14, 13,12, 15,14, 13,12, 15,14, 13,12, 15,14, 13,12,
                7,6, 5,4, 7,6, 5,4, 7,6, 5,4, 7,6, 5,4));
            sss256 = _mm256_add_epi32(sss256, _mm256_madd_epi16(pix, mmk));
        }

        for (; x < xmax - 3; x += 4) {
            __m256i pix, mmk, source;
            __m128i tmp = _mm_loadl_epi64((__m128i *) &k[x]);
            __m256i ksource = _mm256_insertf128_si256(
                _mm256_castsi128_si256(tmp), tmp, 1);

            tmp = _mm_loadu_si128((__m128i *) &lineIn[x + xmin]);
            source = _mm256_insertf128_si256(
                _mm256_castsi128_si256(tmp), tmp, 1);

            pix = _mm256_shuffle_epi8(source, _mm256_set_epi8(
                -1,15, -1,11, -1,14, -1,10, -1,13, -1,9, -1,12, -1,8,
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0));
            mmk = _mm256_shuffle_epi8(ksource, _mm256_set_epi8(
                7,6, 5,4, 7,6, 5,4, 7,6, 5,4, 7,6, 5,4, 
                3,2, 1,0, 3,2, 1,0, 3,2, 1,0, 3,2, 1,0));
            sss256 = _mm256_add_epi32(sss256, _mm256_madd_epi16(pix, mmk));
        }

        sss = _mm_add_epi32(
            _mm256_extractf128_si256(sss256, 0),
            _mm256_extractf128_si256(sss256, 1)
        );

    }

#else

        sss = _mm_set1_epi32((1 << (coefs_precision -1)) + xmax / 2);

        for (; x < xmax - 7; x += 8) {
            __m128i pix, mmk, source;
            __m128i ksource = _mm_loadu_si128((__m128i *) &k[x]);

            source = _mm_loadu_si128((__m128i *) &lineIn[x + xmin]);

            pix = _mm_shuffle_epi8(source, _mm_set_epi8(
                -1,11, -1,3, -1,10, -1,2, -1,9, -1,1, -1,8, -1,0));
            mmk = _mm_shuffle_epi8(ksource, _mm_set_epi8(
                5,4, 1,0, 5,4, 1,0, 5,4, 1,0, 5,4, 1,0));
            sss = _mm_add_epi32(sss, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source, _mm_set_epi8(
                -1,15, -1,7, -1,14, -1,6, -1,13, -1,5, -1,12, -1,4));
            mmk = _mm_shuffle_epi8(ksource, _mm_set_epi8(
                7,6, 3,2, 7,6, 3,2, 7,6, 3,2, 7,6, 3,2));
            sss = _mm_add_epi32(sss, _mm_madd_epi16(pix, mmk));

            source = _mm_loadu_si128((__m128i *) &lineIn[x + 4 + xmin]);

            pix = _mm_shuffle_epi8(source, _mm_set_epi8(
                -1,11, -1,3, -1,10, -1,2, -1,9, -1,1, -1,8, -1,0));
            mmk = _mm_shuffle_epi8(ksource, _mm_set_epi8(
                13,12, 9,8, 13,12, 9,8, 13,12, 9,8, 13,12, 9,8));
            sss = _mm_add_epi32(sss, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source, _mm_set_epi8(
                -1,15, -1,7, -1,14, -1,6, -1,13, -1,5, -1,12, -1,4));
            mmk = _mm_shuffle_epi8(ksource, _mm_set_epi8(
                15,14, 11,10, 15,14, 11,10, 15,14, 11,10, 15,14, 11,10));
            sss = _mm_add_epi32(sss, _mm_madd_epi16(pix, mmk));
        }

        for (; x < xmax - 3; x += 4) {
            __m128i pix, mmk;
            __m128i source = _mm_loadu_si128((__m128i *) &lineIn[x + xmin]);
            __m128i ksource = _mm_loadl_epi64((__m128i *) &k[x]);

            pix = _mm_shuffle_epi8(source, _mm_set_epi8(
                -1,11, -1,3, -1,10, -1,2, -1,9, -1,1, -1,8, -1,0));
            mmk = _mm_shuffle_epi8(ksource, _mm_set_epi8(
                5,4, 1,0, 5,4, 1,0, 5,4, 1,0, 5,4, 1,0));
            sss = _mm_add_epi32(sss, _mm_madd_epi16(pix, mmk));

            pix = _mm_shuffle_epi8(source, _mm_set_epi8(
                -1,15, -1,7, -1,14, -1,6, -1,13, -1,5, -1,12, -1,4));
            mmk = _mm_shuffle_epi8(ksource, _mm_set_epi8(
                7,6, 3,2, 7,6, 3,2, 7,6, 3,2, 7,6, 3,2));
            sss = _mm_add_epi32(sss, _mm_madd_epi16(pix, mmk));
        }

#endif

        for (; x < xmax - 1; x += 2) {
            __m128i pix, mmk;
            __m128i source = _mm_loadl_epi64((__m128i *) &lineIn[x + xmin]);
            __m128i ksource = _mm_cvtsi32_si128(*(int *) &k[x]);

            pix = _mm_shuffle_epi8(source, _mm_set_epi8(
                -1,7, -1,3, -1,6, -1,2, -1,5, -1,1, -1,4, -1,0));
            mmk = _mm_shuffle_epi8(ksource, _mm_set_epi8(
                3,2, 1,0, 3,2, 1,0, 3,2, 1,0, 3,2, 1,0));
            sss = _mm_add_epi32(sss, _mm_madd_epi16(pix, mmk));
        }

        for (; x < xmax; x ++) {
            __m128i pix = _mm_cvtepu8_epi32(*(__m128i *) &lineIn[x + xmin]);
            __m128i mmk = _mm_set1_epi32(k[x]);
            sss = _mm_add_epi32(sss, _mm_madd_epi16(pix, mmk));
        }
        sss = _mm_srai_epi32(sss, coefs_precision);
        sss = _mm_packs_epi32(sss, sss);
        lineOut[xx] = _mm_cvtsi128_si32(_mm_packus_epi16(sss, sss));
    }
}
