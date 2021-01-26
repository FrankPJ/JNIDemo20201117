/*
 *  tslib/tests/ts_calibrate.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the GPL.  Please see the file
 * COPYING for more details.
 *
 * $Id: ts_calibrate.c,v 1.8 2004/10/19 22:01:27 dlowder Exp $
 *
 * Basic test program for touchscreen library.
 */

//#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <linux/kd.h>
#include <linux/input.h>
#include <linux/vt.h>
#include <linux/fb.h>
#include <math.h>

#include <unistd.h>
#include <fcntl.h>
#include "jni.h"
//#include <utils/Log.h>
#include <android/log.h>
//#include <cutils/properties.h>

//#define LOG_TAG "Calibrate"


//#include "tslib.h"

//#include "fbutils.h"
//#include "testutils.h"

//#include "touchp.h"

static const char *TAG="calibrateJni";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

static int palette [] =
{
	0x000000, 0xffe080, 0xffffff, 0xe0c0a0
};
#define NR_COLORS (sizeof (palette) / sizeof (palette [0]))

#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_RIGHT 2
#define BOTTOM_LEFT 3
#define CENTER 4
#define MAX_RANGE_FACTOR 1.3
#define MIN_RANGE_FACTOR 0.7

#define ADC_PRECISION       12          // Precision of ADC output (in bits)
#define MAX_TERM_PRECISION  27          // Reserve 1 bit for sign and two bits for
#define MAX_POINT_ERROR 5

#define BOOL long
typedef int      INT;
typedef signed      char        INT8;
typedef unsigned    char        UINT8;
typedef signed      short       INT16;
typedef unsigned    short       UINT16;
typedef signed      int         INT32;
typedef unsigned    int         UINT32;

#define MAX_COEFF_PRECISION (MAX_TERM_PRECISION - ADC_PRECISION)
#define TRUE 1
#define FALSE 0
#define VOID void

typedef struct {
    INT32   a1;
    INT32   b1;
    INT32   c1;
    INT32   a2;
    INT32   b2;
    INT32   c2;
    INT32   delta;
}   CALIBRATION_PARAMETER, *PCALIBRATION_PARAMETER;

static BOOL                     v_Calibrated = FALSE;
static CALIBRATION_PARAMETER    v_CalcParam;
#define MAX_NUMBER_OF_EVENTS 5


struct ts_sample {
	int		x;
	int		y;
	unsigned int	pressure;
};



#define GRAB_EVENTS_WANTED	1
#define GRAB_EVENTS_ACTIVE	2





#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define BIT(nr)                 (1UL << (nr))
#define BIT_MASK(nr)            (1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)            ((nr) / BITS_PER_LONG)
#define BITS_PER_BYTE           8
#define BITS_PER_LONG           (sizeof(long) * BITS_PER_BYTE)
#define BITS_TO_LONGS(nr)       DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#define MAX_CHECK_READ_COUNT 100

typedef struct {
   INT32	cCalibrationPoints;
   INT32	ScreenXBuffer[5];
   INT32	ScreenYBuffer[5];
   INT32	UncalXBuffer[5];
   INT32 	UncalYBuffer[5];
}   CALIBRATEDATA, *PCALIBRATEDATA;

CALIBRATEDATA cal;

typedef struct {
	int x[5] , xfb[5];
	int y[5] , yfb[5];
	int a[7];
} calibration;

#define NORMAL_MODE             0
#define CALIBRATION_MODE        1
#define SET_CALIBRATE_STATE  4
#define SET_CALIBRATE_DATA  3



#define DISPLAY_X 800
#define DISPLAY_Y 480


//#define DISPLAY_X 1280
//#define DISPLAY_Y 720

int touch_fd = -1;
int calibrate_state = 0;

int get_touch_fd()
{
    return touch_fd;
}

#define TS_DEVICE  "/dev/ns2009"


#define SIZE_OF_LARGENUM    3   // Number of UINT32 in a LARGE_NUM

typedef struct {
    BOOL    fNegative;
    union {
        struct {
            UINT16  s[2*SIZE_OF_LARGENUM];
        }   s16;
        struct {
            UINT32  u[SIZE_OF_LARGENUM];
        }   s32;
    }   u;
} LARGENUM, *PLARGENUM;


typedef struct {
    PLARGENUM   pa11, pa12, pa13;
    PLARGENUM   pa21, pa22, pa23;
    PLARGENUM   pa31, pa32, pa33;
}   MATRIX33, *PMATRIX33;



struct TPDC_CALIBRATION_POINT
{
	int		PointNumber;
	int		cDisplayWidth;
	int		cDisplayHeight;
	int		CalibrationX;
	int		CalibrationY;
};


//
// Function prototypes
//
UINT32
LargeNumSignedFormat(
    PLARGENUM   pNum
    );

//
// Routines start
//

/*++

LargeNumSet:

    Initialized the content of a large integer to a given value.

Arguments:

    pNum    - Points to the large integer to be initializeed.
    n       - the specific signed value used to initialize the large integer.

Return Value:

    Pointer to the large integer

--*/
PLARGENUM
LargeNumSet(
    PLARGENUM   pNum,
    INT32       n
    )
{
    int i;

    if(n < 0){
        pNum->u.s32.u[0] = -n;
        pNum->fNegative = 1;
    } else{
        pNum->u.s32.u[0] = n;
        pNum->fNegative=0;
    }
    for(i=1; i<SIZE_OF_LARGENUM; i++){
        pNum->u.s32.u[i] = 0;
    }
    return pNum;
}

/*++

IsLargeNumNotZero:

    Test the content of a large number for zero.

Arguments:

    pNum    - Points to the large integer to be initializeed.

Return Value:

    TRUE,   if the large number is zero.
    FALSE,  otherwise.

--*/
BOOL
IsLargeNumNotZero(
    PLARGENUM   pNum
    )
{
    int i;

    for(i=0; i<SIZE_OF_LARGENUM; i++){
        if(pNum->u.s32.u[i]){
            return TRUE;
        }
    }
    return FALSE;
}

/*++

IsLargeNumNegative:

    Test the content of a large number for negative value.

Arguments:

    pNum    - Points to the large integer to be initializeed.

Return Value:

    TRUE,   if the large number is negative.
    FALSE,  otherwise.

Note:

    Zero is consider positive.

--*/
BOOL
IsLargeNumNegative(
    PLARGENUM   pNum
    )
{
    return (pNum->fNegative ? TRUE : FALSE);

}

