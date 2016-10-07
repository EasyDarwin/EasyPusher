package org.easydarwin.updatemgr;

/**
 * Created by Kim on 2016/8/25.
 */
public class RemoteVersionInfo {
    private String versionCode;
    private String versionName;
    private String url;

    public String getVersionCode() {
        return versionCode;
    }

    public void setVersionCode(String versionCode) {
        this.versionCode = versionCode;
    }

    public String getVersionName() {
        return versionName;
    }

    public void setVersionName(String versionName) {
        this.versionName = versionName;
    }

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }
}
