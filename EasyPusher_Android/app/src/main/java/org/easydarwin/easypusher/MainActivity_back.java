/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easypusher;

    import android.content.BroadcastReceiver;
    import android.content.Context;
    import android.content.Intent;
    import android.content.IntentFilter;
    import android.content.SharedPreferences;
    import android.graphics.ImageFormat;
    import android.hardware.Camera;
    import android.media.MediaCodec;
    import android.media.MediaFormat;
    import android.net.ConnectivityManager;
    import android.net.NetworkInfo;
    import android.os.Handler;
    import android.os.Message;
    import android.preference.PreferenceManager;
    import android.support.v7.app.AppCompatActivity;
    import android.os.Bundle;
    import android.telephony.TelephonyManager;
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
    import org.easydarwin.rmspai.Api;
    import org.easydarwin.rmspai.ApiCallback;
    import org.easydarwin.rmspai.ApiImpl;
    import org.easydarwin.rmspai.ApiResponse;
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
    public class MainActivity_back extends AppCompatActivity implements SurfaceHolder.Callback, View.OnClickListener {

        static final String TAG="EasyPusher";

        int width = 320, height = 240;
        int framerate, bitrate;
        int mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
        MediaCodec mMediaCodec;
        SurfaceView surfaceView;
        SurfaceHolder surfaceHolder;
        Camera mCamera;
        NV21Convertor mConvertor;
        Button btnSwitch;
        Button btnSetting;
        Button btnRecord;
        Button btnAudio;
        Button btnSwitchCemera;
        //    boolean started = false;
        boolean pushStream = false;//是否要推送数据
        EasyPusher mEasyPusher;
        TextView txtStreamAddress;
        String serverIP = "", serverPort = "", streamID = "";
        String rtspUrl = "";
        AudioStream audioStream;

        private Api api;
        private NetWorkReceiver netWorkReceiver;
        private String netWorkType = "";
        private boolean isCameraBack = true;

        @Override
        protected void onCreate(Bundle savedInstanceState) {
            getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_main);
            api = new ApiImpl();
            netWorkReceiver = new NetWorkReceiver();
            btnSwitch = (Button) findViewById(R.id.btn_switch);
            btnSwitch.setOnClickListener(this);
            btnSetting = (Button) findViewById(R.id.btn_setting);
            btnSetting.setOnClickListener(this);
            btnRecord = (Button) findViewById(R.id.btn_record);
            btnRecord.setOnClickListener(this);
            btnAudio = (Button) findViewById(R.id.btn_audio);
            btnAudio.setOnClickListener(this);
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
            audioStream=new AudioStream(mEasyPusher);
        }

        @Override
        protected void onResume() {
            super.onResume();
            SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
            String ip = sharedPreferences.getString(Config.SERVER_IP, Config.DEFAULT_SERVER_IP);
            String port = sharedPreferences.getString(Config.SERVER_PORT, Config.DEFAULT_SERVER_PORT);
            String id = sharedPreferences.getString(Config.STREAM_ID, Config.DEFAULT_STREAM_ID);
            if (TextUtils.isEmpty(id)) {
                id = String.valueOf(System.nanoTime());
                SharedPreferences sharedPreferences1 = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
                sharedPreferences.edit().putString(Config.STREAM_ID, id).commit();
            }
            if (serverIP.equals(ip) && serverPort.equals(port) && id.equals(streamID)) {
                return;
            }
            serverIP = ip;
            serverPort = port;
            streamID = id;
            rtspUrl = String.format("rtsp://%s:%s/%s.sdp", ip, port, id);
            netWorkType = getNetworkType();
            txtStreamAddress.setText(rtspUrl + "网络:" + netWorkType);
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
                Log.e("cameraRotationOffset",cameraRotationOffset+":"+getDgree());
                if(mCameraId== Camera.CameraInfo.CAMERA_FACING_FRONT)
                    cameraRotationOffset+=180;
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
            IntentFilter intentFilter = new IntentFilter();
            //添加过滤的Action值；
            intentFilter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
            this.registerReceiver(netWorkReceiver, intentFilter);
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            stopPreview();
            stopStream();
            destroyCamera();
            this.unregisterReceiver(netWorkReceiver);
        }

        Camera.PreviewCallback previewCallback = new Camera.PreviewCallback() {
            byte[] mPpsSps = new byte[0];

            @Override
            public void onPreviewFrame(byte[] data, Camera camera) {
                if (data == null || !pushStream) {
                    mCamera.addCallbackBuffer(data);
                    return;
                }
                ByteBuffer[] inputBuffers = mMediaCodec.getInputBuffers();
                ByteBuffer[] outputBuffers = mMediaCodec.getOutputBuffers();
                byte[] dst = new byte[data.length];
                Camera.Size previewSize = mCamera.getParameters().getPreviewSize();
                if (getDgree() == 0) {
                    Camera.CameraInfo camInfo = new Camera.CameraInfo();
                    Camera.getCameraInfo(mCameraId, camInfo);
                    int cameraRotationOffset = camInfo.orientation;
                    Log.e("cameraRotationOffset2",""+cameraRotationOffset);
                    if(cameraRotationOffset==0)
                        dst = data;
                    if(cameraRotationOffset==90)
                        dst = Util.rotateNV21Degree90(data, previewSize.width, previewSize.height);
                    if(cameraRotationOffset==180)
                        dst = Util.rotateNV21Degree90(data, previewSize.width, previewSize.height);
                    if(cameraRotationOffset==270)
                        dst = Util.rotateNV21Negative90(data, previewSize.width, previewSize.height);
                } else {
                    dst = data;
                }
                try {
                    int bufferIndex = mMediaCodec.dequeueInputBuffer(5000000);
                    if (bufferIndex >= 0) {
                        inputBuffers[bufferIndex].clear();
                        mConvertor.convert(dst, inputBuffers[bufferIndex]);
                        mMediaCodec.queueInputBuffer(bufferIndex, 0,inputBuffers[bufferIndex].position(),System.nanoTime() / 1000, 0);
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
                            mEasyPusher.push(outData,System.currentTimeMillis(),1);
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
            pushStream = true;
            btnSwitch.setText("停止");
            audioStream.startRecord();
            btnAudio.setVisibility(View.VISIBLE);
            btnRecord.setVisibility(View.VISIBLE);
            btnRecord.setText("录像");
            btnAudio.setText("静音");
        }

        public void stopStream() {
            pushStream = false;
            btnSwitch.setText("开始");
            audioStream.stop();
            btnAudio.setVisibility(View.GONE);
            btnRecord.setVisibility(View.GONE);
            api.stopCmd(streamID, new ApiCallback<ApiResponse<Void>>() {
                @Override
                public ApiResponse<Void> onSuccess(ApiResponse<Void> data) {
                    return null;
                }

                @Override
                public void onFailure(String errorEvent, String message) {

                }
            });
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
                    try{
                        mCamera.autoFocus(null);
                    }catch (Exception e){
                    }
                    break;
                case R.id.btn_audio:
                    if(btnAudio.getVisibility()==View.VISIBLE){
                        btnAudio.setEnabled(false);
                        if(btnAudio.getText().toString().trim().equals("静音")){
                            audioStream.stop();
                            btnAudio.setText("录音");
                            btnAudio.setEnabled(true);
                        } else {
                            audioStream.startRecord();
                            btnAudio.setText("静音");
                            btnAudio.setEnabled(true);
                        }
                    }
                    break;
                case R.id.btn_record:
                    if(btnRecord.getVisibility()==View.VISIBLE){
                        btnRecord.setEnabled(false);
                        if(btnRecord.getText().toString().trim().equals("录像")){
                            api.startCmd(streamID, rtspUrl, new ApiCallback<ApiResponse<Void>>() {
                                @Override
                                public ApiResponse<Void> onSuccess(ApiResponse<Void> data) {
                                    btnRecord.setText("停止");
                                    btnRecord.setEnabled(true);
                                    return null;
                                }

                                @Override
                                public void onFailure(String errorEvent, String message) {
                                    btnRecord.setEnabled(true);
                                }
                            });
                        } else {
                            api.stopCmd(streamID, new ApiCallback<ApiResponse<Void>>() {
                                @Override
                                public ApiResponse<Void> onSuccess(ApiResponse<Void> data) {
                                    btnRecord.setText("录像");
                                    btnRecord.setEnabled(true);
                                    return null;
                                }

                                @Override
                                public void onFailure(String errorEvent, String message) {
                                    btnRecord.setEnabled(true);
                                }
                            });
                        }
                    }
                    break;
                case R.id.btn_switchCamera:
                {
                    // TODO Auto-generated method stub
                    int cameraCount=0;
                    if(isCameraBack){
                        isCameraBack=false;
                    }else{
                        isCameraBack=true;
                    }
                    Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
                    cameraCount = Camera.getNumberOfCameras();//得到摄像头的个数

                    for(int i = 0; i < cameraCount; i++) {

                        Camera.getCameraInfo(i, cameraInfo);//得到每一个摄像头的信息
                        if(mCameraId == Camera.CameraInfo.CAMERA_FACING_FRONT) {
                            //现在是后置，变更为前置
                            if(cameraInfo.facing  == Camera.CameraInfo.CAMERA_FACING_FRONT) {//代表摄像头的方位，CAMERA_FACING_FRONT前置      CAMERA_FACING_BACK后置

                                mCamera.stopPreview();//停掉原来摄像头的预览
                                stopStream();
                                mCamera.release();//释放资源
                                mCamera = null;//取消原来摄像头
                                mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
                                ctreateCamera(surfaceHolder);
                                startPreview();
                                break;
                            }
                        } else {
                            //现在是前置， 变更为后置
                            if(cameraInfo.facing  == Camera.CameraInfo.CAMERA_FACING_BACK) {//代表摄像头的方位，CAMERA_FACING_FRONT前置      CAMERA_FACING_BACK后置
                                stopStream();
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

        class NetWorkReceiver extends BroadcastReceiver {
            @Override
            public void onReceive(Context context, Intent intent) {
                netWorkType = getNetworkType();
                if(getNetworkType().equals("")||getNetworkType().equals("2G")){//没网或者2G停止推送
                    stopStream();
                }
            }

        }

        public String getNetworkType()
        {
            String strNetworkType = "";

            NetworkInfo networkInfo =((ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE)).getActiveNetworkInfo();
            if (networkInfo != null && networkInfo.isConnected())
            {
                if (networkInfo.getType() == ConnectivityManager.TYPE_WIFI)
                {
                    strNetworkType = "WIFI";
                }
                else if (networkInfo.getType() == ConnectivityManager.TYPE_MOBILE)
                {
                    String _strSubTypeName = networkInfo.getSubtypeName();

                    Log.e(TAG, "Network getSubtypeName : " + _strSubTypeName);

                    // TD-SCDMA   networkType is 17
                    int networkType = networkInfo.getSubtype();
                    switch (networkType) {
                        case TelephonyManager.NETWORK_TYPE_GPRS:
                        case TelephonyManager.NETWORK_TYPE_EDGE:
                        case TelephonyManager.NETWORK_TYPE_CDMA:
                        case TelephonyManager.NETWORK_TYPE_1xRTT:
                        case TelephonyManager.NETWORK_TYPE_IDEN: //api<8 : replace by 11
                            strNetworkType = "2G";
                            break;
                        case TelephonyManager.NETWORK_TYPE_UMTS:
                        case TelephonyManager.NETWORK_TYPE_EVDO_0:
                        case TelephonyManager.NETWORK_TYPE_EVDO_A:
                        case TelephonyManager.NETWORK_TYPE_HSDPA:
                        case TelephonyManager.NETWORK_TYPE_HSUPA:
                        case TelephonyManager.NETWORK_TYPE_HSPA:
                        case TelephonyManager.NETWORK_TYPE_EVDO_B: //api<9 : replace by 14
                        case TelephonyManager.NETWORK_TYPE_EHRPD:  //api<11 : replace by 12
                        case TelephonyManager.NETWORK_TYPE_HSPAP:  //api<13 : replace by 15
                            strNetworkType = "3G";
                            break;
                        case TelephonyManager.NETWORK_TYPE_LTE:    //api<11 : replace by 13
                            strNetworkType = "4G";
                            break;
                        default:
                            // http://baike.baidu.com/item/TD-SCDMA 中国移动 联通 电信 三种3G制式
                            if (_strSubTypeName.equalsIgnoreCase("TD-SCDMA") || _strSubTypeName.equalsIgnoreCase("WCDMA") || _strSubTypeName.equalsIgnoreCase("CDMA2000"))
                            {
                                strNetworkType = "3G";
                            }
                            else
                            {
                                strNetworkType = _strSubTypeName;
                            }

                            break;
                    }

                    Log.e(TAG, "Network getSubtype : " + Integer.valueOf(networkType).toString());
                }
            }

            Log.e(TAG, "Network Type : " + strNetworkType);

            return strNetworkType;
        }
    }


