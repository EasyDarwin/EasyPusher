/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.push;

public class EasyPusher {

    static {
        System.loadLibrary("easypusher");
    }

    /**
     * 初始化
     *
     * @param serverIP   服务器IP
     * @param serverPort 服务端口
     * @param streamName 流名称
     */
    public native void init(String serverIP, String serverPort, String streamName);

    /**
     * 推送编码后的H264数据
     *
     * @param data      H264数据
     * @param timestamp 时间戳，毫秒
     */
    public native void push(byte[] data, long timestamp,int type);


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

    public void initPush(final String serverIP, final String serverPort, final String streamName){
        Thread t=new Thread(new Runnable() {
            @Override
            public void run() {
                stopPush();
                init(serverIP,serverPort,streamName);
            }
        });
        t.start();
    }

}

