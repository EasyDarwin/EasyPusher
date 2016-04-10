/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.push;

import android.content.Context;
import android.util.Log;

public class EasyPusher {

    static {
        System.loadLibrary("easypusher");
    }

    public interface OnInitPusherCallback {
        public void onCallback(int code);

        static class CODE {
            public static final int EASY_ACTIVATE_INVALID_KEY               =   -1;       //无效Key
            public static final int EASY_ACTIVATE_TIME_ERR                  =   -2;       //时间错误
            public static final int EASY_ACTIVATE_PROCESS_NAME_LEN_ERR      =   -3;       //进程名称长度不匹配
            public static final int EASY_ACTIVATE_PROCESS_NAME_ERR          =   -4;       //进程名称不匹配
            public static final int EASY_ACTIVATE_VALIDITY_PERIOD_ERR       =   -5;       //有效期校验不一致
            public static final int EASY_ACTIVATE_SUCCESS                   =   0;        //激活成功
            public static final int EASY_PUSH_STATE_CONNECTING              =   1;        //连接中
            public static final int EASY_PUSH_STATE_CONNECTED               =   2;        //连接成功
            public static final int EASY_PUSH_STATE_CONNECT_FAILED          =   3;        //连接失败
            public static final int EASY_PUSH_STATE_CONNECT_ABORT           =   4;        //连接异常中断
            public static final int EASY_PUSH_STATE_PUSHING                 =   5;        //推流中
            public static final int EASY_PUSH_STATE_DISCONNECTED            =   6;        //断开连接
            public static final int EASY_PUSH_STATE_ERROR                   =   7;
        }

    }

    public native void setOnInitPusherCallback(OnInitPusherCallback callback);

    /**
     * 初始化
     *
     * @param serverIP   服务器IP
     * @param serverPort 服务端口
     * @param streamName 流名称
     * @param key        授权码
     */
    public native void init(String serverIP, String serverPort, String streamName, String key, Context context);

    /**
     * 推送编码后的H264数据
     *
     * @param data      H264数据
     * @param timestamp 时间戳，毫秒
     */
    public native void push(byte[] data, long timestamp, int type);


    /**
     * 停止推送
     */
    private native void stopPush();

    public void stop() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                stopPush();
            }
        }).start();
    }

    public void initPush(final String serverIP, final String serverPort, final String streamName, final Context context) {
        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                stopPush();

                String key = "6A34714D6C3536526D343041462F74577151546E6B755A76636D63755A57467A65575268636E64706269356C59584E356348567A6147567956752B71415036535A57467A65513D3D";

                init(serverIP, serverPort, streamName, key, context);
            }
        });
        t.start();
    }

}