/*++

IsLargeNumMagGreaterThan:

    Compare the contents (magnitude) of two large numbers.

Arguments:

    pNum1   - Points to the first large integer to be compared
    pNum2   - Points to the second large integer to be compared.

Return Value:

    TRUE,   if the first number is greater than the second.
    FALSE,  otherwise.

Note:

    Only the magnitudes of the numbers are compared.  The signs are ignored.

--*/
BOOL
IsLargeNumMagGreaterThan(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2
    )
{
    int i;

    for(i=SIZE_OF_LARGENUM-1; i>=0; i--){
        if(pNum1->u.s32.u[i] > pNum2->u.s32.u[i]){
            return TRUE;
        } else if(pNum1->u.s32.u[i] < pNum2->u.s32.u[i]){
            return FALSE;
        }
    }
    return FALSE;
}

/*++

IsLargeNumMagLessThan:

    Compare the contents (magnitude) of two large numbers.

Arguments:

    pNum1   - Points to the first large integer to be compared
    pNum2   - Points to the second large integer to be compared.

Return Value:

    TRUE,   if the first number is less than the second.
    FALSE,  otherwise.

Note:

    Only the magnitudes of the numbers are compared.  The signs are ignored.

--*/
BOOL
IsLargeNumMagLessThan(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2
    )
{
    int i;

    for(i=SIZE_OF_LARGENUM-1; i>=0; i--){
        if(pNum1->u.s32.u[i] < pNum2->u.s32.u[i]){
            return TRUE;
        } else if(pNum1->u.s32.u[i] > pNum2->u.s32.u[i]){
            return FALSE;
        }
    }
    return FALSE;
}

/*++

LargeNumMagInc:

    Increment the magnitude of a large number.

Arguments:

    pNum    - Points to the large integer to be incremented

Return Value:

    The same pointer to the large number.

Note:

    Only the magnitude of the number is incremented.
    In other words, if the number is negative, this routine actually
    decrement it (by increment its magnitude).

--*/
PLARGENUM
LargeNumMagInc(
    PLARGENUM   pNum
    )
{
    UINT32  c;
    int     i;

    c = 1;
    for(i=0; i<SIZE_OF_LARGENUM; i++){
        pNum->u.s32.u[i] += c;
        if(pNum->u.s32.u[i]){
            c = 0;
        }
    }
    return pNum;
}

/*++

LargeNumMagAdd:

    Sums up the magnitude of two large numbers.  Signs are ignored.

Arguments:

    pNum1   - Points to the first large integer to be added to.
    pNum2   - Points to the second large integer to be added with.
    pResult - Points to the a large integer buffer to receive the sum.

Return Value:

    The same pointer as pResult.

Note:

    Only the magnitudes of the large numbers are summed.
    Signs are ignored.
    In other words, if the signs are different, this routine actually
    subtracts them (by summing their magnitudes).

--*/
PLARGENUM
LargeNumMagAdd(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2,
    PLARGENUM   pResult
    )
{
    UINT32      c;
    UINT32      i;
    UINT32      a;
    UINT32      b;

    c = 0;
    for(i=0; i<SIZE_OF_LARGENUM; i++){
        a = pNum1->u.s32.u[i];
        b = pNum2->u.s32.u[i];
        pResult->u.s32.u[i] = a + b + c;
        if(c){
            if(pResult->u.s32.u[i] <= a){
                c = 1;
            } else {
                c = 0;
            }

        } else {
            if(pResult->u.s32.u[i] < a){
                c = 1;
            } else {
                c = 0;
            }

        }
    }
    return pResult;
}

/*++

LargeNumMagSub:

    Similar to LargeNumMagAdd except that, instead of sum, the difference is taken.

Arguments:

    pNum1   - Points to the first large integer to be subtracted from.
    pNum2   - Points to the second large integer to be substracted with.
    pResult - Points to the a large integer buffer to receive the difference.

Return Value:

    The same pointer as pResult.

Note:

    Only the magnitudes of the large numbers are summed.
    Signs are ignored.
    In other words, if the signs are different, this routine actually
    adds them (by substracting their magnitudes).

--*/
PLARGENUM
LargeNumMagSub(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2,
    PLARGENUM   pResult
    )
{
    UINT32      c;
    UINT32      i;
    UINT32      a;
    UINT32      b;

    c = 1;
    for(i=0; i<SIZE_OF_LARGENUM; i++){
        a = pNum1->u.s32.u[i];
        b = ~(pNum2->u.s32.u[i]);
        pResult->u.s32.u[i] = a + b + c;
        if(c){
            if(pResult->u.s32.u[i] <= a){
                c = 1;
            } else {
                c = 0;
            }

        } else {
            if(pResult->u.s32.u[i] < a){
                c = 1;
            } else {
                c = 0;
            }

        }
    }
    return pResult;
}

/*++

LargeNumAdd:

    Sums up two large numbers.  Signs are taken into account.

Arguments:

    pNum1   - Points to the first large integer to be added to.
    pNum2   - Points to the second large integer to be added with.
    pResult - Points to the a large integer buffer to receive the sum.

Return Value:

    The same pointer as pResult.

--*/
PLARGENUM
LargeNumAdd(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2,
    PLARGENUM   pResult
    )
{
    BOOL    fNegative1;
    BOOL    fNegative2;

    fNegative1 = IsLargeNumNegative(pNum1);
    fNegative2 = IsLargeNumNegative(pNum2);

    if(fNegative1 != fNegative2){
        if(IsLargeNumMagGreaterThan(pNum1, pNum2)){
            LargeNumMagSub(pNum1, pNum2, pResult);
        } else {
            LargeNumMagSub(pNum2, pNum1, pResult);
            fNegative1 = !fNegative1;
        }
    } else {
        LargeNumMagAdd(pNum1, pNum2, pResult);
    }
    if(!IsLargeNumNotZero(pResult)){
        pResult->fNegative = FALSE;
    } else {
        pResult->fNegative = fNegative1;
    }
    return pResult;
}

/*++

LargeNumSub:

    Substracts a large numbers from another.  Signs are taken into account.

Arguments:

    pNum1   - Points to the first large integer to be subtracted from.
    pNum2   - Points to the second large integer to be substracted with.
    pResult - Points to the a large integer buffer to receive the difference.

Return Value:

    The same pointer as pResult.

--*/
PLARGENUM
LargeNumSub(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2,
    PLARGENUM   pResult
    )
{
    BOOL    fNegative1;
    BOOL    fNegative2;

    fNegative1 = IsLargeNumNegative(pNum1);
    fNegative2 = IsLargeNumNegative(pNum2);

    if(fNegative1 == fNegative2){
        if(IsLargeNumMagGreaterThan(pNum1, pNum2)){
            LargeNumMagSub(pNum1, pNum2, pResult);
        } else {
            LargeNumMagSub(pNum2, pNum1, pResult);
            fNegative1 = !fNegative1;
        }
    } else {
        LargeNumMagAdd(pNum1, pNum2, pResult);
    }
    if(!IsLargeNumNotZero(pResult)){
        pResult->fNegative = FALSE;
    } else {
        pResult->fNegative = fNegative1;
    }
    return pResult;
}

