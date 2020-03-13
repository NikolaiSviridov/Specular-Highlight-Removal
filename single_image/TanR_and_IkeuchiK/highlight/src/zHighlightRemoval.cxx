// zHighlightRemoval: to remove highlights or specularities
// by: robby

/**------------------------------
   reference: 
   "separating reflection components of textured surfaces using a single image"
   by Robby T. Tan, Katsushi Ikeuchi,
   IEEE Transactions on Pattern Analysis and Machine Intelligence (PAMI),
   27(2), pp.179-193, February, 2005
/*------------------------------*/


#include "zHighlightRemoval.h"

#define SPECULARX        10
#define SPECULARY        11
#define DIFFUSE            12
#define BOUNDARY        13
#define NOISE            14
#define CAMERA_DARK        15


zHighlightRemoval::zHighlightRemoval(char *fname) {
    zArray2D<s_rgbi> img, diff;
    ::zReadImage(fname, img);

    zRemoveHighlights(img, diff);
    ::zWriteImage(diff, "diffuse.ppm");
}

zHighlightRemoval::~zHighlightRemoval() {

}

int zHighlightRemoval::zRemoveHighlights(zArray2D<s_rgbi> &img,
                                         zArray2D<s_rgbi> &diff) {
    // the main function to remove highlights from a single image

    //SPECULAR-FREE IMAGE

    zArray2D<s_rgbi> sfi;
    zSpecularFreeImage(img, sfi);


    //ITERATIVE PART

    float epsilon = 0.5f;
    float step = 0.01f;

    zArray2D<s_rgbi> src;
    src.zCopyIn(img);

    while (epsilon >= 0.0) {
        // run the main iteration
        printf("*");
        zIteration(src, sfi, epsilon);
        epsilon -= step;
        printf(": %f\n", epsilon);
        //zWriteImage(src,"iteration.ppm");
    }

    diff.zCopyIn(src);
    return 0;
}


int zHighlightRemoval::zSpecularFreeImage(zArray2D<s_rgbi> &src,
                                          zArray2D<s_rgbi> &sfi) {
    sfi.zAllocate(src.zGetMaxY(), src.zGetMaxX());

    float Lambda = 0.6f;

    float camDark = 10.0f;    // for pixels that are too dark

    //SPECULAR FREE IMAGE
    int y, x;
    for (y = 0; y < src.zGetMaxY(); y++) {
        for (x = 0; x < src.zGetMaxX(); x++) {
            //get the rgb values

            //copy the rgb to the sfi
            sfi(y, x) = src(y, x);

            //check for camera dark and achromatic pixels
            if (((src(y, x).r < camDark) && (src(y, x).b < camDark) && (src(y, x).g < camDark))) {
                src(y, x).i = CAMERA_DARK;
                continue;
            }

            //perform the specular-to-diffuse mechanism
            float c = src(y, x).zMaxChroma();
            float dI = (src(y, x).zMax() * (3.0f * c - 1.0f)) / (c * (3.0f * Lambda - 1.0f));
            float sI = (src(y, x).zTotal() - dI) / 3.0f;

            float dr, dg, db;
            dr = (src(y, x).r - sI);
            dg = (src(y, x).g - sI);
            db = (src(y, x).b - sI);

            if (dr < 0) dr = 0;
            if (dg < 0) dg = 0;
            if (db < 0) db = 0;

            if (dr > 255) dr = 255;
            if (dg > 255) dg = 255;
            if (db > 255) db = 255;

            sfi(y, x).r = dr;
            sfi(y, x).g = dg;
            sfi(y, x).b = db;

        }
    }

    ::zWriteImage(sfi, "specular_free.ppm");

    return 0;
}

