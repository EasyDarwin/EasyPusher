package org.easydarwin.rmspai;

/**
 * Created by sevens on 2016/3/30.
 */
public class Header {
    private String MessageType;//MSG_CLI_RMS_RECORD_LIST_ACK
    private String Version;

    public String getMessageType() {
        return MessageType;
    }

    public void setMessageType(String messageType) {
        MessageType = messageType;
    }

    public String getVersion() {
        return Version;
    }

    public void setVersion(String version) {
        Version = version;
    }
}