//
//
//
#if SIZE_OF_LARGENUM < 2
#error  SIZE_OF_LARGENUM must be at least 2
#endif

/*++

LargeNumMulUint32:

    Multiply two unsigned 32-bit numbers.

Arguments:

    a       - the 32-bit multipliant.
    b       - the 32-bit multiplier.
    pResult - Points to the a large integer buffer to receive the difference.

Return Value:

    The same pointer as pResult.

Note:

    The sign of the returned large number is set to positive.

--*/
PLARGENUM
LargeNumMulUint32(
    UINT32      a,
    UINT32      b,
    PLARGENUM   pResult
    )
{
    UINT32  a1, a0;
    UINT32  b1, b0;
    UINT32  r0;
    UINT32  r1;
    UINT32  r2;
    UINT32  c;
    int     i;

    a1 = a >> 16;
    a0 = a & 0xffff;
    b1 = b >> 16;
    b0 = b & 0xffff;

    r0 = a0 * b0;
    r1 = a1 * b0 + a0 * b1;
    r2 = a1 * b1;

    pResult->u.s32.u[0] = (r1 << 16) + r0;
    if(pResult->u.s32.u[0] < r0){
        c = 1;
    } else {
        c = 0;
    }
    pResult->u.s32.u[1] = r2 + (r1 >> 16) + c;
    for(i=2; i<SIZE_OF_LARGENUM; i++){
        pResult->u.s32.u[i] = 0;
    }
    pResult->fNegative = 0;

    return pResult;
}

/*++

LargeNumMulInt32:

    Multiply two signed 32-bit numbers.

Arguments:

    a       - the 32-bit multipliant.
    b       - the 32-bit multiplier.
    pResult - Points to the a large integer buffer to receive the difference.

Return Value:

    The same pointer as pResult.

Note:

    The sign of the returned large number is set according to the
    multiplication result.

--*/
PLARGENUM
LargeNumMulInt32(
    INT32       a,
    INT32       b,
    PLARGENUM   pResult
    )
{
    BOOL        fNegativeA;
    BOOL        fNegativeB;

    if(a < 0){
        fNegativeA = TRUE;
        a = -a;
    } else {
        fNegativeA = FALSE;
    }

    if(b < 0){
        fNegativeB = TRUE;
        b = -b;
    } else {
        fNegativeB = FALSE;
    }

    LargeNumMulUint32(a, b, pResult);

    if(!IsLargeNumNotZero(pResult)){
        pResult->fNegative = FALSE;
    } else {
        if(fNegativeA != fNegativeB){
            pResult->fNegative = TRUE;
        }
    }
    return pResult;
}

/*++

LargeNumMult:

    Multiply two large numbers.

Arguments:

    pNum1   - points to the large multipliant.
    b       - points to the large multiplier.
    pResult - Points to the a large integer buffer to receive the difference.

Return Value:

    The same pointer as pResult.

Note:

    The sign of the returned large number is set according to the
    multiplication result.

--*/
PLARGENUM
LargeNumMult(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2,
    PLARGENUM   pResult
    )
{
    LARGENUM    lNumTemp;
    LARGENUM    lNumSum;
    LARGENUM    lNumCarry;
    int         i;
    int         j;

    LargeNumSet(&lNumCarry, 0);
    for(i=0; i<SIZE_OF_LARGENUM; i++){
        LargeNumSet(&lNumSum, 0);
        for(j=0; j<=i; j++){
            LargeNumMulUint32(pNum1->u.s32.u[j], pNum2->u.s32.u[i-j], &lNumTemp);
            LargeNumMagAdd(&lNumTemp, &lNumSum, &lNumSum);
        }
        LargeNumMagAdd(&lNumCarry, &lNumSum, &lNumSum);
        for(j=0; j<SIZE_OF_LARGENUM-1; j++){
            lNumCarry.u.s32.u[j] = lNumSum.u.s32.u[j+1];
        }
        pResult->u.s32.u[i] = lNumSum.u.s32.u[0];
    }

    if(!IsLargeNumNotZero(pResult)){
        pResult->fNegative = FALSE;
    } else {
        pResult->fNegative = (pNum1->fNegative != pNum2->fNegative);
    }
    return pResult;
}

/*++

LargeNumSignedFormat:

    Convert a LARGENUM into signed 2's complement format.

Arguments:

    pNum    - points to the large number to be shifted.

Return Value:

    0xffffffff  : if the number is negative
    0           : if the number is zero or positive

Note:

    The result is in 2's completement notation and the vacant
    bits are sign extended.

    -- Important ---

    *pNum is detroyed by this operation and should not be used as LARGENUM
    after returned from this routine.

--*/
UINT32
LargeNumSignedFormat(
    PLARGENUM   pNum
    )
{
    int     i;
    UINT32  c;

    if(IsLargeNumNegative(pNum)){
        c = 1;
        for(i=0; i<SIZE_OF_LARGENUM; i++){
            pNum->u.s32.u[i] = ~(pNum->u.s32.u[i]) + c;
            if(pNum->u.s32.u[i]){
                c = 0;
            }
        }
        return 0xffffffff;
    } else {
        return 0;
    }
}

/*++

LargeNumRAShift:

    Arithmatic shift a given large number right by a specific number
    of positions.

Arguments:

    pNum    - points to the large number to be shifted.
    count   - number of pistion to shift.

Return Value:

    -- none --

Note:

    The result is in 2's completement notation and the vacant
    bits are sign extended.

    -- Important ---

    *pNum is detroyed by this operation and should not be used as LARGENUM
    after returned from this routine.

--*/
VOID
LargeNumRAShift(
    PLARGENUM   pNum,
    INT32       count
    )
{
    INT32   shift32;
    INT32   countLeft;
    UINT32  filler;
    int     i;
    int     j;

    filler = LargeNumSignedFormat(pNum);

    shift32 = count / 32;

    if(shift32 > (SIZE_OF_LARGENUM - 1)){
        for(i=0; i<SIZE_OF_LARGENUM; i++){
            pNum->u.s32.u[i] = filler;
        }
        return;
    }

    count %= 32;
    countLeft = 32 - count;
    for(i=0, j=shift32;;){
        pNum->u.s32.u[i] = (pNum->u.s32.u[j] >> count);
        if(j<(SIZE_OF_LARGENUM-1)){
            j++;            
            if (countLeft < 32) {
                // Shifting by >= 32 is undefined.
                pNum->u.s32.u[i] |= pNum->u.s32.u[j] << countLeft;
            }
            i++;
        } else {
            if (countLeft < 32) {
                // Shifting by >= 32 is undefined.
                pNum->u.s32.u[i] |= filler << countLeft;
            }
            i++;
            break;
        }
    }

    for(; i<SIZE_OF_LARGENUM; i++){
        pNum->u.s32.u[i] = filler;
    }
}