int zHighlightRemoval::zIteration(zArray2D<s_rgbi> &src, zArray2D<s_rgbi> &sfi,
                                  float epsilon) {
    int x, y;

    float thR = 0.1f, thG = 0.1f;

    // to have the initial labels
    int count = zInit(src, sfi, epsilon);
    int pcount;

    while (1) {
        printf(".");
        for (y = 0; y < src.zGetMaxY() - 1; y++) {
            for (x = 0; x < src.zGetMaxX() - 1; x++) {

                if (src(y, x).i == CAMERA_DARK) continue;

                float cr = src(y, x).zChroma_r();        // red chroma
                float cg = src(y, x).zChroma_g();        // green chroma

                float cr_next_x = src(y, x + 1).zChroma_r();
                float cg_next_x = src(y, x + 1).zChroma_g();

                float cr_next_y = src(y + 1, x).zChroma_r();
                float cg_next_y = src(y + 1, x).zChroma_g();

                float drx = cr_next_x - cr;//pixel right
                float dgx = cg_next_x - cg;
                float dry = cr_next_y - cr;//pixel below
                float dgy = cg_next_y - cg;

                if (src(y, x).i == SPECULARX) {

                    //if it is  a boundary in the x direction
                    if (fabs(drx) > thR && fabs(dgx) > thG) { //pixel right
                        src(y, x).i = BOUNDARY;
                        continue;
                    }

                    //if it is a noise
                    if (fabs(src(y, x).zMaxChroma() - src(y, x + 1).zMaxChroma()) < 0.01) {
                        src(y, x).i = NOISE;
                        continue;
                    }

                    //reduce the specularity at x direction
                    if (src(y, x).zMaxChroma() < src(y, x + 1).zMaxChroma()) {
                        zSpecular2Diffuse(src(y, x), src(y, x + 1).zMaxChroma());
                        src(y, x).i = DIFFUSE;
                        src(y, x + 1).i = DIFFUSE;
                    } else {
                        zSpecular2Diffuse(src(y, x + 1), src(y, x).zMaxChroma());
                        src(y, x).i = DIFFUSE;
                        src(y, x + 1).i = DIFFUSE;
                    }
                }

                if (src(y, x).i == SPECULARY) {
                    //if it is a boundary in the y direction
                    if (fabs(dry) > thR && fabs(dgy) > thG) { //pixel right
                        src(y, x).i = BOUNDARY;
                        continue;
                    }

                    //if it is a noise
                    if (fabs(src(y, x).zMaxChroma() - src(y + 1, x).zMaxChroma()) < 0.01) {
                        src(y, x).i = NOISE;
                        continue;
                    }

                    //reduce the specularity in y direction
                    if (src(y, x).zMaxChroma() < src(y + 1, x).zMaxChroma()) {
                        zSpecular2Diffuse(src(y, x), src(y + 1, x).zMaxChroma());
                        src(y, x).i = DIFFUSE;
                        src(y + 1, x).i = DIFFUSE;
                    } else {
                        zSpecular2Diffuse(src(y + 1, x), src(y, x).zMaxChroma());
                        src(y, x).i = DIFFUSE;
                        src(y + 1, x).i = DIFFUSE;
                    }
                }
            }
        }
        pcount = count;
        count = zInit(src, sfi, epsilon);
        //::zShow(src);
        if (count < 0)
            break;
        if (pcount <= count)
            break;
    }
    zResetLabels(src);

    return 0;
}

int zHighlightRemoval::zInit(zArray2D<s_rgbi> &src, zArray2D<s_rgbi> &sfi, float epsilon) {
    // to have initial labels

    int x, y;    // pixel iterators

    int count = 0;

    for (y = 1; y < src.zGetMaxY() - 1; y++) {
        for (x = 1; x < src.zGetMaxX() - 1; x++) {
            //

            switch ((int) src(y, x).i) {
                case BOUNDARY:
                case NOISE:
                case CAMERA_DARK:
                    continue;
                    break;
            }

            float dlog_src_x = log(fabs(src(y, x + 1).zTotal() - src(y, x).zTotal()));
            float dlog_src_y = log(fabs(src(y + 1, x).zTotal() - src(y, x).zTotal()));

            float dlog_sfi_x = log(fabs(sfi(y, x + 1).zTotal() - sfi(y, x).zTotal()));
            float dlog_sfi_y = log(fabs(sfi(y + 1, x).zTotal() - sfi(y, x).zTotal()));

            float dlogx = (dlog_src_x - dlog_sfi_x);
            float dlogy = (dlog_src_y - dlog_sfi_y);


            dlogx = fabs(dlogx);
            dlogy = fabs(dlogy);

            // specular in the x direction
            if (dlogx > epsilon) {
                src(y, x).i = SPECULARX;
                count++;
                continue;  // go to the next pixel
            }
            //specular in the y direction
            if (dlogy > epsilon) {
                src(y, x).i = SPECULARY;
                count++;
                continue;
            }

            src(y, x).i = DIFFUSE;
        }
    }
    return count;    // return the number of specular pixels
}


int zHighlightRemoval::zSpecular2Diffuse(s_rgbi &iro, float maxChroma) {
    // to apply specular to diffuse equation or mechanism

    float c = iro.zMaxChroma();
    float dI = (iro.zMax() * (3.0f * c - 1.0f)) / (c * (3.0f * maxChroma - 1.0f));
    float sI = (iro.zTotal() - dI) / 3.0f;


    float nr = (iro.r - sI);
    float ng = (iro.g - sI);
    float nb = (iro.b - sI);


    if (nr <= 0 || ng <= 0 || nb <= 0) {
        iro.i = NOISE;
        return 1;
    }

    iro.b = nb;
    iro.g = ng;
    iro.r = nr;

    return 0;
}


int zHighlightRemoval::zResetLabels(zArray2D<s_rgbi> &src) {
    // to reset the label of the pixels

    for (int y = 0; y < src.zGetMaxY(); y++) {
        for (int x = 0; x < src.zGetMaxX(); x++) {
            if (src(y, x).i == CAMERA_DARK) continue;
            src(y, x).i = 0;
        }
    }
    return 0;
}
