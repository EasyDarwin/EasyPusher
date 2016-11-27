/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easypusher;

import android.content.ComponentName;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.media.projection.MediaProjectionManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.preference.PreferenceManager;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.Spinner;
import android.widget.TextView;

import org.easydarwin.push.EasyPusher;
import org.easydarwin.push.MediaStream;
import org.easydarwin.updatemgr.UpdateMgr;
import org.easydarwin.util.Util;

import java.util.ArrayList;
import java.util.List;

public class StreameActivity extends AppCompatActivity implements SurfaceHolder.Callback, View.OnClickListener {

    static final String TAG = "EasyPusher";
    public static final int REQUEST_MEDIA_PROJECTION = 1002;

    //默认分辨率
    int width = 640, height = 480;
    Button btnSwitch;
    Button btnSetting;
    TextView txtStreamAddress;
    Button btnSwitchCemera;
    Spinner spnResolution;
    List<String> listResolution;
    MediaStream mMediaStream;
    TextView txtStatus;
    static Intent mResultIntent;
    static int mResultCode;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        spnResolution = (Spinner) findViewById(R.id.spn_resolution);

        txtStatus = (TextView) findViewById(R.id.txt_stream_status);
        btnSwitch = (Button) findViewById(R.id.btn_switch);
        btnSwitch.setOnClickListener(this);
        btnSetting = (Button) findViewById(R.id.btn_setting);
        btnSetting.setOnClickListener(this);
        btnSwitchCemera = (Button) findViewById(R.id.btn_switchCamera);
        btnSwitchCemera.setOnClickListener(this);
        txtStreamAddress = (TextView) findViewById(R.id.txt_stream_address);
        SurfaceView surfaceView = (SurfaceView) findViewById(R.id.sv_surfaceview);
        surfaceView.getHolder().addCallback(this);
        surfaceView.getHolder().setFixedSize(getResources().getDisplayMetrics().widthPixels,
                getResources().getDisplayMetrics().heightPixels);
        surfaceView.setOnClickListener(this);

        listResolution = new ArrayList<String>();
        listResolution = Util.getSupportResolution(this);
        boolean supportdefault = listResolution.contains(String.format("%dx%d", width, height));
        if (!supportdefault) {
            String r = listResolution.get(0);
            String[] splitR = r.split("x");
            width = Integer.parseInt(splitR[0]);
            height = Integer.parseInt(splitR[1]);
        }