/*++

LargeNumDivInt32:

    Divide a large number with a signed 32-bit number.

Arguments:

    pNum    - points to the large dividant.
    divisor - the 32-bit divisor.
    pResult - Points to the a large integer buffer to receive the difference.

Return Value:

    The remainder.

Note:

    If divisor is zero, the result will be filled with 0xffffffff and
    the remainder returned is 0xffffffff.

    Also, the remainder is adjusted so that it is always positive and
    is within the range between 0 and the absolute value of divisor.

--*/
UINT32
LargeNumDivInt32(
    PLARGENUM   pNum,
    INT32       divisor,
    PLARGENUM   pResult
    )
{
    UINT32  s[2*SIZE_OF_LARGENUM];
    UINT32  r;
    UINT32  q;
    UINT32  d;
    BOOL    sd;
    int     i;

    for(i=0; i<2*SIZE_OF_LARGENUM; i++){
        s[i] = pNum->u.s16.s[i];
    }

    if(divisor < 0){
        divisor = -divisor;
        sd = TRUE;
    } else if(divisor == 0){
        //
        // This is a divide-by-zero error
        //
        for(i=0; i<SIZE_OF_LARGENUM; i++){
            pResult->u.s32.u[i] = 0xffffffff;
        }
        return 0xffffffff;
    } else {
        sd = FALSE;
    }

    r = 0;
    for(i=(2*SIZE_OF_LARGENUM-1); i>=0; i--){
        d = (r << 16) + s[i];
        q = d / divisor;
        r = d - q * divisor;
        s[i] = q;
    }

    for(i=0; i<2*SIZE_OF_LARGENUM; i++){
        pResult->u.s16.s[i] = s[i];
    }

    if(pNum->fNegative){
        LargeNumMagInc(pResult);
        r = divisor - r;
        if(sd == 0 && IsLargeNumNotZero(pResult)){
            pResult->fNegative = TRUE;
        } else {
            pResult->fNegative = FALSE;
        }

    } else {
        if(sd && IsLargeNumNotZero(pResult)){
            pResult->fNegative = TRUE;
        } else {
            pResult->fNegative = FALSE;
        }
    }

    return r;
}

/*++

LargeNumBits:

    Finds the number of significant bits in a large number.  Basically, it
    is the position of the highest one bit plus 1.

Arguments:

    pNum    - points to the large number.

Return Value:

    The number of significant bits.

--*/
INT32
LargeNumBits(
    PLARGENUM   pNum
    )
{
    static  UINT32 LargeNumMask[32] = {
        0x00000001,
        0x00000002,
        0x00000004,
        0x00000008,
        0x00000010,
        0x00000020,
        0x00000040,
        0x00000080,
        0x00000100,
        0x00000200,
        0x00000400,
        0x00000800,
        0x00001000,
        0x00002000,
        0x00004000,
        0x00008000,
        0x00010000,
        0x00020000,
        0x00040000,
        0x00080000,
        0x00100000,
        0x00200000,
        0x00400000,
        0x00800000,
        0x01000000,
        0x02000000,
        0x04000000,
        0x08000000,
        0x10000000,
        0x20000000,
        0x40000000,
        0x80000000,
        };

    int     i;
    int     j;
    UINT32  u;

    for(i=(SIZE_OF_LARGENUM-1); i>=0; i--){
        u = pNum->u.s32.u[i];
        if(u){
            for(j=31; j>=0; j--){
                if(u & (LargeNumMask[j])){
                    return i * 32 + j + 1;
                }
            }
        }
    }
    return 0;
}

/*++

LargeNumToAscii:

    Converts a large integer into ASCIIZ string.

Arguments:

    pNum    - points to the large number.

Return Value:

    Pointer to the ASCIIZ string.

Note:

    Since the string is a static area inside the routine, this routine is
    not re-entrant and, most importantly, once it is called, the previous
    content is gone.

--*/
char *
LargeNumToAscii(
    PLARGENUM   pNum
    )
{
    static  char    buf[SIZE_OF_LARGENUM * 10 + 2];
    LARGENUM        lNum;
    char            *p;
    char            *q;
    UINT32          r;
    int             s;

    p = buf + sizeof(buf) - 1;
    *p= 0;

    lNum = *pNum;

    s = pNum->fNegative;
    lNum.fNegative = 0;

    while(IsLargeNumNotZero(&lNum)){
        r = LargeNumDivInt32(&lNum, 10, &lNum);
        p--;
        *p = r + '0';
    }

    q = buf;

    if(s){
        *q++='-';
    }
	/*
    while(*p){
        ASSERT(q <= p);
        PREFAST_SUPPRESS(394, "q is <= p");
        *q++ = *p++;
    }
*/
    if((q == buf) || (s && q == &(buf[1]))){
        *q++ = '0';
    }
    *q = 0;
    return buf;
}



VOID
ComputeMatrix33(
    PLARGENUM   pResult,
    PMATRIX33   pMatrix
    )
{
    LARGENUM    lnTemp;

    LargeNumMult(pMatrix->pa11, pMatrix->pa22, &lnTemp);
    LargeNumMult(pMatrix->pa33, &lnTemp, pResult);
    LargeNumMult(pMatrix->pa21, pMatrix->pa32, &lnTemp);
    LargeNumMult(pMatrix->pa13, &lnTemp, &lnTemp);
    LargeNumAdd(pResult, &lnTemp, pResult);
    LargeNumMult(pMatrix->pa12, pMatrix->pa23, &lnTemp);
    LargeNumMult(pMatrix->pa31, &lnTemp, &lnTemp);
    LargeNumAdd(pResult, &lnTemp, pResult);
    LargeNumMult(pMatrix->pa13, pMatrix->pa22, &lnTemp);
    LargeNumMult(pMatrix->pa31, &lnTemp, &lnTemp);
    LargeNumSub(pResult, &lnTemp, pResult);
    LargeNumMult(pMatrix->pa12, pMatrix->pa21, &lnTemp);
    LargeNumMult(pMatrix->pa33, &lnTemp, &lnTemp);
    LargeNumSub(pResult, &lnTemp, pResult);
    LargeNumMult(pMatrix->pa23, pMatrix->pa32, &lnTemp);
    LargeNumMult(pMatrix->pa11, &lnTemp, &lnTemp);
    LargeNumSub(pResult, &lnTemp, pResult);
}


