package org.easydarwin.rmspai;

import android.util.Log;

import com.loopj.android.http.AsyncHttpClient;
import com.loopj.android.http.AsyncHttpResponseHandler;
import com.loopj.android.http.RequestParams;

/**
 * Created by heer on 15/11/1.
 */
public class DcRestClient implements IConstant {

    private static AsyncHttpClient client = new AsyncHttpClient();

    public static void get(String url, RequestParams params, AsyncHttpResponseHandler responseHandler) {
        Log.e("DcRestClient",url);
        client.setEnableRedirects(false);
        client.get(url, params, responseHandler);
    }

    public static void post(String url, RequestParams params, AsyncHttpResponseHandler responseHandler) {
        Log.e("DcRestClient",url);
        client.setEnableRedirects(false);
        client.post(url, params, responseHandler);
    }
}
