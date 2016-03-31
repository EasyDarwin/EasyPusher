package org.easydarwin.rmspai;

/**
 * Created by heer on 15/11/1.
 */
public interface ApiCallback<T> {
    /**
     * 成功时调用
     *
     * @param data 返回的数据
     */
    public T onSuccess(T data);

    /**
     * 失败时调用
     *
     * @param errorEvent 错误码
     * @param message    错误信息
     */
    public void onFailure(String errorEvent, String message);
}