BOOL getTouchPoint(CALIBRATEDATA*pXY)
{
    // Why INT/INT32? It is used for WinCE API.
    INT32 i32DisplayWidth = 0;
    INT32 i32DisplayHeight = 0;
    INT iCalibrationRadiusX = 0;
    INT iCalibrationRadiusY = 0;

	i32DisplayWidth = DISPLAY_X;
	i32DisplayHeight = DISPLAY_Y;

	iCalibrationRadiusX = i32DisplayWidth / 20;
	iCalibrationRadiusY = i32DisplayHeight / 20;

	for (int i=0;i<5;i++)
	{
		switch (i)
		{
		case  0:  // Middle
			pXY->ScreenXBuffer[i] = i32DisplayWidth / 2;
			pXY->ScreenYBuffer[i] = i32DisplayHeight / 2;
			break;

		case  1:  // Upper Left
			pXY->ScreenXBuffer[i] = iCalibrationRadiusX * 2;//80
			pXY->ScreenYBuffer[i] = iCalibrationRadiusY * 2;//48
			break;

		case  2:  // Lower Left
			pXY->ScreenXBuffer[i] = iCalibrationRadiusX * 2;//80
			pXY->ScreenYBuffer[i] = i32DisplayHeight - iCalibrationRadiusY * 2;//480-96
			break;

		case  3:  // Lower Right
			pXY->ScreenXBuffer[i] = i32DisplayWidth - iCalibrationRadiusX * 2;
			pXY->ScreenYBuffer[i]  = i32DisplayHeight - iCalibrationRadiusY * 2;
			break;

		case  4:  // Upper Right
			pXY->ScreenXBuffer[i] = i32DisplayWidth - iCalibrationRadiusX * 2;
			pXY->ScreenYBuffer[i]  = iCalibrationRadiusY * 2;
			break;

		default:
			pXY->ScreenXBuffer[i] = i32DisplayWidth / 2;
			pXY->ScreenYBuffer[i] = i32DisplayHeight / 2;
			return (FALSE);
		}
	}
	return TRUE;
}



/*++

Routine Description:

    Convert uncalibrated points to calibrated points.


Autodoc Information:

    @doc INTERNAL DRIVERS MDD TOUCH_DRIVER
    @func VOID | TouchPanelCalibrateAPoint |
    Convert uncalibrated points to calibrated points.

    @comm
    The transform coefficients are already in vCalcParam.<nl>


    @devnote
    Note that there is a *4 hiding in the calculations of X and
    Y.  This is a means of providing sub-pixel accuracy to GWE,
    which will theoretically help improve accuracy in inking. It
    would be nice if this multiplier were defined in a .h file
    by gwe, but it isn't.  So make very sure that this value
    agrees with the divisor used by gwe in touch.cpp.
--*/


void
TouchPanelCalibrateAPoint(
    INT32   UncalX,     //@PARM The uncalibrated X coordinate
    INT32   UncalY,     //@PARM The uncalibrated Y coordinate
    INT32   *pCalX,     //@PARM The calibrated X coordinate
    INT32   *pCalY      //@PARM The calibrated Y coordinate
    )
{
    INT32   x, y;

    if ( !v_Calibrated ){
        *pCalX = UncalX;
        *pCalY = UncalY;
        return;
    }
     //
     // Note the *4 in the expression below.  This is a workaround
     // on behalf of gwe.  It provides a form of
     // sub-pixel accuracy desirable for inking
     //
    x = (v_CalcParam.a1 * UncalX + v_CalcParam.b1 * UncalY +
         v_CalcParam.c1) * 4 / v_CalcParam.delta;
    y = (v_CalcParam.a2 * UncalX + v_CalcParam.b2 * UncalY +
         v_CalcParam.c2) * 4 / v_CalcParam.delta;
    if ( x < 0 ){
        x = 0;
    }

    if ( y < 0 ){
        y = 0;
    }
        LOGD("TouchPanelCalibrateAPoint   v_CalcParam.a1 = %d v_CalcParam.b1 =%d UncalX = %d  UncalY = %d v_CalcParam.c1 = %d v_CalcParam.delta = %d \r\n",
            v_CalcParam.a1, v_CalcParam.b1, UncalX, UncalY, v_CalcParam.c1, v_CalcParam.delta);

    *pCalX = x;
    *pCalY = y;
}

/*++

    @doc INTERNAL DRIVERS MDD TOUCH_DRIVER
    @func VOID | ErrorAnalysis |
    Display info on accuracy of the calibration.


--*/
     
BOOL
ErrorAnalysis(
    INT32   cCalibrationPoints,     //@PARM The number of calibration points
    INT32   *pScreenXBuffer,        //@PARM List of screen X coords displayed
    INT32   *pScreenYBuffer,        //@PARM List of screen Y coords displayed
    INT32   *pUncalXBuffer,         //@PARM List of X coords collected
    INT32   *pUncalYBuffer          //@PARM List of Y coords collected
    )
{
    int     i;
    UINT32  maxErr, err;
    INT32   x,y;
    INT32   dx,dy;
    UINT32  errThreshold = MAX_POINT_ERROR;  // Can be overridden by registry entry
    UINT32  status, ValType, ValLen;
errThreshold = 0xA;
    maxErr = 0;
    for(i=0; i<cCalibrationPoints; i++){
        TouchPanelCalibrateAPoint(  pUncalXBuffer[i],
                                    pUncalYBuffer[i],
                                    &x,
                                    &y
                                    );
        x /= 4;
        y /= 4;

        dx = x - pScreenXBuffer[i];
        dy = y - pScreenYBuffer[i];
        err = dx * dx + dy * dy;
        if(err > maxErr){
            maxErr = err;
        }
        LOGD("TouchPanelCalibrateAPoint   x = %d y =%d pScreenXBuffer[i] = %d  pScreenYBuffer[i] = %d \r\n",x, y, pScreenXBuffer[i], pScreenYBuffer[i]);
    }

    if( maxErr < (errThreshold * errThreshold) ){
       return TRUE;
    } else {
           LOGD("Maximum error %u exceeds calibration threshold %u\r\n",maxErr, errThreshold);

       return FALSE;
    }

}

int set_Calibrate_data(void)
{

	int fd = get_touch_fd();
	if (fd < 0) 
	{
		LOGD("open tsdevice");
		return -1;
	}
	if (ioctl(fd, SET_CALIBRATE_DATA, &v_CalcParam) < 0) 
	{
		LOGD("ioctl set_Calibrate_data");
		return -1;
	}    
	return 0;
}


typedef struct Pointxy{
	
	int			PointInx;
	unsigned short	ScreenX;
	unsigned short	ScreenY;
	unsigned short	CoordsX;
	unsigned short	CoordsY;
}PointXY;


