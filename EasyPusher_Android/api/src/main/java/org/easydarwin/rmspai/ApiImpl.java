package org.easydarwin.rmspai;

import android.util.Log;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import com.loopj.android.http.RequestParams;
import com.loopj.android.http.TextHttpResponseHandler;

import org.apache.http.Header;

import java.lang.reflect.Type;


/**
 * Created by heer on 15/11/1.
 */
public class ApiImpl implements Api, IConstant {
    private final static String TIME_OUT_EVENT = "CONNECT_TIME_OUT";
    private final static String INVID_MESSAGE = "INVID_MESSAGE";
    private final static String TIME_OUT_EVENT_MSG = "连接服务器失败";
    private final static String INVID_MESSAGE_MSG = "命令发送错误";
    private final static String RMS_URL="http://203.195.131.217:10000/api/easyrecordmodule";

    @Override
    public void startCmd(String id,String url,final ApiCallback<ApiResponse<Void>> callback) {
        RequestParams params = new RequestParams();
        params.put("name",id);
        params.put("url",url);
        try {
            DcRestClient.get(RMS_URL, params, new TextHttpResponseHandler() {
                @Override
                public void onFailure(int statusCode, Header[] headers, String responseString, Throwable throwable) {
                    callback.onFailure(TIME_OUT_EVENT, TIME_OUT_EVENT_MSG);
                }

                @Override
                public void onSuccess(int statusCode, Header[] headers, String responseString) {
                    Log.e("DcRestClient", responseString);
                    Type type = new TypeToken<ApiResponse<Void>>() {
                    }.getType();
                    Gson gson = new Gson();
                    ApiResponse<Void> bean = gson.fromJson(responseString, type);
                    if (bean.getEasyDarwin().getHeader().getMessageType().equals("MSG_CLI_SMS_HLS_ACK"))
                        callback.onSuccess(bean);
                    else
                        callback.onFailure(INVID_MESSAGE, INVID_MESSAGE_MSG);
                }
            });
        } catch (Exception e) {
            callback.onFailure(TIME_OUT_EVENT,TIME_OUT_EVENT_MSG);
        }
    }

    @Override
    public void stopCmd(String id,final ApiCallback<ApiResponse<Void>> callback) {
        RequestParams params = new RequestParams();
        params.put("name",id);
        params.put("cmd","stop");
        try {
            DcRestClient.get(RMS_URL, params, new TextHttpResponseHandler() {
                @Override
                public void onFailure(int statusCode, Header[] headers, String responseString, Throwable throwable) {
                    callback.onFailure(TIME_OUT_EVENT, TIME_OUT_EVENT_MSG);
                }

                @Override
                public void onSuccess(int statusCode, Header[] headers, String responseString) {
                    Log.e("DcRestClient", responseString);
                    Type type = new TypeToken<ApiResponse<Void>>() {
                    }.getType();
                    Gson gson = new Gson();
                    ApiResponse<Void> bean = gson.fromJson(responseString, type);
                    if (bean.getEasyDarwin().getHeader().getMessageType().equals("MSG_CLI_SMS_HLS_ACK"))
                        callback.onSuccess(bean);
                    else
                        callback.onFailure(INVID_MESSAGE, INVID_MESSAGE_MSG);
                }
            });
        } catch (Exception e) {
            callback.onFailure(TIME_OUT_EVENT,TIME_OUT_EVENT_MSG);
        }
    }

    @Override
    public void listCmd(String id,final ApiCallback<ApiResponse<Void>> callback) {
        RequestParams params = new RequestParams();
        params.put("name",id);
        params.put("cmd","list");
        params.put("begin","20160101000000");
        params.put("end","21160101000000");
        try {

            DcRestClient.get(RMS_URL, params, new TextHttpResponseHandler() {
                @Override
                public void onFailure(int statusCode, Header[] headers, String responseString, Throwable throwable) {
                    callback.onFailure(TIME_OUT_EVENT, TIME_OUT_EVENT_MSG);
                }

                @Override
                public void onSuccess(int statusCode, Header[] headers, String responseString) {
                    Log.e("DcRestClient", responseString);
                    Type type = new TypeToken<ApiResponse<Void>>() {
                    }.getType();
                    Gson gson = new Gson();
                    ApiResponse<Void> bean = gson.fromJson(responseString, type);
                    if (bean.getEasyDarwin().getHeader().getMessageType().equals("MSG_CLI_RMS_RECORD_LIST_ACK"))
                        callback.onSuccess(bean);
                    else
                        callback.onFailure(INVID_MESSAGE, INVID_MESSAGE_MSG);
                }
            });
        } catch (Exception e) {
            callback.onFailure(TIME_OUT_EVENT,TIME_OUT_EVENT_MSG);
        }
    }

}
