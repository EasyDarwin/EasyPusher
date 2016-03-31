package org.easydarwin.rmspai;

/**
 * Created by heer on 15/10/30.
 */
public interface Api {

    public void startCmd(String id, String url, ApiCallback<ApiResponse<Void>> callback);

    public void stopCmd(String id, ApiCallback<ApiResponse<Void>> callback);

    public void listCmd(String id, ApiCallback<ApiResponse<Void>> callback);

}