#define TOUCH_FILENAME "/data/local/calibratefile"
void SaveCalibrationPoints(INT32 cCalibrationPoints, INT32 * pScreenXBuffer, INT32 * pScreenYBuffer, INT32 * pUncalXBuffer, INT32 * pUncalYBuffer)
{
	PointXY		CalibrationPoints[5];	//5��У׼��
	int 			i;
	int		hFile;
	char		SaveFileName[] = TOUCH_FILENAME;

	if(calibrate_state == 0)
		return;

    remove(SaveFileName);
    usleep(5000);
    LOGD("SaveCalibrationPoints\n");

	if((hFile=open(SaveFileName, O_WRONLY|O_CREAT,0777))==-1) 
	{
		LOGD("open %s fail\n",SaveFileName);
		return;
	}

	if(cCalibrationPoints <= 0)
	{
        close(hFile); 
		return ;
	}

	for(i = 0; i < cCalibrationPoints; i++)
	{
		CalibrationPoints[i].PointInx = (int)i;
		CalibrationPoints[i].ScreenX = pScreenXBuffer[i];
		CalibrationPoints[i].ScreenY = pScreenYBuffer[i];
		CalibrationPoints[i].CoordsX = pUncalXBuffer[i];
		CalibrationPoints[i].CoordsY = pUncalYBuffer[i];
		LOGD("save:%d:	0x%x, 0x%x, 0x%x, 0x%x\n", i, pScreenXBuffer[i], pScreenYBuffer[i], pUncalXBuffer[i], pUncalYBuffer[i]);
}
	write(hFile, CalibrationPoints, sizeof(PointXY) * 5);
	write(hFile, CalibrationPoints, sizeof(PointXY) * 5);
	fsync(hFile);
	close(hFile); 
	return;
	
}

int	ReadCalibrationPoints(
		INT32	*pScreenXBuffer,
		INT32	*pScreenYBuffer,
		INT32	*pUncalXBuffer,
		INT32	*pUncalYBuffer
								)
{
	PointXY		CalibrationPoints[5];
	PointXY 		DumpCalibrationPoints[5];
	int			i;
	int			point = 0;
	int 		hFile;
	char		SaveFileName[] = TOUCH_FILENAME;
    int check = 0;
    LOGD("ReadCalibrationPoints\n");

	if((hFile=open(SaveFileName, O_RDONLY))==-1) 
	{		
		LOGD("open /data/calibrate/calibratefile fail\n");
		return 0;
	}

	read(hFile, CalibrationPoints, sizeof(PointXY) * 5) ;
	read(hFile, DumpCalibrationPoints, sizeof(PointXY)*5);
	//--------------------------------------------------------------------
	if(memcmp(CalibrationPoints, DumpCalibrationPoints, sizeof(PointXY) * 5) != 0)
	{
		LOGD("read data is error\n");
		point = 0;
	}
	else
	{
		for(i = 0; i < 5; i++)
		{
			pUncalXBuffer[i] = CalibrationPoints[i].CoordsX;
			pUncalYBuffer[i] = CalibrationPoints[i].CoordsY;
			LOGD("%d:	0x%x, 0x%x, 0x%x, 0x%x\n", i, pScreenXBuffer[i], pScreenYBuffer[i], pUncalXBuffer[i], pUncalYBuffer[i]);
		  
        }
		point = 5;
	}

	close(hFile);
	return point;
	
}


