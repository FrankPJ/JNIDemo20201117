package com.choiceway.calibrate;

//import com.choiceway.eventcenter.EventUtils;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;

public class CalibrateService extends Service {

    private static final String TAG = "Calibrate";
    private CalibrateView mCalibrateView = null;
    //	private 					WndFrame 								mRightView 				= null;
    private final WindowManager.LayoutParams wmCalibrateParams = new WindowManager.LayoutParams();
    private WindowManager wm = null;
    private boolean mIsAddLeftView = false;
    private boolean mIsAddRightView = false;
    private boolean mIsShow = false;
    private boolean mIsRightShow = false;
    private Object mObject = new Object();
    private Handler handler = null;
    private int point;

    public native int openCalibrate();

    public native void closeCalibrate();

    public native int setCalibrateData();

    public native void setCalibratePoint(int index);

    public native int checkCalibrate();


    public void setpoint(int index) {
        Log.d(TAG, "point " + index);
        point = index;
        handler.post(runnableUi);
        setCalibratePoint(index);

    }

    protected void startReceiveThread() {
        new Thread(() -> {
            try {

                if (setCalibrateData() > 0) {
                    Log.d(TAG, "setCalibrateData ok");
                    stopSelf();
                } else {
                    Log.d(TAG, "setCalibrateData error");
                    openCalibrate();

                    while (true) {
                        for (int i = 0; i < 5; i++) {
                            setpoint(i);
                        }
                        if (checkCalibrate() > 0) {
                            point = 5;
                            handler.post(runnableUi);
                            Log.d(TAG, "calibrate ok");
                            closeCalibrate();
                            stopSelf();
                            break;
                        }
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }).start();
    }

    @Override
    public void onCreate() {
        super.onCreate();

        Log.d(TAG, "Calibrate service create");
        handler = new Handler();
        createCalibrateView();


        addCalibrateView();

//	openCalibrate();
        startReceiveThread();
        Log.i(TAG, "onCreate");
    }

    @Override
    public void onDestroy() {

        removeCalibrateView();
        super.onDestroy();
        Log.i(TAG, "onDestroy");
    }

    @Override
    public IBinder onBind(Intent intent) {

        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "onStartCommand");
        return START_STICKY;
    }


    public void createCalibrateView() {
        if (mCalibrateView == null) {
            mCalibrateView = new CalibrateView(this);
        }

        if (mCalibrateView != null) {
            if (wm == null)
                wm = (WindowManager) getApplicationContext().getSystemService(Context.WINDOW_SERVICE);

            wmCalibrateParams.type = LayoutParams.TYPE_SYSTEM_OVERLAY;
            //		wmCalibrateParams.format = PixelFormat.RGBA_8888;
            wmCalibrateParams.flags = LayoutParams.FLAG_NOT_TOUCH_MODAL
                    | LayoutParams.FLAG_NOT_FOCUSABLE
                    | LayoutParams.FLAG_NOT_TOUCHABLE
                    | LayoutParams.FLAG_LAYOUT_IN_SCREEN;
            //		wmCalibrateParams.gravity = Gravity.LEFT|Gravity.TOP;
            //		wmCalibrateParams.x = 0;
            //		wmCalibrateParams.y = 0;
            wmCalibrateParams.width = LayoutParams.MATCH_PARENT;
            wmCalibrateParams.height = LayoutParams.MATCH_PARENT;
        }
    }

    public void addCalibrateView() {
        synchronized (mObject) {
            if (mIsAddRightView)
                return;

            if (wm != null && mCalibrateView != null) {
                //			mCalibrateView.getView().setX(337.0f);
                wm.addView(mCalibrateView, wmCalibrateParams);
            }
            mIsAddRightView = true;
        }
    }

    public void updateViewLayout() {
        synchronized (mObject) {
            if (!mIsAddLeftView)
                return;
            if (wm != null && mCalibrateView != null)
                wm.updateViewLayout(mCalibrateView, wmCalibrateParams);
        }
    }

    Runnable runnableUi = new Runnable() {
        @Override
        public void run() {
            mCalibrateView.setCalibViewVisible(point);
        }

    };

    public void removeCalibrateView() {
        synchronized (mObject) {
            if (!mIsAddRightView) {
                return;
            }
            if (mCalibrateView != null) {
                if (wm == null)
                    wm = (WindowManager) getApplicationContext().getSystemService(Context.WINDOW_SERVICE);
                if (wm != null)
                    wm.removeView(mCalibrateView);
            }
            mIsAddRightView = false;
        }
    }

    static {
        // The runtime will add "lib" on the front and ".o" on the end of
        // the name supplied to loadLibrary.
        System.loadLibrary("ts_calibrate");
    }


}
