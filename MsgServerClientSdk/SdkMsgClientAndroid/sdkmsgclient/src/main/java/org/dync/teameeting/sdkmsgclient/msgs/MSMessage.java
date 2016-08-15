package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public class MSMessage {
    private String content;
    private String extra;
    private String msgId;
    private String toId;
    private String fromId;
    private String groupId;
    private String nickName;
    private String uiconUrl;
    private String cash;
    private String wishcont;

    private int    millSec;//send second
    private int    msgType;
    private int    flag;

    public MSMessage() {
    }

    public String getContent() {
        return content;
    }

    public void setContent(String content) {
        this.content = content;
    }

    public String getExtra() {
        return extra;
    }

    public void setExtra(String extra) {
        this.extra = extra;
    }

    public String getMsgId() {
        return msgId;
    }

    public void setMsgId(String msgId) {
        this.msgId = msgId;
    }

    public String getToId() {
        return toId;
    }

    public void setToId(String toId) {
        this.toId = toId;
    }

    public String getFromId() {
        return fromId;
    }

    public void setFromId(String fromId) {
        this.fromId = fromId;
    }

    public String getGroupId() {
        return groupId;
    }

    public void setGroupId(String groupId) {
        this.groupId = groupId;
    }

    public String getNickName() {
        return nickName;
    }

    public void setNickName(String nickName) {
        this.nickName = nickName;
    }

    public String getUiconUrl() {
        return uiconUrl;
    }

    public void setUiconUrl(String uiconUrl) {
        this.uiconUrl = uiconUrl;
    }

    public String getCash() {
        return cash;
    }

    public void setCash(String cash) {
        this.cash = cash;
    }

    public String getWishcont() {
        return wishcont;
    }

    public void setWishcont(String wishcont) {
        this.wishcont = wishcont;
    }

    public int getMillSec() {
        return millSec;
    }

    public void setMillSec(int millSec) {
        this.millSec = millSec;
    }

    public int getMsgType() { return msgType; }

    public void setMsgType(int msgType) {
        this.msgType = msgType;
    }

    public int getFlag() { return flag; }

    public void setFlag(int flag) { this.flag = flag; }
}