BOOL
TouchPanelSetCalibration(
    INT32   cCalibrationPoints,     //@PARM The number of calibration points
    INT32   *pScreenXBuffer,        //@PARM List of screen X coords displayed
    INT32   *pScreenYBuffer,        //@PARM List of screen Y coords displayed
    INT32   *pUncalXBuffer,         //@PARM List of X coords collected
    INT32   *pUncalYBuffer          //@PARM List of Y coords collected
    )
{
	BOOL bOK = FALSE;

    LARGENUM    a11;
    LARGENUM    a21, a22;
    LARGENUM    a31, a32, a33;
    LARGENUM    b11, b12, b13;
    LARGENUM    b21, b22, b23;
    LARGENUM    lnScreenX;
    LARGENUM    lnScreenY;
    LARGENUM    lnTouchX;
    LARGENUM    lnTouchY;
    LARGENUM    lnTemp;
    LARGENUM    delta;
    LARGENUM    a1, b1, c1;
    LARGENUM    a2, b2, c2;
    MATRIX33    Matrix;
    INT32       cShift;
    INT32       minShift;
    int         i;


     //
     // If the calibration data is being cleared, set the flag so
     // that the conversion operation is a noop.
     //
    LOGD("TouchPanelSetCalibration\n");

    if ( cCalibrationPoints == 0 )
    {
        v_Calibrated = FALSE;
        return TRUE;
    }

    //
    // Compute these large numbers
    //
    LargeNumSet(&a11, 0);
    LargeNumSet(&a21, 0);
    LargeNumSet(&a31, 0);
    LargeNumSet(&a22, 0);
    LargeNumSet(&a32, 0);
    LargeNumSet(&a33, cCalibrationPoints);
    LargeNumSet(&b11, 0);
    LargeNumSet(&b12, 0);
    LargeNumSet(&b13, 0);
    LargeNumSet(&b21, 0);
    LargeNumSet(&b22, 0);
    LargeNumSet(&b23, 0);
    for(i=0; i<cCalibrationPoints; i++){
        LargeNumSet(&lnTouchX, pUncalXBuffer[i]);
        LargeNumSet(&lnTouchY, pUncalYBuffer[i]);
        LargeNumSet(&lnScreenX, pScreenXBuffer[i]);
        LargeNumSet(&lnScreenY, pScreenYBuffer[i]);
        LargeNumMult(&lnTouchX, &lnTouchX, &lnTemp);
        LargeNumAdd(&a11, &lnTemp, &a11);
        LargeNumMult(&lnTouchX, &lnTouchY, &lnTemp);
        LargeNumAdd(&a21, &lnTemp, &a21);
        LargeNumAdd(&a31, &lnTouchX, &a31);
        LargeNumMult(&lnTouchY, &lnTouchY, &lnTemp);
        LargeNumAdd(&a22, &lnTemp, &a22);
        LargeNumAdd(&a32, &lnTouchY, &a32);
        LargeNumMult(&lnTouchX, &lnScreenX, &lnTemp);
        LargeNumAdd(&b11, &lnTemp, &b11);
        LargeNumMult(&lnTouchY, &lnScreenX, &lnTemp);
        LargeNumAdd(&b12, &lnTemp, &b12);
        LargeNumAdd(&b13, &lnScreenX, &b13);
        LargeNumMult(&lnTouchX, &lnScreenY, &lnTemp);
        LargeNumAdd(&b21, &lnTemp, &b21);
        LargeNumMult(&lnTouchY, &lnScreenY, &lnTemp);
        LargeNumAdd(&b22, &lnTemp, &b22);
        LargeNumAdd(&b23, &lnScreenY, &b23);
    }

    Matrix.pa11 = &a11;
    Matrix.pa21 = &a21;
    Matrix.pa31 = &a31;
    Matrix.pa12 = &a21;
    Matrix.pa22 = &a22;
    Matrix.pa32 = &a32;
    Matrix.pa13 = &a31;
    Matrix.pa23 = &a32;
    Matrix.pa33 = &a33;
    ComputeMatrix33(&delta, &Matrix);

    Matrix.pa11 = &b11;
    Matrix.pa21 = &b12;
    Matrix.pa31 = &b13;
    ComputeMatrix33(&a1, &Matrix);

    Matrix.pa11 = &a11;
    Matrix.pa21 = &a21;
    Matrix.pa31 = &a31;
    Matrix.pa12 = &b11;
    Matrix.pa22 = &b12;
    Matrix.pa32 = &b13;
    ComputeMatrix33(&b1, &Matrix);

    Matrix.pa12 = &a21;
    Matrix.pa22 = &a22;
    Matrix.pa32 = &a32;
    Matrix.pa13 = &b11;
    Matrix.pa23 = &b12;
    Matrix.pa33 = &b13;
    ComputeMatrix33(&c1, &Matrix);

    Matrix.pa13 = &a31;
    Matrix.pa23 = &a32;
    Matrix.pa33 = &a33;
    Matrix.pa11 = &b21;
    Matrix.pa21 = &b22;
    Matrix.pa31 = &b23;
    ComputeMatrix33(&a2, &Matrix);

    Matrix.pa11 = &a11;
    Matrix.pa21 = &a21;
    Matrix.pa31 = &a31;
    Matrix.pa12 = &b21;
    Matrix.pa22 = &b22;
    Matrix.pa32 = &b23;
    ComputeMatrix33(&b2, &Matrix);

    Matrix.pa12 = &a21;
    Matrix.pa22 = &a22;
    Matrix.pa32 = &a32;
    Matrix.pa13 = &b21;
    Matrix.pa23 = &b22;
    Matrix.pa33 = &b23;
    ComputeMatrix33(&c2, &Matrix);

#if 1
    {
        LARGENUM    halfDelta;
        //
        // Take care of possible truncation error in later mapping operations
        //
        if(IsLargeNumNegative(&delta)){
            LargeNumDivInt32(&delta, -2, &halfDelta);
        } else {
            LargeNumDivInt32(&delta, 2, &halfDelta);
        }
        LargeNumAdd(&c1, &halfDelta, &c1);
        LargeNumAdd(&c2, &halfDelta, &c2);
    }
#endif

    //
    // All the numbers are determined now.
    // Let's scale them back to 32 bit world
    //
    minShift = 0;
    cShift = LargeNumBits(&a1) - MAX_COEFF_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&b1) - MAX_COEFF_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&a2) - MAX_COEFF_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&b2) - MAX_COEFF_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&c1) - MAX_TERM_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&c2) - MAX_TERM_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&delta) - 31;
    if(cShift > minShift){
        minShift = cShift;
    }

    //
    // Now, shift count is determined, shift all the numbers
    //  right to obtain the 32-bit signed values
    //
    if(minShift){
        LargeNumRAShift(&a1, minShift);
        LargeNumRAShift(&a2, minShift);
        LargeNumRAShift(&b1, minShift);
        LargeNumRAShift(&b2, minShift);
        LargeNumRAShift(&c1, minShift);
        LargeNumRAShift(&c2, minShift);
        LargeNumRAShift(&delta, minShift);
    }
    v_CalcParam.a1      = a1.u.s32.u[0];
    v_CalcParam.b1      = b1.u.s32.u[0];
    v_CalcParam.c1      = c1.u.s32.u[0];
    v_CalcParam.a2      = a2.u.s32.u[0];
    v_CalcParam.b2      = b2.u.s32.u[0];
    v_CalcParam.c2      = c2.u.s32.u[0];
    v_CalcParam.delta   = delta.u.s32.u[0];

     // Don't allow delta to be zero, since it gets used as a divisor
    if( ! v_CalcParam.delta )
    {
        v_CalcParam.delta = 1;  // any non-zero value to prevents DivByZero traps later
        v_Calibrated = FALSE;
    }
    else
        v_Calibrated = TRUE;

//bOK : TRUE:	У׼�ɹ�	FALSE:У׼ʧ��	
//�����ʱ��û��У׼5���㣬�򷵻�ʧ�ܡ�
{
    LOGD("return bOk = %ld",bOK );

  	bOK = ErrorAnalysis(
                    cCalibrationPoints,
                    pScreenXBuffer,
                    pScreenYBuffer,
                    pUncalXBuffer,
                    pUncalYBuffer
                );
  }


LOGD("v_CalcParam:%d,%d,%d,%d,%d,%d,%d",v_CalcParam.a1,v_CalcParam.b1,v_CalcParam.c1,v_CalcParam.a2,v_CalcParam.b2,v_CalcParam.c2,v_CalcParam.delta  );
		for(i = 0; i < 5; i++)
		{
		}

		if(bOK)
		{	

			set_Calibrate_data();

			//��У׼ֵ���浽ָ����nand flash
			SaveCalibrationPoints(
				cCalibrationPoints,
				pScreenXBuffer,
				pScreenYBuffer,
				pUncalXBuffer,
				pUncalYBuffer
				);
		}

LOGD("return bOk = %ld",bOK );

	return bOK;
	
}

int set_calibrationmode(void)
{
    int fd = get_touch_fd();
    int val = 1;
	if (fd < 0) 
	{
		LOGD("open tsdevice error");
		return -1;
	}

	if (ioctl(fd, SET_CALIBRATE_STATE, &val) < 0) 
	{
		LOGD("ioctl TS_IOC_SET_TOUCHMODE");
		return -1;
	}    
	return 0;
}


int set_normalmode(void)
{

    int fd = get_touch_fd();
    int val = 0;
    if (fd < 0) 
    {
        LOGD("open tsdevice");
        return -1;
    }

    if (ioctl(fd, SET_CALIBRATE_STATE, &val) < 0) 
    {
        LOGD("ioctl TS_IOC_SET_TOUCHMODE");
        return -1;
    }    
    return 0;

}

static void sig(int sig)
{
//	close_framebuffer ();
	fflush (stderr);
	printf ("signal %d caught\n", sig);
	fflush (stdout);
	exit (1);
}

