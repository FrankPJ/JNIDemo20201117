package com.choiceway.calibrate;

import android.content.Context;
import android.util.Log;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.TextView;


public class CalibrateView extends RelativeLayout {
    private Context mContext = null;
    private View mRootView = null;

    private View mLeftTop = null;
    private View mLeftBottom = null;
    private View mRightTop = null;
    private View mRightBottom = null;
    private View mMiddle = null;
    private View mOK = null;

    public CalibrateView(Context context) {
        super(context);
        mContext = context;
        init(R.layout.calibrate);
    }


    private void init(int resId) {

        //	if (resId == R.layout.left_wnd) {
//			bLeftWndRes = true;
//		}

        mRootView = View.inflate(mContext, resId, null);

        this.addView(mRootView);

        mLeftTop = mRootView.findViewById(R.id.LeftTop);
        mLeftBottom = mRootView.findViewById(R.id.LeftBottom);
        mRightTop = mRootView.findViewById(R.id.RightTop);
        mRightBottom = mRootView.findViewById(R.id.RightBottom);
        mMiddle = mRootView.findViewById(R.id.Middle);
        mOK = mRootView.findViewById(R.id.OK);
//		mOK						= (TextView)mRootView.findViewById(R.id.OK);
        mMiddle.setVisibility(View.INVISIBLE);
        mLeftTop.setVisibility(View.INVISIBLE);
        mLeftBottom.setVisibility(View.INVISIBLE);
        mRightBottom.setVisibility(View.INVISIBLE);
        mRightTop.setVisibility(View.INVISIBLE);

    }

    public View getView() {
        return mRootView;
    }

    public void setCalibViewVisible(int index) {
        Log.d("Calibrate", "setViewVisible");
        switch (index) {
            case 0:
                mMiddle.setVisibility(View.VISIBLE);
                mLeftTop.setVisibility(View.INVISIBLE);
                mLeftBottom.setVisibility(View.INVISIBLE);
                mRightBottom.setVisibility(View.INVISIBLE);
                mRightTop.setVisibility(View.INVISIBLE);

                break;
            case 1:
                mMiddle.setVisibility(View.INVISIBLE);
                mLeftTop.setVisibility(View.VISIBLE);
                mLeftBottom.setVisibility(View.INVISIBLE);
                mRightBottom.setVisibility(View.INVISIBLE);
                mRightTop.setVisibility(View.INVISIBLE);

                break;
            case 2:
                mMiddle.setVisibility(View.INVISIBLE);
                mLeftTop.setVisibility(View.INVISIBLE);
                mLeftBottom.setVisibility(View.VISIBLE);
                mRightBottom.setVisibility(View.INVISIBLE);
                mRightTop.setVisibility(View.INVISIBLE);

                break;
            case 3:
                mMiddle.setVisibility(View.INVISIBLE);
                mLeftTop.setVisibility(View.INVISIBLE);
                mLeftBottom.setVisibility(View.INVISIBLE);
                mRightBottom.setVisibility(View.VISIBLE);
                mRightTop.setVisibility(View.INVISIBLE);

                break;
            case 4:
                mMiddle.setVisibility(View.INVISIBLE);
                mLeftTop.setVisibility(View.INVISIBLE);
                mLeftBottom.setVisibility(View.INVISIBLE);
                mRightBottom.setVisibility(View.INVISIBLE);
                mRightTop.setVisibility(View.VISIBLE);

                break;
            case 5:
                mMiddle.setVisibility(View.INVISIBLE);
                mLeftTop.setVisibility(View.INVISIBLE);
                mLeftBottom.setVisibility(View.INVISIBLE);
                mRightBottom.setVisibility(View.INVISIBLE);
                mRightTop.setVisibility(View.INVISIBLE);
                mOK.setVisibility(View.VISIBLE);
                break;
            default:
                break;
        }
    }

    private void rotationView(View view, float rotation) {
        if (view != null) {
            view.setRotation(rotation);
        }
    }

    private void setViewText(TextView view, String str) {
        if (view != null) {
            view.setText(str);
        }
    }


}