        Button pushScreen = (Button) findViewById(R.id.push_screen);
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP){
            pushScreen.setVisibility(View.GONE);
        }

        Button button = (Button) findViewById(R.id.push_screen);
        if (RecordService.mEasyPusher != null){
            button.setText("停止推送屏幕");


            String ip = EasyApplication.getEasyApplication().getIp();
            String port = EasyApplication.getEasyApplication().getPort();
            String id = EasyApplication.getEasyApplication().getId();
            TextView viewById = (TextView) findViewById(R.id.push_screen_url);
            viewById.setText(String.format("视频URL:\nrtsp://%s:%s/%s_s.sdp", ip, port, id));
        }

        UpdateMgr update = new UpdateMgr(this);
        update.checkUpdate();
    }

    private void startScreenPushIntent() {
        if (StreameActivity.mResultIntent != null && StreameActivity.mResultCode != 0) {
            Intent intent = new Intent(getApplicationContext(), RecordService.class);
            startService(intent);

            RecordService.mEasyPusher = new EasyPusher();


            TextView viewById = (TextView) findViewById(R.id.push_screen_url);

            String ip = EasyApplication.getEasyApplication().getIp();
            String port = EasyApplication.getEasyApplication().getPort();
            String id = EasyApplication.getEasyApplication().getId();


            viewById.setText(String.format("视频URL:\nrtsp://%s:%s/%s_s.sdp", ip, port, id));


            Button button = (Button) findViewById(R.id.push_screen);
            button.setText("停止推送屏幕");
        } else {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {

                MediaProjectionManager mMpMngr = (MediaProjectionManager) getApplicationContext().getSystemService(MEDIA_PROJECTION_SERVICE);
                startActivityForResult(mMpMngr.createScreenCaptureIntent(), StreameActivity.REQUEST_MEDIA_PROJECTION);

            }
        }
    }

    public void onPushScreen(final View view) {
        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.LOLLIPOP){
            new AlertDialog.Builder(this).setMessage("推送屏幕需要安卓5.0以上,您当前系统版本过低,不支持该功能。").setTitle("抱歉").show();
            return;
        }

        if (!PreferenceManager.getDefaultSharedPreferences(this).getBoolean("alert_screen_background_pushing", false)){
            new AlertDialog.Builder(this).setTitle("提醒").setMessage("屏幕直播将要开始,直播过程中您可以切换到其它屏幕。不过记得直播结束后,再进来停止直播哦!").setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    PreferenceManager.getDefaultSharedPreferences(StreameActivity.this).edit().putBoolean("alert_screen_background_pushing", true).apply();
                    onPushScreen(view);
                }
            }).show();
            return;
        }
        Button button = (Button) findViewById(R.id.push_screen);
        if (RecordService.mEasyPusher != null) {
            Intent intent = new Intent(getApplicationContext(), RecordService.class);
            stopService(intent);

            TextView viewById = (TextView) findViewById(R.id.push_screen_url);
            viewById.setText(null);
            button.setText("推送屏幕");
        }else{
            startScreenPushIntent();
        }
    }


    private static final String STATE = "state";
    private static final int MSG_STATE = 1;

    private void sendMessage(String message) {
        Message msg = Message.obtain();
        msg.what = MSG_STATE;
        Bundle bundle = new Bundle();
        bundle.putString(STATE, message);
        msg.setData(bundle);
        handler.sendMessage(msg);
    }

    Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_STATE:
                    String state = msg.getData().getString("state");
                    txtStatus.setText(state);
                    break;
            }
        }
    };

    private void initSpninner() {
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, R.layout.spn_item, listResolution);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spnResolution.setAdapter(adapter);
        int position = listResolution.indexOf(String.format("%dx%d", width, height));
        spnResolution.setSelection(position);
        spnResolution.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                String r = listResolution.get(position);
                String[] splitR = r.split("x");
                width = Integer.parseInt(splitR[0]);
                height = Integer.parseInt(splitR[1]);
                if (mMediaStream != null) {
                    mMediaStream.updateResolution(width, height);
                    mMediaStream.reStartStream();
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
    }

    @Override
    public void surfaceCreated(final SurfaceHolder holder) {
        if (EasyApplication.sMS == null) {
            mMediaStream = new MediaStream(getApplicationContext(), holder);
            EasyApplication.sMS = mMediaStream;

            startCamera();
        } else {
            bindService(new Intent(this, BackgroundCameraService.class), new ServiceConnection() {
                @Override
                public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
                    BackgroundCameraService service = ((BackgroundCameraService.LocalBinder) iBinder).getService();
                    service.stopMySelf();


                    mMediaStream = EasyApplication.sMS;
                    mMediaStream.setSurfaceHolder(holder);
                    startCamera();

                    unbindService(this);
                }

                @Override
                public void onServiceDisconnected(ComponentName componentName) {

                }
            }, 0);
        }
    }

    private void startCamera() {
        mMediaStream.updateResolution(width, height);
        mMediaStream.setDgree(getDgree());
        mMediaStream.createCamera();
        mMediaStream.startPreview();

        if (mMediaStream.isStreaming()) {
            sendMessage("推流中");
            btnSwitch.setText("停止");


            String ip = EasyApplication.getEasyApplication().getIp();
            String port = EasyApplication.getEasyApplication().getPort();
            String id = EasyApplication.getEasyApplication().getId();
            txtStreamAddress.setText(String.format("rtsp://%s:%s/%s.sdp", ip, port, id));
        }

        initSpninner();
    }


    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        boolean isStreaming = mMediaStream != null && mMediaStream.isStreaming();
        mMediaStream.stopPreview();
        mMediaStream.destroyCamera();

        if (isStreaming && PreferenceManager.getDefaultSharedPreferences(StreameActivity.this).getBoolean("key_enable_background_camera", true)) {
            startService(new Intent(StreameActivity.this, BackgroundCameraService.class));
        } else {
            mMediaStream.stopStream();
            EasyApplication.sMS = null;
        }
        mMediaStream = null;
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
            case R.id.push_screen:{
                startScreenPushIntent();
            }
            break;
            case R.id.btn_switch:
                if (!mMediaStream.isStreaming()) {
                    String ip = EasyApplication.getEasyApplication().getIp();
                    String port = EasyApplication.getEasyApplication().getPort();
                    String id = EasyApplication.getEasyApplication().getId();

                    mMediaStream.startStream(ip, port, id, new EasyPusher.OnInitPusherCallback() {
                        @Override
                        public void onCallback(int code) {
                            switch (code) {
                                case CODE.EASY_ACTIVATE_INVALID_KEY:
                                    sendMessage("无效Key");
                                    break;
                                case CODE.EASY_ACTIVATE_SUCCESS:
                                    sendMessage("激活成功");
                                    break;
                                case CODE.EASY_PUSH_STATE_CONNECTING:
                                    sendMessage("连接中");
                                    break;
                                case CODE.EASY_PUSH_STATE_CONNECTED:
                                    sendMessage("连接成功");
                                    break;
                                case CODE.EASY_PUSH_STATE_CONNECT_FAILED:
                                    sendMessage("连接失败");
                                    break;
                                case CODE.EASY_PUSH_STATE_CONNECT_ABORT:
                                    sendMessage("连接异常中断");
                                    break;
                                case CODE.EASY_PUSH_STATE_PUSHING:
                                    sendMessage("推流中");
                                    break;
                                case CODE.EASY_PUSH_STATE_DISCONNECTED:
                                    sendMessage("断开连接");
                                    break;
                                case CODE.EASY_ACTIVATE_PLATFORM_ERR:
                                    sendMessage("平台不匹配");
                                    break;
                                case CODE.EASY_ACTIVATE_COMPANY_ID_LEN_ERR:
                                    sendMessage("断授权使用商不匹配");
                                    break;
                                case CODE.EASY_ACTIVATE_PROCESS_NAME_LEN_ERR:
                                    sendMessage("进程名称长度不匹配");
                                    break;
                            }
                        }
                    });
                    btnSwitch.setText("停止");

                    txtStreamAddress.setText(String.format("rtsp://%s:%s/%s.sdp", ip, port, id));
                } else {
                    mMediaStream.stopStream();
                    btnSwitch.setText("开始");
                }
                break;
            case R.id.btn_setting:
                startActivity(new Intent(this, SettingActivity.class));
                break;
            case R.id.sv_surfaceview:
                try {
                    mMediaStream.getCamera().autoFocus(null);
                } catch (Exception e) {
                }
                break;
            case R.id.btn_switchCamera: {
                mMediaStream.setDgree(getDgree());
                mMediaStream.switchCamera();
            }
            break;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_MEDIA_PROJECTION) {
            if (resultCode == RESULT_OK) {
                Log.e(TAG,"get capture permission success!");
                mResultCode = resultCode;
                mResultIntent = data;

                startScreenPushIntent();

            }
        }
    }


    /**
     * Take care of popping the fragment back stack or finishing the activity
     * as appropriate.
     */
    @Override
    public void onBackPressed() {
        boolean isStreaming = mMediaStream != null && mMediaStream.isStreaming();
        if (isStreaming && PreferenceManager.getDefaultSharedPreferences(this).getBoolean("key_enable_background_camera", true)){
            if (!PreferenceManager.getDefaultSharedPreferences(this).getBoolean("background_camera_alert", false)){
                new AlertDialog.Builder(this).setTitle("提醒").setMessage("您设置了使能摄像头后台采集,因此摄像头将会继续在后台采集并上传视频。记得直播结束后,再回来这里关闭直播。").setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        PreferenceManager.getDefaultSharedPreferences(StreameActivity.this).edit().putBoolean("background_camera_alert", true).apply();
                        StreameActivity.super.onBackPressed();
                    }
                }).show();
                return;
            }
            super.onBackPressed();
        }else{
            super.onBackPressed();
        }
    }
}