static void sigTerm(int sig)
{

    if (set_normalmode() < 0) {
        printf("set_normalmode fail!! \n");
        exit(1);
    }
	//cancelUseCalibrationFrameBuffer();
	
//	close_framebuffer ();
	fflush (stderr);
	printf ("signal %d caught\n", sig);
	fflush (stdout);
	exit (1);
}





int ts_read_raw(struct ts_sample *samp, int nr)
{
    int dwRead = 0;
    int fd = get_touch_fd();
    int touch_data = -1;
    if(fd < 0)
    {
            LOGD("ts_read_raw fd<0");

        return -1;
    }
    dwRead = read(fd,&touch_data,4);

    if(touch_data < 0)
    {
        touch_data = -touch_data;
        samp->x = (touch_data >> 16)&0xFFFF;
        samp->y = (touch_data )&0xFFFF;
        samp->pressure = 0;
    }
    else
    {
        samp->x = (touch_data >> 16)&0xFFFF;
        samp->y = (touch_data )&0xFFFF;
        samp->pressure = 1;
    }

        LOGD("x = %d,y = %d",samp->x,samp->y);

	return 0;
}

static int sort_by_x(const void* a, const void *b)
{
	return (((struct ts_sample *)a)->x - ((struct ts_sample *)b)->x);
}

static int sort_by_y(const void* a, const void *b)
{
	return (((struct ts_sample *)a)->y - ((struct ts_sample *)b)->y);
}


void getxy(int *x, int *y)
{
    #define MAX_SAMPLES 128
	struct ts_sample samp[MAX_SAMPLES];
	int index, middle;

	index = 0;
	do {
		if (index < MAX_SAMPLES-1)
			index++;
		if (ts_read_raw(&samp[index], 1) < 0) {
//			close_framebuffer ();
			return ;
		}
	} while (samp[index].pressure > 0);

	LOGD("Took %d samples...\n",index);

	middle = index/2;
	if (x) {
		qsort(samp, index, sizeof(struct ts_sample), sort_by_x);
		if (index & 1)
			*x = samp[middle].x;
		else
			*x = (samp[middle-1].x + samp[middle].x) / 2;
	}
	if (y) {
		qsort(samp, index, sizeof(struct ts_sample), sort_by_y);
		if (index & 1)
			*y = samp[middle].y;
		else
			*y = (samp[middle-1].y + samp[middle].y) / 2;
	}
}


static void get_sample(CALIBRATEDATA*cal,int index)
{
	getxy(&cal->UncalXBuffer[index], &cal->UncalYBuffer[index]);	
	LOGD("%d : X = %4d Y = %4d\n", index, cal->UncalXBuffer [index], cal->UncalYBuffer [index]);
}
static int calibrationCheckPoints(CALIBRATEDATA*cal)
{

return TouchPanelSetCalibration(
	5,     //@PARM The number of calibration points
	cal->ScreenXBuffer,        //@PARM List of screen X coords displayed
	cal->ScreenYBuffer,        //@PARM List of screen Y coords displayed
	cal->UncalXBuffer,         //@PARM List of X coords collected
	cal->UncalYBuffer//@PARM List of Y coords collected
    );
}



static void
setCalibratePoint(JNIEnv *env, jobject thiz,jint index){
	get_sample (&cal, (int)index );
}


static int
setCalibrateData(JNIEnv *env, jobject thiz){

	int val = 0;
        touch_fd = open(TS_DEVICE, O_RDWR);
    LOGD("setCalibrateData\n");
    getTouchPoint(&cal);

	if(!ReadCalibrationPoints(cal.ScreenXBuffer, cal.ScreenYBuffer,cal.UncalXBuffer, cal.UncalYBuffer))
		return 0;
	return TouchPanelSetCalibration(
		5,     //@PARM The number of calibration points
		cal.ScreenXBuffer,        //@PARM List of screen X coords displayed
		cal.ScreenYBuffer,        //@PARM List of screen Y coords displayed
		cal.UncalXBuffer,         //@PARM List of X coords collected
		cal.UncalYBuffer//@PARM List of Y coords collected
	    );
    close(touch_fd);

}

static int
checkCalibrate(JNIEnv *env, jobject thiz){

for (int i =0;i<5;i++)
	LOGD("index =%d,x=%d,y=%d,screenx=%d.screeny=%d",i,cal.UncalXBuffer[i],cal.UncalYBuffer[i],cal.ScreenXBuffer[i],cal.ScreenYBuffer[i]);
	return calibrationCheckPoints(&cal );
//		saveCalibrationValue(cal_buffer,strlen (cal_buffer) + 1);
}


static void
closeCalibrate(JNIEnv *env, jobject thiz){
    if (set_normalmode() < 0) {
        LOGD("set_normalmode fail!! \n");
        return;
    }
    close(touch_fd);
}

static int
openCalibrate(JNIEnv *env, jobject thiz){
    

	int cal_fd;
	char cal_buffer[256];
	char *tsdevice = NULL;
	char *calfile = NULL;
	unsigned int i;
	int ret = 0;
	int tempx, tempy;
	calibrate_state = 1;
	touch_fd = open(TS_DEVICE, O_RDWR);

	if (touch_fd < 0) {
		LOGD("open /dev/ns2009 fail");
		return 0;
	}

	getTouchPoint(&cal);

    if (set_calibrationmode() < 0) {
        LOGD("set_calibrationmode fail!! \n");
        return 0;    
    }

	return 1;

}

static const char *classPathName = "com.choiceway.calibrate.CalibrateService";

static JNINativeMethod methods[] = {
  {"openCalibrate", "()I", (void*)openCalibrate },
  {"closeCalibrate", "()V", (void*)closeCalibrate },
  {"setCalibrateData", "()I", (void*)setCalibrateData },
  {"setCalibratePoint", "(I)V", (void*)setCalibratePoint },
  {"checkCalibrate", "()I", (void*)checkCalibrate },
//	{"closetouch", "()V", (void*)closetouch },
};

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 *
 * returns JNI_TRUE on success.
 */
static int registerNatives(JNIEnv* env)
{
  if (!registerNativeMethods(env, classPathName,
                 methods, sizeof(methods) / sizeof(methods[0]))) {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}


// ----------------------------------------------------------------------------

/*
 * This is called by the VM when the shared library is first loaded.
 */
 
typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;

    signal(SIGSEGV, sigTerm);
    signal(SIGINT, sigTerm);
    signal(SIGTERM, sigTerm);

    LOGD("JNI_OnLoad");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    if (registerNatives(env) != JNI_TRUE) {
        LOGE("ERROR: registerNatives failed");
        goto bail;
    }
    
    result = JNI_VERSION_1_4;
    
bail:
    return result;
}

