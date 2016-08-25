package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public final class MSMessage {
    private String content = null;
    private String extra = null;
    private String msgId = null;
    private String toId = null;
    private String fromId = null;
    private String groupId = null;
    private String nickName = null;
    private String uiconUrl = null;
    private String cash = null;
    private String wishcont = null;
    private String toNickName = null;

    private int    millSec = 0;//send second
    private int    msgType = 0;
    private int    flag = 0;
    private int    push = 0;

    public MSMessage() {
    }

    public int getPush() {
        return push;
    }

    public void setPush(int push) {
        this.push = push;
    }

    public int getFlag() {
        return flag;
    }

    public void setFlag(int flag) {
        this.flag = flag;
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

    public int getMsgType() {
        return msgType;
    }

    public void setMsgType(int msgType) {
        this.msgType = msgType;
    }

    public String getContent() {
        return content;
    }

    public void setContent(String content) {
        this.content = content;
    }

    public String getToNickName() { return toNickName; }

    public void setToNickName(String toNickName) { this.toNickName = toNickName; }
}
