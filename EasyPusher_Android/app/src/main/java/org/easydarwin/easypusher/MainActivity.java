/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easypusher;

import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.media.MediaCodec;
import android.media.MediaFormat;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import org.easydarwin.audio.AudioStream;
import org.easydarwin.util.Util;
import org.easydarwin.config.Config;
import org.easydarwin.hw.EncoderDebugger;
import org.easydarwin.hw.NV21Convertor;
import org.easydarwin.push.EasyPusher;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.List;

@SuppressWarnings("deprecation")
public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback, View.OnClickListener {

    static final String TAG = "EasyPusher";

    int width = 640, height = 480;
    int framerate, bitrate;
    int mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
    MediaCodec mMediaCodec;
    SurfaceView surfaceView;
    SurfaceHolder surfaceHolder;
    Camera mCamera;
    NV21Convertor mConvertor;
    Button btnSwitch;
    Button btnSetting;
    boolean pushStream = false;//是否要推送数据
    EasyPusher mEasyPusher;
    TextView txtStreamAddress;
    AudioStream audioStream;
    Button btnSwitchCemera;
    private boolean isCameraBack = true;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        btnSwitch = (Button) findViewById(R.id.btn_switch);
        btnSwitch.setOnClickListener(this);
        btnSetting = (Button) findViewById(R.id.btn_setting);
        btnSetting.setOnClickListener(this);
        btnSwitchCemera = (Button) findViewById(R.id.btn_switchCamera);
        btnSwitchCemera.setOnClickListener(this);
        txtStreamAddress = (TextView) findViewById(R.id.txt_stream_address);
        initMediaCodec();
        surfaceView = (SurfaceView) findViewById(R.id.sv_surfaceview);
        surfaceView.getHolder().addCallback(this);
        surfaceView.getHolder().setFixedSize(getResources().getDisplayMetrics().widthPixels,
                getResources().getDisplayMetrics().heightPixels);
        surfaceView.setOnClickListener(this);
        mEasyPusher = new EasyPusher();
        mEasyPusher.setOnInitPusherCallback(new EasyPusher.OnInitPusherCallback() {

            @Override
            public void onCallback(int code) {
                switch (code) {
                    case CODE.EASY_ACTIVATE_INVALID_KEY:
                        Log.i(TAG, "无效Key");
                        break;
                    case CODE.EASY_ACTIVATE_SUCCESS:
                        Log.e(TAG, "激活成功");
                        break;
                    case CODE.EASY_PUSH_STATE_CONNECTING:
                        Log.e(TAG, "连接中");
                        break;
                    case CODE.EASY_PUSH_STATE_CONNECTED:
                        Log.e(TAG, "连接成功");
                        break;
                    case CODE.EASY_PUSH_STATE_CONNECT_FAILED:
                        Log.e(TAG, "连接失败");
                        break;
                    case CODE.EASY_PUSH_STATE_CONNECT_ABORT:
                        Log.e(TAG, "连接异常中断");
                        break;
                    case CODE.EASY_PUSH_STATE_PUSHING:
                        Log.e(TAG, "推流中");
                        break;
                    case CODE.EASY_PUSH_STATE_DISCONNECTED:
                        Log.e(TAG, "断开连接");
                        break;
                }

            }
        });
        audioStream = new AudioStream(mEasyPusher);
    }


    @Override
    protected void onResume() {
        super.onResume();
    }


    private void initPusher() {
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        String ip = sharedPreferences.getString(Config.SERVER_IP, Config.DEFAULT_SERVER_IP);
        String port = sharedPreferences.getString(Config.SERVER_PORT, Config.DEFAULT_SERVER_PORT);
        String id = sharedPreferences.getString(Config.STREAM_ID, "");
        if (TextUtils.isEmpty(id)) {
            id = String.valueOf(System.nanoTime());
            SharedPreferences sharedPreferences1 = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
            sharedPreferences.edit().putString(Config.STREAM_ID, id).commit();
        }
        txtStreamAddress.setText(String.format("rtsp://%s:%s/%s.sdp", ip, port, id));
        mEasyPusher.initPush(ip, port, String.format("%s.sdp", id), getApplicationContext());
    }

    private void initMediaCodec() {
        int dgree = getDgree();
        framerate = 25;
        bitrate = 2 * width * height * framerate / 20;
        EncoderDebugger debugger = EncoderDebugger.debug(getApplicationContext(), width, height);
        mConvertor = debugger.getNV21Convertor();
        try {
            mMediaCodec = MediaCodec.createByCodecName(debugger.getEncoderName());
            MediaFormat mediaFormat;
            if (dgree == 0) {
                mediaFormat = MediaFormat.createVideoFormat("video/avc", height, width);
            } else {
                mediaFormat = MediaFormat.createVideoFormat("video/avc", width, height);
            }
            mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, bitrate);
            mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, framerate);
            mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT,
                    debugger.getEncoderColorFormat());
            mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1);
            mMediaCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            mMediaCodec.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static int[] determineMaximumSupportedFramerate(Camera.Parameters parameters) {
        int[] maxFps = new int[]{0, 0};
        List<int[]> supportedFpsRanges = parameters.getSupportedPreviewFpsRange();
        for (Iterator<int[]> it = supportedFpsRanges.iterator(); it.hasNext(); ) {
            int[] interval = it.next();
            if (interval[1] > maxFps[1] || (interval[0] > maxFps[0] && interval[1] == maxFps[1])) {
                maxFps = interval;
            }
        }
        return maxFps;
    }

    private boolean ctreateCamera(SurfaceHolder surfaceHolder) {
        try {
            mCamera = Camera.open(mCameraId);
            Camera.Parameters parameters = mCamera.getParameters();
            int[] max = determineMaximumSupportedFramerate(parameters);
            Camera.CameraInfo camInfo = new Camera.CameraInfo();
            Camera.getCameraInfo(mCameraId, camInfo);
            int cameraRotationOffset = camInfo.orientation;
            if (mCameraId == Camera.CameraInfo.CAMERA_FACING_FRONT)
                cameraRotationOffset += 180;
            int rotate = (360 + cameraRotationOffset - getDgree()) % 360;
            parameters.setRotation(rotate);
            parameters.setPreviewFormat(ImageFormat.NV21);
            List<Camera.Size> sizes = parameters.getSupportedPreviewSizes();
            parameters.setPreviewSize(width, height);
            parameters.setPreviewFpsRange(max[0], max[1]);
            mCamera.setParameters(parameters);
            int displayRotation;
            displayRotation = (cameraRotationOffset - getDgree() + 360) % 360;
            mCamera.setDisplayOrientation(displayRotation);
            mCamera.setPreviewDisplay(surfaceHolder);
            return true;
        } catch (Exception e) {
            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            e.printStackTrace(pw);
            String stack = sw.toString();
            Toast.makeText(this, stack, Toast.LENGTH_LONG).show();
            destroyCamera();
            e.printStackTrace();
            return false;
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        surfaceHolder = holder;
        ctreateCamera(surfaceHolder);
        startPreview();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        stopPreview();
        stopStream();
        destroyCamera();
    }

    Camera.PreviewCallback previewCallback = new Camera.PreviewCallback() {
        byte[] mPpsSps = new byte[0];
        ByteBuffer[] inputBuffers;
        ByteBuffer[] outputBuffers;
        byte[] dst;

        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
            if (data == null || !pushStream) {
                mCamera.addCallbackBuffer(data);
                return;
            }
            inputBuffers = mMediaCodec.getInputBuffers();
            outputBuffers = mMediaCodec.getOutputBuffers();
            dst = new byte[data.length];
            Camera.Size previewSize = mCamera.getParameters().getPreviewSize();
            if (getDgree() == 0) {
                Camera.CameraInfo camInfo = new Camera.CameraInfo();
                Camera.getCameraInfo(mCameraId, camInfo);
                int cameraRotationOffset = camInfo.orientation;
                if (cameraRotationOffset == 0)
                    dst = data;
                if (cameraRotationOffset == 90)
                    dst = Util.rotateNV21Degree90(data, previewSize.width, previewSize.height);
                if (cameraRotationOffset == 180)
                    dst = Util.rotateNV21Degree90(data, previewSize.width, previewSize.height);
                if (cameraRotationOffset == 270)
                    dst = Util.rotateNV21Negative90(data, previewSize.width, previewSize.height);
            } else {
                dst = data;
            }
            try {
                int bufferIndex = mMediaCodec.dequeueInputBuffer(5000000);
                if (bufferIndex >= 0) {
                    inputBuffers[bufferIndex].clear();
                    mConvertor.convert(dst, inputBuffers[bufferIndex]);
                    mMediaCodec.queueInputBuffer(bufferIndex, 0, inputBuffers[bufferIndex].position(), System.nanoTime() / 1000, 0);
                    MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
                    int outputBufferIndex = mMediaCodec.dequeueOutputBuffer(bufferInfo, 0);
                    while (outputBufferIndex >= 0) {
                        ByteBuffer outputBuffer = outputBuffers[outputBufferIndex];
                        byte[] outData = new byte[bufferInfo.size];
                        outputBuffer.get(outData);
                        //记录pps和sps
                        if (outData[0] == 0 && outData[1] == 0 && outData[2] == 0 && outData[3] == 1 && outData[4] == 103) {
                            mPpsSps = outData;
                        } else if (outData[0] == 0 && outData[1] == 0 && outData[2] == 0 && outData[3] == 1 && outData[4] == 101) {
                            //在关键帧前面加上pps和sps数据
                            byte[] iframeData = new byte[mPpsSps.length + outData.length];
                            System.arraycopy(mPpsSps, 0, iframeData, 0, mPpsSps.length);
                            System.arraycopy(outData, 0, iframeData, mPpsSps.length, outData.length);
                            outData = iframeData;
                        }
                        mEasyPusher.push(outData, System.currentTimeMillis(), 1);
                        mMediaCodec.releaseOutputBuffer(outputBufferIndex, false);
                        outputBufferIndex = mMediaCodec.dequeueOutputBuffer(bufferInfo, 0);
                    }
                } else {
                    Log.e(TAG, "No buffer available !");
                }
            } catch (Exception e) {
                StringWriter sw = new StringWriter();
                PrintWriter pw = new PrintWriter(sw);
                e.printStackTrace(pw);
                String stack = sw.toString();
                Log.e("save_log", stack);
                e.printStackTrace();
            } finally {
                mCamera.addCallbackBuffer(dst);
            }
        }

    };

    /**
     * 开启预览
     */
    public synchronized void startPreview() {
        if (mCamera != null) {
            mCamera.startPreview();
            try {
                mCamera.autoFocus(null);
            } catch (Exception e) {
                //忽略异常
                Log.i(TAG, "auto foucus fail");
            }

            int previewFormat = mCamera.getParameters().getPreviewFormat();
            Camera.Size previewSize = mCamera.getParameters().getPreviewSize();
            int size = previewSize.width * previewSize.height
                    * ImageFormat.getBitsPerPixel(previewFormat)
                    / 8;
            mCamera.addCallbackBuffer(new byte[size]);
            mCamera.setPreviewCallbackWithBuffer(previewCallback);
        }
    }


    /**
     * 停止预览
     */
    public synchronized void stopPreview() {
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.setPreviewCallbackWithBuffer(null);
        }
    }

    public void startStream() {
        initPusher();
        pushStream = true;
        btnSwitch.setText("停止");
        audioStream.startRecord();
    }

    public void stopStream() {
        mEasyPusher.stop();
        pushStream = false;
        btnSwitch.setText("开始");
        audioStream.stop();
    }


    /**
     * 销毁Camera
     */
    protected synchronized void destroyCamera() {
        if (mCamera != null) {
            mCamera.stopPreview();
            try {
                mCamera.release();
            } catch (Exception e) {

            }
            mCamera = null;
        }
    }

    private int getDgree() {
        int rotation = getWindowManager().getDefaultDisplay().getRotation();
        int degrees = 0;
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 0;
                break; // Natural orientation
            case Surface.ROTATION_90:
                degrees = 90;
                break; // Landscape left
            case Surface.ROTATION_180:
                degrees = 180;
                break;// Upside down
            case Surface.ROTATION_270:
                degrees = 270;
                break;// Landscape right
        }
        return degrees;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_switch:
                if (!pushStream) {
                    startStream();
                } else {
                    stopStream();
                }
                break;
            case R.id.btn_setting:
                startActivity(new Intent(this, SettingActivity.class));
                break;
            case R.id.sv_surfaceview:
                try {
                    mCamera.autoFocus(null);
                } catch (Exception e) {
                }
                break;
            case R.id.btn_switchCamera: {
                // TODO Auto-generated method stub
                int cameraCount = 0;
                if (isCameraBack) {
                    isCameraBack = false;
                } else {
                    isCameraBack = true;
                }
                Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
                cameraCount = Camera.getNumberOfCameras();//得到摄像头的个数
                for (int i = 0; i < cameraCount; i++) {
                    Camera.getCameraInfo(i, cameraInfo);//得到每一个摄像头的信息
                    if (mCameraId == Camera.CameraInfo.CAMERA_FACING_FRONT) {
                        //现在是后置，变更为前置
                        if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {//代表摄像头的方位，CAMERA_FACING_FRONT前置      CAMERA_FACING_BACK后置
                            mCamera.stopPreview();//停掉原来摄像头的预览
                            mCamera.release();//释放资源
                            mCamera = null;//取消原来摄像头
                            mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
                            ctreateCamera(surfaceHolder);
                            startPreview();
                            break;
                        }
                    } else {
                        //现在是前置， 变更为后置
                        if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_BACK) {//代表摄像头的方位，CAMERA_FACING_FRONT前置      CAMERA_FACING_BACK后置
                            mCamera.stopPreview();//停掉原来摄像头的预览
                            mCamera.release();//释放资源
                            mCamera = null;//取消原来摄像头
                            mCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
                            ctreateCamera(surfaceHolder);
                            startPreview();
                            break;
                        }
                    }
                }
            }
            break;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        destroyCamera();
        mMediaCodec.stop();
        mMediaCodec.release();
        mMediaCodec = null;
        mEasyPusher.stop();
    }
}
