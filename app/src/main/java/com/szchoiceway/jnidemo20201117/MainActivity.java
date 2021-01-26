package com.szchoiceway.jnidemo20201117;

import android.os.Bundle;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;


public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(objectArrayFromJni()[3]+"");
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String helloWorld();
    public native int intFromJni();
    public native float floatFromJni();
    public native double doubleFromJni();
    public native byte byteFromJni();
    public native long longFromJni();
    public native char charFromJni();
    public native boolean booleanFromJni();
    public native int[] intArrayFromJni();
    public native String[] objectArrayFromJni();






}