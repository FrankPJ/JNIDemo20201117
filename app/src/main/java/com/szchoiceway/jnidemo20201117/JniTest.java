package com.szchoiceway.jnidemo20201117;

/**
 * create by FrankP on 2021/1/26
 */
public class JniTest {


    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native String helloWorld();

    public static native int intFromJni();

    public static native float floatFromJni();

    public static native double doubleFromJni();

    public static native byte byteFromJni();

    public static native long longFromJni();

    public static native char charFromJni();

    public static native boolean booleanFromJni();

    public static native int[] intArrayFromJni();

    public static native String[] objectArrayFromJni();

    public static native String stringFromJNI();

    public static native void callPerson();
}
